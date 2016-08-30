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
  AMQP::TcpConnection c(&handler, AMQP::Address("amqp://guest:guest@172.16.2.109/"));  
  AMQP::TcpChannel channel(&c);

  const char *message = "{\"video_source_path\":\"/data/note/201603181703265628/147245182912437800/orig/isli_video_147245182912437500.mp4\",\"video_target_path\":\"/data/note/201603181703265628/147245182912437800/ld/orig/\",\"target_id\":\"147245182912437800\",\"sample\":\"48000\",\"frame_size\":\"320x240\",\"frame_aspect\":\"4:3\",\"frame_rate\":\"25\",\"rate_bit\":\"350k\",\"time\":\"10\",\"url_prefix\":\"http://172.16.2.103/data/note/201603181703265628/147245182912437800/ld/orig/\",\"m3u8_name\":\"my.m3u8\"}";

  channel.declareQueue("video_rpc_queue");
  channel.publish("", "video_rpc_queue", message);

  timeout = event_new(evbase, -1, EV_TIMEOUT, timeout_cb, nullptr);
  struct timeval future_time;
  future_time.tv_sec = 3;
  future_time.tv_usec = 0;

  event_add(timeout, &future_time);

  ::event_base_dispatch(evbase);
  ::event_base_free(evbase);

  return 0;
}
