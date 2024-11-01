#include "interrupts.h"

int currentPid = 0;
const int NUM_PARTITIONS = 6;
const int MEMORY_SIZES[NUM_PARTITIONS] = {40, 25, 15, 10, 8, 2};
static std::ofstream status("system_status.txt");

struct MemoryPartition
{
    int size;
    std::string status;
};

struct PCB
{
    int pid;
    int cpu;
    int cpuTime;
    int ioTime;
    int partitionNumber;
    std::string programName;
};

struct Program
{
    std::string name;
    int size;
};

std::vector<MemoryPartition> memoryPartitions;
std::vector<PCB> pcbTable;
std::vector<Program> programs;

void initialize_memoryPartitions()
{
    for (int i = 0; i < NUM_PARTITIONS; ++i)
    {
        MemoryPartition partition = {MEMORY_SIZES[i], "free"};
        memoryPartitions.push_back(partition);
    }
}

// Function to map the vector table (chose to hard code it).
std::map<int, std::string> loadVectorTable()
{
    std::map<int, std::string> vectorTable;

    vectorTable[0] = "0X01E3";
    vectorTable[1] = "0X029C";
    vectorTable[2] = "0X0695";
    vectorTable[3] = "0X042B";
    vectorTable[4] = "0X0292";
    vectorTable[5] = "0X048B";
    vectorTable[6] = "0X0639";
    vectorTable[7] = "0X00BD";
    vectorTable[8] = "0X06EF";
    vectorTable[9] = "0X036C";
    vectorTable[10] = "0X07B0";
    vectorTable[11] = "0X01F8";
    vectorTable[12] = "0X03B9";
    vectorTable[13] = "0X06C7";
    vectorTable[14] = "0X0165";
    vectorTable[15] = "0X0584";
    vectorTable[16] = "0X02DF";
    vectorTable[17] = "0X05B3";
    vectorTable[18] = "0X060A";
    vectorTable[19] = "0X0765";
    vectorTable[20] = "0X07B7";
    vectorTable[21] = "0X0523";
    vectorTable[22] = "0X03B7";
    vectorTable[23] = "0X028C";
    vectorTable[24] = "0X05E8";
    vectorTable[25] = "0X05D3";

    return vectorTable;
}

// Function to log events
void logEvent(std::ofstream &output, int time, int duration, const std::string &eventType)
{
    output << time << ", " << duration << ", " << eventType << std::endl;
}

// Function to log system status
void log_system_status(int currentTime) {
    status << "! -----------------------------------------------------------!" << std::endl;
    status << "Save Time: " << currentTime << " ms" << std::endl;
    status << "+--------------------------------------------+" << std::endl;
    status << "| " << std::setw(4) << "PID" 
           << " | " << std::setw(12) << "Program Name"
           << " | " << std::setw(15) << "Partition Number"
           << " | " << std::setw(4) << "size" << " |" << std::endl;
    status << "+--------------------------------------------+" << std::endl;
    PCB pcb;
    for (int i = 0; i < pcbTable.size(); ++i) {
        pcb = pcbTable[i];
        status << "| " << std::setw(4) << pcb.pid
               << " | " << std::setw(12) << pcb.programName
               << " | " << std::setw(15) << pcb.partitionNumber
               << " | " << std::setw(4) << MEMORY_SIZES[pcb.partitionNumber - 1] << " |" << std::endl;
    }

    status << "+--------------------------------------------+" << std::endl;
    status << "!-----------------------------------------------------------!" << std::endl;
}

