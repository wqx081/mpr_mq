#ifndef SERVER_ASYNC_SERVICE_INTERFACE_H_
#define SERVER_ASYNC_SERVICE_INTERFACE_H_
#include "base/status.h"

#include <string>

namespace server {

class ServiceHandler {
 public:
  virtual ~ServiceHandler() {}
  virtual base::Status Handle(const std::string& message,
                              std::string* reply) = 0;
};

class AsyncServiceInterface {
 public:
  virtual ~AsyncServiceInterface() {}

  // Handle loop
  virtual void HandleLoop() = 0;
  virtual void Shutdown() = 0;
  virtual void SetHandler(ServiceHandler* service_handler) = 0;
};

} // namespace server
#endif // SERVER_AMQP_ASYNC_SERVICE_INTERFACE_H_
