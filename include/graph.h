#pragma once

#include <vector>
#include <unordered_set>

using namespace std;

enum subTaskMode {NORMAL_T, C_INTERN_T, C_SOURCE_T, C_SINK_T};

// Structure of a Task, which represents a vertex (node) in the Graph
struct Task {
    int id;
    int wcet;
    int width;
    int depth;
    subTaskMode mode;
    // int period;
    // int deadline;
};

// Graph representation with adjacency and reverse adjacency lists
class Graph {
    private:
        int _period;
        int _deadline;
    public:
        // adj[i] = successors of node i
        vector<vector<int>> adj;  
        // reverse_adj[i] = predecessors of node i
        vector<vector<int>> reverse_adj; 
        // Used to determine the undeleted (existing) nodes in a graph
        vector<int> is_node_present;    
        // Indirect edges adjacencies
        vector<vector<int>> indirect_adj;    
        // Ray tasks present in graph (id, wcet, deadline...)
        vector<Task> tasks;

        Graph(){};

        void setPeriod(int period);
        void setDeadline(int deadline);

        int getPeriod();
        int getDeadline();

        // Resizes dynamic vectors to dodge out of bounds memory issues. is_present controls the values of the is_node_present array
        void resize(int num_vertices, bool is_present);

        // Make a copy of the graph (deep copy)
        Graph copy() const;

        // Remove a node and its edges from the graph
        void remove_node(int v);


        // Find the predecessor nodes of a given node
        vector<int> predecessors(int v) const;

        // Find the succesor nodes of a given node
        vector<int> successors(int v) const;

        // Return a set of vertices of the graph
        unordered_set<int> get_vertices() const;

        // Check if graph is empty
        bool empty() const;

        // Compute the set of ancestors of a node using BFS
        unordered_set<int> get_ancestors(int node) const;

        // Compute the set of descendants of a node using BFS
        unordered_set<int> get_descendants(int node) const;

        // Prints graph in an informative way
        void print_graph(const string& title) const;

        // Create an induced subgraph based on a set of nodes
        Graph induced_subgraph(const std::unordered_set<int>& nodes) const;

        // Add an edge between two nodes
        void add_edge(int u, int v);

        // Add an indirect edge between two nodes
        void add_indirect_edge(int u, int v);
};
