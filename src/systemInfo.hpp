#ifndef SYSTEM_INFO_HPP
#define SYSTEM_INFO_HPP

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

class SystemFetcher {
public:
    std::vector<std::string> fetch(const json& config);

private:
    std::string getHeader();
    std::string getOS();
    std::string getKernel();
    std::string getCPU();
    std::string getRAM();
    std::string getDisk();
    
    std::string formatBytes(unsigned long long bytes);
    std::string getColorCode(const std::string& colorName);
    std::string resetColor();
};

#endif