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

/* Sanatize strings */
std::string cleanString(std::string s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

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
    std::string chars;
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

    std::string raw_chars = reader.Get("donut", "chars", ".,-~:;=!*#$@");
    p.chars = cleanString(raw_chars);

    p.K1 = screen_width * p.K2 * 3.0f / (8.0f * (p.R1 + p.R2));

    return p;

}

// Donut math
std::string donutRenderFrame(float A, float B, const DonutParams& p, int width, int height) {
    
    /* https://www.a1k0n.net/2011/07/20/donut-math.html */

    /* Precompute sines and cosines of A and B */
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);

    /* Buffers using vector for safety */
    // index = x + y * width
    std::vector<char> output(width * height, ' ');
    std::vector<float> zbuffer(width * height, 0.0f);

    /* Theta goes around the cross-sectional circle of a torus */
    for (float theta = 0; theta < 2 * M_PI; theta += p.theta_spacing) {

        /* Precompute sines and cosines of theta */
        float costheta = cos(theta), sintheta = sin(theta);

        /* Phi goes around the center of revolution of a torus */
        for (float phi = 0; phi < 2 * M_PI; phi += p.phi_spacing) {
            // precompute sines and cosines of phi
            float cosphi = cos(phi), sinphi = sin(phi);

            // the x,y coordinate of the circle, before revolving
            float circlex = p.R2 + p.R1 * costheta;
            float circley = p.R1 * sintheta;

            // final 3D (x,y,z) coordinate after rotations
            float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
            float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
            float z = p.K2 + cosA * circlex * sinphi + circley * sinA;
            float ooz = 1.0f / z;  // "one over z"

            // x and y projection
            int xp = (int)(width / 2 + p.K1 * ooz * x);
            int yp = (int)(height / 2 - p.K1 * ooz * y);

            // Bounds check
            if (xp >= 0 && xp < width && yp >= 0 && yp < height) {
                
                // Calculate luminance
                float L = cosphi * costheta * sinB - cosA * costheta * sinphi -
                          sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

                // L ranges from -sqrt(2) to +sqrt(2). If it's < 0, the surface is pointing away.
                if (L > 0) {
                    int idx = xp + yp * width;
                    
                    if (ooz > zbuffer[idx]) {
                        zbuffer[idx] = ooz;
                        
                        int char_count = p.chars.size();
                        int luminance_index = (int)(L * (char_count / 1.414f)); // 1.414 é aprox sqrt(2)
                        
                        if (luminance_index < 0) luminance_index = 0;
                        if (luminance_index >= char_count) luminance_index = char_count - 1;

                        output[idx] = p.chars[luminance_index];
                    }
                }
            }
        }
    }

    std::string frame;
    frame.reserve(width * height + height + 10); 

    frame += "\x1b[H"; // Cursor home
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            frame += output[i + j * width];
        }
        frame += '\n';
    }

    return frame;
}