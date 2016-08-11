#include "server/amqp/amqp_server.h"

#include <limits>
#include <memory>

namespace server {

AmqpServer::AmqpServer(const std::string& server_def)
    : server_def_(server_def),
      state_(NEW) {}

AmqpServer::~AmqpServer() {
  Stop();
  Join();

  // delete
}

Status AmqpServer::Init() {
  std::lock_guard<std::mutex> l(mu_);
  //
  // parse server_def_, get the needed infomation 
  // init the services
  return Status::OK();
}

Status AmqpServer::Start() {
  std::lock_guard<std::mutex> l(mu_);

  switch (state_) {
    case NEW: {
      // Start the serivce run in new thread
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
      // TODO:
      // reset
      return Status::OK();
    default:
      CHECK(false);
  }
  // make the compiler silence.
  return Status::OK();
}

const std::string AmqpServer::target() const {
  //TODO
  return "amqp://localhost";
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
