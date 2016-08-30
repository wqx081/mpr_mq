#ifndef SERVICE_RPC_EPUB_INFO_SERVICE_HANDLER_H_
#define SERVICE_RPC_EPUB_INFO_SERVICE_HANDLER_H_
#include "base/macros.h"
#include "base/status.h"

#include "db/frontend/session.h"

#include "service/amqp_consumer_service.h"

#include <memory>
#include <grpc++/grpc++.h>
#include "protos/epub_info.grpc.pb.h"


namespace server {

class RpcEpubInfoServiceHandler : public ServiceHandler {
 public:
  RpcEpubInfoServiceHandler(const std::string& address,
                            const std::string& db_connection);

  virtual ~RpcEpubInfoServiceHandler() {} 

  // From AmqpConsumerServiceHandler
  // @message { "book_id":"NUMBER", "book_path" : "PATH" }
  // @output  nullptr
  //
  // json = dump_json(message);
  // response = RPC_GetEpubCatalog(json_to_proto(json));
  // UpdateDB(response);
  //
  virtual base::Status Handle(const std::string& message, std::string* output) override;

 private:
  const std::string address_; // "host:port"
  std::unique_ptr<epub_info::EpubInfo::Stub> stub_; 
  std::shared_ptr<grpc::Channel> channel_;
  
  const std::string db_connection_info_;

  void Init();
};

} // namespace server
#endif // SERVICE_RPC_EPUB_INFO_SERVICE_H_
