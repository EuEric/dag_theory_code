#pragma once

#include <iostream>
#include <fstream>
#include <vector>

// Used to extrace the YAML filename from a command line input
std::string extract_filename(const std::string& path);

// Dump priority vector to a YAML file
void dump_priorities_to_yaml(const std::string& file_name, const std::vector<int>& priority);