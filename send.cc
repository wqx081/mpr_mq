#include "base/macros.h"

#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <memory>

static void timeout_cb(int /* fd */, short /* event */, void* /* arg */) {
  //LOG(INFO) << "--timeout_cb";
  exit(1);
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  struct event* timeout;
  auto evbase = ::event_base_new();
  AMQP::LibEventHandler handler(evbase);
  AMQP::TcpConnection c(&handler, AMQP::Address("amqp://fb04/"));  
  AMQP::TcpChannel channel(&c);

  channel.declareQueue("epub_info_queue");
  channel.publish("", "epub_info_queue", "{\"book_id\":\"12333333333345642\", \"book_path\":\"/tmp/1.epub\"}");

  timeout = event_new(evbase, -1, EV_TIMEOUT, timeout_cb, nullptr);
  struct timeval future_time;
  future_time.tv_sec = 3;
  future_time.tv_usec = 0;

  event_add(timeout, &future_time);

  ::event_base_dispatch(evbase);
  ::event_base_free(evbase);

  return 0;
}
