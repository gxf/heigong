#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

class Logger{
    public:
        Logger(const char* filename = NULL):
            logfile(filename)
        {
            if(!logfile)
                outfile.open(filename);
        }
        ~Logger(){
            if(!logfile)
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

    public:
        const char*     logfile;
        std::ofstream   outfile;
};

#define LOG_EVENT(str)      logger->LogEvent(str, Logger::EVENT);
#define LOG_WARNING(str)    logger->LogEvent(str, Logger::WARN);
#define LOG_ERROR(str)      logger->LogEvent(str, Logger::ERR);

#endif
