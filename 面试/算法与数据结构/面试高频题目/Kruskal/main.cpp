#include <iostream>
#include <algorithm>
#define MAX 105
using std::cin;
using std::cout;
using std::sort;
using std::endl;
struct edge {
    int x, y;
    int w;
}e[MAX];

int fa[MAX];
int rank[MAX];
int sum = 0;

// 初始化节点
void make_set(int x) {
    fa[x] = x;
    rank[x] = 0;
}
// 查找父节点
int find(int x) {
    return fa[x] == x ? x : fa[x] = find(fa[x]);
}

// 合并结点
void union_set(int x, int y, int w) {
    if (rank[x] > rank[y]) {
        rank[y] = x;
    } else if (rank[x] < rank[y]) {
        rank[x] = y;
    } else {
        rank[x]++;
        rank[y] = x;
    }
    // 总权值加上w
    sum += w;
}
int main()
{
    int x, y, w;
    int m, n; // n个点， m条边
    cin >> n >> m;
    for (int i = 0; i < m; ++i) {
        cin >> x >> y >> w;
        e[i].x = x;
        e[i].y = y;
        e[i].w = w;
        make_set(x);
        make_set(y);
    }
    sort(e, e + m, [](edge& a, edge& b){
        return a.w == b.w ? a.x < b.x : a.w < b.w;
    });

    for (int i = 0; i < n; ++i) {
        x = find(e[i].x);
        y = find(e[i].y);
        w = e[i].w;
        if (x != y) {
            union_set(x, y, w);
        }
    }
    cout << sum << endl;
    return 0;
}
