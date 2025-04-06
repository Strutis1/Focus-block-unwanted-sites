#ifndef FOCUS_H
#define FOCUS_H

#include <string>

void showUsage();

void showBlockList();

bool isUrl(const std::string& str);

bool addUrl(const std::string& url);

void clearBlockList();

bool inBlockList(const std::string& url);

std::string cleanUrl(const std::string& url);

bool deleteUrl(const std::string& url);

std::string trim(const std::string& str);

#endif