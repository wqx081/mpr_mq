#ifndef SERVER_SERVER_INTERFACE_H_
#define SERVER_SERVER_INTERFACE_H_

#include <memory>

#include "base/status.h"
#include "base/macros.h"


namespace server {

using base::Status;

class ServerInterface {
 public:
  ServerInterface() {}
  virtual ~ServerInterface() {}

  virtual Status Start() = 0;
  virtual Status Stop() = 0;
  virtual Status Join() = 0;

  virtual const std::string target() const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(ServerInterface);
};

class ServerFactory {
 public:
  virtual Status NewServer(const std::string& server_def,
                           std::unique_ptr<ServerInterface> *out_server) = 0;
  virtual bool AcceptsOptions(const std::string& server_def) = 0;
  virtual ~ServerFactory() {}
  
  static void Register(const std::string& server_type, ServerFactory* factory);
  static Status GetFactory(const std::string& server_def,
                           ServerFactory** out_factory);
};

Status NewServer(const std::string& server_def,
                 std::unique_ptr<ServerInterface>* out_server);

} // namespace server
#endif // SERVER_SERVER_INTERFACE_H_
