#ifndef SERVICE_RPC_TRANSCODER_SERVICE_HANDLER_H_
#define SERVICE_RPC_TRANSCODER_SERVICE_HANDLER_H_
#include "base/macros.h"
#include "base/status.h"

#include "db/frontend/session.h"
#include "service/amqp_consumer_service.h"

#include <memory>
#include <grpc++/grpc++.h>
#include "protos/crypto_server.grpc.pb.h"
#include "protos/transcode.grpc.pb.h"


namespace server {


class RpcTranscoderServiceHandler : public ServiceHandler {
 public:
  RpcTranscoderServiceHandler(const std::string& transcoder_service_address,
                              const std::string& crypto_service_address,
                              const std::string& db_connection);

  virtual ~RpcTranscoderServiceHandler() {}

  virtual base::Status Handle(const std::string& message, std::string* output) override;

 private:
  const std::string transcoder_service_address_; // host:port
  std::unique_ptr<transcoder::Transcoder::Stub> transcoder_service_stub_;
  std::shared_ptr<grpc::Channel> transcoder_service_channel_;

  const std::string symmetric_service_address_;
  std::unique_ptr<crypto::SymmetricService::Stub> symmetric_service_stub_;
  std::shared_ptr<grpc::Channel> symmetric_service_channel_;

  const std::string asymmetric_service_address_;
  std::unique_ptr<crypto::AsymmetricService::Stub> asymmetric_service_stub_;
  std::shared_ptr<grpc::Channel> asymmetric_service_channel_;

  const std::string db_connection_info_;
  //db::Session sql_;

  void Init();
};

} // namespace server
#endif //  SERVICE_RPC_TRANSCODER_SERVICE_HANDLER_H_
