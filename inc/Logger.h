#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <iostream>
#include <fstream>
#include <cstring>

class Logger{
    public:
        Logger(const char* filename = NULL):
            logfile(filename)
        {
            if(logfile)
                outfile.open(filename);
        }
        ~Logger(){
            if(logfile)
                outfile.close();
        }

    public:
        enum EVENT_T{
            EVENT,
            WARN,
            ERR,
        };

        void LogEvent(const char* event, EVENT_T type){
            if (!logfile){
                std::cout << GetEventTypeStr(type) << event << std::endl;
                return;
            }
            outfile << GetEventTypeStr(type) << event << std::endl;
        }

        char* GetBuf(){ return buffer; }

    private:
        inline const char *GetEventTypeStr(EVENT_T type){
            switch(type){
                case EVENT:
                    return "Event: ";
                case WARN:
                    return "Warning: ";
                case ERR:
                    return "Error: ";
                default:
                    break;
            }
            return NULL;
        }

    private:
        char buffer[200];

    public:
        const char*     logfile;
        std::ofstream   outfile;
};

#define LOG_EVENT(str)      logger->LogEvent(str, Logger::EVENT);
#define LOG_WARNING(str)    logger->LogEvent(str, Logger::WARN);
#define LOG_ERROR(str)      logger->LogEvent(str, Logger::ERR);
#define LOG_EVENT_STR(str)  \
    std::memset(logger->GetBuf(), 0x0, 200); \
    sprintf(logger->GetBuf(), str "%s"); \
    logger->LogEvent(logger->GetBuf(), Logger::EVENT);

#define LOG_EVENT_STR2(str, val)  \
    std::memset(logger->GetBuf(), 0x0, 200); \
    sprintf(logger->GetBuf(), str "%d", val); \
    logger->LogEvent(logger->GetBuf(), Logger::EVENT);

#define LOG_EVENT_STR3(str, val, val2)  \
    std::memset(logger->GetBuf(), 0x0, 200); \
    sprintf(logger->GetBuf(), str "%d, %d", val, val2); \
    logger->LogEvent(logger->GetBuf(), Logger::EVENT);
#endif
