#pragma once
#include <fstream>
#include "Utils.h"

class Logger {
   inline static std::fstream outPut;
   bool initialized = false;

   public : 
   Logger& getInstance()
   {
       static Logger instance;
       return instance;
   }

    void initializeLogger(const std::string &fileName)
   {
       if (initialized) return;
       Logger::outPut.open(fileName,std::fstream::out | std::fstream::trunc);
   }

   static void LOG_IT(const std::string log)
   {
       Logger::outPut << getCurrentTime() << "  " << log  << "\n";
   }
};