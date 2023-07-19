#include "graph.h"
#include <random>
#include <algorithm>
#include <queue>
#include <ctime>

using namespace std;

typedef pair<int, double> EDGE;
typedef unordered_map<int, vector<EDGE>> GRAPH;

vector<vector<int>> GraphLib::readMatrixFromFile() {
    ifstream file(graph_input_file);
    vector<vector<int>> matrix;

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            vector<int> row;
            istringstream iss(line);
            int value;
            while (iss >> value) {
                row.push_back(value);
            }
            matrix.push_back(row);
        }
        file.close();
    } else {
        cerr << "Error reading file: " << graph_input_file << endl;
    }
    for (int i=1; i<=(int)matrix.size(); i++) {
        vertices.push_back(i);
    }

    //printGraphMatrix(matrix);

    return matrix;
}

void GraphLib::printGraphMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (const auto& element : row) {
            cout << element << ' ';
        }
        cout << '\n';
    }
}

bool GraphLib::createGraph(const string & filepath) {
    graph_input_file = filepath;
    vector<vector<int>> graph_matrix = readMatrixFromFile();
    int rows = (int)graph_matrix.size();
    int cols = (int)graph_matrix[0].size();  

    if (rows != cols) {
        cout << "Problem in creating graph" <<endl;
        return 0;
    }

    for (int i=1; i<=rows; i++) {
        for (int j=1; j<=cols; j++) {
            if (graph_matrix[i-1][j-1] != 0) {
                graph[i].push_back(make_pair(j, graph_matrix[i-1][j-1]));
            }
        }
    }
    return true;
}

vector<int> & GraphLib::permuteVertices() {
    static vector<int> vs;    
    if (vs.empty()) {
        vs = vertices;
    }

    // Seed the random number generator
    random_device rd;
    mt19937 gen(rd());

    // Shuffle the digits randomly
    shuffle(vs.begin(), vs.end(), gen);

    // check if we already get this permutation
    while (visited_permutations.find(vs) != visited_permutations.end()) {
        shuffle(vs.begin(), vs.end(), gen);   
    }

    visited_permutations.insert(vs);
    return vs;
}

void GraphLib::createDAG(GRAPH & dag) {
    vector<int> pvs = permuteVertices();
    unordered_set<int> visited;

    for (int v : pvs) {
        visited.insert(v);
        vector<EDGE> edges = graph[v];
        for (auto edge : edges) {
            if (visited.find(edge.first) == visited.end()) {
                dag[v].push_back(edge);
            }
        }
    }
}

void GraphLib::printDAG(GRAPH & dag) {
    cout << "DAG is:" << endl;
    for (GRAPH::iterator it = dag.begin(); it != dag.end(); it++) {
        int src = it->first;
        cout << src << " {";
        vector<EDGE> edges = it->second;
        for (auto edge: edges) {
            cout << edge.first << " , ";
        }
        cout << "}" << endl;
    }
}

void GraphLib::topologicalSort(GRAPH & dag, vector<int> & tvs) {
    //cout << "TP debug-----" << endl;

    // filling the vector with zero initially
    unordered_map<int, int> indegree_count;
    for (int v : vertices) {
        indegree_count[v] = 0;
    }

    for(int v : vertices) {
      if (dag.find(v) == dag.end()) {
        continue;
      }
      vector<EDGE> edges = dag[v];
      for(auto edge : edges) {
        indegree_count[edge.first] += 1;
      }
    }

    /*cout << "indegree count" << endl;
    for (auto x : indegree_count) {
        cout << x.first << "----" << x.second << endl;
    }*/

    queue<int> Q;
    for(int v : vertices){
      if(indegree_count.find(v) != indegree_count.end() && indegree_count[v]==0){
        Q.push(v);
      }
    }

    int visited_node = 0;
    while(!Q.empty()){
      int u = Q.front();
      Q.pop();
      tvs.push_back(u); 
      vector<EDGE>::iterator itr;
      for(itr=dag[u].begin(); itr!=dag[u].end();itr++){
        int dest = (*itr).first;
        if (indegree_count.find(dest) != indegree_count.end()) {
            indegree_count[dest] -= 1;
            if (indegree_count[dest] == 0) {
                Q.push(dest);
            }
        }
      }

      visited_node++;
    }

    if(visited_node != vertices.size()){
      cout<<"There's a cycle present in the Graph.\nGiven graph is not DAG"<<endl;
      return;
    }

    /*cout << endl<< "topological sorted order" <<endl;
    for(int i=0; i<tvs.size();i++){
      cout<<tvs[i]<<"\t";
    }
    cout << endl;*/
}


void GraphLib::colorGraph(unordered_map<int, char> & colorMap, int k) {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator
    
    // Create a random number generator and uniform distribution
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, k - 1);
    
    for (int v : vertices) {
        int randomIndex = dist(gen); // Generate a random index within the range of the array
        char randomColor = colors[randomIndex]; // Retrieve the color at the random index
        colorMap[v] = randomColor;
    }
}