# animations.cpp

/*
 * Functions to generate ASCIIs Real-Time Animations 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <INIReader.h>
#include "json.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using json = nlohmann::json;

/* Automatic file location */
std::string getConfPath() {
    if (std::filesystem::exists("config/parameters.conf")) {
        return "config/parameters.conf";
    }

    if (std::filesystem::exists("/usr/share/screenfetch/parameters.conf")) {
        return "/usr/share/screenfetch/parameters.conf";
    }

    throw std::runtime_error("Cannot find parameters.conf");
}

std::string getJsonPath() {

    std::string home = std::getenv("HOME");

    std::string local = home + "/.local/share/screenFetch/config.json";
    if (std::filesystem::exists(local))
        return local;

    if (std::filesystem::exists("config/config.json"))
        return "config/config.json";

    throw std::runtime_error("Cannot get config.json");
}

/* Automatically read JSON */
json loadJson() {
    std::string path = getJsonPath();
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open JSON: " + path);
    }

    json data;
    file >> data;

    return data;
}

// int screen_width = reader.GetInt("screen_width" || 80);

/* --- Donut animation settings --- */

struct DonuParams {
    float theta_spacing;
    float phi_spacing;
    float R1;
    float R2;
    float K2;
    float K1;
};

DonuParams loadDonutParams(int screen_width) {

    std::string path = getConfPath();
    INIReader reader(path);

    if (reader.ParseError() < 0) {
        std::cerr << "Err" << path << "/n";
        exit(1);
    }

    DonutParams p;

    p.theta_spacing = reader.GetFloat("donut", "theta_spacing", 0.07f);
    p.phi_spacing   = reader.GetFloat("donut", "phi_spacing",   0.02f);

    p.R1 = reader.GetFloat("donut", "R1", 1.0f);
    p.R2 = reader.GetFloat("donut", "R2", 2.0f);
    p.K2 = reader.GetFloat("donut", "K2", 5.0f);

    p.K1 = screen_width * p.K2 * 3.0f / (8.0f * (p.R1 + p.R2));

    return p;

}

// Donut math
std::string donutRenderFrame(float a, float b, const DonutParams& p, int width, int height) {
    
    /* https://www.a1k0n.net/2011/07/20/donut-math.html */

    /* Precompute sines and cosines of A and B */
    float cosA = cos(a), sinA = sin(a);
    float cosb = cos(b), sinB = sin(b);

    /* Buffers for vectors and complete dinamic 2D alocation */
    // index = x + y * width
    std::vector<char> output(width * height, ' ');
    std::vector<float> zbuffer(width * height, 0.0f);

    

};