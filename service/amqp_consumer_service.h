#ifndef SERVER_AMQP_CONSUMER_SERVICE_H_
#define SERVER_AMQP_CONSUMER_SERVICE_H_
#include "base/macros.h"

#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <functional>
#include <string>
#include "server/async_service_interface.h"

namespace server {

class WrapperEventBase {
 public:
  WrapperEventBase() : event_base_(::event_base_new()) {
    DCHECK(event_base_);
  }

  event_base* get() {
    return event_base_;
  }

  ~WrapperEventBase() {
    ::event_base_free(event_base_);
  }

 private:
  struct event_base* event_base_;
  DISALLOW_COPY_AND_ASSIGN(WrapperEventBase);
};


AsyncServiceInterface* NewAmqpConsumer(const std::string& info);

// AmqpConsumerServiceStragey
class AmqpConsumerServiceHandler {
 public:
  virtual ~AmqpConsumerServiceHandler() {}
  virtual void Handle(const AMQP::Message& message, 
                      uint64_t delivery_tag, 
                      bool redelivered) = 0;
};

class AmqpConsumerService : public AsyncServiceInterface {
 public:
  AmqpConsumerService(const std::string& address,
                      const std::string& queue_name) 
    : address_(address),
      queue_name_(queue_name),
      event_base_(),
      handler_(nullptr){
  }

  virtual ~AmqpConsumerService() {
    
  }

  void Shutdown() override {
  }

  void HandleLoop() override {
    AMQP::LibEventHandler event_handler(event_base_.get());
    AMQP::TcpConnection connection(&event_handler, AMQP::Address(address_));
    AMQP::TcpChannel channel(&connection);

    channel.declareQueue(queue_name_);
    std::function<void(const AMQP::Message&, uint64_t, bool)> callback = 
        [=] (const AMQP::Message& message, uint64_t delivery_tag, bool redelivered) {
      LOG(INFO) << "Received message: " << message.message();
      if (this->handler_) {
        this->handler_->Handle(message, delivery_tag, redelivered);
      } else {
        LOG(INFO) << "No consumer Handler";
      }
    };

    channel.consume(queue_name_, AMQP::noack).onReceived(callback);
    
    LOG(INFO) << "--Waiting for message.";
    ::event_base_dispatch(event_base_.get());
  }

 private:  
#if 0
  std::string host_;
  uint16_t port_;
  std::string vhost_;
  
  std::string username_;
  std::string password_;
#endif
  const std::string address_; // "amqp://"
  const std::string queue_name_;

  WrapperEventBase event_base_;
  AmqpConsumerServiceHandler* handler_;
};

} // namespace server
#endif // SERVER_AMQP_AMQP_CONSUMER_SERVICE_H_
