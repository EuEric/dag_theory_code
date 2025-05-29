#include "utils.h"

std::string extract_filename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return path; // No path separator found
    return path.substr(pos + 1);
}

void dump_priorities_to_yaml(const std::string& file_name, const std::vector<int>& priority) {
    std::ofstream fout("output_files/" + file_name);
    if (!fout.is_open()) {
    std::cerr << "Failed to open output file: output_files/" << file_name << "\n";
    return;
    }

    fout << "priorities:\n";
    int n = priority.size();

    for (int j = 0; j < n; ++j) {
        fout << "  - id: " << j << "\n";
        fout << "    priority: " << priority[j] << "\n";
    }
    fout.close();
}