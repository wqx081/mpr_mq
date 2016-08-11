#include "base/macros.h"

#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <memory>

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  auto evbase = ::event_base_new();
  AMQP::LibEventHandler handler(evbase);
  AMQP::TcpConnection c(&handler, AMQP::Address("amqp://fb04/"));  
  AMQP::TcpChannel channel(&c);

  channel.declareQueue("hello_queue");

  channel.publish("", "hello_queue", "hi it's me");

  //::event_base_dispatch(evbase);
  ::event_base_free(evbase);

  return 0;
}
