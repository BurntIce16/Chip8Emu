#include "logger.h"

/**
 * Logging class - Pretty self explanitory
 * Logs in jsonl format (because im lame and like that format)
 * Will be updated as I go for certain
 */

Logger::Logger(const char *filename)
{
    try
    {
        logFile.open(filename);
    }catch(int e){
        std::cerr << "Cant open log file" << std::endl;
    }
}

bool Logger::closeLog()
{
    try
    {
        logFile.close();
        return true;
    }
    catch (int e)
    {
        std::cerr << "Log file not open" << std::endl;
    }
    return false;
}


/**
 * Try to write sting to file
 * 
 * Format is: <timestamp> <string>
 * 
 * Returns:
 * true on success
 * false on error
 */
bool Logger::writeLog(const char *string)
{
    try
    {
        if (logFile.is_open())
        {
            
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            //std::time_t currentTime = std::time(nullptr);
            logFile << "{\"time\": \"" << std::to_string(ms.count()) << "\", " << string << "}" << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Log file not open" << std::endl;
            return false;
        }
    }
    catch (int e)
    {
        std::cerr << "Error writing to log file" << std::endl;
        return false;
    }
}