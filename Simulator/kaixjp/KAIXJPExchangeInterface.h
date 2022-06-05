#pragma once


#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"
#include "quickfix/Mutex.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix42/OrderCancelReplaceRequest.h"
#include "quickfix/fix42/OrderCancelRequest.h"
#include <unordered_set>
#include "KAIXJPConfig.h"
#include "Order.h"
#include "quickfix/Message.h"
#include <string>


    class KAIXJPExchangeInterface : public FIX::Application, public FIX::MessageCracker
    {
    private:
        bool isValidNewOrder(const FIX42::NewOrderSingle & newOrder);
        bool isValidCancelReplace(const FIX42::OrderCancelReplaceRequest & replace);
        bool isValidCancel(const FIX42::OrderCancelRequest & cancel);
        void processMessagesForIncomingType(Order *order_store ,const std::forward_list<FlowProcessingTypes::SentTypesEnum> &flowMessages,
                                             const FIX::SessionID &sessionID);
        FIX::Message*  getResponseForInValidMessage(FIX42::NewOrderSingle &newOrder);
        FIX::Message*  getResponseForInValidMessage(FIX42::OrderCancelReplaceRequest &amend);
        FIX::Message*  getResponseForInValidMessage(FIX42::OrderCancelRequest &cancel);
        FIX::Message* processOrderAck(Order *store);
        FIX::Message* processOrderReject(Order *store);
        FIX::Message* processAmendAck(Order *store);
        FIX::Message* processAmendReject(Order *store);
        FIX::Message* processCancelAck(Order *store);
        FIX::Message* processCancelReject(Order *store);
        FIX::Message* processFill(Order *store);
        FIX::Message* processFillCorrection(Order *store);
        FIX::Message* processDelay(Order *store);
        FIX::Message* processDoneForDay(Order *store);
        FIX::Message* processUnSolCancel(Order *store);
        FIX::Message* processBustFill(Order *store);
        

    public:
        KAIXJPExchangeInterface(KAIXJPConfig *received_Config) : m_orderID(0), m_execID(0),kaixjpConfig(received_Config) {}

        // Application overloads
        void onCreate(const FIX::SessionID &);
        void onLogon(const FIX::SessionID &sessionID);
        void onLogout(const FIX::SessionID &sessionID);
        void toAdmin(FIX::Message &, const FIX::SessionID &);
        void toApp(FIX::Message &, const FIX::SessionID &) EXCEPT(FIX::DoNotSend);
        void fromAdmin(const FIX::Message &, const FIX::SessionID &) EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
        void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

        void onMessage(const FIX42::NewOrderSingle &, const FIX::SessionID &);
        void onMessage(const FIX42::OrderCancelReplaceRequest &, const FIX::SessionID &);
        void onMessage(const FIX42::OrderCancelRequest &, const FIX::SessionID &);

        std::string genOrderID()
        {
            std::stringstream stream;
            stream << ++m_orderID;
            return stream.str();
        }
        std::string genExecID()
        {
            std::stringstream stream;
            stream << ++m_execID;
            return stream.str();
        }

    private:
        int m_orderID, m_execID;
        std::unordered_map<std::string,Order*> processedClordids ;
        KAIXJPConfig *kaixjpConfig;
        //std::unordered_map<FIX::OrderID,Order*> all_orders;
    };

