/**
 * Soluzione per la sufficienza
 * assume K=0, archi non pesati
 */
#include <bits/stdc++.h>
using namespace std;

#ifdef EVAL
ifstream in("input.txt");
ofstream out("output.txt");
#else
istream &in(cin);
ostream &out(cout);
#endif

int N, M, K, s1, s2, f;
vector<vector<int>> graph;

int erdos(int source, int target, vector<int> & parent){
    vector<int> d(N,-1);
    queue<int> q;
    q.push(source);
    d[source] = 0;

    int u, v;
    while(!q.empty()) {
        u = q.front();
        q.pop();
        for(int i = 0; i<graph[u].size(); i++){
            v = graph[u][i];
            if (d[v] == -1) {
                d[v] = d[u]+1;
                q.push(v);
                parent[v]=u;
            }
            if(v == target){
                return d[v];
                // stop
            }
        }
    } 
    return -1;
}

int main(){

    in >> N >> M >> K;
    in >> s1 >> s2 >> f;

    graph.resize(N);
    vector<int> parent(N,-1);

    int u, v, w;
    for(int i = 0 ; i < M ; i++){
        in >> u >> v >> w;      // w=1 per i casi base
        graph[u].push_back(v);
    }

    int dist_s1 = erdos(s1, f, parent);
    // calcolo percorso di s1
    int node = f;
    vector<int> path;
    while (node!=s1){
        path.push_back(node);
        node=parent[node];
    }path.push_back(s1);

    int dist_s2 = erdos(s2, f, parent);
    int winner;
    
    if (dist_s1 < dist_s2){
        winner = 1;
    } else if (dist_s2 < dist_s1){
        winner = 2;
    } else{
        winner = 0;             // draw
    }
    out << winner << endl;
    out << dist_s1 << " " << dist_s2 << endl;

    out << endl;                // riga vuota perchè K=0
    out << dist_s1+1 << endl;     // numero di nodi nel percorso di s1

    // stampa percorso s1-->f
    for(int i=path.size()-1;i>=0;i--){
        out << path[i] << " ";
    }
    out << endl;


    return 0;
}