#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

const int INF = numeric_limits<int>::max();

class MultistageGraph {
private:
    int stageCount;
    int nodeCount;
    vector<vector<int>> stageNodes;   // nodes belonging to each stage
    vector<int> stageOf;              // stage index of each node
    vector<vector<int>> weight;       // adjacency matrix, INF = no edge

public:
    MultistageGraph(const vector<int>& stageSizes) {
        stageCount = static_cast<int>(stageSizes.size());
        nodeCount = 0;
        stageNodes.resize(stageCount);

        for (int s = 0; s < stageCount; ++s) {
            for (int i = 0; i < stageSizes[s]; ++i) {
                stageNodes[s].push_back(nodeCount);
                stageOf.push_back(s);
                ++nodeCount;
            }
        }
        weight.assign(nodeCount, vector<int>(nodeCount, INF));
    }

    int totalNodes() const { return nodeCount; }

    bool addEdge(int u, int v, int w) {
        if (u < 0 || v < 0 || u >= nodeCount || v >= nodeCount || w < 0)
            return false;
        if (stageOf[v] != stageOf[u] + 1)   // edges only go to the next stage
            return false;
        weight[u][v] = w;
        return true;
    }

    void solve() const {
        int source = stageNodes.front().front();
        int sink = stageNodes.back().front();

        vector<int> dist(nodeCount, INF);
        vector<int> parent(nodeCount, -1);
        vector<long long> ways(nodeCount, 0);

        dist[source] = 0;
        ways[source] = 1;

        cout << "\nStage-wise DP table (cost from source):\n";
        cout << left << setw(8) << "Stage" << setw(8) << "Node"
             << setw(10) << "Cost" << "Parent\n";
        cout << string(34, '-') << '\n';
        cout << setw(8) << 0 << setw(8) << source << setw(10) << 0 << "-\n";

        for (int s = 1; s < stageCount; ++s) {
            for (int v : stageNodes[s]) {
                for (int u : stageNodes[s - 1]) {
                    if (weight[u][v] == INF || dist[u] == INF) continue;

                    int candidate = dist[u] + weight[u][v];
                    if (candidate < dist[v]) {
                        dist[v] = candidate;
                        parent[v] = u;
                        ways[v] = ways[u];
                    } else if (candidate == dist[v]) {
                        ways[v] += ways[u];
                    }
                }

                cout << setw(8) << s << setw(8) << v;
                if (dist[v] == INF) {
                    cout << setw(10) << "INF" << "-\n";
                } else {
                    cout << setw(10) << dist[v] << parent[v] << '\n';
                }
            }
        }

        if (dist[sink] == INF) {
            cout << "\nNo path from source to sink.\n";
            return;
        }

        vector<int> path;
        for (int cur = sink; cur != -1; cur = parent[cur])
            path.push_back(cur);
        reverse(path.begin(), path.end());

        cout << "\nShortest path: ";
        for (size_t i = 0; i < path.size(); ++i)
            cout << path[i] << (i + 1 < path.size() ? " -> " : "\n");
        cout << "Minimum cost : " << dist[sink] << '\n';
        cout << "Number of distinct shortest paths: " << ways[sink] << '\n';
    }
};

int main() {
    int k;
    cout << "Enter number of stages (at least 2): ";
    cin >> k;
    if (k < 2) {
        cerr << "Need at least 2 stages.\n";
        return 1;
    }

    vector<int> sizes(k);
    for (int s = 0; s < k; ++s) {
        cout << "Nodes in stage " << s << ": ";
        cin >> sizes[s];
        if (sizes[s] <= 0) {
            cerr << "Each stage needs at least one node.\n";
            return 1;
        }
    }
    if (sizes.front() != 1 || sizes.back() != 1) {
        cerr << "First and last stages must have exactly one node.\n";
        return 1;
    }

    MultistageGraph graph(sizes);
    cout << "Nodes are numbered 0.." << graph.totalNodes() - 1
         << " stage by stage.\n";

    int e;
    cout << "Enter number of edges: ";
    cin >> e;

    cout << "Enter each edge as: from to weight\n";
    for (int i = 0; i < e; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        if (!graph.addEdge(u, v, w)) {
            cerr << "Invalid edge (must go to the next stage, weight >= 0).\n";
            return 1;
        }
    }

    graph.solve();
    return 0;
}

/*
Sample input (4 stages, 8 nodes, 12 edges):
4
1 3 3 1
12
0 1 2
0 2 4
0 3 3
1 4 6
1 5 3
2 4 2
2 6 5
3 5 4
3 6 1
4 7 5
5 7 3
6 7 4

Expected: shortest path 0 -> 1 -> 5 -> 7, minimum cost 8, 2 distinct shortest paths.
*/
