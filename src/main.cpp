#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "json.hpp"
#include "animations.cpp"
#include "system_info.hpp"

int main() {

    json config;
    try {
        config = loadJson();
    } catch (const std::exception& e) {
        std::cerr << "Erro config: " << e.what() << std::endl;
        return 1;
    }

    SystemFetcher sysFetcher;
    std::vector<std::string> sysInfo = sysFetcher.fetch(config);

    int screen_w = 80;
    int screen_h = 24;
    
    DonutParams donutP = loadDonutParams(screen_w);

    float A = 0, B = 0;
    bool running = true;

    std::cout << "\033[2J";

    while (running) {

        std::cout << "\033[H"; 

        for (int y = 0; y < screen_h; y++) {
            std::string leftSide = "";
            if (y < sysInfo.size()) {
                leftSide = sysInfo[y];
            }
            
            int padding = 35 - visibleLength(leftSide);
            if (padding < 0) padding = 1;
            
            std::cout << leftSide << std::string(padding, ' ');
            
            std::cout << "\n";
        }

        A += 0.04;
        B += 0.02;

        int speed = config["animation"]["donut"]["speed"];
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }

    return 0;
}