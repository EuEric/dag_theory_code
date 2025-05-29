#include <iostream>
#include <string>
#include <vector>
#include "assign_priority.h"
#include "compute_length.h"
#include "graph.h"
#include "rt_analysis.h"
#include "dag_parser.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv[]) {
    int nr_proc;
    DAGParser dagParser;
    vector<Graph> dags;
    std::string filename;
    
    // Default configuration
    nr_proc = 2;

    // Argument parser
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "-f" || arg == "--file") && i + 1 < argc) {
            filename = argv[++i];
        }
    }

    if (!filename.empty()){
        cout << "Reading DAG(s) from file: " << filename << "\n";

        if (filename.find(".yaml") != std::string::npos || filename.find(".yml") != std::string::npos) {
            dagParser.readMultipleDAGSFromYAML(filename, dags);    
        } else if (filename.find(".txt") != std::string::npos) {
            cout << "dot";
            dagParser.readMultipleDAGSFromDOT(filename, dags);
        } else {
            std::cerr << "Unsupported file format. Use .dot or .yaml/.yml.\n";
            return 1;
        }
    } else {
        std::cerr << "Usage:\n"
                  << argv[0] << " -f <taskset.yaml>\n"
                  << "    OR\n"
                  << argv[0] << " -g [-n <num_dags>]\n";
        return 1;
    }


    // Define WCETs

    // Paper example
    // vector<Task> tasks = {
    //     {0, 1}, {1, 3}, {2, 1}, {3, 2}, {4, 1}, {5, 1}, {6, 1}
    // };

    // // Ray example
    // vector<Task> tasks = {
    //     {0, 0}, {1, 6}, {2, 4}, {3, 4}, {4, 15}, {5, 3}, {6, 0}
    // };

    // First example paper
    // vector<Task> tasks = {
    //     {0, 1}, {1, 4}, {2, 2}, {3, 2}, {4, 1}
    // };

    //int n = tasks.size(); // Number of vertices (Tasks)

    // Paper example
    // G.add_edge(0, 1);
    // G.add_edge(0, 2);
    // G.add_edge(0, 3);
    // G.add_edge(1, 4);
    // G.add_edge(2, 4);
    // G.add_edge(3, 6);
    // G.add_edge(4, 5);
    // G.add_edge(5, 6);


    // Ray example
    // G.add_edge(0, 1);
    // G.add_edge(0, 4);
    // G.add_edge(1, 2);
    // G.add_edge(1, 3);
    // G.add_edge(2, 5);
    // G.add_edge(3, 5);
    // G.add_edge(4, 6);
    // G.add_edge(5, 6);
    // G.add_indirect_edge(1, 5);

    // First example paper
    // G.add_edge(0, 1);
    // G.add_edge(0, 2);
    // G.add_edge(0, 3);
    // G.add_edge(1, 4);
    // G.add_edge(2, 4);
    // G.add_edge(3, 4);

    // Big example Ray, on my paper
    // G.add_edge(0, 1);
    // G.add_edge(0, 2);
    // G.add_edge(0, 3);
    // G.add_edge(1, 4);
    // G.add_edge(1, 5);
    // G.add_edge(2, 6);
    // G.add_edge(2, 7);
    // G.add_edge(2, 8);
    // G.add_edge(3, 12);
    // G.add_edge(4, 9);
    // G.add_edge(5, 9);
    // G.add_edge(6, 10);
    // G.add_edge(7, 10);
    // G.add_edge(8, 10);
    // G.add_edge(9, 11);
    // G.add_edge(10, 12);
    // G.add_edge(11, 12);
    // G.add_indirect_edge(1, 9);
    // G.add_indirect_edge(9, 11);
    // G.add_indirect_edge(2, 10);

    // // Big example Ray, on my paper
    // G.tasks = {
    //     {0, 0}, {1, 7}, {2, 5}, {3, 10}, {4, 4},
    //     {5, 8}, {6, 8}, {7, 2}, {8, 1}, {9, 1},
    //     {10, 3}, {11, 1}, {12, 0}
    // };

    cout << "dags size: " << dags.size() << " \n";

    for(int i = 0; i < dags.size(); i++) {
        int n = dags[i].tasks.size(); // Number of vertices (Tasks)

        vector<int> lf(n, 0), lb(n, 0), l(n, 0);

        // Compute l, lf, and lb values for every vertex
        compute_length(dags[i], lf, lb, l);

        // Display computed length values
        cout << "Task\tWCET\tlf\tlb\tl\n";
        for (int j = 0; j < n; ++j) {
            cout << j << "\t" << dags[i].tasks[j].wcet << "\t" << lf[j] << "\t" << lb[j] << "\t" << l[j] << "\n";
        }

        // Initialize priority list and counter, mark as uninitialized with -1
        vector<int> priority(n, -1);
        int p = 0;
        float final_rt;

        // Assign priorities
        assign_priority(dags[i], l, lb, priority, p);

        //Check what is the RT before another round of assign
        final_rt = compute_rt_bound(dags[i], priority, nr_proc);
        cout << "RT bound BEFORE COMPUTING NEW PRIORITIES: " << final_rt << "\n";

        // Assign 'correct' priorities to fork-join nodes
        assign_indirect_priority(dags[i], priority);

        cout << "Assigned priorities:\n";
        for (int j = 0; j < n; ++j) {
            cout << "Task " << j << ": priority = " << priority[j] << "\n";
        }

        dump_priorities_to_yaml("priorities_" + extract_filename(filename), priority);

        final_rt = compute_rt_bound(dags[i], priority, nr_proc);
        cout << "RT bound AFTER COMPUTING NEW PRIORITIES: " << final_rt << "\n";
    }

    return 0;
}
