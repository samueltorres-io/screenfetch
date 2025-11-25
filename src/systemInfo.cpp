#include "systemInfo.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

/* Kernel and OS */
#include <sys/utsname.h>

/* RAM and Uptime */
#include <sys/sysinfo.h>

/* Disk */
#include <sys/statvfs.h>
#include <cmath>
#include <map>

std::vector<std::string> SystemFetcher::fetch(const json& config) {
    std::vector<std::string> infoLines;
    
    std::string color = getColorCode(config["theme"]["foreground"]);
    std::string reset = resetColor();

    infoLines.push_back(color + getHeader() + reset);
    infoLines.push_back(color + "----------------" + reset);

    infoLines.push_back(color + "OS: " + reset + getOS());
    infoLines.push_back(color + "Kernel: " + reset + getKernel());

    if (config["display"].value("cpu", true)) {
        infoLines.push_back(color + "CPU: " + reset + getCPU());
    }

    if (config["display"].value("ram", true)) {
        infoLines.push_back(color + "RAM: " + reset + getRAM());
    }

    if (config["display"].value("disk", true)) {
        infoLines.push_back(color + "Disk (/): " + reset + getDisk());
    }

    return infoLines;
}

std::string SystemFetcher::getHeader() {
    char hostname[1024];
    gethostname(hostname, 1024);
    std::string user = std::getenv("USER");
    return user + "@" + std::string(hostname);
}

std::string SystemFetcher::getOS() {
    std::ifstream file("/etc/os-release");
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.rfind("PRETTY_NAME=", 0) == 0) {
                size_t first = line.find('"');
                size_t last = line.rfind('"');
                if (first != std::string::npos && last != std::string::npos) {
                    return line.substr(first + 1, last - first - 1);
                }
            }
        }
    }
    return "Linux";
}

std::string SystemFetcher::getKernel() {
    struct utsname buffer;
    if (uname(&buffer) != 0) return "Unknown";
    return std::string(buffer.release);
}

std::string SystemFetcher::getCPU() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.rfind("model name", 0) == 0) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string cpu = line.substr(colon + 2); 
                    return cpu;
                }
            }
        }
    }
    return "Unknown CPU";
}

std::string SystemFetcher::getRAM() {
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    
    long totalPhysMem = memInfo.totalram;
    totalPhysMem *= memInfo.mem_unit;
    
    long physMemUsed = memInfo.totalram - memInfo.freeram;
    physMemUsed -= memInfo.bufferram; 
    physMemUsed *= memInfo.mem_unit;

    return formatBytes(physMemUsed) + " / " + formatBytes(totalPhysMem);
}

std::string SystemFetcher::getDisk() {
    struct statvfs buffer;
    if (statvfs("/", &buffer) != 0) return "Err";

    unsigned long long total = buffer.f_blocks * buffer.f_frsize;
    unsigned long long available = buffer.f_bavail * buffer.f_frsize;
    unsigned long long used = total - available;

    return formatBytes(used) + " / " + formatBytes(total);
}

std::string SystemFetcher::formatBytes(unsigned long long bytes) {
    const char* suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    int s = 0;
    double count = bytes;

    while (count >= 1024 && s < 4) {
        s++;
        count /= 1024;
    }

    char buf[20];
    sprintf(buf, "%.2f %s", count, suffixes[s]);
    return std::string(buf);
}

std::string SystemFetcher::getColorCode(const std::string& colorName) {
    std::map<std::string, std::string> colors = {
        {"black", "\033[30m"}, {"red", "\033[31m"},
        {"green", "\033[32m"}, {"yellow", "\033[33m"},
        {"blue", "\033[34m"}, {"purple", "\033[35m"},
        {"cyan", "\033[36m"}, {"white", "\033[37m"}
    };

    if (colors.find(colorName) != colors.end()) {
        return colors[colorName];
    }
    return "\033[37m";
}

std::string SystemFetcher::resetColor() {
    return "\033[0m";
}