#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#else
#include "config.h"
#endif

#include "KAIXJPExchangeInterface.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix42/OrderCancelReject.h"
#include "Utils.h"
#include <forward_list>

void KAIXJPExchangeInterface::onCreate(const FIX::SessionID &sessionID) {}
void KAIXJPExchangeInterface::onLogon(const FIX::SessionID &sessionID) {}
void KAIXJPExchangeInterface::onLogout(const FIX::SessionID &sessionID) {}
void KAIXJPExchangeInterface::toAdmin(FIX::Message &message,
                                      const FIX::SessionID &sessionID) {}
void KAIXJPExchangeInterface::toApp(FIX::Message &message,
                                    const FIX::SessionID &sessionID)
    EXCEPT(FIX::DoNotSend) {}

void KAIXJPExchangeInterface::fromAdmin(const FIX::Message &message,
                                        const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}

void KAIXJPExchangeInterface::fromApp(const FIX::Message &message,
                                      const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    crack(message, sessionID);
}

bool KAIXJPExchangeInterface::isValidNewOrder(const FIX42::NewOrderSingle &newOrder)
{
    if (!newOrder.isSetField(11))
        return false;
    return true;
}
bool KAIXJPExchangeInterface::isValidCancelReplace(const FIX42::OrderCancelReplaceRequest &replace)
{
    if (!replace.isSetField(11) || !replace.isSetField(41))
        return false;
    return true;
}
bool KAIXJPExchangeInterface::isValidCancel(const FIX42::OrderCancelRequest &cancel)
{
    if (!cancel.isSetField(11) || !cancel.isSetField(41))
        return false;
    return true;
}

void KAIXJPExchangeInterface::onMessage(const FIX42::NewOrderSingle &message, const FIX::SessionID &sessionID)
{

    Order *new_order_store = new Order(message, this);
    FIX42::ExecutionReport execReport;
    if (!isValidNewOrder(message))
    {
        processOrderReject(nullptr);
        try
        {
            FIX::Session::sendToTarget(execReport, sessionID);
        }
        catch (FIX::SessionNotFound &e)
        {
            Logger::LOG_IT("onMessage::NewOrderSingle failed to send Message : " + execReport.toString());
            return;
        }
    }
    else
    {
        FIX::ClOrdID clorderId;
        message.get(clorderId);
        processedClordids.insert({clorderId, new_order_store});
        auto flowMessages = kaixjpConfig->getReverseFlowOrder(message);
        processMessagesForIncomingType(new_order_store, flowMessages, sessionID);
    }
}

void KAIXJPExchangeInterface::onMessage(const FIX42::OrderCancelReplaceRequest &message, const FIX::SessionID &sessionID)
{
    FIX::Message *response_message = nullptr;
    if (!isValidCancelReplace(message))
    {
        response_message = processAmendReject(nullptr);
    }
    else
    {
        FIX::OrigClOrdID origClordid;
        message.get(origClordid);
        auto iterator = processedClordids.find(origClordid);
        if (iterator == processedClordids.end() || iterator->second->latestExecType_ == Order::ExecTypeEnum::CLOSED || iterator->second->latestExecType_ == Order::ExecTypeEnum::TEMP_CLOSE)
            response_message = processAmendReject(nullptr);
        else
        {
            iterator->second->update_Order_Store(message);
            auto flowMessages = kaixjpConfig->getReverseFlowOrder(message);
            processMessagesForIncomingType(iterator->second, flowMessages, sessionID);
        }
    }
    if (response_message)
    {
        try
        {
            FIX::Session::sendToTarget(*response_message, sessionID);
        }
        catch (FIX::SessionNotFound &e)
        {
            Logger::LOG_IT("onMessage::NewOrderSingle failed to send Message : " + response_message->toString());
        }
        delete response_message;
    }
}