// Function to process SYSCALL
void processSYSCALL(int eventValue, int duration, std::ofstream &execLogFile, int &currentTime, std::map<int, std::string> vectorTable)
{
    // Since the distribution of the time is meant to be randmly distributed among three steps,
    // we chose to split the time into three.
    int third = duration / 3;     // Split the duration into three parts
    int remainder = duration % 3; // Handle the remainder by adding it to ISR

    int isrDuration = third + remainder;
    int transferDuration = third;
    int errorCheckDuration = third;

    // Simulate the system call and interrupt handling
    // Switch to kernel mode
    logEvent(execLogFile, currentTime, 1, "switch to kernel mode");
    currentTime += 1;

    // Save context
    int contextSaveTime = rand() % 3 + 1; // Random time between 1-3 ms
    logEvent(execLogFile, currentTime, contextSaveTime, "context saved");
    currentTime += contextSaveTime;

    std::string isrAddress = vectorTable[eventValue];

    // Calculate the memory position (multiplr by 2 and convert to hexadecimal)
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << std::hex << (eventValue * 2);
    std::string hexString = ss.str();

    // Find vector and ISR address
    logEvent(execLogFile, currentTime, 1, "find vector " + std::to_string(eventValue) + " in memory position 0X" + hexString);
    currentTime += 1;

    // Load address into PC
    logEvent(execLogFile, currentTime, 1, "load address " + isrAddress + " into the PC");
    currentTime += 1;

    // Run the ISR
    logEvent(execLogFile, currentTime, isrDuration, "SYSCALL: run the ISR");
    currentTime += isrDuration;

    // Transfer the data
    logEvent(execLogFile, currentTime, transferDuration, "transfer data");
    currentTime += transferDuration;

    // Check for errors
    logEvent(execLogFile, currentTime, errorCheckDuration, "check for errors");
    currentTime += errorCheckDuration;

    // Return from ISR operation
    logEvent(execLogFile, currentTime, 1, "IRET");
    currentTime += 1;
}

// Function to process CPU events
void processCPU(int cpuDuration, std::ofstream &execLogFile, int &currentTime)
{
    logEvent(execLogFile, currentTime, cpuDuration, "CPU");
    currentTime += cpuDuration;
}

// Function to process END_IO events
void processEND_IO(int eventValue, int duration, std::ofstream &execLogFile, int &currentTime, std::map<int, std::string> vectorTable)
{
    std::string isrAddress = vectorTable[eventValue];

    // Check priority of the interrupt
    logEvent(execLogFile, currentTime, 1, "check priority of interrupt");
    currentTime += 1;

    // Check if the interrupt is masked
    logEvent(execLogFile, currentTime, 1, "check if masked");
    currentTime += 1;

    // Switch to kernel mode
    logEvent(execLogFile, currentTime, 1, "switch to kernel mode");
    currentTime += 1;

    // Save context
    int contextSaveTime = rand() % 3 + 1; // Random between 1-3 ms
    logEvent(execLogFile, currentTime, contextSaveTime, "context saved");
    currentTime += contextSaveTime;

    // Calculate memory postition (multiply by 2 and convert to hexadecimal)
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << std::hex << (eventValue * 2);
    std::string hexString = ss.str();

    // Find vector and ISR address
    logEvent(execLogFile, currentTime, 1, "find vector " + std::to_string(eventValue) + " in memory position 0X" + hexString);
    currentTime += 1;

    // Load the ISR address into the PC
    logEvent(execLogFile, currentTime, 1, "load address " + isrAddress + " into the PC");
    currentTime += 1;

    // End I/O operation
    logEvent(execLogFile, currentTime, duration, "END_IO");
    currentTime += duration;

    // Return from ISR operation
    logEvent(execLogFile, currentTime, 1, "IRET");
    currentTime += 1;
}

