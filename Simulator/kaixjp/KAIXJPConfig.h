#pragma once
#include <string>
#include <forward_list>
#include <unordered_map>
#include <fstream>
#include "Utils.h"
#include "FlowProcessingTypes.h"
#include <iostream>
#include "quickfix/Message.h"
#include "quickfix/Log.h"
#include "Logger.h"


    class KAIXJPConfig
    {
    public:
        long partiall_fill_qty = 0;
        double partiall_fill_price = 0;
        long full_fill_qty = 0;
        double full_fill_price = 0;
        double delay;
        std::unordered_map<FlowProcessingTypes::ReceivedTypesEnum, std::forward_list<FlowProcessingTypes::SentTypesEnum> *> messageOrder;

    public:
        void loadConfig()
        {
            std::fstream input(configFileName, std::fstream::in);
            if (!input.is_open())
            {
                throw "Invalid config File " + configFileName;
            }
            std::string line;
            while (std::getline(input, line))
            {
                std::size_t current = line.find(':', 0);
                if (current == std::string::npos)
                    continue;
                std::string configItem = line.substr(0, current);
                trim(configItem);
                if (configItem == "partiall_fill_qty")
                    partiall_fill_qty = std::stol(trim_copy(line.substr(current + 1)));
                else if (configItem == "partiall_fill_price")
                    partiall_fill_price = std::stod(trim_copy(line.substr(current + 1)));
                else if (configItem == "full_fill_qty")
                    full_fill_qty = std::stol(trim_copy(line.substr(current + 1)));
                else if (configItem == "full_fill_price")
                    full_fill_price = std::stod(trim_copy(line.substr(current + 1)));
                else if (configItem == "delay")
                    delay = std::stod(trim_copy(line.substr(current + 1)));
                else if (configItem == "ORDER" || configItem == "AMEND" || configItem == "CANCEL")
                {
                    std::size_t prev = current + 1;
                    std::forward_list<FlowProcessingTypes::SentTypesEnum> *list = new std::forward_list<FlowProcessingTypes::SentTypesEnum>();
                    FlowProcessingTypes::ReceivedTypesEnum receive_type = FlowProcessingTypes::getReceiveTypeFromString(configItem);
                    while ((current = line.find(',',prev)) && current != std::string::npos)
                    {
                        list->push_front(FlowProcessingTypes::getSentTypeFromString(trim_copy(line.substr(prev, current - prev))));
                        prev = current + 1;
                    }
                    if (prev < line.size() -1)
                    {
                        list->push_front(FlowProcessingTypes::getSentTypeFromString(trim_copy(line.substr(prev))));
                    }
                    if (!list->empty())
                    {
                        list->reverse();
                        messageOrder.insert({receive_type, list});
                    }
                }
            }
        }

    public:
        KAIXJPConfig(const std::string &fileName) : configFileName(fileName)
        { 
            loadConfig(); 
        }

    public:
        std::forward_list<FlowProcessingTypes::SentTypesEnum> &getReverseFlowOrder(const FIX::Message &msg)
        {
            FlowProcessingTypes::ReceivedTypesEnum received_type;
            if (msg.getHeader().getFieldRef(35).getString() == "D")
                received_type = FlowProcessingTypes::ReceivedTypesEnum::ORDER;
            else if (msg.getHeader().getFieldRef(35).getString() == "G")
                received_type = FlowProcessingTypes::ReceivedTypesEnum::AMEND;
            else if (msg.getHeader().getFieldRef(35).getString() == "F")
                received_type = FlowProcessingTypes::ReceivedTypesEnum::CANCEL;
            else
            {
                Logger::LOG_IT("Invalid Receive Type " + msg.getHeader().getFieldRef(35).getString());
            }
            return *(messageOrder[received_type]);
        }
    
     public :
        std::string configFileName ;

    };