void KAIXJPExchangeInterface::onMessage(const FIX42::OrderCancelRequest &message, const FIX::SessionID &sessionID)
{
    FIX::Message *response_message = nullptr;
    if (!isValidCancelReplace(message))
    {
        response_message = processCancelReject(nullptr);
    }
    else
    {
        FIX::OrigClOrdID origClordid;
        message.get(origClordid);
        auto iterator = processedClordids.find(origClordid);
        if (iterator == processedClordids.end() || iterator->second->latestExecType_ == Order::ExecTypeEnum::CLOSED || iterator->second->latestExecType_ == Order::ExecTypeEnum::TEMP_CLOSE)
            response_message = processCancelReject(nullptr);
        else
        {
            iterator->second->update_Order_Store(message);
            auto flowMessages = kaixjpConfig->getReverseFlowOrder(message);
            processMessagesForIncomingType(iterator->second, flowMessages, sessionID);
        }
    }
    if (response_message)
    {
        try
        {
            FIX::Session::sendToTarget(*response_message, sessionID);
        }
        catch (FIX::SessionNotFound &e)
        {
            Logger::LOG_IT("onMessage::NewOrderSingle failed to send Message : " + response_message->toString());
        }
        delete response_message;
    }
}

void KAIXJPExchangeInterface::processMessagesForIncomingType(Order *order_store, const std::forward_list<FlowProcessingTypes::SentTypesEnum> &flowMessages, const FIX::SessionID &sessionID)
{

    using sentType = FlowProcessingTypes::SentTypesEnum;
    FIX::Message *message = nullptr;
    for (auto iterator = flowMessages.cbegin(); iterator != flowMessages.cend(); ++iterator)
    {
        switch (*iterator)
        {
        case sentType::ORDER_ACK:
            message = processOrderAck(order_store);
            break;

        case sentType::ORDER_REJECT:
            message = processOrderReject(order_store);
            break;
        case sentType::AMEND_ACK:
            message = processAmendAck(order_store);
            break;
        case sentType::AMEND_REJECT:
            message = processAmendReject(order_store);
            break;
        case sentType::CANCEL_ACK:
            message = processCancelAck(order_store);
            break;
        case sentType::CANCEL_REJECT:
            message = processCancelReject(order_store);
            break;
        case sentType::FILL:
            message = processFill(order_store);
            break;
        case sentType::FILL_CORRECTION:
            message = processFillCorrection(order_store);
            break;
        case sentType::BUST_FILL:
            message = processBustFill(order_store);
            break;
        case sentType::DELAY:
            processDelay(order_store);
            break;
        case sentType::DONE_FOR_DAY:
            message = processDoneForDay(order_store);
            break;
        }

        if (message)
        {
            try
            {
                FIX::Session::sendToTarget(*message, sessionID);
            }
            catch (FIX::SessionNotFound &e)
            {
                Logger::LOG_IT("onMessage::NewOrderSingle failed to send Message : " + message->toString());
            }
            if (*iterator != sentType::FILL)
                delete message;
        }
        message = nullptr;
    }
}

