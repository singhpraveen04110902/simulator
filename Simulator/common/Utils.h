#pragma once

#include <algorithm> 
#include <cctype>
#include <locale>

#include <chrono>
#include <ctime>
#include <sys/time.h>

static inline std::string getCurrentTime()
{
    char fmt[64];
    char buf[64];
    struct timeval tv;
    struct tm *tm;
    gettimeofday (&tv, NULL);
    tm = localtime (&tv.tv_sec);
    strftime (fmt, sizeof (fmt), "%H:%M:%S:%%06u", tm);
    snprintf (buf, sizeof (buf), fmt, tv.tv_usec);
    return std::string(buf,15);
}

static inline std::string getUTCTime(bool nano = false)
{
    char fmt[64];
    char buf[64];
    struct timeval tv;
    struct tm *tm;
    gettimeofday (&tv, NULL);
    tm = gmtime(&tv.tv_sec);
    strftime (fmt, sizeof (fmt), "%Y%m%d-%H:%M:%S.%%06u", tm);
    snprintf (buf, sizeof (buf), fmt, tv.tv_usec);
    if (!nano)
        return std::string(buf,21);
    else return std::string(buf,24) + "000";
}

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}