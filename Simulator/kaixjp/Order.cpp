
#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#else
#include "config.h"
#endif

#include "Order.h"
#include "KAIXJPExchangeInterface.h"

Order::Order(const FIX42::NewOrderSingle &newOrder, KAIXJPExchangeInterface *received_interface) : interface_(received_interface),
                                                                                                   totalNotional_(0),
                                                                                                   latestExecType_(ExecTypeEnum::OPEN),
                                                                                                   totalExecutedQty_(0)

{

    orderId_ = interface_->genOrderID();
    currentMessage = new FIX42::NewOrderSingle(newOrder);
}

void Order::update_Order_Store(const FIX42::ExecutionReport &execReport)
{
    FIX::ExecType execStatus;
    execReport.get(execStatus);
    switch (execStatus)
    {
    case '0':
        updateOnNewOrderAck(execReport);
        break;
    case '1':
        updateOnFill(execReport);
        break;
    case '2':
        updateOnFill(execReport);
        break;
    case '3':
        updateOnDFD(execReport);
        break;
    case '4':
        updateOnCancelAck(execReport);
        break;
    case '5':
        updateOnReplaceAck(execReport);
        break;
/*     case '6':
        updateOnPendingCancel(execReport);
        break; */
    case '8':
        updateOnOrderReject(execReport);
        break;
/*     case 'E':
        updateOnPendingReplace(execReport);
        break; */
    default:
        break;
    }
}

void Order::update_Order_Store(const FIX42::OrderCancelReplaceRequest &replace_request)
{
    lastPendingMessage = new FIX42::OrderCancelReplaceRequest(replace_request);
    latestExecType_ = ExecTypeEnum::PENDING;
}

void Order::update_Order_Store(const FIX42::OrderCancelRequest &cancel_request)
{
    lastPendingMessage = new FIX42::OrderCancelRequest(cancel_request);
    latestExecType_ = ExecTypeEnum::PENDING;
}

void Order::update_Order_Store(const FIX42::OrderCancelReject &cancelReject)
{
    if (lastPendingMessage)
        delete lastPendingMessage;
    if (latestExecType_ != ExecTypeEnum::CLOSED)
        latestExecType_ = ExecTypeEnum::OPEN;
}


void Order::updateOnNewOrderAck(const FIX42::ExecutionReport &execReport)
{
    if (lastPendingMessage)
        currentMessage = lastPendingMessage;
    lastPendingMessage = nullptr;
    totalExecutedQty_ = 0;
    totalNotional_ = 0;
    latestExecType_ = ExecTypeEnum::OPEN;
}

void Order::updateOnFill(const FIX42::ExecutionReport &execReport)
{
    FIX::ExecTransType execTransType;
    execReport.get(execTransType);
    totalExecutedQty_ = std::stol(execReport.getField(14));
    if (execTransType == 0)
        totalNotional_ += std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
    else if (execTransType == 1)
        totalNotional_ -= std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
    if (totalExecutedQty_ < std::stol(execReport.getField(38))) latestExecType_ = ExecTypeEnum::OPEN;
    else latestExecType_ = ExecTypeEnum::TEMP_CLOSE; 
}

/* void Order::updateOnFullFill(const FIX42::ExecutionReport &execReport)
{
    totalExecutedQty_ == std::stol(execReport.getField(14));
    totalNotional_ += std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
} */
void Order::updateOnDFD(const FIX42::ExecutionReport &execReport)
{
    totalExecutedQty_ += std::stol(execReport.getField(32));
    latestExecType_ = ExecTypeEnum::CLOSED; 
}
void Order::updateOnCancelAck(const FIX42::ExecutionReport &execReport)
{
    lastPendingMessage = nullptr;
    totalExecutedQty_ = std::stol(execReport.getField(14));
    totalNotional_ += std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
    latestExecType_ = ExecTypeEnum::CLOSED; 
}

void Order::updateOnReplaceAck(const FIX42::ExecutionReport &execReport)
{
    if (lastPendingMessage->isSetField(11))
        currentMessage->setField(11, lastPendingMessage->getField(11));
    if (lastPendingMessage->isSetField(18))
        currentMessage->setField(18, lastPendingMessage->getField(18));
    if (lastPendingMessage->isSetField(21))
        currentMessage->setField(21, lastPendingMessage->getField(21));
    if (lastPendingMessage->isSetField(38))
        currentMessage->setField(38, lastPendingMessage->getField(38));
    if (lastPendingMessage->isSetField(40))
        currentMessage->setField(18, lastPendingMessage->getField(40));
    if (lastPendingMessage->isSetField(41))
        currentMessage->setField(41, lastPendingMessage->getField(41));
    if (lastPendingMessage->isSetField(44))
        currentMessage->setField(44, lastPendingMessage->getField(44));
    if (lastPendingMessage->isSetField(110))
        currentMessage->setField(110, lastPendingMessage->getField(110));
    if (lastPendingMessage->isSetField(111))
        currentMessage->setField(41, lastPendingMessage->getField(111));
    if (lastPendingMessage->isSetField(7714))
        currentMessage->setField(41, lastPendingMessage->getField(7714));
    if (lastPendingMessage->isSetField(8174))
        currentMessage->setField(8174, lastPendingMessage->getField(8174));
    totalExecutedQty_ = std::stol(execReport.getField(14));
    totalNotional_ += std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
    lastPendingMessage = nullptr;
    if (totalExecutedQty_ < std::stol(execReport.getField(38))) latestExecType_ = ExecTypeEnum::OPEN;
    else latestExecType_ = ExecTypeEnum::TEMP_CLOSE; 
}

void Order::updateOnOrderReject(const FIX42::ExecutionReport &execReport)
{
    totalExecutedQty_ = std::stol(execReport.getField(14));
    totalNotional_ += std::stol(execReport.getField(32)) * std::stol(execReport.getField(31));
    lastPendingMessage = nullptr;
    latestExecType_ = ExecTypeEnum::CLOSED; 
}

