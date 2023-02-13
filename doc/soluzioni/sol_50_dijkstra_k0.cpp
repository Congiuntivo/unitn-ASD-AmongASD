/**
 * Soluzione intermedia con dijkstra
 * assume K=0, archi pesati
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

typedef uint64_t ll;


int N, M, K, s1, s2, f;
vector<vector<int>> graph;          // graph[s] contiene un vettoree con gli indici degli archi da s ai vicini
vector<pair<int,int>> edge_dir;     // edge edge_dir.first->edge_dir.second
//vector<pair<int,int>> edge_range;   // with weight in [edge_weight.first, edge_weight.second]
vector<int> edge_weight;
vector<int> parent;

uint64_t dijkstra(int source, int target){
    parent.assign(N,-1);
    vector<ll> dist(N,INT_MAX);
    set<pair<ll,int>> Q;
    dist[source] = 0; Q.insert({0,source});

    while(Q.size()){
        auto it = *Q.begin();
        Q.erase(Q.begin());
        int v=it.second;
        for (int i : graph[v]){
            int u = edge_dir[i].second;
            if (dist[u] > dist[v] + edge_weight[i]){
                Q.erase({dist[u], u});
                parent[u]=v;
                dist[u]=dist[v]+edge_weight[i];
                Q.insert({dist[u],u});
            }
        }
    }
    return dist[target];
}
int main(){
    in >> N >> M >> K;
    in >> s1 >> s2 >> f;

    graph.resize(N); parent.resize(N);
    edge_dir.resize(M+K);
    edge_weight.resize(M+K);
    //edge_range.resize(M+K);
    int u, v, w, w_low, w_high;
    for(int i = 0 ; i < M ; i++){
        in >> u >> v >> w;  
        graph[u].push_back(i);
        edge_dir[i]=make_pair(u, v);
        //edge_range[i]=make_pair(w,w);
        edge_weight[i]=w;
    } 
    // K=0, no need for this
    // for(int i = M ; i < M+K ; i++){
    //     in >> u >> v >> w_low >> w_high;      
    //     graph[u].push_back(i);
    //     edge_dir[i]=make_pair(u, v);
    //     edge_range[i]=make_pair(w_low, w_high);
    //     edge_weight[i]=w_low;
    // }

    int dist_s2 = dijkstra(s2, f);
    int dist_s1 = dijkstra(s1, f);
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
    // K=0 hence we do not set weights

    int node=f;
    int player=s1;
    vector<int> path;
    while (node!=player){
        path.push_back(node);
        node=parent[node];
    }
    path.push_back(player);
    out << path.size() << endl;
    for (int i=path.size()-1; i >=0; i--){
        out << path[i] << " " ;
    } out << endl;

    return 0;
}