#include "graph.h"
#include <algorithm>
#include <queue>
#include <stack>
#include <iostream>

// Graph constructor to initialize adjacency lists
// Graph::Graph() {
//     adj.resize(n);
//     reverse_adj.resize(n);
//     is_node_present.resize(n);
//     indirect_adj.resize(n);
// }

// Add an edge between two nodes
void Graph::add_edge(int u, int v) {
    adj[u].push_back(v);
    reverse_adj[v].push_back(u);
    is_node_present[u] = true;
    is_node_present[v] = true;
}

// Add an indirect edge between two nodes
void Graph::add_indirect_edge(int u, int v) {
    indirect_adj[u].push_back(v);
}

// Make a copy of the graph (deep copy)
Graph Graph::copy() const {
    Graph g;
    g.adj = adj;
    g.reverse_adj = reverse_adj;
    g.is_node_present = is_node_present;
    g.indirect_adj = indirect_adj;
    g.tasks = tasks;
    return g;
}

// Remove a node and its edges from the graph
void Graph::remove_node(int v) {
    if (v < 0 || v >= adj.size() || !is_node_present[v]) {
        // Invalid or already removed
        return;
    }

    // Remove v from the successor lists of its predecessors
    for (int u : reverse_adj[v]) {
        auto& succs = adj[u];
        succs.erase(std::remove(succs.begin(), succs.end(), v), succs.end());
    }

    // Remove v from the predecessor lists of its successors
    for (int u : adj[v]) {
        auto& preds = reverse_adj[u];
        preds.erase(std::remove(preds.begin(), preds.end(), v), preds.end());
    }

    adj[v].clear();
    reverse_adj[v].clear();
    is_node_present[v] = false;
}

// Find the predecessors of a given node
std::vector<int> Graph::predecessors(int v) const {
    if (v < 0 || v >= reverse_adj.size() || !is_node_present[v]) {
        return {};
    }
    return reverse_adj[v];
}


// Find the successors of a given node
vector<int> Graph::successors(int v) const {
    if (v < 0 || v >= adj.size() || !is_node_present[v]) {
        return {};
    }
    return adj[v];
}

// Return a set of vertices of the graph
unordered_set<int> Graph::get_vertices() const {
    unordered_set<int> result;
    for (int i = 0; i < adj.size(); ++i) {
        if (!adj[i].empty() || !reverse_adj[i].empty()) {
            result.insert(i);
        }
    }
    return result;
}

// Check if graph is empty
bool Graph::empty() const {
    // If no nodes are marked as present, the graph is empty
    for (bool present : is_node_present) {
        if (present) {
            return false;
        }
    }
    return true;
}

// Compute the set of ancestors of a node using BFS
unordered_set<int> Graph::get_ancestors(int node) const {
    unordered_set<int> ancestors;
    queue<int> q;
    q.push(node);

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        for (int pred : reverse_adj[curr]) {
            // If we haven't already visited this node, add and continue
            if (ancestors.insert(pred).second) {
                q.push(pred);
            }
        }
    }
    return ancestors;
}

// Compute the set of descendants of a node using BFS
unordered_set<int> Graph::get_descendants(int node) const {
    unordered_set<int> descendants;
    queue<int> q;
    q.push(node);

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        for (int pred : adj[curr]) {
            // If we haven't already visited this node, add and continue
            if (descendants.insert(pred).second) {
                q.push(pred);
            }
        }
    }
    return descendants;
}

void Graph::print_graph(const string& title) const {
    cout << title << endl;

    // Check if graph has any nodes
    bool has_nodes = false;
    for (bool node : is_node_present) {
        if (node) {
            has_nodes = true;
            break;
        }
    }

    if (!has_nodes) {
        cout << "(Empty graph)" << endl;
        return;
    }

    // Print each node and its outgoing edges
    for (int i = 0; i < adj.size(); ++i) {
        if (is_node_present[i]) {  // Check if node exists in the graph
            cout << "Node " << i << " -> ";
            if (adj[i].empty()) {
                cout << "(no outgoing edges)";
            } else {
                for (int neighbor : adj[i]) {
                    cout << neighbor << " ";
                }
            }
            cout << endl;
        }
    }

    // Print tasks information
    cout << "\nTasks Information:\n";
    for (size_t i = 0; i < tasks.size(); ++i) {
        const Task& t = tasks[i];
        cout << "Task " << i << ": "
             << "id=" << t.id << ", "
             << "wcet=" << t.wcet << ", "
             << endl;
    }
}

// Create an induced subgraph based on a set of nodes
Graph Graph::induced_subgraph(const std::unordered_set<int>& nodes) const {
    Graph subgraph;

    // Copy existing task information
    subgraph.tasks = tasks;

    // Resize to accommodate all potentially large node indices
    size_t max_index = *std::max_element(nodes.begin(), nodes.end());
    subgraph.resize(max_index + 1, false);
    // subgraph.adj.resize(max_index + 1);
    // subgraph.reverse_adj.resize(max_index + 1);

    // Initialize with 'no' nodes
    //subgraph.is_node_present.resize(max_index + 1, false);

    // Loop through each node in the nodes set
    for (int u : nodes) {
        if (u < 0 || u >= adj.size()) {
            cout << "Error: Node " << u << " is out of bounds!" << endl;
            continue;
        }

        // Mark u as present in the graph. NOT SURE IF THIS DOES ANYTHING USEFUL
        if (subgraph.is_node_present.size() <= u) {
            subgraph.is_node_present.resize(u + 1, false);
        }
        subgraph.is_node_present[u] = true; // mark node u as present

        // Loop through the successors of the node in the original graph
        for (int succ : adj[u]) {
            if (nodes.count(succ)) {
                subgraph.adj[u].push_back(succ);
                subgraph.reverse_adj[succ].push_back(u);
                subgraph.is_node_present[succ] = true; // mark succ as present too
            }
        }
    }

    return subgraph;
}

// Used to resize vectors of the graph, if needed.
void Graph::resize(int num_vertices, bool is_present) {
    adj.resize(num_vertices);
    reverse_adj.resize(num_vertices);
    indirect_adj.resize(num_vertices);
    is_node_present.resize(num_vertices, is_present);  // All nodes present initially
}

void Graph::setPeriod(int period) {
    _period = period;
}

void Graph::setDeadline(int deadline) {
    _deadline = deadline;
}

int Graph::getPeriod() {
    return _period;
}

int Graph::getDeadline() {
    return _deadline;
}