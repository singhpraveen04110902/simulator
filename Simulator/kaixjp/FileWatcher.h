#pragma once

#include "KAIXJPConfig.h"
#include <filesystem>
#include <chrono>
#include "quickfix/Utility.h"

#include "Logger.h"
#include <iostream>
#include <thread>

namespace fs = std::filesystem;

class FileWatcher
{
private:
    KAIXJPConfig *config = nullptr;
    std::filesystem::path p;
    std::time_t lastUpdateTime;

private:
    template <typename TP>
    static std::time_t to_time_t(TP tp)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
        return system_clock::to_time_t(sctp);
    }

public:
    FileWatcher(KAIXJPConfig *receivedConfig) : config(receivedConfig), p(receivedConfig->configFileName)
    {
        lastUpdateTime = FileWatcher::to_time_t((std::filesystem::last_write_time(p)));
    }

    void operator()()
    {
        using namespace std::literals::chrono_literals;
        while (true)
        {
            auto latestUpdateTime = FileWatcher::to_time_t(std::filesystem::last_write_time(p));
            if (latestUpdateTime != lastUpdateTime)
            {
                lastUpdateTime = latestUpdateTime;
                config->loadConfig();
                Logger::LOG_IT("Config change detected , Reloaded Config completed");
                std::cout << "Config change detected , Reloaded Config completed"
                          << "\n";
            }
            std::this_thread::sleep_for(1s);
        }
    }
};