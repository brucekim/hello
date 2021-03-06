/* time limit <= 1sec
 * weight w, w <= 10 자연수
 * vertexes V, edges E (1<=V<=20'000, 1<=E<=300'000)
 *
 * 시간제한이 1초여서,
 * min edge선택할 때, for loop으로 V번 수행해야 된다면
 * 20'000 * 300'000 = 6'000'000'000 => 1초 넘김
 * min edge를 뽑을 때 minheap으로 해야 lgV 수행하면 되소,
 * lgV 약 15, 15 * 300'000 = 4'500'000 -> 1초 안넘김
 *
 * input
 * V E
 * K (1<=K<=V)
 * u v w
 * ...
 *
 * output
 * value(the distance of shortest path from k to ith vertex)
 * 0 (if ith = k)
 * INF (no shortest path)
 */
#include <cstdio>

#define DBG_STDIN
#define DBG

#ifdef DBG
#define dbg(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define dbg(fmt, ...)
#endif

const static int const_max_sz = 300001;
const static int const_err_code = 0x3f3f3f3f;
const static int INF = 0x7FFFFFFF;

struct vertex {
    vertex(int _v=0, int _w=0) : v(_v), w(_w) {}
    int v, w;
};

template <typename T>
struct node {
    node(T &_data) : data(_data), next(nullptr) {}
    T data;
    node<T> *next;
};

template <typename T>
class xlist_iterator {
    public:
    xlist_iterator(node<T> *it) : current(it) {}
    T& operator*() {
        return current->data;
    }
    xlist_iterator& operator++() {
        current = current->next;
        return *this;
    }
    bool operator==(const xlist_iterator<T>& it) const {
        return current == it.current;
    }
    bool operator!=(const xlist_iterator<T>& it) const {
        return current != it.current;
    }
    private:
    node<T> *current;
};

template <typename T>
class xlist {
    public:
        using iterator = xlist_iterator<T>;
        xlist() : sz(0), head(nullptr), tail(nullptr) {}
        virtual ~xlist() {
            while(head != nullptr) {
                node<T> *tmp = head;
                head = head->next;
                delete tmp;
            }
        }
        iterator begin() {
            return xlist_iterator<T>(head);
        }
        iterator end() {
            return xlist_iterator<T>(nullptr);
        }
        int push_back(T data) {
            node<T> *tmp = new node<T>(data);
            ++sz;
            if (head == nullptr) {
                head = tail = tmp;
            } else {
                tail->next = tmp;
                tail = tmp;
            }
            return 0;
       }
        int getSize() {
            return sz;
        }
        node<T> *getHead() {
            return head;
        }
        bool isEmpty() {
            return sz == 0;
        }

    private:
        int sz;
        node<T> *head;
        node<T> *tail;
};

// adjacent graph
class graph {
    public:
        void init(int _cntV) {
            cntV = _cntV;
            adj = new xlist<vertex>[cntV]();
       }
        virtual ~graph() {
            delete[] adj;
        }
        void addEdge(int u, int v, int w) {
            adj[u].push_back(vertex(v, w));
        }
        xlist<vertex>& operator[] (int idx) {
            return adj[idx];
        }
    private:
        xlist<vertex>* adj;
        int cntV;
};

template <typename T>
struct pv {
    pv(int _p=0, T _v=T()) : p(_p), v(_v) {}
    int p;
    T v;
};
template <typename T>
class priority_queue{
    public:
        priority_queue(int _capa=const_max_sz) : capa(_capa), sz(0) {
            arr = new pv<T>[capa]();
        }
        virtual ~priority_queue() {
            delete[] arr;
        }
        T& top() {
            return arr[0].v;
        }
        int push(int p, T& v) {
            pv<T> _pv(p, v);

            if (sz == capa) {
                return const_err_code;
            }
            int i = sz++;

            arr[i] = _pv;
            while(i != 0 && arr[i].p < arr[parent(i)].p) {
                swap(arr[parent(i)], arr[i]);
                i = parent(i);
            }
            return 0;
        }
        int pop() {
            if (sz == 0) {
                return const_err_code;
            } else if (sz == 1) {
                --sz;
            } else {
                arr[0] = arr[sz-1];
                --sz;
                heapify(0);
            }
            return 0;
        }
        bool isEmpty() {
            return sz == 0;
        }
        int getSize() {
            return sz;
        }
    private:
        int parent(int i) { return (i-1)/2; }
        int left(int i) { return i*2+1; }
        int right(int i) { return i*2+2; }
        void heapify(int v) {
            int smallest = v;
            int l = left(v);
            int r = right(v);

            if (l < sz && arr[l].p < arr[smallest].p) {
                smallest = l;
            }
            if (r < sz && arr[r].p < arr[smallest].p) {
                smallest = r;
            }
            if (smallest != v) {
                swap(arr[smallest], arr[v]);
                heapify(smallest);
            }
        }
        void swap(pv<T> &a, pv<T> &b) {
            pv<T> tmp = a;
            a = b;
            b = tmp;
        }
 
