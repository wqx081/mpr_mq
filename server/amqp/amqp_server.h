#ifndef SERVER_AMQP_SERVER_H_
#define SERVER_AMQP_SERVER_H_
#include <memory>
#include <mutex>

#include "server/server_interface.h"

namespace server {

class AmqpServer : public ServerInterface {
 protected:
  AmqpServer(const std::string& server_def);
  
 public:
  static Status Create(const std::string& server_def,
                       std::unique_ptr<ServerInterface>* out_server);

  virtual ~AmqpServer();

  // ServerInterface
  Status Start() override;
  Status Stop() override;
  Status Join() override;
  const std::string target() const override;

 protected:
  Status Init();

 private:
  const std::string& server_def_;
  
  // Guards state transitions.
  std::mutex mu_;

  enum State { NEW, STARTED, STOPPED };
  State state_ GUARDED_BY(mu_); 

  // TODO
  // Implementation of a AMQP Consumer Service
  // AsyncServiceInterface* consumer_ = nullptr;
  // std::unique_ptr<threading::Thread> consumer_thread_ GUARDED_BY(mu_);
  //
};

} // namespace
#endif // SERVER_AMQP_SERVER_H_
