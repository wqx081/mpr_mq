#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "protos/epub_info.grpc.pb.h"

#include <glog/logging.h>

#include "base/file.h"
#include "base/file_path.h"
#include "base/file_util.h"

#include "third_party/epubtools/public/info/EpubInfo.h"
#include "third_party/epubtools/public/info/CatalogItem.h"

#include "third_party/rapidjson/include/rapidjson/rapidjson.h"
#include "third_party/rapidjson/include/rapidjson/stringbuffer.h"
#include "third_party/rapidjson/include/rapidjson/writer.h"


using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using epub_info::GetEpubCatalogRequest;
using epub_info::GetEpubCatalogResponse;
//using epub_info::EpubInfo;

class EpubInfoServiceImpl final : public epub_info::EpubInfo::Service {
 public:
  Status GetEpubCatalog(ServerContext* context,
                        const GetEpubCatalogRequest* request,
                        GetEpubCatalogResponse* response) override {
    (void) context;
    int64_t book_id = request->book_id();
    std::string book_path = request->book_path();
    std::string catalog_path = request->catalog_path();

    LOG(INFO) << "book_id: " << book_id << ", book_path: " << book_path;

    base::FilePath epub_path(book_path);
    if (!base::PathExists(epub_path)) {
      return Status(grpc::StatusCode::NOT_FOUND, "Not found the file: " + book_path);
    }
    
    EpubInfo epub_info(epub_path.value().c_str());
    if (!epub_info.IsValid()) {
      return Status(grpc::StatusCode::INTERNAL, "epub library can't not handler file: " + book_path);
    }

    base::FilePath info_path(catalog_path);
    
    auto catalogs = epub_info.GetCatalogs();

    rapidjson::StringBuffer json_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> json_writter(json_buffer);
    
    json_writter.StartObject();
    json_writter.Key("book_catalog");
    json_writter.StartArray();
    for (auto& item : catalogs) {
      json_writter.StartObject();
      json_writter.Key("title");
      json_writter.String(item.title.c_str());

      json_writter.Key("href");
      json_writter.String(item.href.c_str());

      json_writter.Key("level");
      json_writter.String(std::to_string(item.level).c_str());

      json_writter.Key("section_index");
      json_writter.String(std::to_string(item.sectionIndex).c_str());
      json_writter.EndObject();

    }
    json_writter.EndArray();
    json_writter.EndObject();

    LOG(INFO) << "json_string: " << json_buffer.GetString();

    if (base::WriteFile(info_path, json_buffer.GetString(), json_buffer.GetSize()) == -1) {
      return Status(grpc::StatusCode::INTERNAL, "ERROR: Write info to file: " + info_path.value());
    }
    response->set_catalog_path(info_path.value().c_str());
    return Status::OK;
  }
};

void RunServer(const std::string& address, int port) {
  std::string server_address = address + ":" + std::to_string(port);
  EpubInfoServiceImpl epub_info_service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&epub_info_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server listening on " << server_address;

  server->Wait();
}

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;

  RunServer("0.0.0.0", 50051);
  return 0;
}
