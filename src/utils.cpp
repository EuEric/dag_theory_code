#include "utils.h"

// No longer needed with C++17 version
std::string extract_filename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return path; // No path separator found
    return path.substr(pos + 1);
}

void dump_priorities_to_yaml(const fs::path& path, const std::vector<int>& priority) {
    std::ofstream fout(path);
    if (!fout) {
        std::cerr << "Failed to open output file: " << path << "\n";
        return;
    }

    // Write YAML content
    try {
        fout << "priorities:\n";
        int n = priority.size();
        
        for (int j = 0; j < n; ++j) {
            fout << "  - id: " << j << "\n"
                 << "    priority: " << priority[j] << "\n";
        }
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Error writing to file: " << e.what() << "\n";
    }
    fout.close();
}