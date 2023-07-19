#include <iostream>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

class GraphGenerator {
    random_device rd;
    mt19937 gen;

public:
    GraphGenerator() : gen(rd()) {}

    void writeMatrixToFile(const vector<vector<int>>& matrix, const string& filePath) {
        ofstream file(filePath);

        if (file.is_open()) {
            for (const auto& row : matrix) {
                for (const auto& element : row) {
                    file << element << ' ';
                }
                file << '\n';
            }
            cout << "Matrix written to file: " << filePath << endl;
            file.close();
        } else {
            cerr << "Error writing to file: " << filePath << endl;
        }
    }

    vector<vector<int>> generateRandomGraph(int numVertices, int numEdges, int weightRangeMin, int weightRangeMax) {
        if (numEdges > numVertices * (numVertices - 1) / 2) {
            cerr << "Invalid number of edges. Maximum possible edges for " << numVertices << " vertices is " << numVertices * (numVertices - 1) / 2 << endl;
            return vector<vector<int>>();
        }

        vector<vector<int>> graph(numVertices, vector<int>(numVertices, 0));

        uniform_int_distribution<> distribution(0, numVertices - 1);
        uniform_int_distribution<> weightDistribution(weightRangeMin, weightRangeMax);
        int edgeCount = 0;

        while (edgeCount < numEdges) {
            int source = distribution(gen);
            int destination = distribution(gen);

            if (source != destination && graph[source][destination] == 0) {
                int weight = weightDistribution(gen);
                graph[source][destination] = weight;
                graph[destination][source] = weight;
                edgeCount++;
            }
        }
        string filename = "graph_with_" + to_string(numVertices) + "V_" + to_string(numEdges) + "E.txt";
        string filePath = "./graph_dataset/"+ filename;
        writeMatrixToFile(graph, filePath);

        return graph;
    }
};

void printGraph(const vector<vector<int>>& graph) {
    int numVertices = graph.size();

    cout << "Graph adjacency matrix:" << endl;
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            cout << graph[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    GraphGenerator graphGenerator;

    int numVertices, numEdges, weightRangeMin, weightRangeMax;
    cout << "Enter the number of vertices: ";
    cin >> numVertices;
    cout << "Enter the number of edges: ";
    cin >> numEdges;
    cout << "Enter the minimum weight: ";
    cin >> weightRangeMin;
    cout << "Enter the maximum weight: ";
    cin >> weightRangeMax;

    vector<vector<int>> graph = graphGenerator.generateRandomGraph(numVertices, numEdges, weightRangeMin, weightRangeMax);

    if (graph.empty()) {
        return 0;
    }

    ofstream MyFile("filename.txt");

    MyFile.close();

    printGraph(graph);

    return 0;
}
