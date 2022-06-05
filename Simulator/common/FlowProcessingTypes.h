#pragma once
#include <string>

    class FlowProcessingTypes
    {
    public:
        enum class ReceivedTypesEnum : char
        {
            ORDER,
            AMEND,
            CANCEL
        };

        enum class SentTypesEnum : char
        {
            ORDER_ACK,
            AMEND_ACK,
            CANCEL_ACK,
            ORDER_REJECT,
            AMEND_REJECT,
            CANCEL_REJECT,
            FILL,
            FILL_CORRECTION,
            BUST_FILL,
            DELAY,
            DONE_FOR_DAY
        };

        static ReceivedTypesEnum getReceiveTypeFromString ( const std::string &receiveType)
        {
            if (receiveType == "ORDER") return ReceivedTypesEnum::ORDER;
            else if (receiveType == "AMEND") return ReceivedTypesEnum::AMEND;
            else if (receiveType == "CANCEL") return ReceivedTypesEnum::CANCEL;
            else throw "Envalid config " +  receiveType ;
        }

        static SentTypesEnum getSentTypeFromString( const std::string &sentType)
        {
            if (sentType == "ORDER_ACK") return SentTypesEnum::ORDER_ACK;
            else  if (sentType == "AMEND_ACK") return SentTypesEnum::AMEND_ACK;
            else  if (sentType == "CANCEL_ACK") return SentTypesEnum::CANCEL_ACK;
            else  if (sentType == "ORDER_REJECT") return SentTypesEnum::ORDER_REJECT;
            else  if (sentType == "AMEND_REJECT") return SentTypesEnum::AMEND_REJECT;
            else  if (sentType == "CANCEL_REJECT") return SentTypesEnum::CANCEL_REJECT;
            else  if (sentType == "FILL") return SentTypesEnum::FILL;
            else  if (sentType == "FILL_CORRECTION") return SentTypesEnum::FILL_CORRECTION;
            else  if (sentType == "BUST_FILL") return SentTypesEnum::BUST_FILL;
            else  if (sentType == "DELAY") return SentTypesEnum::DELAY;
            else if (sentType == "DONE_FOR_DAY") return SentTypesEnum::DONE_FOR_DAY;
            else throw "Invalid sentType " + sentType;
        }
    };

