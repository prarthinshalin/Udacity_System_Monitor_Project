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

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const {
    std::string line, number;
    float utime, stime, cutime, cstime, starttime;

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
        float seconds = UpTime() - (starttime / sysconf(_SC_CLK_TCK));
        return (total_time / sysconf(_SC_CLK_TCK)) / seconds;
    }
    return 0.0;
}

string Process::Command() {
    std::string trimmed_cmd;
    std::string cmd = LinuxParser::Command(pid_); 
    if(cmd.length() > 50) {
        trimmed_cmd = cmd.substr(0, 49) + "...";
    } else { trimmed_cmd = cmd; }
    return trimmed_cmd;
}

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return (LinuxParser::UpTime() - LinuxParser::UpTime(pid_)); }

bool Process::operator<(Process const& a) const {
    //Determining precedence based on process memory utilization
    return this->CpuUtilization() > a.CpuUtilization();
}

void Process::Pid(int pid) {
    pid_ = pid;
}