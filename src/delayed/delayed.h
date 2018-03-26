#ifndef RCIB_DELAYED_
#define RCIB_DELAYED_

class DelayedHelper {
public:
  //constructor
  explicit DelayedHelper();
  //static
  static DelayedHelper* GetInstance();
  
  void DelayByMil(rcib::async_req * req);
  void DelayBySec(rcib::async_req * req);
  void DelayByMin(rcib::async_req * req);
  void DelayByHour(rcib::async_req * req);

protected:
  void CommonPross(rcib::async_req * req);
};

#endif
