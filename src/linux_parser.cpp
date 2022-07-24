#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    float t, f;
    std::string totalLine, freeLine, k1, k2, total, free;
    std::ifstream instream(kProcDirectory + kMeminfoFilename);

    if(instream.is_open()) {
        //Getting the total memory utilization
        std::getline(instream, totalLine);
        std::istringstream totalStream(totalLine);
        totalStream >> k1 >> total;
        t = std::stof(total);
        //std::cout << t << "\n";

        //Getting the memory not utilizied
        std::getline(instream, freeLine);
        std::istringstream freeStream(freeLine);
        freeStream >> k2 >> free;
        f = std::stof(free);
        //std::cout << f << "\n";

        return (t-f)/t;
    }
    return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
    std::string line, ut;
    std::ifstream instream(kProcDirectory + kUptimeFilename);

    if(instream.is_open()) {
        std::getline(instream, line);
        std::istringstream lineStream(line);
        lineStream >> ut;
        return std::stof(ut);
    }

    return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    std::vector<std::string> info;
    std::string line, key, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice, number;
    std::ifstream instream(kProcDirectory + kStatFilename);
    if(instream.is_open()) {
        std::getline(instream, line);
        std::istringstream lineStream(line);
        for(int i = 0; i < 10; i++) {
            if(i == 0) {
                lineStream >> key >> number;
            } else {
                lineStream >> number;
            }
            info.emplace_back(number);
        }

        /* Used for debugging purposes only
        for(auto s: info) {
            std::cout << s << "\n";
        }*/
    }
    return info;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line, key, value;
    std::ifstream instream(kProcDirectory + kStatFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream lineStream(line);
            lineStream >> key >> value;
            if(key == "processes") {
                return std::stoi(value);
            }
        }
    }
    return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    std::string line, key, value;
    std::ifstream instream(kProcDirectory + kStatFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream lineStream(line);
            lineStream >> key >> value;
            if(key == "procs_running") {
                return std::stoi(value);
            }
        }
    }
    return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
    std::string cmdline;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    if(instream.is_open()) {
        std::getline(instream, cmdline);
        return cmdline;
    }
    return cmdline;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) {
    int memUtil;
    std::string line, key, value;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream statusline(line);
            statusline >> key >> value;
            if(key == "VmSize:") {
                memUtil = static_cast<int>(std::round(std::stoi(value)/1000.0));
                return std::to_string(memUtil);
            }
        }
    }
    return std::string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) {
    std::string line, key, value;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if(key == "Uid:") {
                return value;
            }
        }
    }
    return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) {
  std::string uid, line, name, x, userid;

  //Step 1: Retrieve User ID
  uid = LinuxParser::Uid(pid);

  //Step 2: Retrieve the user name utilizing the user id
  std::ifstream instream(kPasswordPath);
  if(instream.is_open()) {
    while(std::getline(instream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> userid;
      if(userid == uid) {
        return name;
      }
    }
  }
  return name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    long uptime = 0;
    std::string line, utstring;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kStatFilename);

    if(instream.is_open()) {
        std::getline(instream, line);
        std::istringstream linestream(line);
        for (int i = 0; i < 22; i++)
        {
            linestream >> utstring;
        }
        uptime = std::stol(utstring);
        uptime /= sysconf(_SC_CLK_TCK);
    }

    return uptime;
}