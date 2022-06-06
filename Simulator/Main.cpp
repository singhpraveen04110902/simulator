#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
#else
#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/Log.h"
#include "quickfix/SessionSettings.h"
#include <string>
#include <iostream>
#include <fstream>
#include "KAIXJPExchangeInterface.h"
#include "FlowProcessingTypes.h"
#include "Logger.h"
#include "KAIXJPConfig.h"
#include "FileWatcher.h"
#include <thread>

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << "usage: " << argv[0]
                  << " quickFixConfig  simulatorConfig appLog" << std::endl;
        return 0;
    }

    try
    {
        Logger logger;
        logger.getInstance().initializeLogger(argv[3]);
        std::string quickFixConfig = argv[1];
        std::string applicationConfig = argv[2];
        FIX::Acceptor *acceptor;
        KAIXJPConfig config(applicationConfig);
        Logger::LOG_IT("Initialized application config");
        Logger::LOG_IT("partiall_fill_qty  " + std::to_string(config.partiall_fill_qty));
        Logger::LOG_IT("partiall_fill_qty  " + std::to_string(config.partiall_fill_price));
        Logger::LOG_IT("full_fill_qty  " + std::to_string(config.full_fill_qty));
        Logger::LOG_IT("full_fill_price  " + std::to_string(config.full_fill_price));

        FIX::SessionSettings settings(quickFixConfig);
        KAIXJPExchangeInterface application(&config);
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        std::thread t {FileWatcher(&config)};
        t.detach();

        acceptor = new FIX::SocketAcceptor(application, storeFactory, settings, logFactory);
        acceptor->start();
        while (true)
            FIX::process_sleep(1);
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }
}
