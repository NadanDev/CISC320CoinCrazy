
#ifndef OPEN_WEBPAGE
#define OPEN_WEBPAGE

#include <string>
#include <regex>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

const std::string pattern = "[a-zA-Z0-9]*\\.html$";
const std::regex rgx(pattern);

inline std::string getHtmlFile(std::string origin){
    std::string dir = "./Client";
    for (const auto &entry : fs::directory_iterator(dir)){
        fs::path path = entry.path();
        std::string file = path.filename().generic_string();
        if(std::regex_match(file, rgx)){
            std::cout << file << std::endl;
            return file;
        }
    }
    throw std::runtime_error("NO HTML FILE FOUND");
}

//windows code
#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Required for ShellExecute

    inline void open(std::string adr, std::string origin) {
        // The URL to open
        std::string url = adr + "/" + getHtmlFile(origin);

        // Open the URL using the default web browser
        ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
    }
#endif

//linux code
#ifdef linux
    #include <cstdlib>
    inline void open(std::string adr, std::string origin) {
        std::string url = adr + "/" + getHtmlFile(origin);
        //std::string cmd = "xdg-open " + url;
        std::string cmd = "sensible-browser " + url;
        system(cmd.c_str());
    }
#endif

// macOS code
#ifdef __APPLE__
    #include <cstdlib>
    inline void open(std::string adr, std::string origin) {
        std::string url = adr + "/" + getHtmlFile(origin);
        std::string cmd = "open " + url;
        system(cmd.c_str());
    }
#endif

#endif