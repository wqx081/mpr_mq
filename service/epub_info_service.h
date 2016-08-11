#ifndef SERVICE_EPUB_INFO_SERVICE_H_
#define SERVICE_EPUB_INFO_SERVICE_H_

#include "base/macros.h"
#include "service/amqp_consumer_service.h"

namespace server {

class EpubInfoServiceHandler : public AmqpConsumerServiceHandler {
 public:
  
  // From AmqpConsumerServiceHandler
  // @message { "book_id":"NUMBER", "book_path" : "PATH" }
  // @output  nullptr
  //
  // json = dump_json(message);
  // response = RPC_GetEpubCatalog(json_to_proto(json));
  // UpdateDB(response);
  //
  virtual Status Handle(const std::string& message, std::string* output) override;

 private:
  const std::string address_; // "host:port"
};

} // namespace server
#endif // SERVICE_EPUB_INFO_SERVICE_H_
