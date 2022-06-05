#pragma once
#include "quickfix/Message.h"
#include <string>
    class AuditItem {
         std::string type;
         FIX::Message message;
         public :
         AuditItem(const std::string recType,const FIX::Message &recMessage) : type(recType) , message(recMessage) {}
    };
