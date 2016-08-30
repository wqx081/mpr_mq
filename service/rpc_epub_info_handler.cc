#include "base/numbers.h"
#include "base/file_path.h"
#include "service/rpc_epub_info_handler.h"
#include "third_party/rapidjson/include/rapidjson/reader.h"
#include "third_party/rapidjson/include/rapidjson/document.h"

#include "db/frontend/common.h"
#include "db/frontend/result.h"
#include "db/frontend/statement.h"
#include "db/frontend/session.h"

namespace server {

RpcEpubInfoServiceHandler::RpcEpubInfoServiceHandler(const std::string& address,
                                                     const std::string& db_connection_info)
  : address_(address) ,
    db_connection_info_(db_connection_info) {
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
  rapidjson::ParseResult ok = d.Parse(message.c_str());
  if (!ok) {
    return base::Status(base::Code::INVALID_ARGUMENT, "Can't parse json: " + message);
  }

  std::string book_id;
  std::string book_path;

  if (!d.HasMember("book_id")  &&
      !d.HasMember("book_path")) {
    return base::Status(base::Code::INVALID_ARGUMENT, "expect book_id and book_path");
  }

  rapidjson::Value& book_id_val = d["book_id"];
  rapidjson::Value& book_path_val = d["book_path"];
  book_id = book_id_val.GetString();
  book_path = book_path_val.GetString();
  const std::string catalog_path =  base::FilePath(book_path).ReplaceExtension(".info").value();

  // handle rpc
  epub_info::GetEpubCatalogRequest request;
  epub_info::GetEpubCatalogResponse response;

  int64_t book_id_int = 0;
  base::StringAsValue<int64_t>(book_id, &book_id_int);
  request.set_book_id(book_id_int);
  request.set_book_path(book_path);
  request.set_catalog_path(catalog_path);

  grpc::ClientContext context;
  grpc::Status rcp_status = stub_->GetEpubCatalog(&context, request, &response);

  if (rcp_status.ok()) {
    std::string ret_catalog_path = response.catalog_path();
    LOG(INFO) << "-----catalog_path: " << ret_catalog_path; 
    output->assign(ret_catalog_path);
    // persistence::GetInstance().UpdateEpubCatalog(book_id_int, catalog_path);
    try {
      db::Session sql(db_connection_info_);
      db::Statement statement = sql << "UPDATE t_book SET epub_info_dir=? WHERE id=?"
            << ret_catalog_path
            << book_id_int;
      statement.Execute();
      LOG(INFO) << "-----Affected rows " << statement.Affected();
    } catch (...) {
    }

    return base::Status::OK();
  } else {
    LOG(ERROR) << "rpc error: " << rcp_status.error_message();
  }
  
  return base::Status(base::Code::DATA_LOSS, "RCP: book_path " + book_path + " Loss");
}

} // namespace server
