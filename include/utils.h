#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// Used to extrace the YAML filename from a command line input
// No longer needed with C++17 version
std::string extract_filename(const std::string& path);

// Dump priority vector to a YAML file
void dump_priorities_to_yaml(const fs::path& path, const std::vector<int>& priority);