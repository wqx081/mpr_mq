#include "service/rpc_transcoder_handler.h"

#include "third_party/rapidjson/include/rapidjson/reader.h"
#include "third_party/rapidjson/include/rapidjson/document.h"

#include "base/file_util.h"
#include "base/file_path.h"
#include "base/dir_reader.h"
#include "base/string_util.h"
#include "base/string_encode.h"
#include "base/numbers.h"

#include "db/frontend/common.h"
#include "db/frontend/result.h"
#include "db/frontend/statement.h"
#include "db/frontend/session.h"
#include "db/common/exception.h"

#include <exception>

namespace server {

RpcTranscoderServiceHandler::RpcTranscoderServiceHandler(
        const std::string& transcoder_service_address,
        const std::string& crypto_service_address,
        const std::string& db_connection_info) 
    : transcoder_service_address_(transcoder_service_address),
      symmetric_service_address_(crypto_service_address),
      asymmetric_service_address_(crypto_service_address),
      db_connection_info_(db_connection_info) {
  Init();      
}

void RpcTranscoderServiceHandler::Init() {
  transcoder_service_channel_ = grpc::CreateChannel(transcoder_service_address_,
                                                    grpc::InsecureChannelCredentials());
  DCHECK(transcoder_service_channel_);
  transcoder_service_stub_ = transcoder::Transcoder::NewStub(transcoder_service_channel_);
  DCHECK(transcoder_service_stub_);

  symmetric_service_channel_ = grpc::CreateChannel(symmetric_service_address_,
                                                   grpc::InsecureChannelCredentials());
  asymmetric_service_channel_ = grpc::CreateChannel(asymmetric_service_address_,
                                                    grpc::InsecureChannelCredentials());
  DCHECK(symmetric_service_channel_);
  DCHECK(asymmetric_service_channel_);

  symmetric_service_stub_ = crypto::SymmetricService::NewStub(symmetric_service_channel_);
  DCHECK(symmetric_service_stub_);

  asymmetric_service_stub_ = crypto::AsymmetricService::NewStub(asymmetric_service_channel_);
  DCHECK(asymmetric_service_stub_);
}
    
static void ReplaceAll(std::string& source, 
                       const std::string& search, 
                       const std::string replace) {
  for (size_t pos = 0; ;pos += replace.length()) {
    pos = source.find(search, pos);  
    if (pos == std::string::npos) {
      return;
    }
    source.erase(pos, search.length());
    source.insert(pos, replace);
  }
}

static std::string MakeExtM3u8KeyPath(const std::string& http_key_path) {
  return std::string("METHOD=AES-128,URI=") + "\"" + http_key_path + "\"";
}

base::Status RpcTranscoderServiceHandler::Handle(const std::string& message,
                                                 std::string* output) {
  LOG(INFO) << "message: " << message;
  // handle Json
  rapidjson::Document d;
  rapidjson::ParseResult ok = d.Parse(message.c_str()); 
  if (!ok) {
    return base::Status(base::Code::INVALID_ARGUMENT, "Can't parse json: " + message);
  }
  //TODO
  std::string video_source_path = d["video_source_path"].GetString();
  std::string video_target_path = d["video_target_path"].GetString();
  std::string target_id = d["target_id"].GetString();
  std::string sample = d["sample"].GetString();
  std::string frame_size = d["frame_size"].GetString();
  std::string frame_aspect = d["frame_aspect"].GetString();
  std::string frame_rate = d["frame_rate"].GetString();
  std::string rate_bit = d["rate_bit"].GetString();
  std::string time = d["time"].GetString();
  std::string url_prefix = d["url_prefix"].GetString();
  std::string m3u8_name = d["m3u8_name"].GetString();

  std::string out("hello, world");
  output->resize(out.size());
  output->swap(out);

  grpc::Status rpc_status;

  //TODO
  // Check file path
  
  // Generate CBC key
  grpc::ClientContext context;
  crypto::CreateSymmetricKeyRequest cbc_key_request;
  crypto::CreateSymmetricKeyResponse cbc_key_response;
  cbc_key_request.set_key_bits(crypto::SymmetricKey128Bits);
  rpc_status = symmetric_service_stub_->CreateSymmetricKey(&context,
                                                     cbc_key_request,
                                                     &cbc_key_response);
  if (!rpc_status.ok()) {
    return base::Status(base::Code::INTERNAL, "rpc generate cbc key error");
  }
  LOG(INFO) << "cbc_key: " << cbc_key_response.key();

  // Generate Sm2 key pair
  grpc::ClientContext sm2_key_context;
  crypto::CreateKeyPairRequest sm2_key_request;
  crypto::CreateKeyPairResponse sm2_key_response;
  sm2_key_request.set_type(crypto::SM2);
  sm2_key_request.set_key_bits(crypto::KEY256BITS);
  rpc_status = asymmetric_service_stub_->CreateKeyPair(&sm2_key_context,
                                                       sm2_key_request,
                                                       &sm2_key_response);
  if (!rpc_status.ok()) {
    return base::Status(base::Code::INTERNAL, "rpc generate sm2 key error");
  }
  LOG(INFO) << "public_key: " << sm2_key_response.public_key();
  LOG(INFO) << "private_key: " << sm2_key_response.private_key();

  // Transcode And Segment
  grpc::ClientContext transcode_context;
  transcoder::TranscodeRequest transcode_request;
  transcoder::TranscodeResponse transcode_response;
  transcoder::AudioData* audio_data = new transcoder::AudioData();
  transcoder::VideoData* video_data = new transcoder::VideoData();
  transcoder::SegmentData* segment_data = new transcoder::SegmentData();

  transcode_request.set_media_source_path(video_source_path);
  transcode_request.set_media_target_path(video_target_path);

  audio_data->set_sample(sample);

  video_data->set_frame_size(frame_size);
  video_data->set_frame_aspect(frame_aspect);
  video_data->set_frame_rate(frame_rate);
  video_data->set_rate_bit(rate_bit);

  segment_data->set_time(time);
  segment_data->set_url_prefix(url_prefix);
  segment_data->set_m3u8_name(video_target_path + m3u8_name);

  transcode_request.set_allocated_audio_data(audio_data);
  transcode_request.set_allocated_video_data(video_data);
  transcode_request.set_allocated_segment_data(segment_data);

  std::unique_ptr<grpc::ClientReader<transcoder::TranscodeResponse>> reader(
          transcoder_service_stub_->Transcode(&transcode_context,
                                              transcode_request));
  while (reader->Read(&transcode_response)) {
    int64_t duration=0;
    int64_t out_time=0;
    DCHECK(base::safe_strto64(transcode_response.duration(), &duration));
    DCHECK(base::safe_strto64(transcode_response.out_time(), &out_time));
    //LOG(INFO) << "transcode Done: " << duration / out_time;
    LOG(INFO) << "duration: " << transcode_response.duration()
      << ", out_time: " << transcode_response.out_time();
  }
  rpc_status = reader->Finish();
  if (!rpc_status.ok()) {
    return base::Status(base::Code::INTERNAL, "trancode and segment error");
  }
  LOG(INFO) << "Transcode: " << video_source_path << "  ---Done";
  // Encrypt ts Media 
  //
  base::FilePath orig_path(video_target_path);
  base::FilePath enc_path = orig_path.DirName().Append("enc");
  if (base::DirectoryExists(enc_path)) {
    DCHECK(base::DeleteFile(enc_path, true));
  }
  DCHECK(base::CreateDirectory(enc_path));
  DCHECK(base::SetPosixFilePermissions(enc_path, base::FILE_PERMISSION_MASK));

  base::DirReader dir_reader(orig_path.value().c_str());
  while (dir_reader.Next()) {
    std::string name = dir_reader.name();
    std::string ext = base::FilePath(name).Extension();
    if (name != ".." && name != "." && base::CompareCaseInsensitiveASCII(ext, ".ts") == 0) {
      grpc::ClientContext cbc_enc_context;
      crypto::CbcEncryptFileRequest cbc_enc_request;
      crypto::CbcEncryptFileResponse cbc_enc_response;
      cbc_enc_request.set_file_source_path(orig_path.Append(name).value());
      cbc_enc_request.set_file_target_path(enc_path.Append(name).value());
      cbc_enc_request.set_key(cbc_key_response.key());
      rpc_status = symmetric_service_stub_->CbcEncryptFile(&cbc_enc_context,
                                                           cbc_enc_request,
                                                           &cbc_enc_response);
      if (!rpc_status.ok()) {
        DCHECK(base::DeleteFile(enc_path, true)); // clean up
        LOG(ERROR) << "encrypt_file: " << orig_path.Append(name).value();
        return base::Status(base::Code::INTERNAL,
                            "encrypt_file error");
      } else {
        LOG(INFO) << "Encrypt file: " << cbc_enc_request.file_source_path() << " Ok";
      }
    }
  }
  LOG(INFO) << "------- Encrypt TS DONE";
  // Encrypt MP4 media
  base::FilePath video_source_fp(video_source_path);
  std::string full_video_target = enc_path.Append(video_source_fp.BaseName().value()).value();

  grpc::ClientContext ecb_enc_context;
  crypto::EcbEncryptFileRequest ecb_enc_request;
  crypto::EcbEncryptFileResponse ecb_enc_response;
  ecb_enc_request.set_key(cbc_key_response.key());
  ecb_enc_request.set_file_source_path(video_source_path);
  ecb_enc_request.set_file_target_path(full_video_target);

  rpc_status = symmetric_service_stub_->EcbEncryptFile(&ecb_enc_context,
                                                       ecb_enc_request,
                                                       &ecb_enc_response);
  if (!rpc_status.ok()) {
    DCHECK(base::DeleteFile(enc_path, true)); // clean up
    LOG(ERROR) << "ECB encrypt_file: " << full_video_target;
    return base::Status(base::Code::INTERNAL, "ECB encrypt_file error");
  }
  LOG(INFO) << "------- Encrypt: " << full_video_target << " DONE";

  // Handle M3U8
  base::FilePath orig_m3u8_path = base::FilePath(orig_path.Append(m3u8_name));
  base::FilePath video_key_path = enc_path.Append("video.key");
  std::string cbc_key_hex = base::HexDecode(cbc_key_response.key());
  DCHECK(base::WriteFile(video_key_path, cbc_key_hex.data(), cbc_key_hex.size())); 
  LOG(INFO) << "------ video.key path: " << video_key_path.value();

  // Handle Sm2 
  base::FilePath orig_url_path = base::FilePath(url_prefix);
  base::FilePath enc_url_path = orig_url_path.DirName().Append("enc").Append("video.key");
  grpc::ClientContext sm2_enc_context;
  crypto::PublicKeyEncryptRequest public_key_enc_request;
  crypto::PublicKeyEncryptResponse public_key_enc_response;
  public_key_enc_request.set_type(crypto::SM2);
  public_key_enc_request.set_public_key(sm2_key_response.public_key());
  public_key_enc_request.set_plaintext(MakeExtM3u8KeyPath(enc_url_path.value()));

  rpc_status = asymmetric_service_stub_->PublicKeyEncrypt(&sm2_enc_context,
                                                          public_key_enc_request,
                                                          &public_key_enc_response);
  if (!rpc_status.ok()) {
    LOG(ERROR) << "sm2 encrypt video.key path error";
    return base::Status(base::Code::INTERNAL,
                            "encrypt video key path error");
  }
  LOG(INFO) << "Encrypt: " << enc_url_path.value() << " -- OK";
  LOG(INFO) << "Encrypted video key path: " << public_key_enc_response.cipher();

  // Handle m3u8 content
  std::string m3u8_content;
  base::FilePath enc_m3u8_path = base::FilePath(enc_path.Append(m3u8_name));
  DCHECK(base::ReadFileToString(orig_m3u8_path, &m3u8_content));
  ReplaceAll(m3u8_content, "/orig/", "/enc/");
  ReplaceAll(m3u8_content, "#EXTM3U\n", std::string("#EXTM3U\n") + "#EXT-X-KEY:" +
                                             public_key_enc_response.cipher() + "\n");
  DCHECK(base::WriteFile(enc_m3u8_path, m3u8_content.data(), m3u8_content.size()));
  //std::string new
  // Update DB
  // mpr_metadb.t_isli_target
  // m3u8_key   => sm2_private_key
  // m3u8_path  =>                       /data/.....
  // video_key  => cbc_key
  // status => -1 
  // encrypted_target_content_path TODO  /data/.... 

  int64_t target_id_int = 0;
  base::StringAsValue<int64_t>(target_id, &target_id_int);
  try {
    db::Session sql(db_connection_info_);
    db::Statement statement = sql << "UPDATE "
    " t_isli_target SET status=-1 , m3u8_key=?,m3u8_path=?,video_key=?,encrypted_target_content_path=?"
           " WHERE id=? "
       << sm2_key_response.private_key()
       << enc_m3u8_path.value()
       << cbc_key_response.key()
       << full_video_target
       << target_id_int;
    statement.Execute();
  } catch (const db::DBException& e) {
    LOG(ERROR) << "Mysql Update Error: " << e.what();
  }

  return base::Status::OK();
}


} // namespace server

