#include "assign_priority.h"
#include "topological_sort.h"
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <iostream>

using namespace std;

// Recursive function to assign priorities to tasks in a DAG
void assign_priority(
    Graph G,                           // Graph (passed by value to avoid mutation)
    const vector<int>& l,             // l(v) values
    const vector<int>& lb,            // lb(v) values
    vector<int>& priority,            // Priority assignment array
    int& p                            // Current priority counter (increasing)
) {
    int n = G.tasks.size();

    while (!G.empty()) {
    
        // Find source node with max length
        int v = -1;
        int max_l = -1;
    
        for (int i = 0; i < G.is_node_present.size(); ++i) {
            if (!G.is_node_present[i]) continue; // Skip inactive nodes
    
            const vector<int>& preds = G.predecessors(i);
    
            if (preds.empty() && priority[i] == -1) {
                // Tie-break prefers smaller id
                if (l[i] > max_l || (l[i] == max_l && (v == -1 || i < v))) {
                    v = i;
                    max_l = l[i];
                }
            }
        }
    
        if (v == -1) break;

        // Assign priority to current node
        priority[v] = p;
        // Increase priority for other vertices
        p++;

        // Initialize list A with successors of v
        vector<int> A = G.successors(v);
        G.remove_node(v);  // remove v from the graph

        while (!A.empty()) {

            // Select node u from A with highest l (break ties using lb)
            int v = -1;
            int max_lu = -1;
            for (int candidate : A) {
                if (priority[candidate] == -1) {
                    if (l[candidate] > max_lu || (l[candidate] == max_lu && lb[candidate] > lb[v])) {
                        v = candidate;
                        max_lu = l[candidate];
                    }
                }
            }

            if (v == -1) break;

            // If v has ancestors, recurse
            if (!G.predecessors(v).empty()) {

                unordered_set<int> ancestors = G.get_ancestors(v);

                // Build subgraph G' induced by ancestors of u
                Graph Gprime = G.induced_subgraph(ancestors);

                // Recursive call on subgraph
                assign_priority(Gprime, l, lb, priority, p);

                // Remove all ancestor nodes from G
                for (int node : ancestors) {
                    G.remove_node(node);
                }
            } 

            // Assign priority to current vertex
            priority[v] = p;
            //Increase priority for other vertices
            p++;

            // A <- succ(v)
            A = G.successors(v);
            // Remove node v
            G.remove_node(v);
        }
    }
}


void dfs_propagate(Graph G,
                int node,
                vector<int>& priority,
                unordered_set<int>& visited) {
    visited.insert(node);

    for (int neighbor : G.indirect_adj[node]) {
        priority[neighbor] = priority[node];
        cout << "Propagated priority " << priority[node]
                << " from node " << node << " to node " << neighbor << "\n";
        dfs_propagate(G, neighbor, priority, visited);
    }
}

void assign_indirect_priority(Graph G,
                            vector<int>& priority) {

    vector<int> topo_order = topological_sort(G.adj);                           
    int n = topo_order.size();
    unordered_set<int> visited;

    for (int i = 0; i < n; ++i) {
        if (priority[i] != -1 && visited.count(i) == 0) {
           dfs_propagate(G, i, priority, visited);
        }
    }
}