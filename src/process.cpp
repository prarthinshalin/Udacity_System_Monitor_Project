#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
    std::string line, number;
    float utime, stime, cutime, cstime, starttime;
    long uptime = LinuxParser::UpTime();

    std::ifstream instream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);
    if(instream.is_open()) {
        std::getline(instream, line);
        std::istringstream cpuline(line);
        for(int i = 0; i < 22; i++) {
            cpuline >> number;

            if(i == 13) { utime = std::stof(number); }
            if(i == 14) { stime = std::stof(number); }
            if(i == 15) { cutime = std::stof(number); }
            if(i == 16) { cstime = std::stof(number); }
            if(i == 21) { starttime = std::stof(number); }
        }

        //Calculation
        float total_time = utime + stime + cutime + cstime;
        float seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
        return (total_time / sysconf(_SC_CLK_TCK)) / seconds;
    }
    return 0.0;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    //Determining precedence based on process memory utilization
    return this->CpuUtilization() > a.CpuUtilization();
}

void Process::Pid(int pid) {
    pid_ = pid;
}