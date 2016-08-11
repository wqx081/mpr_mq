#ifndef SERVER_AMQP_SERVER_H_
#define SERVER_AMQP_SERVER_H_
#include <memory>
#include <mutex>
#include <list>
#include <set>
#include <unordered_map>

#include "server/server_interface.h"
#include "threading/thread.h"
#include "threading/thread_factory.h"
#include "threading/monitor.h"

namespace server {

class ServiceExecutor : public threading::Runnable {
 public:
  explicit ServiceExecutor(std::shared_ptr<AsyncServiceInterface> service,
                           threading::Monitor& monitor,
                           int& total_tasks)
      : service_(service),
        monitor_(monitor),
        total_tasks_(total_tasks){}     

  void Run() override {
    service_->HandleLoop();

    threading::Synchronized s(monitor_);
    total_tasks_--;
    if (total_tasks_ == 0) {
      monitor_.Notify();
    }
  }
  void Stop() {
    service_->Shutdown();
  }
  const std::shared_ptr<AsyncServiceInterface> service() const {
    return service_;
  }

 private:
  const std::shared_ptr<AsyncServiceInterface> service_;      
  threading::Monitor& monitor_;
  int& total_tasks_;
};

class AmqpServer : public ServerInterface {
 protected:
  AmqpServer(const std::string& server_def);
  
 public:
  static Status Create(const std::string& server_def,
                       std::unique_ptr<ServerInterface>* out_server);

  virtual ~AmqpServer();

  // ServerInterface
  virtual Status Start() override;
  virtual Status Stop() override;
  virtual Status Join() override;
  virtual const std::string target() const override;

  virtual Status InsertAsyncService(std::shared_ptr<AsyncServiceInterface> service) override;   
  virtual Status RemoveAsyncService(std::shared_ptr<AsyncServiceInterface> service) override;

 protected:
  Status Init();

 private:
  const std::string& server_def_;
  
  // Guards state transitions.
  std::mutex mu_;

// Represents the current state of the server, which changes as follows:
//
//                 Join()            Join()
//                  ___               ___
//      Start()     \ /    Stop()     \ /
// NEW ---------> STARTED --------> STOPPED
//   \                          /
//    \________________________/
//            Stop(), Join()
  enum State { NEW, STARTED, STOPPED };
  State state_ GUARDED_BY(mu_); 


  std::list<std::shared_ptr<ServiceExecutor>> service_executor_list_ GUARDED_BY(mu_);
  using ServiceExecutorIterator = std::list<std::shared_ptr<ServiceExecutor>>::iterator;
  std::set<std::shared_ptr<threading::Thread>> thread_pool_;
  threading::Monitor monitor_;
  int total_tasks_;

  std::unique_ptr<threading::ThreadFactory> thread_factory_;
};

} // namespace
#endif // SERVER_AMQP_SERVER_H_
