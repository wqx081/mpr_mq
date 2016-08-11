#include <iostream>
#include "base/status.h"
#include "server/server_interface.h"
#include "service/amqp_consumer_service.h"

#include <gflags/gflags.h>

namespace server {

} // namespace server

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  std::string server_def = "address='amqp://guest:guest@localhost/';"
                           "queue_name=epub_catgoery;";
  
  std::unique_ptr<server::ServerInterface> server;
  server::NewServer(server_def, &server);
 
  //TODO
  std::shared_ptr<server::AsyncServiceInterface> epub_info_service = 
     std::make_shared<server::AmqpConsumerService>("amqp://fb04/", "hello_queue");
  //std::shared_ptr<server::AsyncServiceInterface> epub_splitter_service = nullptr;
  //std::shared_ptr<server::AsyncServiceInterface> video_process_service = nullptr;
  server->InsertAsyncService(epub_info_service);
  //server->InsertAsyncService(epub_splitter_service);
  //server->InsertAsyncService(video_process_service);

//
  server->Start();
  server->Join();
  return 0;
};
