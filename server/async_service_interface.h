#ifndef SERVER_ASYNC_SERVICE_INTERFACE_H_
#define SERVER_ASYNC_SERVICE_INTERFACE_H_
namespace server {

class AsyncServiceInterface {
 public:
  virtual ~AsyncServiceInterface() {}

  // Handle loop
  virtual void HandleLoop() = 0;
  virtual void Shutdown() = 0;
};

} // namespace server
#endif // SERVER_AMQP_ASYNC_SERVICE_INTERFACE_H_
