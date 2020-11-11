///////////////////////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: Logger.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: September 16, 2020
//  Start Date: September 9, 2020
//  Last Updated: September 9, 2020
///////////////////////////////////////////////////////////////

#include "Logger.h"
#include <chrono>
#include <ctime>
#include <sys/time.h>
#include <string.h>

/*
 *  function: paramaterized constructor for Logger
 *  ----------------------------------------------
 *  Initializes a new Logger with the filename as the
 *  location for logging the information given
 *
 *  filename: name of the file where the logging will be stored
 */
Logger::Logger(const std::string& filename) : m_filename(filename)
{
    // open the file for logging upon initialization
    m_log_file.open(m_filename, std::ios::out | std::ios::app);
    log("LOG FILE OPENED");
}

std::string Logger::get_current_time()
{
    timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int milli = cur_time.tv_usec / 1000;

    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&cur_time.tv_sec));

    char current_time_char[84] = "";
    sprintf(current_time_char, "%s:%03d", buffer, milli);
    std::string current_time(current_time_char);
    return current_time;
}


/*
 *  function: log
 *  -------------
 *  Log the string to the external file
 *
 *  line: the string to log
 *
 *  returns: nothing
 */
void Logger::log(const std::string& line)
{
    auto curr_time = get_current_time();
    m_log_file << curr_time << " -- " << line << std::endl;
}


/*
 *  function: log_and_echo
 *  ----------------------
 *  Log the line given and print it to the screen
 *
 *  line: the string to log and echo
 *
 *  returns: nothing
 */
void Logger::log_and_echo(const std::string& line)
{
    auto curr_time = get_current_time();
    std::cout << line << std::endl;
    m_log_file << curr_time << " -- " << line << std::endl;
}

void Logger::log_from_sender(const std::string& sender, const std::string& payload)
{
    m_log_file << get_current_time() << " -- " << sender << ": " << payload << std::endl;
}

/*
 *  function: end_log_session
 *  -------------------------
 *  ends the logging session by closing the
 *  stream to the file 
 *
 *  returns: nothing
 */
void Logger::end_log_session()
{
    log("LOG FILE CLOSED");
    m_log_file.close();
}

