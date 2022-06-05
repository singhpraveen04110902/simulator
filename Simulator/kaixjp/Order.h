#pragma once
#include <list>
#include <string>
#include "AuditItem.h"
#include "quickfix/Message.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelReject.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"
#include "quickfix/fix42/OrderCancelRequest.h"

class KAIXJPExchangeInterface;
class Order
{
  public :
  enum class ExecTypeEnum : char
  {
    OPEN,
    TEMP_CLOSE,
    CLOSED,
    PENDING
  };

private:
  void updateLastStatus();
  void updateOnNewOrderAck(const FIX42::ExecutionReport &execReport);
  void updateOnFill(const FIX42::ExecutionReport &execReport);
  void updateOnDFD(const FIX42::ExecutionReport &execReport);
  void updateOnCancelAck(const FIX42::ExecutionReport &execReport);
  void updateOnReplaceAck(const FIX42::ExecutionReport &execReport);
  void updateOnOrderReject(const FIX42::ExecutionReport &execReport);
  void updateOnPendingReplace(const FIX42::ExecutionReport &execReport);
  void updateOnPendingCancel(const FIX42::ExecutionReport &execReport);

public:
  Order(const FIX42::NewOrderSingle &newOrder, KAIXJPExchangeInterface *received_interface);
  void update_Order_Store(const FIX42::ExecutionReport &execReport);
  void update_Order_Store(const FIX42::OrderCancelReplaceRequest &replace_request);
  void update_Order_Store(const FIX42::OrderCancelRequest &cancel_request);
  void update_Order_Store(const FIX42::OrderCancelReject &cancelReject);

public:
  ExecTypeEnum latestExecType_;
  long totalExecutedQty_;
  double totalNotional_;
  FIX::OrderID orderId_;
  FIX::Message *lastPendingMessage;
  FIX::Message *currentMessage;
  KAIXJPExchangeInterface *interface_;
  std::list<FIX42::ExecutionReport *> indicativeFillQueue;
};
