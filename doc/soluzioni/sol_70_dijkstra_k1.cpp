/**
 * Soluzione intermedia con dijkstra assume K<=1, archi pesati
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
vector<vector<int>> graph;          // graph[s] contiene un vettore con gli indici degli archi da s ai vicini
vector<pair<int,int>> edge_dir;     // edge edge_dir.first->edge_dir.second
vector<pair<int,int>> edge_range;   // se arco senza ventole edge_range.first=edge_range.second
vector<int> parent_min;
vector<int> parent_max;
vector<int> parent;


uint64_t dijkstra_try_min(int source, int target){
    parent_min.assign(N,-1);
    vector<ll> dist(N,INT_MAX);
    set<pair<ll,int>> Q;
    dist[source] = 0; Q.insert({0,source});

    while(Q.size()){
        auto it = *Q.begin();
        Q.erase(Q.begin());
        int v=it.second;
        for (int i : graph[v]){
            int u = edge_dir[i].second;
            if (dist[u] > dist[v] + edge_range[i].first){
                Q.erase({dist[u], u});
                parent_min[u]=v;
                dist[u]=dist[v]+edge_range[i].first;
                Q.insert({dist[u],u});
            }
        }
    }
    return dist[target];
}

uint64_t dijkstra_try_max(int source, int target){
    parent_max.assign(N,-1);
    vector<ll> dist(N,INT_MAX);
    set<pair<ll,int>> Q;
    dist[source] = 0; Q.insert({0,source});

    while(Q.size()){
        auto it = *Q.begin();
        Q.erase(Q.begin());
        int v=it.second;
        for (int i : graph[v]){
            int u = edge_dir[i].second;
            if (dist[u] > dist[v] + edge_range[i].second){
                Q.erase({dist[u], u});
                parent_max[u]=v;
                dist[u]=dist[v]+edge_range[i].second;
                Q.insert({dist[u],u});
            }
        }
    }
    return dist[target];
}

int main(){
    in >> N >> M >> K;
    in >> s1 >> s2 >> f;

    graph.resize(N); parent_min.resize(N); parent_max.resize(N);
    parent.resize(N); 
    edge_dir.resize(M+K);
    edge_range.resize(M+K);
    
    int u, v, w, w_low, w_high;
    for(int i = 0 ; i < M ; i++){
        in >> u >> v >> w;  
        graph[u].push_back(i);
        edge_dir[i]=make_pair(u, v);
        edge_range[i]=make_pair(w,w);
    } 
    for(int i = M ; i < M+K ; i++){
        in >> u >> v >> w_low >> w_high;      
        graph[u].push_back(i);
        edge_dir[i]=make_pair(u, v);
        edge_range[i]=make_pair(w_low, w_high);
    }

    // Tento ventola al minimo
    int dist_s2_min = dijkstra_try_min(s2, f);
    int dist_s1_min = dijkstra_try_min(s1, f);
    int winner_min;
    
    if (dist_s1_min < dist_s2_min){
        winner_min = 1;
    } else if (dist_s2_min < dist_s1_min){
        winner_min = 2;
    } else{
        winner_min = 0;             
    }
    // Tento ventola al massimo
    int dist_s2_max = dijkstra_try_max(s2, f);
    int dist_s1_max = dijkstra_try_max(s1, f);
    int winner_max;
    
    if (dist_s1_max < dist_s2_max){
        winner_max = 1;
    } else if (dist_s2_max < dist_s1_max){
        winner_max = 2;
    } else{
        winner_max = 0;             // draw
    }

    // Scelta peso
    int winner, dist_s1, dist_s2, peso_ventola;
    if (dist_s1_min < dist_s2_min)  {           // conviene mettere al minimo, vince I
        winner = 1;
        parent = parent_min;
        dist_s1 = dist_s1_min;
        dist_s2 = dist_s2_min;
        peso_ventola = edge_range[M+K-1].first;
    } else if (dist_s1_max < dist_s2_max)  {    // conviene mettere al massimo, vince I
        winner = 1;
        parent = parent_max;
        dist_s1 = dist_s1_max;
        dist_s2 = dist_s2_max;
        peso_ventola = edge_range[M+K-1].second;
    } else if (dist_s1_min == dist_s2_min)  {   // conviene mettere al minimo, pareggio
        winner = 0;
        parent = parent_min;
        dist_s1 = dist_s1_min;
        dist_s2 = dist_s2_min;
        peso_ventola = edge_range[M+K-1].first;
    } else if (dist_s1_max == dist_s2_max)  {   // conviene mettere al massimo, pareggio
        winner = 0;
        parent = parent_max;
        dist_s1 = dist_s1_max;
        dist_s2 = dist_s2_max;
        peso_ventola = edge_range[M+K-1].second;
    } else {                                    // vincono sempre gli studenti, la scelta e' indifferente
        winner = 2;
        parent = parent_max;
        dist_s1 = dist_s1_max;
        dist_s2 = dist_s2_max;
        peso_ventola = edge_range[M+K-1].second;
    }

    // stampa output

    out << winner << endl;
    out << dist_s1 << " " << dist_s2 << endl;
    if (K==0) {
        cout << endl;
    } else {    // K=1
        cout << peso_ventola << endl;
    }

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