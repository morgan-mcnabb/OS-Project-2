///////////////////////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: Logger.h
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: September 16, 2020
//  Start Date: September 9, 2020
//  Last Updated: September 9, 2020
///////////////////////////////////////////////////////////////

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>

/*
 *  for Logging strings to a file
 */
class Logger
{

public:
    Logger(const std::string& filename="the_forest");
    void log(const std::string& line);
    void log_and_echo(const std::string& line);
    void log_from_sender(const std::string& sender, const std::string& payload);
    void end_log_session();
private:
    std::ofstream m_log_file;
    std::string m_filename;
    std::string get_current_time();
};

#endif
