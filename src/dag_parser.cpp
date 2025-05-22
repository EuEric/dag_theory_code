#include "dag_parser.h"

using namespace std;

std::vector<std::pair<std::string, std::string>> separateOnComma(const std::string& line){
    std::string left = "", rigth = "";
    bool read_equal = false;
    std::vector<std::pair<std::string, std::string>> pairs;
    for(int i=0; i<line.size(); ++i){
        if(line[i] == ','){
            if(!read_equal || left.empty() || rigth.empty())
                cout << "Wrong DOT file format!";
            pairs.push_back(std::make_pair(left, rigth));
            left = "";
            rigth = "";
            read_equal = false;
            continue;
        }
        else if (line[i] == '='){
            read_equal = true;
            continue;
        }
        else if (line[i] == '\"' || line[i] == ' ')
            continue;

        if(read_equal)
            rigth += line[i];
        else
            left += line[i];
    }
    if(!read_equal || left.empty() || rigth.empty())
        cout << "Wrong DOT file format!";
    pairs.push_back(std::make_pair(left, rigth));

    return pairs;
}

dot_info parseDOTLine(const std::string& line) {
    dot_info line_info;

    size_t start_graph = line.find("{");
    size_t end_graph = line.find("}");
    size_t arrow = line.find("->");
    size_t box = line.find("box");
    size_t start_node = line.find("[");
    size_t end_node = line.find("]");

    if (start_graph != std::string::npos) {
        line_info.lineType = DOTLine_t::DOT_BEGIN;
    } else if (end_graph != std::string::npos) {
        line_info.lineType = DOTLine_t::DOT_END;
    } else if (arrow != std::string::npos) {
        // Determine if this is an indirect edge
        bool is_indirect = line.find("indirect") != std::string::npos || line.find("style=dashed") != std::string::npos;

        line_info.lineType = is_indirect ? DOTLine_t::DOT_INDIRECT_EDGE : DOTLine_t::DOT_EDGE;

        std::string id_from_str = line.substr(0, arrow);
        line_info.id_from = std::stoi(id_from_str);

        std::string id_to_str = line.substr(arrow + 2, line.find(";") - (arrow + 2));
        line_info.id_to = std::stoi(id_to_str);
    } else if (box != std::string::npos) {
        line_info.lineType = DOTLine_t::DAG_INFO;
        std::string brackets_content = line.substr(start_node + 1, end_node - (start_node + 1));
        auto pairs = separateOnComma(brackets_content);
        for (auto p : pairs) {
            if (p.first == "D")
                line_info.deadline = std::stof(p.second);
            if (p.first == "T")
                line_info.period = std::stof(p.second);
        }
    } else if (start_node != std::string::npos && end_node != std::string::npos) {
        line_info.lineType = DOTLine_t::DOT_NODE;
        std::string node_id_str = line.substr(0, start_node);
        line_info.id = std::stoi(node_id_str);

        std::string brackets_content = line.substr(start_node + 1, end_node - (start_node + 1));
        auto pairs = separateOnComma(brackets_content);
        for (auto p : pairs) {
            if (p.first == "label") 
                line_info.wcet = std::stof(p.second);
            if (p.first == "p")
                line_info.p = std::stof(p.second);
            if (p.first == "s")
                line_info.s = std::stof(p.second);
        }
    } else {
        line_info.lineType = DOTLine_t::VOID_LINE;
    }

    return line_info;
}

