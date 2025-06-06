#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "assign_priority.h"
#include "compute_length.h"
#include "graph.h"
#include "rt_analysis.h"
#include "dag_parser.h"
#include "utils.h"

namespace fs = std::filesystem;
using namespace std;

// Function to process a single YAML file and save data in a vector
void process_yaml_file(const fs::path& path, DAGParser& dagParser, vector<pair<fs::path, Graph>>& dags) {
    cout << "Processing file: " << path << "\n";
    vector<Graph> yaml_dags;
    dagParser.readMultipleDAGSFromYAML(path.string(), yaml_dags);
    for (auto& dag : yaml_dags) {
        dags.emplace_back(path, move(dag));
    }
}

// Recursive function that processes input folder/yaml and saves data in a vector
void process_input_path(const fs::path& input_path, DAGParser& dagParser, vector<pair<fs::path, Graph>>& dags) {
    if (!fs::exists(input_path)) {
        cerr << "Input path does not exist.\n";
        exit(1);
    }

    if (fs::is_regular_file(input_path)) {
        cout << "Reading DAG(s) from file: " << input_path << "\n";

        if (input_path.extension() == ".yaml" || input_path.extension() == ".yml") {
            process_yaml_file(input_path, dagParser, dags);
        } else {
            cerr << "Unsupported file format. Use .yaml or .yml.\n";
            exit(1);
        }
    } else if (fs::is_directory(input_path)) {
        cout << "Reading DAG(s) from directory: " << input_path << "\n";

        for (const auto& entry : fs::recursive_directory_iterator(input_path)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                if (path.extension() == ".yaml" || path.extension() == ".yml") {
                    process_yaml_file(path, dagParser, dags);
                }
            }
        }
    } else {
        cerr << "Input path is neither a file nor a directory.\n";
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    int nr_proc = 2;
    DAGParser dagParser;
    string input_path;
    string output_dir = "output_files"; // Default output directory

    // Argument parser
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            input_path = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output_dir = argv[++i];
        } else if ((arg == "--proc") && i + 1 < argc) {
            nr_proc = stoi(argv[++i]);
        }
    }

    cout << "Output directory: " << output_dir << '\n';

    if (input_path.empty()) {
        cerr << "Usage:\n"
                  << argv[0] << " -i <input_path> [-o <output_dir>] [--proc <nr_proc>]\n"
                  << "  input_path: a YAML file or a directory containing YAML files\n"
                  << "  output_dir: directory to save the priority YAML files\n";
        return 1;
    }

    if (!fs::exists(output_dir)) {
        fs::create_directories(output_dir);
    }

    vector<pair<fs::path, Graph>> dags;

    // Process either entire folder or a yaml file
    process_input_path(input_path, dagParser, dags);

    fs::path input_root = fs::absolute(input_path);

    // Main algorithm loop
    for (const auto& [filepath, dag] : dags) {
        int n = dag.tasks.size();

        vector<int> lf(n, 0), lb(n, 0), l(n, 0);

        // Compute l, lf, and lb values for every vertex
        compute_length(dag, lf, lb, l);

        // Display computed length values
        cout << "Task\tWCET\tlf\tlb\tl\n";
        for (int j = 0; j < n; ++j) {
            cout << j << "\t" << dag.tasks[j].wcet << "\t" << lf[j] << "\t" << lb[j] << "\t" << l[j] << "\n";
        }

        vector<int> priority(n, -1);
        int p = 0;
        float final_rt;

        // Assign priorities to edges using He algorithm
        assign_priority(dag, l, lb, priority, p);

        // Check RT bound for He algorithm priorities
        final_rt = compute_rt_bound(dag, priority, nr_proc);
        cout << "RT bound BEFORE COMPUTING NEW PRIORITIES: " << final_rt << "\n";

        // Use our method to propagate priorities on indirect edges
        assign_indirect_priority(dag, priority);

        cout << "Assigned priorities:\n";
        for (int j = 0; j < n; ++j) {
            cout << "Task " << j << ": priority = " << priority[j] << "\n";
        }

        // Create the relative path from input_root to the file
        fs::path relative_path = fs::relative(filepath, input_root);

        // Create the output path maintaining the folder structure
        fs::path output_path = fs::path(output_dir) / relative_path.parent_path() / ("priorities_" + filepath.stem().string() + ".yaml");

        // Make sure the parent directory exists
        fs::create_directories(output_path.parent_path());

        dump_priorities_to_yaml(output_path, priority);

        cout << "Saved priorities to: " << output_path.string() << "\n";

        // Check RT bound after our priority assignment
        final_rt = compute_rt_bound(dag, priority, nr_proc);
        cout << "RT bound AFTER COMPUTING NEW PRIORITIES: " << final_rt << "\n";
    }

    return 0;
}