FIX::Message *KAIXJPExchangeInterface::processOrderAck(Order *store)
{
    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    if (newOrder.isSetField(1))
        executionReport->setField(1, newOrder.getField(1));
    executionReport->setField(6, "0.0");
    if (newOrder.isSetField(11))
        executionReport->setField(11, newOrder.getField(11));
    executionReport->setField(14, "0");
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, "0.0");
    executionReport->setField(32, "0");
    executionReport->setField(37, store->orderId_);
    if (newOrder.isSetField(38))
        executionReport->setField(38, newOrder.getField(38));
    executionReport->setField(39, "0");
    if (newOrder.isSetField(40))
        executionReport->setField(40, newOrder.getField(40));
    if (newOrder.isSetField(44))
        executionReport->setField(44, newOrder.getField(44));
    if (newOrder.isSetField(47))
        executionReport->setField(47, newOrder.getField(47));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    if (newOrder.isSetField(59))
        executionReport->setField(59, newOrder.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(76, "111111");
    executionReport->setField(109, "InternalConnectionID");
    if (newOrder.isSetField(110))
        executionReport->setField(110, newOrder.getField(110));
    if (newOrder.isSetField(111))
        executionReport->setField(111, newOrder.getField(111));
    executionReport->setField(150, "0");
    if (newOrder.isSetField(38))
        executionReport->setField(151, newOrder.getField(38));
    if (newOrder.isSetField(544))
        executionReport->setField(544, newOrder.getField(544));
    if (newOrder.isSetField(1092))
        executionReport->setField(1092, newOrder.getField(1092));
    if (newOrder.isSetField(7714))
        executionReport->setField(7714, newOrder.getField(7714));
    if (newOrder.isSetField(8021))
        executionReport->setField(8021, newOrder.getField(8021));
    if (newOrder.isSetField(8060))
        executionReport->setField(8060, newOrder.getField(8060));
    if (newOrder.isSetField(8174))
        executionReport->setField(8174, newOrder.getField(8174));
    if (newOrder.isSetField(8182))
        executionReport->setField(8182, newOrder.getField(8182));
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processOrderReject(Order *store)
{
    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    if (newOrder.isSetField(1))
        executionReport->setField(1, newOrder.getField(1));
    executionReport->setField(6, "0.0");
    if (newOrder.isSetField(11))
        executionReport->setField(11, newOrder.getField(11));
    else
        executionReport->setField(11, "UNKNOWN");
    executionReport->setField(14, "0");
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, "0.0");
    executionReport->setField(32, "0");
    executionReport->setField(37, store->orderId_);
    if (newOrder.isSetField(38))
        executionReport->setField(38, newOrder.getField(38));
    else
        executionReport->setField(38, "0");
    executionReport->setField(39, "8");
    if (newOrder.isSetField(40))
        executionReport->setField(40, newOrder.getField(40));
    if (newOrder.isSetField(44))
        executionReport->setField(44, newOrder.getField(44));
    if (newOrder.isSetField(47))
        executionReport->setField(47, newOrder.getField(47));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    else
        executionReport->setField(54, "X");
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    if (newOrder.isSetField(59))
        executionReport->setField(59, newOrder.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(76, "111111");
    executionReport->setField(109, "InternalConnectionID");
    if (newOrder.isSetField(110))
        executionReport->setField(110, newOrder.getField(110));
    if (newOrder.isSetField(111))
        executionReport->setField(111, newOrder.getField(111));
    executionReport->setField(150, "8");
    executionReport->setField(151, "0");
    if (newOrder.isSetField(544))
        executionReport->setField(544, newOrder.getField(544));
    if (newOrder.isSetField(1092))
        executionReport->setField(1092, newOrder.getField(1092));
    if (newOrder.isSetField(7714))
        executionReport->setField(7714, newOrder.getField(7714));
    if (newOrder.isSetField(8021))
        executionReport->setField(8021, newOrder.getField(8021));
    if (newOrder.isSetField(8060))
        executionReport->setField(8060, newOrder.getField(8060));
    if (newOrder.isSetField(8174))
        executionReport->setField(8174, newOrder.getField(8174));
    if (newOrder.isSetField(8182))
        executionReport->setField(8182, newOrder.getField(8182));
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processAmendAck(Order *store)
{

    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    auto &amend = *(store->lastPendingMessage);
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    if (newOrder.isSetField(1))
        executionReport->setField(1, newOrder.getField(1));
    auto ordeQty = std::stol(amend.getField(38));
    auto leavesQty = ordeQty - store->totalExecutedQty_;
    double averagepx = 0;
    if (store->totalExecutedQty_)
        averagepx = store->totalNotional_ / store->totalExecutedQty_;
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    executionReport->setField(6, stream.str());
    if (amend.isSetField(11))
        executionReport->setField(11, amend.getField(11));
    executionReport->setField(14, std::to_string(store->totalExecutedQty_));
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, "0.0");
    executionReport->setField(32, "0");
    executionReport->setField(37, store->orderId_);
    if (amend.isSetField(38))
        executionReport->setField(38, amend.getField(38));
    if (store->totalExecutedQty_ && store->totalExecutedQty_ != ordeQty)
        executionReport->setField(39, "1");
    else if (store->totalExecutedQty_ == ordeQty)
        executionReport->setField(39, "2");
    else
        executionReport->setField(39, "0");

    if (amend.isSetField(40))
        executionReport->setField(40, amend.getField(40));
    if (amend.isSetField(41))
        executionReport->setField(41, amend.getField(41));
    if (amend.isSetField(44))
        executionReport->setField(44, amend.getField(44));
    if (amend.isSetField(47))
        executionReport->setField(47, amend.getField(47));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (amend.isSetField(55))
        executionReport->setField(55, amend.getField(55));
    if (amend.isSetField(59))
        executionReport->setField(59, amend.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(76, "111111");
    executionReport->setField(109, "InternalConnectionID");
    if (amend.isSetField(110))
        executionReport->setField(110, amend.getField(110));
    if (amend.isSetField(111))
        executionReport->setField(111, amend.getField(111));
    executionReport->setField(150, "5");
    executionReport->setField(151, std::to_string(leavesQty));
    if (newOrder.isSetField(544))
        executionReport->setField(544, newOrder.getField(544));
    if (newOrder.isSetField(1092))
        executionReport->setField(1092, newOrder.getField(1092));
    if (amend.isSetField(7714))
        executionReport->setField(7714, amend.getField(7714));
    if (newOrder.isSetField(8021))
        executionReport->setField(8021, newOrder.getField(8021));
    if (newOrder.isSetField(8060))
        executionReport->setField(8060, newOrder.getField(8060));
    if (amend.isSetField(8174))
        executionReport->setField(8174, amend.getField(8174));
    if (newOrder.isSetField(8182))
        executionReport->setField(8182, newOrder.getField(8182));

    FIX::ClOrdID clorderId;
    FIX::OrigClOrdID origclordid;
    clorderId = store->lastPendingMessage->getField(11);
    origclordid = store->lastPendingMessage->getField(41);
    processedClordids.insert({clorderId, store});
    processedClordids.erase(origclordid);
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processAmendReject(Order *store)
{
    auto orderQty = 0;
    if (store)
        orderQty = std::stol(store->lastPendingMessage->getField(38));
    FIX42::OrderCancelReject *amendreject = new FIX42::OrderCancelReject();

    if (store && store->lastPendingMessage->isSetField(11))
        amendreject->setField(11, store->lastPendingMessage->getField(11));
    else
        amendreject->setField(11, "UNKNOWN");

    if (store)
        amendreject->setField(37, store->orderId_);
    else
        amendreject->setField(37, "Unknown");

    if (store && store->totalExecutedQty_ != orderQty)
        amendreject->setField(39, "1");
    else if (store && store->totalExecutedQty_)
        amendreject->setField(39, "2");
    else if (store)
        amendreject->setField(39, "0");
    else
        amendreject->setField(39, "8");

    if (store && store->lastPendingMessage->isSetField(41))
        amendreject->setField(41, store->lastPendingMessage->getField(41));
    else
        amendreject->setField(41, "UNKNOWN");

    amendreject->setField(58, "Exchange Reject");
    amendreject->setField(434, "1");
    if (store)
        store->update_Order_Store(*amendreject);
    return amendreject;
}

FIX::Message *KAIXJPExchangeInterface::processCancelAck(Order *store)
{
    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    auto &cancel = *(store->lastPendingMessage);

    std::cout << " Found LastPending as will as current Message \n";
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    double averagepx = 0;
    if (store->totalExecutedQty_)
        averagepx = store->totalNotional_ / store->totalExecutedQty_;
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    executionReport->setField(6, stream.str());
    if (cancel.isSetField(11))
        executionReport->setField(11, cancel.getField(11));
    executionReport->setField(14, std::to_string(store->totalExecutedQty_));
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, "0.0");
    executionReport->setField(32, "0");
    executionReport->setField(37, store->orderId_);
    if (newOrder.isSetField(38))
        executionReport->setField(38, newOrder.getField(38));
    executionReport->setField(39, "4");

    if (newOrder.isSetField(40))
        executionReport->setField(40, newOrder.getField(40));
    if (cancel.isSetField(41))
        executionReport->setField(41, cancel.getField(41));
    if (newOrder.isSetField(44))
        executionReport->setField(44, newOrder.getField(44));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    if (newOrder.isSetField(59))
        executionReport->setField(59, newOrder.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(109, "InternalConnectionID");
    executionReport->setField(150, "4");
    executionReport->setField(151, "0");

    FIX::ClOrdID clorderId;
    FIX::OrigClOrdID origclordid;
    clorderId = store->lastPendingMessage->getField(11);
    origclordid = store->lastPendingMessage->getField(41);
    processedClordids.insert({clorderId, store});
    processedClordids.erase(clorderId);
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processCancelReject(Order *store)
{

    auto orderQty = 0;
    if (store)
        orderQty = std::stol(store->lastPendingMessage->getField(38));
    FIX42::OrderCancelReject *cancelReject = new FIX42::OrderCancelReject();

    if (store && store->lastPendingMessage->isSetField(11))
        cancelReject->setField(11, store->lastPendingMessage->getField(11));
    else
        cancelReject->setField(11, "UNKNOWN");

    if (store)
        cancelReject->setField(37, store->orderId_);
    else
        cancelReject->setField(37, "Unknown");

    if (store && store->totalExecutedQty_ != orderQty)
        cancelReject->setField(39, "1");
    else if (store && store->totalExecutedQty_)
        cancelReject->setField(39, "2");
    else if (store)
        cancelReject->setField(39, "0");
    else
        cancelReject->setField(39, "8");

    if (store && store->lastPendingMessage->isSetField(41))
        cancelReject->setField(41, store->lastPendingMessage->getField(41));
    else
        cancelReject->setField(41, "UNKNOWN");

    cancelReject->setField(58, "Exchange Reject");
    cancelReject->setField(434, "1");
    if (store)
        store->update_Order_Store(*cancelReject);
    return cancelReject;
}

FIX::Message *KAIXJPExchangeInterface::processFill(Order *store)
{
    auto &newOrder = *(store->currentMessage);
    long fill_qty = kaixjpConfig->partiall_fill_qty;
    double fill_price = kaixjpConfig->partiall_fill_price;
    long orderQty = std::stoi(newOrder.getField(38));
    auto leavesQty = orderQty - (store->totalExecutedQty_ + fill_qty);
    double averagepx = 0;
    if (store->totalExecutedQty_ + fill_qty)
        averagepx = (store->totalNotional_ + (fill_qty*fill_price))/ (store->totalExecutedQty_ + fill_qty);
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();

    if (newOrder.isSetField(1))
        executionReport->setField(1, newOrder.getField(1));
    executionReport->setField(6, stream.str());
    if (newOrder.isSetField(11))
        executionReport->setField(11, newOrder.getField(11));
    executionReport->setField(14, std::to_string(store->totalExecutedQty_ + fill_qty));
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, std::to_string(fill_price));
    executionReport->setField(32, std::to_string(fill_qty));
    executionReport->setField(37, store->orderId_);
    if (newOrder.isSetField(38))
        executionReport->setField(38, newOrder.getField(38));
    if (store->totalExecutedQty_ + fill_qty == orderQty)
        executionReport->setField(39, "2");
    else
        executionReport->setField(39, "1");

    if (newOrder.isSetField(40))
        executionReport->setField(40, newOrder.getField(40));
    if (newOrder.isSetField(44))
        executionReport->setField(44, newOrder.getField(44));
    if (newOrder.isSetField(47))
        executionReport->setField(47, newOrder.getField(47));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    if (newOrder.isSetField(59))
        executionReport->setField(59, newOrder.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(109, "InternalConnectionID");
    executionReport->setField(150, executionReport->getField(39));
    executionReport->setField(151, std::to_string(leavesQty));
    executionReport->setField(375, "123456");
    if (newOrder.isSetField(544))
        executionReport->setField(544, newOrder.getField(544));
    executionReport->setField(851, "1");
    executionReport->setField(6226, "Y");
    executionReport->setField(8031, std::to_string(fill_price));
    executionReport->setField(8032, std::to_string(fill_price));
    executionReport->setField(8033, std::to_string(fill_price));
    store->update_Order_Store(*executionReport);
    store->indicativeFillQueue.push_back(executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processFillCorrection(Order *store)
{
    auto &orderStore = *store;
    if (store->indicativeFillQueue.empty())
        return nullptr;
    FIX42::ExecutionReport *execReport = store->indicativeFillQueue.front();
    store->indicativeFillQueue.pop_front();
    execReport->setField(20, "2");
    execReport->setField(6226, "N");
    execReport->setField(60, getUTCTime());
    execReport->setField(19, execReport->getField(17));
    execReport->setField(17, genExecID());
    execReport->setField(8101, genOrderID());
    execReport->setField(8102, genExecID());
    execReport->setField(8106, getUTCTime());
    store->update_Order_Store(*execReport);
    return execReport;
}

FIX::Message *KAIXJPExchangeInterface::processDelay(Order *store)
{
    FIX::process_sleep(kaixjpConfig->delay);
    return nullptr;
}
FIX::Message *KAIXJPExchangeInterface::processDoneForDay(Order *store)
{
    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    long orderQty = std::stoi(newOrder.getField(38));
    double averagepx = 0;
    if (store->totalExecutedQty_)
        averagepx = store->totalNotional_ / store->totalExecutedQty_;
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    executionReport->setField(6, stream.str());
    if (newOrder.isSetField(11))
        executionReport->setField(11, newOrder.getField(11));
    executionReport->setField(14, std::to_string(store->totalExecutedQty_));
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "3");
    executionReport->setField(37, store->orderId_);
    if (newOrder.isSetField(38))
        executionReport->setField(38, newOrder.getField(38));
    executionReport->setField(39, "3");
    if (newOrder.isSetField(47))
        executionReport->setField(47, newOrder.getField(47));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    executionReport->setField(150, "3");
    executionReport->setField(151, "0");
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processUnSolCancel(Order *store)
{
    auto &orderStore = *store;
    auto &newOrder = *(store->currentMessage);
    long orderQty = std::stoi(newOrder.getField(38));
    auto leavesQty = orderQty - store->totalExecutedQty_;
    double averagepx = 0;
    if (store->totalExecutedQty_)
        averagepx = store->totalNotional_ / store->totalExecutedQty_;
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    FIX42::ExecutionReport *executionReport = new FIX42::ExecutionReport();
    executionReport->setField(6, stream.str());
    if (newOrder.isSetField(11))
    {
        executionReport->setField(11, newOrder.getField(11));
        executionReport->setField(11, newOrder.getField(41));
    }
    executionReport->setField(14, std::to_string(store->totalExecutedQty_));
    executionReport->setField(17, genExecID());
    executionReport->setField(20, "0");
    executionReport->setField(31, "0");
    executionReport->setField(32, "0.0");
    executionReport->setField(38, store->orderId_);
    executionReport->setField(39, "4");
    if (newOrder.isSetField(40))
        executionReport->setField(40, newOrder.getField(40));
    if (newOrder.isSetField(44))
        executionReport->setField(44, newOrder.getField(44));
    if (newOrder.isSetField(54))
        executionReport->setField(54, newOrder.getField(54));
    if (newOrder.isSetField(55))
        executionReport->setField(55, newOrder.getField(55));
    executionReport->setField(58, "Unsolicited Cancel");
    if (newOrder.isSetField(59))
        executionReport->setField(59, newOrder.getField(59));
    executionReport->setField(60, getUTCTime());
    executionReport->setField(109, "Internal Conection ID");
    executionReport->setField(150, "4");
    executionReport->setField(151, "0");
    store->update_Order_Store(*executionReport);
    return executionReport;
}

FIX::Message *KAIXJPExchangeInterface::processBustFill(Order *store)
{
    auto &orderStore = *store;
    if (store->indicativeFillQueue.empty())
        return nullptr;

    FIX42::ExecutionReport *execReport = store->indicativeFillQueue.front();
    auto &newOrder = *(store->currentMessage);
    store->indicativeFillQueue.pop_front();
    long lastExecuted = std::stol(execReport->getField(32));
    double lastPrice = std::stod(execReport->getField(31));
    double averagepx = 0;
    long orderQty = std::stol(newOrder.getField(38));

    if (store->totalExecutedQty_ - lastExecuted)
        averagepx = (store->totalNotional_ - lastExecuted * lastPrice) / (store->totalExecutedQty_ - lastExecuted);
    std::stringstream stream;
    stream << std::setprecision(7) << averagepx;
    execReport->setField(6, stream.str());
    execReport->setField(14, std::to_string(store->totalExecutedQty_ - lastExecuted));
    execReport->setField(17, genExecID());
    execReport->setField(20, "1");

    FIX::OrdStatus orderStatus;
    execReport->get(orderStatus);
    if (orderStatus == 1 && !(store->totalExecutedQty_ - lastExecuted))
    {
        execReport->setField(39, "0");
        execReport->setField(150, "0");
    }
    execReport->setField(151, std::to_string(orderQty - store->totalExecutedQty_ - lastExecuted));
    execReport->setField(6226, "N");
    execReport->setField(60, getUTCTime());
    execReport->setField(19, execReport->getField(17));
    execReport->setField(17, genExecID());
    execReport->removeField(8101);
    execReport->removeField(8102);
    execReport->setField(8106, getUTCTime());
    store->update_Order_Store(*execReport);
    // this is because we need to update totalNotional in orderStore
    execReport->setField(31, "0.0");
    execReport->setField(32, "0");
    return execReport;
}