void DAGParser::readDAGFromYAML(YAML::Node dag_node, Graph& g) {
    // Read period and deadline of a vertex
    int t = dag_node["t"] ? dag_node["t"].as<int>() : 0;
    int d = dag_node["d"] ? dag_node["d"].as<int>() : 0;
    g.setDeadline(d);
    g.setPeriod(t);

    YAML::Node vert = dag_node["vertices"];
    int num_vertices = vert.size();
    cout <<"num vertices: " << num_vertices << "\n";

    // Vectors to temporarily hold edges
    std::vector<std::pair<int, int>> edge_list;
    std::vector<std::pair<int, int>> indirect_edge_list;
    std::map<int, int> id_pos;

    // Assign internal graph IDs (0 to n-1)
    for (int j = 0; j < num_vertices; ++j) {
        int external_id = vert[j]["id"].as<int>();
        int task_wcet = vert[j]["c"].as<int>();
        id_pos[external_id] = j;

        Task t;
        t.id = external_id;
        t.wcet = task_wcet;
        g.tasks.push_back(t);
    }

    YAML::Node edges = dag_node["edges"];
    for (int j = 0; j < edges.size(); ++j) {
        int from = id_pos[edges[j]["from"].as<int>()];
        int to = id_pos[edges[j]["to"].as<int>()];

        edge_list.emplace_back(from, to);
    }

    YAML::Node indirect_edges = dag_node["indirect_edges"];
    for (int j = 0; j < indirect_edges.size(); ++j) {
        int from = id_pos[indirect_edges[j]["from"].as<int>()];
        int to = id_pos[indirect_edges[j]["to"].as<int>()];

        indirect_edge_list.emplace_back(from, to);
    }

    g.resize(num_vertices, false); // Resize graph after all nodes are known

    // Fill adjacency lists with gathered data

    for (const auto& edge : edge_list) {
        int from = id_pos[edge.first];
        int to = id_pos[edge.second];
        g.add_edge(from, to);
    }

    for (const auto& edge : indirect_edge_list) {
        int from = id_pos[edge.first];
        int to = id_pos[edge.second];
        g.add_indirect_edge(from, to);
    }

    g.print_graph("Graph from YAML");
}


void DAGParser::readDAGFromDOT(const std::string &filename, Graph& g) {
    std::ifstream dot_dag(filename);
    std::string line;

    std::unordered_map<int, int> id_pos;
    std::vector<std::pair<int, int>> edge_list;
    std::vector<std::pair<int, int>> indirect_edge_list;
    int node_count = 0;

    while (std::getline(dot_dag, line)) {
        dot_info di = parseDOTLine(line);

        if (di.lineType == DOTLine_t::DAG_INFO) {
            g.setPeriod(di.period);
            g.setDeadline(di.deadline);
        }
        else if (di.lineType == DOTLine_t::DOT_NODE) {
            int external_id = di.id;
            id_pos[external_id] = node_count++;

            Task t;
            t.id = external_id;
            t.wcet = di.wcet;
            g.tasks.push_back(t);
        }
        else if (di.lineType == DOTLine_t::DOT_EDGE) {
            edge_list.emplace_back(di.id_from, di.id_to);
        } else if(di.lineType == DOTLine_t::DOT_INDIRECT_EDGE) {
            indirect_edge_list.emplace_back(di.id_from, di.id_to);
        }
    }

    g.resize(node_count, false); // Resize graph after all nodes are known

    // Fill adjacency lists with gathered data

    for (const auto& edge : edge_list) {
        int from = id_pos[edge.first];
        int to = id_pos[edge.second];
        g.add_edge(from, to);
    }

    for (const auto& edge : indirect_edge_list) {
        int from = id_pos[edge.first];
        int to = id_pos[edge.second];
        g.add_indirect_edge(from, to);
    }

    g.print_graph("Graph from DOT");

    dot_dag.close();
}

void DAGParser::readMultipleDAGSFromYAML(const std::string& file_path, vector<Graph>& graphs) {
    YAML::Node config   = YAML::LoadFile(file_path);
    YAML::Node dags_node = config["dags"];

    for(int i=0; i<dags_node.size(); i++) {
        Graph g;
        readDAGFromYAML(dags_node[i], g);
        // Add parsed DAG graph to list of graphs
        graphs.push_back(g);
    }
}

void DAGParser::readMultipleDAGSFromDOT(const std::string& file_path, vector<Graph>& graphs) {
    ifstream dot_paths(file_path);
    string line;
    while (getline(dot_paths, line)){
        Graph g;
        readDAGFromDOT(line, g);

        // Add parsed DAG graph to list of graphs
        graphs.push_back(g);
    }

    dot_paths.close();
}