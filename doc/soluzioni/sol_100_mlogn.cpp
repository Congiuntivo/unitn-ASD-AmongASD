/**
 * Complexity: O(m * log(n))
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

#define INF ULONG_MAX

typedef uint64_t ll;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef pair<ll, int> li;
typedef vector<vector<int>> vvi;

enum Ans {
    D, W, L
};

struct Edge {
    int from, to;
    ll l, r, w;

    Edge(int from, int to, ll l, ll r) : from(from), to(to), l(l), r(r), w(0) {}

    Edge() : Edge(-1, -1, -1, -1) {}
};

typedef vector<Edge> ve;

// Global variables
int n;    // # nodes
int m;    // # fixed-length edges
int k;    // # variable-length edges
int s1;   // 1st player's starting node
int s2;   // 2nd player's starting node
int f;    // destination node
vvi g;    // graph
ve edges; // vector of edges

/**
 * Find distance from node 'from' to every other node using the Dijkstra's algorithm
 *
 * @param from starting node
 * @param dist vector filled with the distances
 */
 /**
  * Run Dijkstra's algorithm starting from both from1 and from2, taking decisions on variable-length edges
  * according to who arrives first at each node:
  * - if the first player arrives first, we set the weight of the variable-length edge to its minimum value
  * - if the second player arrives first, we set the weight of the variable-length edge to its maximum value
  * - if both players arrive at the same time, we set the weight of the variable-length edge according to the
  *  value of the 'try_to_win' parameter: if we are trying to make the first player win, we set the weight to
  *  its minimum value, if we are looking for a draw, we set the weight to its maximum value.
  * @param from1 first player's starting node
  * @param from2 second player's starting node
  * @param dist1 vector filled with the distances of the first player
  * @param dist2 vector filled with the distances of the second player
  * @param parent1 vector filled with the parent of each node in the first player's shortest path
  * @param parent2 vector filled with the parent of each node in the second player's shortest path
  * @param try_to_win if true, we are trying to win, otherwise we are trying to draw
  */
void dij(int from1, int from2, vl &dist1, vl &dist2, vi &parent1, vi &parent2, bool try_to_win) {
    dist1.assign(n + 1, INF);
    dist2.assign(n + 1, INF);
    parent1.assign(n + 1, -1);
    parent2.assign(n + 1, -1);
    dist1[from1] = 0;
    dist2[from2] = 0;
    parent1[from1] = from1;
    parent2[from2] = from2;
    priority_queue<li, vector<li>, greater<li>> q;
    q.emplace(0, from1);
    q.emplace(0, from2);
    while (!q.empty()) {
        int v = q.top().second;
        ll d_v = q.top().first;
        q.pop();
        if (d_v > dist1[v] && d_v > dist2[v]) {
            continue;
        }
        bool reach1 = (d_v == dist1[v]), reach2 = (d_v == dist2[v]);

        for (int e: g[v]) {
            Edge& edge = edges[e];
            int to = edge.to;
            if (edge.w == 0) { // decide how to set the weight:
                // if s1 is trying to win and arrives strictly before s2, or
                // if s1 is trying to draw and arrives before or at the same time as s2,
                // minimize the weight
                if ((try_to_win && !reach2) || (!try_to_win && reach1)) {
                    edge.w = edge.l;
                } else {
                    edge.w = edge.r;
                }
            }
            ll new_dist = d_v + edge.w;

            if ((reach1 && new_dist < dist1[to]) || (reach2 && new_dist < dist2[to])) {
                q.emplace(new_dist, to);
            }
            if (reach1 && new_dist < dist1[to]) {
                dist1[to] = new_dist;
                parent1[to] = v;
            }
            if (reach2 && new_dist < dist2[to]) {
                dist2[to] = new_dist;
                parent2[to] = v;
            }

        }
    }
}

Ans find_sol(int &d1, int &d2, vi &path1) {
    vl dist1, dist2;
    vi parent1, parent2;
    // try to win
    dij(s1, s2, dist1, dist2, parent1, parent2, true);
    if (dist1[f] >= dist2[f]) { // 1st player can't win, try to draw
        dij(s1, s2, dist1, dist2, parent1, parent2, false);
    }

    d1 = dist1[f];
    d2 = dist2[f];
    // fill path1 as a stack, then reverse it
    int cur = f;
    assert(parent1.size() >= s1);
    while (cur != s1) {
        path1.push_back(cur);
        cur = parent1[cur];
        assert(cur != -1);
    }
    path1.push_back(s1);
    reverse(path1.begin(), path1.end());
    return d1 < d2 ? W : dist1[f] == dist2[f] ? D : L;
}

int main(int argc, char *argv[]) {
    in >> n >> m >> k >> s1 >> s2 >> f;
    g.resize(n + 1);
    edges.resize(m + k);

    // read fixed-length edges
    for (int i = 0; i < m; i++) {
        int a, b;
        ll c;
        in >> a >> b >> c;
        if (a == b)
            continue;
        edges[i] = {a, b, c, c};
        g[a].emplace_back(i);
    }

    // read variable-length edges and set length to max value
    for (int i = m; i < m + k; i++) {
        int a, b;
        ll l, r;
        in >> a >> b >> l >> r;
        edges[i] = {a, b, l, r};
        if (a == b)
            continue;
        g[a].emplace_back(i);
    }

    // find and print solution
    int dist1, dist2;
    vi path1;
    Ans ans = find_sol(dist1, dist2, path1);

    out << endl;
    out << ans << endl;
    out << dist1 << " " << dist2 << endl;
    for (auto edge_it = edges.begin() + m; edge_it != edges.end(); edge_it++) {
        if (edge_it->w == 0)
            edge_it->w = edge_it->l;
        out << edge_it->w << " ";
    }
    out << endl << path1.size() << endl;
    for (int v: path1) {
        out << v << " ";
    }
    out << endl;

    return 0;
}
