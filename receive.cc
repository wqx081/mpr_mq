#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  auto evbase = ::event_base_new();
  AMQP::LibEventHandler handler(evbase);
  AMQP::TcpConnection c(&handler, AMQP::Address("amqp://localhost/"));  
  AMQP::TcpChannel channel(&c);

  channel.declareQueue("hello");
  channel.consume("hello", AMQP::noack).onReceived(
    [] (const AMQP::Message& message,
        uint64_t deliveryTag,
	bool redelivered) {
      (void) deliveryTag;
      (void) redelivered;
      LOG(INFO) << " [x] Received " << message.message();
    });

  LOG(INFO) << " [*] Waiting for messages. To exit press CTRL-C\n";
  ::event_base_dispatch(evbase);
  ::event_base_free(evbase);
  return 0;
}
