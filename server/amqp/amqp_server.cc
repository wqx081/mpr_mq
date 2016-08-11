#include "server/amqp/amqp_server.h"
#include "threading/thread.h"

#include <limits>
#include <memory>
#include <algorithm>

namespace server {

AmqpServer::AmqpServer(const std::string& server_def)
    : server_def_(server_def),
      state_(NEW),
      total_tasks_(0),
      thread_factory_(make_unique<threading::PosixThreadFactory>()){
}

AmqpServer::~AmqpServer() {
  Stop();
  Join();

  // delete
}

Status AmqpServer::Init() {
  std::lock_guard<std::mutex> l(mu_);
  // TODO
  // parse server_def_, get the needed infomation 
  // init the services
  return Status::OK();
}

Status AmqpServer::Start() {
  std::lock_guard<std::mutex> l(mu_);

  switch (state_) {
    case NEW: {
      // Start the serivce run in new thread
      for (ServiceExecutorIterator it = service_executor_list_.begin();
           it != service_executor_list_.end();
           ++it) {
        std::shared_ptr<threading::Thread> thread = thread_factory_->NewThread(*it);
        thread_pool_.insert(thread);
      }
      for (std::set<std::shared_ptr<threading::Thread>>::iterator thread = thread_pool_.begin();
           thread != thread_pool_.end();
           ++thread) {
        (*thread)->Start();
      }

      state_ = STARTED;
      LOG(INFO) << "Started server with target: " << target();
      return Status::OK();
    }
    case STARTED:
      LOG(INFO) << "Server already started (target: " << target() << ")";
      return Status::OK();
    case STOPPED:
      return Status(base::Code::FAILED_PRECONDITION, "Server has stopped");
    default:
      CHECK(false);
  }
  // make the compiler silence.
  return Status::OK();
}

Status AmqpServer::Stop() {
  std::lock_guard<std::mutex> l(mu_);
  switch (state_) {
    case NEW:
      state_ = STOPPED;
      return Status::OK();
    case STARTED:
      // server_ shutdown()
      state_ = STOPPED;
      return Status::OK();
    case STOPPED:
      LOG(INFO) << "Server already stopped (target: " << target() << ")";
      return Status::OK();
    default:
      CHECK(false);
  }
  // make the compiler silence.
  return Status::OK();
}

Status AmqpServer::Join() {
  std::lock_guard<std::mutex> l(mu_);
  switch (state_) {
    case NEW:
      state_ = STOPPED;
      return Status::OK();
    case STARTED:
    case STOPPED:
      // reset and Join
      {
        threading::Synchronized s(monitor_);
        while (total_tasks_ > 0) {
          monitor_.Wait();
        }
      }
      return Status::OK();
    default:
      CHECK(false);
  }
  // make the compiler silence.
  return Status::OK();
}

const std::string AmqpServer::target() const {
  return "amqp://localhost";
}

Status AmqpServer::InsertAsyncService(std::shared_ptr<AsyncServiceInterface> service) {
  std::lock_guard<std::mutex> l(mu_);
  for (ServiceExecutorIterator it = service_executor_list_.begin();
       it != service_executor_list_.end();
       ++it) {
    if ((*it)->service() == service) {
      return Status(base::Code::ALREADY_EXISTS, "service already existed");
    }
  }
  service_executor_list_.push_back(std::make_shared<ServiceExecutor>(service, monitor_, total_tasks_));
  total_tasks_++;
  return Status::OK();
}

Status AmqpServer::RemoveAsyncService(std::shared_ptr<AsyncServiceInterface> service) {
  std::lock_guard<std::mutex> l(mu_);
  for (ServiceExecutorIterator it = service_executor_list_.begin();
       it != service_executor_list_.end();
       ++it) {
    if ((*it)->service() == service) {
      total_tasks_--;
      service_executor_list_.erase(it);
    }
  }
  return Status::OK();
}

// static
Status AmqpServer::Create(const std::string& server_def,
                          std::unique_ptr<ServerInterface>* out_server) {
  std::unique_ptr<AmqpServer> ret(new AmqpServer(server_def));
  RETURN_IF_ERROR(ret->Init());
  *out_server = std::move(ret);
  return Status::OK();
}

// register
namespace {

class AmqpServerFactory : public ServerFactory {
 public:
  bool AcceptsOptions(const std::string& server_def) {
    return server_def.find("amqp:") != std::string::npos;
  }
  Status NewServer(const std::string& server_def,
                   std::unique_ptr<ServerInterface>* out_server) override {
    return AmqpServer::Create(server_def, out_server);
  }
};

// Registers a `ServerFactory` for `AmqpServer` instances.
class AmqpServerRegistrar {
 public:
  AmqpServerRegistrar() {
    ServerFactory::Register("AMQP_SERVER", new AmqpServerFactory());
  }
};

static AmqpServerRegistrar registrar;

} // namespace
} // namespace server
