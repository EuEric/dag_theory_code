#include "assign_priority.h"
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <iostream>

using namespace std;

// Recursive function to assign priorities to tasks in a DAG
void assign_priority(
    Graph G,                           // Graph (passed by value to avoid mutation)
   // const vector<Task>& tasks,        // Task metadata
    const vector<int>& l,             // l(v) values
    const vector<int>& lb,            // lb(v) values
    vector<int>& priority,            // Priority assignment array
    int& p                            // Current priority counter (increasing)
) {
    int n = G.tasks.size();
    //cout << "inside assign priority" << "\n";

    while (!G.empty()) {
        //cout << "Inside while if G is NOT Empty" << "\n";
    
        // Step 1: Find source node with max length
        int v = -1;
        int max_l = -1;
    
        for (int i = 0; i < G.is_node_present.size(); ++i) {
            if (!G.is_node_present[i]) continue; // Skip inactive nodes
    
            const vector<int>& preds = G.predecessors(i);
    
            // cout << "Predecessors of node " << i << " -";
            // for (int pred : preds) {
            //     cout << " " << pred;
            // }
            // cout << "\n";
    
            if (preds.empty() && priority[i] == -1) {
                // Tie-break prefers smaller id
                if (l[i] > max_l || (l[i] == max_l && (v == -1 || i < v))) {
                    v = i;
                    max_l = l[i];
                }
            }
        }
    
        if (v == -1) break;

        //cout << "Node with no predecessor selected is: " << v << "\n";

        // Step 2: Assign priority to source node
        // cout << "###############################" << "\n";
        // cout << "Added to Verrtex v: " << v << " the priority: " << p << "\n";
        // cout << "###############################" << "\n";
        priority[v] = p;

        // Extra: add the same priority to the 'tail' node(s) (ending in the indirect edges)
        //assign_indirect_priority(v, p, G.indirect_adj, priority);

        //Increase priority for other vertices
        p++;

        // Step 3: Initialize list A with successors of v
        vector<int> A = G.successors(v);
        //G.print_graph("Graph before removing node " + to_string(v));
        G.remove_node(v);  // remove v from the graph
        //G.print_graph("Graph after removing node " + to_string(v));

        while (!A.empty()) {
            // cout << "A is: " << "\n";
            // for ( int a : A ) {
            //     cout << " " << a;
            // }
            // cout << "\n";

            // Step 4: Select node u from A with highest l (break ties using lb)
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
            // A.erase(remove(A.begin(), A.end(), u), A.end());

            //cout << "Vertex v inside while A is: " << v << "\n";

            // Step 5: If v has ancestors, recurse
            if (!G.predecessors(v).empty()) {
                // cout << "Inside predecessors for Vertex v: " << v << " -";
                // for ( int v : G.predecessors(v) ) {
                //     cout << " " << v;
                // }
                // cout << "\n";

                unordered_set<int> ancestors = G.get_ancestors(v);

                // Build subgraph G' induced by ancestors of u
                Graph Gprime = G.induced_subgraph(ancestors);
                //Gprime.print_graph("GPRIME: ");

                // Recursive call on subgraph
                assign_priority(Gprime, l, lb, priority, p);

                // Remove all ancestor nodes from G
                //G.print_graph("Graph before removing all ancestors of node " + to_string(v));
                for (int node : ancestors) {
                    //cout << "Removing ancestor: " << node << "\n";
                    G.remove_node(node);
                }
                //G.print_graph("Graph after removing all ancestors of node " + to_string(v));
            } 
            // else {
            //     cout << "Vertex v: " << v << " has no predecessors!" << "\n";
            // }

            // Step 6: Assign priority to u
            // cout << "###############################" << "\n";
            // cout << "Added to Vertex v: " << v << " the priority: " << p << "\n";
            // cout << "###############################" << "\n";
            priority[v] = p;

            // Extra: add the same priority to the 'tail' node(s) (ending in the indirect edges)
            //assign_indirect_priority(v, p, G.indirect_adj, priority);
    
            //Increase priority for other vertices
            p++;

            // Step 7: A <- succ(v)
            A = G.successors(v);
            // cout << "A now contained the successors of: " << v <<" -";
            // for ( int succ : A ) {
            //     cout << " " << succ;
            // }
            // cout << "\n";
            // Remove node v
            G.remove_node(v);
        }
    }
}


void dfs_propagate(int node,
                  const vector<vector<int>>& indirect_adj,
                  vector<int>& priority,
                  unordered_set<int>& visited) {
    visited.insert(node);

    for (int neighbor : indirect_adj[node]) {
        priority[neighbor] = priority[node];
        cout << "Propagated priority " << priority[node]
                << " from node " << node << " to node " << neighbor << "\n";
        dfs_propagate(neighbor, indirect_adj, priority, visited);
    }
}

void assign_indirect_priority(const vector<vector<int>>& indirect_adj,
                            vector<int>& priority) {
    int n = indirect_adj.size();
    unordered_set<int> visited;

    for (int i = 0; i < n; ++i) {
        if (priority[i] != -1 && visited.count(i) == 0) {
           dfs_propagate(i, indirect_adj, priority, visited);
        }
    }
}