        pv<T> *arr;
        int capa, sz;
};

class solver {
    public:
        solver() {
            //input
            scanf("%d %d", &cntV, &cntE);
            scanf("%d", &k);
            g.init(cntV+1);
            for(int i=0; i<cntE; ++i) {
                int u, v, w;
                scanf("%d %d %d", &u, &v, &w);
                g.addEdge(u, v, w);
            }

            //init
            sptSet = new bool[cntV+1];
            dist = new int[cntV+1];
            for(int i=0; i<cntV+1; ++i) {
                sptSet[i] = false;
                dist[i] = INF;
            }
 
        }
        virtual ~solver() {
            delete[] dist;
            delete[] sptSet;
        }
        void run() {
#ifdef DBG
            info();
#endif
            dijkstra_pq(k);
            //dijkstra(k);
            printspt();
        }
        void dijkstra(int s) {
            dist[s] = 0;

            //for all vertexes
            for(int cnt=0; cnt<cntV-1; ++cnt) {
                //pick a vertex that has min edge in sptSet
                int u = getMinimumEdge();
                if (u == -1) {
                    continue;
                }
                sptSet[u] = true;
                dbg("pick %d as min and insert to sptSet\n", u);
                //traverse all adjacent nodes of the vertex picked
                xlist<vertex>::iterator _end  = g[u].end();
                xlist<vertex>::iterator it = g[u].begin();
                for(; it != _end; ++it) {
                    int v = (*it).v;
                    int w = (*it).w;
                    dbg("search neighbor %d .. dist[%d]=%d, w=%d <? dist[%d]=%d\n", v, u, dist[u], w, v, dist[v]);
                    if (sptSet[v] == false && dist[u] != INF && dist[u] + w < dist[v]) {
                        dist[v] = dist[u] + w;
                        dbg("update dist[%d] to %d(dist[%d] + %d\n", v, dist[u] + w, u, w);
                    }
                }
            }
        }
        int getMinimumEdge() {
            //iterative -> todo priority_queue
            int min = INF;
            int u = -1;
            for(int i=1; i<cntV+1; ++i) {
                if (sptSet[i] == false && dist[i] < min) {
                    min = dist[i];
                    u = i;
                }
            }
            return u;
        }
        void dijkstra_pq(int s) {
            priority_queue<int> pq;
            dist[s] = 0;
            pq.push(0, s);

            while(!pq.isEmpty()) {
                //pick a vertex that has min edge in sptSet
                int u = pq.top();
                pq.pop();

                sptSet[u] = true;
                dbg("pick %d as min and insert to sptSet\n", u);
                //traverse all adjacent nodes of the vertex picked
                xlist<vertex>::iterator _end  = g[u].end();
                xlist<vertex>::iterator it = g[u].begin();
                for(; it != _end; ++it) {
                    int v = (*it).v;
                    int w = (*it).w;
                    dbg("search neighbor %d .. dist[%d]=%d, w=%d <? dist[%d]=%d\n", v, u, dist[u], w, v, dist[v]);
                    if (sptSet[v] == false && dist[u] != INF && dist[u] + w < dist[v]) {
                        dist[v] = dist[u] + w;
                        pq.push(dist[v], v);
                        dbg("update dist[%d] to %d(dist[%d] + %d\n", v, dist[u] + w, u, w);
                    }
                }
            }
        }
 

        void info() {
            printf("info.. V:%d E:%d k:%d\n", cntV, cntE, k);
            for(int i=1; i<=cntV; ++i) {
                printf("%d(sz:%d) -> ", i, g[i].getSize());
                xlist<vertex>::iterator _end = g[i].end();
                for(xlist<vertex>::iterator it = g[i].begin(); it != _end; ++it) {
                    printf("%d(%d) ", (*it).v, (*it).w);
                }
               printf("\n");
            }
        }
        void printspt() {
            for (int i=1; i<=cntV; ++i) {
                if (dist[i] == INF) {
                    printf("INF\n");
                } else {
                    printf("%d\n", dist[i]);
                }
            }
        }
    private:
        int cntV, cntE, k;
        bool *sptSet;
        int *dist;
        graph g;
};

void solve() {
    solver sol;
    sol.run();
}

int main() {
#ifdef DBG_STDIN
    freopen("inputs/acmicpc_1753_input.txt", "r", stdin);
#endif
    solve();
    return 0;
}
