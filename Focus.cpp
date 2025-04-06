#include <iostream>
#include <regex>
#include <fstream>
#include "Focus.h"

#define BLOCKLISTPATH   R"(C:\\Windows\\System32\\drivers\etc\\hosts)"
#define BLOCKLISTTXT    "BlockList.txt"
#define IP "0.0.0.0"

//TODO add GUI
//TODO expand to browser extensions, then we can automatically close the unwanted tabs

int main(int argc, char** argv){
    if(argc == 2){
        if(std::string(argv[1]) == "-h"){
            std::cout << "commands:\n";
            std::cout << "add <urls[]>    - <url>= String[] of the urls you want to block\n";
            std::cout << "del <urls[]>    - <url>= String[] of the urls you want to delete from block list\n";
            std::cout << "list            - List all blocked urls\n";
            std::cout << "clear           - Clear the block list\n";
            std::cout << "-h              - Show this help\n";
            return 0;
        }
        else if(std::string(argv[1]) == "list"){
            showBlockList();
            return 0;
        }else if(std::string(argv[1]) == "clear"){
            clearBlockList();
            return 0;
        }
    }

    if(argc < 3){
        showUsage();
        return 1;
    }
    std::string command = std::string(argv[1]);

    if(command == "add"){
        for(int i = 2; i < argc; ++i){
            if(!isUrl(std::string(argv[i]))){
                std::cout << "Invalid url: " << argv[i] << "\n";
                continue;
            }
            std::string cleaned = cleanUrl(argv[i]);
            if(addUrl(cleaned)){
                std::cout << "Url " << argv[i] << " added to block list\n";
            }
        }
    }
    else if(command == "del"){
        for(int i = 2; i < argc; ++i){
            if(!isUrl(std::string(argv[i]))){
                std::cout << "Invalid url: " << argv[i] << "\n";
                continue;
            }
            std::string cleaned = cleanUrl(argv[i]);
            if(!deleteUrl(cleaned)){
                std::cout << "Url is not in block list: " << argv[i] << "\n";
            }
            else{
                std::cout << "Url " << argv[i] << " deleted from block list\n";
            }
        }
    } else{
        showUsage();
    }
    return 0;
}

void showUsage(){
    std::cout << "usage: <command> <urls> or type -h for help\n";
}

void clearBlockList(){
    std::ifstream blockListFile(BLOCKLISTTXT);
    if (!blockListFile.is_open()) {
        std::cerr << "Could not open BlockList.txt.\n";
        return;
    }

    std::vector<std::string> blockedLines;
    std::string line;
    while (std::getline(blockListFile, line)) {
        blockedLines.push_back(trim(line));
    }
    blockListFile.close();

    std::ofstream clearBlockListFile(BLOCKLISTTXT, std::ios::trunc);
    clearBlockListFile.close();

    std::ifstream hostsFile(BLOCKLISTPATH);
    std::ofstream tempHosts("temp_hosts.txt");

    if (!hostsFile.is_open() || !tempHosts.is_open()) {
        std::cerr << "Could not access hosts file. Run as admin.\n";
        std::cerr << "Could not access hosts file. Run as admin.\n";
        return;
    }

    while (std::getline(hostsFile, line)) {
        bool isBlockedLine = false;
        for (const auto& blocked : blockedLines) {
            if (line == std::string(IP) + " " + trim(blocked)){
                isBlockedLine = true;
                break;
            }
        }
        if (!isBlockedLine) {
            tempHosts << line << "\n";
        }
    }

    hostsFile.close();
    tempHosts.close();

    std::ifstream tempInput("temp_hosts.txt");
    std::ofstream finalHosts(BLOCKLISTPATH);
    if (!finalHosts.is_open()) {
        std::cerr << "Could not write to hosts file. Run as admin.\n";
        return;
    }

    finalHosts << tempInput.rdbuf();
    tempInput.close();
    finalHosts.close();
    std::remove("temp_hosts.txt");

    std::cout << "Block list cleared successfully.\n";
}

void showBlockList(){
    std::ifstream file(BLOCKLISTTXT);
    std::string line;
    std::cout << "Block list:\n";
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    file.close();
}

bool isUrl(const std::string& url){
    std::string domain = cleanUrl(url);
    std::regex url_regex(R"(^([\da-z\.-]+)\.([a-z\.]{2,6})$)");
    return std::regex_match(domain, url_regex);
}

bool inBlockList(const std::string& url){
    std::ifstream file(BLOCKLISTTXT);
    std::string line;
    std::string domain = cleanUrl(url);
    std::string search = domain;
    while (std::getline(file, line)) {
        if (line == search) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

std::string cleanUrl(const std::string& url) {
    std::regex re(R"(^(https?:\/\/)?([^\/]+))");
    std::smatch match;
    if (std::regex_search(url, match, re)) {
        return match[2];
    }
    return url;
}

bool addUrl(const std::string& url){
    if (inBlockList(url)) {
        std::cout << "Already in block list: " << url << "\n";
        return false;
    }

    std::ofstream hostsFile(BLOCKLISTPATH, std::ios::app);
    if (!hostsFile.is_open()) {
        std::cerr << "Could not open hosts file. Run as administrator.\n";
        return false;
    }

    std::ofstream blockListFile(BLOCKLISTTXT, std::ios::app);
    if (!blockListFile.is_open()) {
        std::cerr << "Could not open BlockList.txt.\n";
        return false;
    }

    std::string entry = url;

    blockListFile << entry << "\n";

    entry = std::string(IP) + " " + url;

    hostsFile << entry << "\n";

    hostsFile.close();
    blockListFile.close();
    return true;
}

bool deleteUrl(const std::string& url){
    std::string target = url;
    bool found = false;
    std::string line;

    // ----------- Clean BlockList.txt ------------
    std::ifstream input1(BLOCKLISTTXT);
    std::ofstream temp1("temp_blocklist.txt");
    if (!input1.is_open() || !temp1.is_open()) {
        std::cerr << "Could not access BlockList.txt.\n";
        return false;
    }

    while (std::getline(input1, line)) {
        if (line != target) {
            temp1 << line << "\n";
        } else {
            found = true;
        }
    }

    input1.close();
    temp1.close();

    if (found) {
        std::remove(BLOCKLISTTXT);
        std::rename("temp_blocklist.txt", BLOCKLISTTXT);
    } else {
        std::remove("temp_blocklist.txt");
    }

    // ----------- Clean hosts file safely ------------

    target = std::string(IP) + " " + url;
    std::ifstream input2(BLOCKLISTPATH);
    std::ofstream tempHosts("temp_hosts.txt");
    if (!input2.is_open() || !tempHosts.is_open()) {
        std::cerr << "Could not open hosts file for editing. Run as admin.\n";
        return found;
    }

    while (std::getline(input2, line)) {
        if (line != target) {
            tempHosts << line << "\n";
        }
    }

    input2.close();
    tempHosts.close();

    std::ifstream tempInput("temp_hosts.txt");
    std::ofstream finalHosts(BLOCKLISTPATH);
    if (!finalHosts.is_open()) {
        std::cerr << "Could not reopen hosts file for final write.\n";
        return found;
    }

    finalHosts << tempInput.rdbuf();
    tempInput.close();
    finalHosts.close();
    std::remove("temp_hosts.txt");

    return found;
}



std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}