void processFORK(int duration, std::ofstream &execLogFile, int &currentTime, std::map<int, std::string> vectorTable)
{
    PCB child = pcbTable[currentPid];
    child.pid = ++currentPid;
    pcbTable.push_back(child);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << std::hex << (2 * 2);
    std::string hexString = ss.str();

    logEvent(execLogFile, currentTime, 1, "switch to kernel mode");
    currentTime += 1;

    // Save context
    int contextSaveTime = rand() % 3 + 1;
    logEvent(execLogFile, currentTime, contextSaveTime, "context saved");
    currentTime += contextSaveTime;

    logEvent(execLogFile, currentTime, 1, "find vector 2 in memory position 0X" + hexString);
    currentTime += 1;

    std::string isrAddress = vectorTable[2];

    logEvent(execLogFile, currentTime, 1, "load address " + isrAddress + " into the PC");
    currentTime += 1;

    int twos = duration / 2;      // Split the duration into five parts
    int remainder = duration % 2; // Handle the remainder by adding it to ISR

    int forkDuration = twos + remainder;
    int schedulerDuration = twos;
    logEvent(execLogFile, currentTime, forkDuration, "FORK: copy parent PCB to child PCB");
    currentTime += forkDuration;

    log_system_status(currentTime);

    logEvent(execLogFile, currentTime, schedulerDuration, "scheduler called");
    currentTime += schedulerDuration;

    logEvent(execLogFile, currentTime, 1, "IRET");
    currentTime += 1;

    logEvent(execLogFile, currentTime, 1, "switch to kernel mode");
    currentTime += 1;

    // Save context
    contextSaveTime = rand() % 3 + 1;
    logEvent(execLogFile, currentTime, contextSaveTime, "context saved");
    currentTime += contextSaveTime;
}

void processEXEC(int duration, std::ofstream &execLogFile, int &currentTime, std::map<int, std::string> vectorTable, std::string programName)
{
    // Find the program in external files
    int programSize = -1;
    for (int i = 0; i < programs.size(); ++i)
    {
        if (programs[i].name == programName)
        {
            programSize = programs[i].size;
        }
    }

    if (programSize == -1)
    {
        std::cerr << "Program not found in external files: " << programName << std::endl;
        return;
    }

    // Find best-fit partition
    int bestPartition = -1;
    int bestSize = INT_MAX;
    std::string free = "free";
    for (size_t i = 0; i < memoryPartitions.size(); ++i)
    {
        if (memoryPartitions[i].status == free && memoryPartitions[i].size >= programSize)
        {
            if (memoryPartitions[i].size < bestSize)
            {
                bestSize = memoryPartitions[i].size;
                bestPartition = i;
            }
        }
    }

    if (bestPartition == -1)
    {
        std::cerr << "No suitable memory partition found for " << programName << std::endl;
        return;
    }

    // Allocate memory and update PCB
    memoryPartitions[bestPartition].status = programName;
    pcbTable[currentPid].partitionNumber = bestPartition + 1;
    pcbTable[currentPid].programName = programName;

    // Simulation

    // Calculate memory postition (multiply by 2 and convert to hexadecimal)
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << std::hex << (3 * 2);
    std::string hexString = ss.str();

    std::string isrAddress = vectorTable[3];

    // Find vector and ISR address
    logEvent(execLogFile, currentTime, 1, "find vector " + std::to_string(3) + " in memory position 0X" + hexString);
    currentTime += 1;

    // Load the ISR address into the PC
    logEvent(execLogFile, currentTime, 1, "load address " + isrAddress + " into the PC");
    currentTime += 1;
    // Since the distribution of the time is meant to be randmly distributed among five steps,
    // we chose to split the time into five.
    int fifths = duration / 5;    // Split the duration into five parts
    int remainder = duration % 5; // Handle the remainder by adding it to ISR

    int execDuration = fifths + remainder;
    int findPartitionDuration = fifths;
    int markOccupiedDuration = fifths;
    int updatePCBDuration = fifths;
    int callSchedulerDuration = fifths;

    logEvent(execLogFile, currentTime, execDuration, "EXEC: load " + programName + " of size " + std::to_string(programSize) + "MB");
    currentTime += execDuration;

    logEvent(execLogFile, currentTime, findPartitionDuration, "found partition " + std::to_string(bestPartition + 1) + " with " + std::to_string(programSize) + "MB of space.");
    currentTime += findPartitionDuration;

    logEvent(execLogFile, currentTime, markOccupiedDuration, "partition " + std::to_string(bestPartition + 1) + " marked as occupied");
    currentTime += markOccupiedDuration;

    logEvent(execLogFile, currentTime, updatePCBDuration, "updating PCB with new information");
    currentTime += updatePCBDuration;

    log_system_status(currentTime);

    logEvent(execLogFile, currentTime, callSchedulerDuration, "scheduler called");
    currentTime += callSchedulerDuration;

    logEvent(execLogFile, currentTime, 1, "IRET");
    currentTime += 1;

    processTraceFile(programName + ".txt", execLogFile, vectorTable, currentTime);
}

