#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

// Function to load the vector table
std::map<int, std::string> loadVectorTable();

// Function to log events
void logEvent(std::ofstream &output, int time, int duration, const std::string &eventType);

// Function to process SYSCALL
void processSYSCALL(int eventValue, int duration, std::ofstream &execLogFile, int &currentTime, const std::map<int, std::string> vectorTable);

// Function to process CPU event
void processCPU(int eventValue, std::ofstream &execLogFile, int &currentTime);

// Function to process END_IO event
void processEND_IO(int eventValue, int duration, std::ofstream &execLogFile, int &currentTime, const std::map<int, std::string> vectorTable);

// Function to process the trace file
void processTraceFile(const std::string &traceFileName, std::ofstream &execLogFile, std::map<int, std::string> &vectorTable, int &currentTime);

void log_system_status(int currentTime);
#endif // INTERRUPTS_H