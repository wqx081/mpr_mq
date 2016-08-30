#include <iostream>
#include "base/status.h"
#include "server/server_interface.h"
#include "service/amqp_consumer_service.h"
#include "service/rpc_epub_info_handler.h"
#include "service/rpc_transcoder_handler.h"

#include <gflags/gflags.h>

namespace server {

} // namespace server

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  std::string server_def = "address='amqp://guest:guest@172.16.2.109/';"
                           "queue_name=epub_catgoery;";
  
  std::unique_ptr<server::ServerInterface> server;
  server::NewServer(server_def, &server);
 
  // Handle Epub Info
  std::shared_ptr<server::AsyncServiceInterface> epub_info_service = 
     std::make_shared<server::AmqpConsumerService>("amqp://guest:guest@172.16.2.109/", 
                                                   "epub_info_queue");
  epub_info_service->SetHandler(new server::RpcEpubInfoServiceHandler("localhost:50051",
   "mysql:host='172.16.2.110';user='root'; password='111111'; database='mpr_cpdb';@pool_size=2"));
  server->InsertAsyncService(epub_info_service);

  // Handle Ts Transcode
  std::shared_ptr<server::AsyncServiceInterface> transcoder_service = 
    std::make_shared<server::AmqpConsumerService>("amqp://guest:guest@172.16.2.109/",
                                                  "video_rpc_queue");
  transcoder_service->SetHandler(new server::RpcTranscoderServiceHandler("localhost:50053",
                                                                         "localhost:50052",
  "mysql:host='172.16.2.110';user='root'; password='111111'; database='mpr_metadb';@pool_size=2"));
  server->InsertAsyncService(transcoder_service);

//
  server->Start();
  server->Join();
  return 0;
};
