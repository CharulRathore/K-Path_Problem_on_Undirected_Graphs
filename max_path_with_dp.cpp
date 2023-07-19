#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
#include "graph.h"

using namespace std;
using namespace std::chrono;

typedef unordered_map<int, unordered_map<int, double>> WEIGHTS_MAP;
typedef unordered_map<int, unordered_map<int, int>> PREV_VERTICES_MAP;

void initialize_weights(vector<int> & tvs, WEIGHTS_MAP & weights, PREV_VERTICES_MAP & prev_vertices, int k) {
    for (int i=0; i < (int)tvs.size()-1; i++) {
        int vertex = tvs[i];
        weights[vertex][1] = 0;
        prev_vertices[vertex][1] = -1;
        for (int h = 2; h<=k; h++) {
            weights[vertex][h] = -9999;
            prev_vertices[vertex][h] = -1;
        }
    }
}

int dagMaxPath(GRAPH & graph, vector<int> & tvs, int k) {
    WEIGHTS_MAP weights;
    PREV_VERTICES_MAP prev_vertices;

    initialize_weights(tvs, weights, prev_vertices, k);

    for (int i=0; i<(int)tvs.size()-1; i++) {
        int vertex = tvs[i];
        vector<EDGE> edges = graph[vertex];
        for (vector<EDGE>::iterator eit = edges.begin(); eit != edges.end(); eit++) {
            EDGE edge = *eit;
            for (int h=1; h<k; h++) {
                int next_v = edge.first;
                double cost = edge.second;
                if (weights[vertex][h] != -9999 &&  
                         weights[vertex][h] + cost > weights[next_v][h+1]) {
                    weights[next_v][h+1] = weights[vertex][h] + cost;
                    prev_vertices[next_v][h+1] = vertex;            
                }
            }
        }
    }

    int max = -999;
    for (int i=0; i < (int)tvs.size(); i++) {
        int vertex = tvs[i];
        if (weights[vertex][k] > max) {
            max = weights[vertex][k];
        }
    }

    return max;
} 

unsigned long long int fact(int n) {
    if (n==1) {
        return 1;
    }
    return n * fact(n-1);
}

int main() {
    int k = 9;
    string graphInputFile = "./graph_dataset/graph_with_20V_25E.txt";

    GraphLib g;
    if (!g.createGraph(graphInputFile)) {
        cout << "unable to create graph" << endl;
        return 0;
    }

    
    GRAPH graph = g.getGraph(); // got an undirected graph
    int global_max = -9999;

    //time START
    auto start = high_resolution_clock::now();
    for (unsigned long long int i=0; i < 10 * fact(k); i++) {
        // cout << "START" << endl;
        GRAPH dag; 
        g.createDAG(dag); // create DAG from undirected graph
        //g.printDAG(dag);
        // cout << "createDAG" << endl;
       
        vector<int> tvs; // tvs is the vector of vertices in topologically sorted order
        g.topologicalSort(dag, tvs);
        // cout << "topologicalSort" << endl;
        int maxKPath = dagMaxPath(dag, tvs, k);
        // cout << "dagMaxPath" << endl;
        if (maxKPath > global_max) {
            global_max = maxKPath;
        }
        cout << i << endl;
    }
    //time STOP
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
 
    cout << "Time taken by function: " << duration.count() << "milliseconds" << endl;

    cout << endl << "maxPath = " << global_max << endl;
    return 0;
}