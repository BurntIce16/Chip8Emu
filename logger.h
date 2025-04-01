#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <chrono>


class Logger{
    public:
        // Opens/creates the log file 
        Logger(const char *filename);


        // tries to close the log file
        bool closeLog();


        bool writeLog(const char *string);



    private:

        const char *filename;

        std::ofstream logFile; //write only file

        bool fileOpen;


};

#endif // LOGGER_H
