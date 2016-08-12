#include "base/numbers.h"
#include "service/rpc_epub_info_handler.h"
#include "third_party/rapidjson/include/rapidjson/reader.h"
#include "third_party/rapidjson/include/rapidjson/document.h"

namespace server {

RpcEpubInfoServiceHandler::RpcEpubInfoServiceHandler(const std::string& address)
  : address_(address) {
  Init();
}

void RpcEpubInfoServiceHandler::Init() {
  channel_ = grpc::CreateChannel(address_, grpc::InsecureChannelCredentials());  
  CHECK(channel_);
  stub_ = epub_info::EpubInfo::NewStub(channel_);
  CHECK(stub_);
}

base::Status
RpcEpubInfoServiceHandler::Handle(const std::string& message, std::string* output) {
  // handle json
  rapidjson::Document d;
  d.Parse(message.c_str());
  
  std::string book_id;
  std::string book_path;
  rapidjson::Value& book_id_val = d["book_id"];
  rapidjson::Value& book_path_val = d["book_path"];
  book_id = book_id_val.GetString();
  book_path = book_path_val.GetString();
  
  // handle rpc
  epub_info::GetEpubCatalogRequest request;
  epub_info::GetEpubCatalogResponse response;

  int64_t book_id_int = 0;
  base::StringAsValue<int64_t>(book_id, &book_id_int);
  request.set_book_id(book_id_int);
  request.set_book_path(book_path);

  grpc::ClientContext context;
  grpc::Status rcp_status = stub_->GetEpubCatalog(&context, request, &response);

  if (rcp_status.ok()) {
    std::string catalog_path = response.catalog_path();
    LOG(INFO) << "-----catalog_path: " << catalog_path; 
    output->assign(catalog_path);
    // persistence::GetInstance().UpdateEpubCatalog(book_id_int, catalog_path);
    return base::Status::OK();
  } else {
    LOG(ERROR) << "rpc error: " << rcp_status.error_message();
  }
  
  return base::Status(base::Code::DATA_LOSS, "RCP: book_path " + book_path + " Loss");
}

} // namespace server
