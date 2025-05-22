#pragma once

#include <yaml-cpp/yaml.h>
#include "graph.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <set>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <fstream>
#include <unordered_map>
#include "generator_params.h"

enum DOTLine_t {DOT_BEGIN, DOT_END, DOT_NODE, DOT_EDGE, DOT_INDIRECT_EDGE, DAG_INFO, VOID_LINE};

struct dot_info{
    DOTLine_t lineType;
    int p           = -1;
    int s           = -1;
    int id          = -1;
    int id_from     = -1;
    int id_to       = -1;
    float wcet      = 0;
    float period    = 0;
    float deadline  = 0;
};

class DAGParser {
public:
    DAGParser(){};
    ~DAGParser(){};

    // Parses a graph given a YAML representation of a single graph
    void readDAGFromYAML(YAML::Node dag_node, Graph& g);

    // Parses a graph given a DOT file
    void readDAGFromDOT(const std::string& file_path, Graph& g);

    // Read multiple DAGS from a single .yaml file
    void readMultipleDAGSFromYAML(const std::string& file_path, vector<Graph>& graphs);

    // Read multiple DOT files from a .txt file containing their paths
    void readMultipleDAGSFromDOT(const std::string& file_path, vector<Graph>& graphs);

};