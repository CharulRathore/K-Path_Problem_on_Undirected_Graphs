#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

typedef pair<int, double> EDGE;
typedef unordered_map<int, vector<EDGE>> GRAPH;

class GraphLib {
private:
    GRAPH graph;
    string graph_input_file;

    struct myVHash {
        size_t operator()(const vector<int> &myVector) const {
            std::hash<int> hasher;
            size_t answer = 0;
        
            for (int i : myVector) {
                answer ^= hasher(i) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
            }
            return answer;
        } 
    };

    unordered_set<vector<int>, myVHash> visited_permutations;
    vector<int> vertices;
    vector<char> colors;

private:
    vector<vector<int>> readMatrixFromFile();
    void printGraphMatrix(const vector<vector<int>> & matrix);
    vector<int> & permuteVertices();

public:
    GraphLib() {
        vector<char> c{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                        'K', 'L', 'M', 'N', 'O', 'P'};
        colors = c;
    }
    bool createGraph(const string & filepath);
    GRAPH & getGraph() {
        return graph;

    }
    vector<int> & getVertices() {
        return vertices;
    }
    void createDAG(GRAPH & dag);
    void colorGraph(unordered_map<int, char> & colorMap, int k);
    void printDAG(GRAPH & dag);
    void topologicalSort(GRAPH & dag, vector<int> & tvs);
};