void processExternalFile(const std::string &externalFileName)
{
    std::ifstream externalFile(externalFileName);

    if (!externalFile.is_open())
    {
        std::cerr << "Error opening trace file: " << externalFileName << std::endl;
        return;
    }

    std::string line;

    // Process each line of the trace file
    while (getline(externalFile, line))
    {
        std::stringstream ss(line);
        std::string programName;
        int size;

        // Read program name until comma
        if (getline(ss, programName, ','))
        {
            // Remove any leading/trailing spaces
            programName.erase(programName.find_last_not_of(" \t") + 1);

            // Read the size
            ss >> size;

            // Add the program to the list
            Program program = {programName, size};
            programs.push_back(program);
        }
    }
}

// Function to process a single trace file that is taken in as a command line argument.
void processTraceFile(const std::string &traceFileName, std::ofstream &execLogFile, std::map<int, std::string> &vectorTable, int &currentTime)
{
    std::ifstream traceFile(traceFileName);

    if (!traceFile.is_open())
    {
        std::cerr << "Error opening trace file: " << traceFileName << std::endl;
        return;
    }

    std::string line;

    // Process each line of the trace file
    while (getline(traceFile, line))
    {
        std::stringstream ss(line);
        std::string eventType, eventValue;
        int duration;

        ss >> eventType >> eventValue >> duration;

        // Process the event based on type (CPU, SYSCALL, or END_IO), handle unknown events.
        if (eventType.find("CPU") != std::string::npos)
        {
            processCPU(stoi(eventValue), execLogFile, currentTime);
        }
        else if (eventType.find("SYSCALL") != std::string::npos)
        {
            processSYSCALL(stoi(eventValue), duration, execLogFile, currentTime, vectorTable);
        }
        else if (eventType.find("END_IO") != std::string::npos)
        {
            processEND_IO(stoi(eventValue), duration, execLogFile, currentTime, vectorTable);
        }
        else if (eventType.find("FORK") != std::string::npos)
        {
            processFORK(stoi(eventValue), execLogFile, currentTime, vectorTable);
        }
        else if (eventType.find("EXEC") != std::string::npos)
        {
            eventValue.pop_back();
            processEXEC(duration, execLogFile, currentTime, vectorTable, eventValue);
        }
        else
        {
            std::cerr << "Unknown event type: " << eventType << "\n";
        }
    }

    // Close the files
    traceFile.close();
    execLogFile.close();
}

// Main simulator function to handle multiple trace files
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <trace_file_name>" << std::endl;
        return 1;
    }

    std::string traceFileName = argv[1];
    std::string externalFileName = argv[2];
    PCB pcb = {0, 0, 0, 0, 6, "init"};
    pcbTable.push_back(pcb);
    initialize_memoryPartitions();

    processExternalFile(externalFileName);

    // Extract the trace file number and append it to the execution file name.
    size_t tracePos = traceFileName.find("trace");
    size_t dotPos = traceFileName.find(".txt");
    if (tracePos == std::string::npos || dotPos == std::string::npos)
    {
        std::cerr << "Error: Invalid trace file name format" << std::endl;
        return 1;
    }

    // Extract the number
    std::string number = traceFileName.substr(tracePos + 5, dotPos - (tracePos + 5));

    // Create the execution file name based on the number extracted from the trace file
    std::string execFileName = "execution" + number + ".txt";

    // Load the vector table from helper function.
    std::map<int, std::string> vectorTable = loadVectorTable();

    std::ofstream execLogFile(execFileName);

    if (!execLogFile.is_open())
    {
        std::cerr << "Error opening execution log file: " << execFileName << std::endl;
        return 1;
    }

    // Log in terminal which trace and execution files are being processed.
    std::cout << "Processing " << traceFileName << " -> " << execFileName << std::endl;
    int currentTime = 0;
    log_system_status(currentTime);
    processTraceFile(traceFileName, execLogFile, vectorTable, currentTime);

    status.close();

    return 0;
}