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

template <typename T>
T findValueByKey(std::string const &filter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream instream(LinuxParser::kProcDirectory + filename);
  if(instream.is_open()) {
    while(std::getline(instream, line)) {
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if(key == filter) {
          instream.close();
          return value;
        }
      }
    }
  } 
  return value;
}

template <typename T>
T getValueFromFile(std::string const &fileStr) {
  std::string line;
  T value;
  std::ifstream instream(fileStr);
  if(instream.is_open()) {
    std::getline(instream, line);
    std::istringstream lineStream(line);
    lineStream >> value;
    instream.close();
    return value;
  }
  return value;
}

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
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

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

float LinuxParser::MemoryUtilization() {
  std::string memTotalFilter = "MemTotal:";
  std::string memFreeFilter = "MemFree:";
  float total = findValueByKey<float>(memTotalFilter, kMeminfoFilename);
  float free = findValueByKey<float>(memFreeFilter, kMeminfoFilename);

  return (total-free)/total;
}

long LinuxParser::UpTime() {
  long uptime = getValueFromFile<long>(kProcDirectory + kUptimeFilename);
  return uptime;
}

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
        instream.close();
        /* Used for debugging purposes only
        for(auto s: info) {
            std::cout << s << "\n";
        }*/
    }
    return info;
}

int LinuxParser::TotalProcesses() {
    std::string filterStr = "processes";
    int processes = findValueByKey<int>(filterStr, kStatFilename);
    return processes;
}

int LinuxParser::RunningProcesses() {
  std::string filterStr = "procs_running";
  int runningProcs = findValueByKey<int>(filterStr, kStatFilename);
  return runningProcs;
}

string LinuxParser::Command(int pid) {
  std::string cmd = getValueFromFile<std::string>(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  return cmd;
}

string LinuxParser::Ram(int pid) {
    int memUtil;
    std::string line, key, value;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream statusline(line);
            statusline >> key >> value;
            if(key == "VmRSS:") {
                memUtil = static_cast<int>(std::round(std::stoi(value)/1024.0));
                instream.close();
                return std::to_string(memUtil);
            }
        }
    }
    return std::string();
}

string LinuxParser::Uid(int pid) {
    std::string line, key, value;
    std::ifstream instream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if(instream.is_open()) {
        while(std::getline(instream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if(key == "Uid:") {
              instream.close();
                return value;
            }
        }
    }
    return value;
}

string LinuxParser::User(int pid) {
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
        instream.close();
        return name;
      }
    }
  }
  return name;
}

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
        instream.close();
    }

    return uptime;
}