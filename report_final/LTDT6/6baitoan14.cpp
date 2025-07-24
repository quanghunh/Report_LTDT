#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>

using namespace std;

const int INF = numeric_limits<int>::max();

// Cấu trúc biểu diễn một cạnh
struct Edge {
    int to;      // đỉnh đích
    int weight;  // trọng số cạnh
    int id;      // định danh cạnh (cho đa đồ thị)
    
    Edge(int t, int w, int i = 0) : to(t), weight(w), id(i) {}
};

// Cấu trúc cho hàng đợi ưu tiên trong thuật toán Dijkstra
struct Node {
    int vertex;    // đỉnh
    int distance;  // khoảng cách
    
    Node(int v, int d) : vertex(v), distance(d) {}
    
    // Cho min-heap (hàng đợi ưu tiên)
    bool operator>(const Node& other) const {
        return distance > other.distance;
    }
};

class Graph {
protected:
    int numVertices;                      // số đỉnh
    vector<vector<Edge>> adjList;         // danh sách kề
    
public:
    Graph(int n) : numVertices(n), adjList(n) {}
    
    virtual void addEdge(int u, int v, int weight, int edgeId = 0) = 0;
    virtual void printGraph() const = 0;
    virtual ~Graph() = default;
    
    // Cài đặt thuật toán Dijkstra chung cho tất cả loại đồ thị
    pair<vector<int>, vector<int>> dijkstra(int source) {
        vector<int> dist(numVertices, INF);           // mảng khoảng cách
        vector<int> parent(numVertices, -1);          // mảng cha
        vector<bool> visited(numVertices, false);     // mảng đánh dấu đã thăm
        
        // Hàng đợi ưu tiên lưu (khoảng cách, đỉnh)
        priority_queue<Node, vector<Node>, greater<Node>> pq;
        
        dist[source] = 0;
        pq.push(Node(source, 0));
        
        cout << "\nCác bước thực hiện thuật toán Dijkstra:\n";
        cout << "Bước | Hiện tại | Khoảng cách | Cha | Cập nhật kề\n";
        cout << "-----|----------|-------------|-----|---------------\n";
        
        int step = 1;
        
        while (!pq.empty()) {
            Node current = pq.top();
            pq.pop();
            
            int u = current.vertex;
            
            if (visited[u]) continue;
            visited[u] = true;
            
            cout << setw(4) << step++ << " |";
            cout << setw(9) << u << " |";
            cout << setw(12) << (dist[u] == INF ? -1 : dist[u]) << " |";
            cout << setw(4) << parent[u] << " | ";
            
            bool firstUpdate = true;
            
            // Thực hiện relaxation cho tất cả cạnh kề
            for (const Edge& edge : adjList[u]) {
                int v = edge.to;
                int weight = edge.weight;
                
                if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push(Node(v, dist[v]));
                    
                    if (!firstUpdate) cout << ", ";
                    cout << "(" << u << "->" << v << ":" << dist[v] << ")";
                    firstUpdate = false;
                }
            }
            
            if (firstUpdate) cout << "Không có";
            cout << "\n";
        }
        
        return {dist, parent};
    }
    
    // In đường đi ngắn nhất từ nguồn đến tất cả đỉnh khác
    void printShortestPaths(int source, const vector<int>& dist, const vector<int>& parent) {
        cout << "\nĐường đi ngắn nhất từ đỉnh " << source << ":\n";
        cout << "Đích | Khoảng cách | Đường đi\n";
        cout << "-----|-------------|----------\n";
        
        for (int i = 0; i < numVertices; i++) {
            if (i == source) continue;
            
            cout << setw(4) << i << " |";
            
            if (dist[i] == INF) {
                cout << setw(12) << "VÔ CỰC" << " | Không có đường đi\n";
            } else {
                cout << setw(12) << dist[i] << " | ";
                
                // Khôi phục đường đi
                vector<int> path;
                int curr = i;
                while (curr != -1) {
                    path.push_back(curr);
                    curr = parent[curr];
                }
                reverse(path.begin(), path.end());
                
                for (size_t j = 0; j < path.size(); j++) {
                    if (j > 0) cout << " -> ";
                    cout << path[j];
                }
                cout << "\n";
            }
        }
    }
};

// Bài toán 14: Đơn đồ thị (không có cạnh song song, không có khuyên)
class SimpleGraph : public Graph {
public:
    SimpleGraph(int n) : Graph(n) {}
    
    void addEdge(int u, int v, int weight, int edgeId = 0) override {
        // Kiểm tra cạnh đã tồn tại (cho đơn đồ thị)
        for (const Edge& edge : adjList[u]) {
            if (edge.to == v) {
                cout << "Cảnh báo: Cạnh " << u << "->" << v << " đã tồn tại trong đơn đồ thị!\n";
                return;
            }
        }
        
        adjList[u].emplace_back(v, weight);
        // Cho đơn đồ thị vô hướng, thêm cạnh ngược
        adjList[v].emplace_back(u, weight);
    }
    
    void printGraph() const override {
        cout << "\nDanh sách kề của Đơn đồ thị:\n";
        for (int i = 0; i < numVertices; i++) {
            cout << "Đỉnh " << i << ": ";
            for (const Edge& edge : adjList[i]) {
                cout << "(" << edge.to << ", " << edge.weight << ") ";
            }
            cout << "\n";
        }
    }
};

// Bài toán 15: Đa đồ thị (cho phép nhiều cạnh giữa cùng hai đỉnh)
class Multigraph : public Graph {
private:
    int nextEdgeId;
    
public:
    Multigraph(int n) : Graph(n), nextEdgeId(0) {}
    
    void addEdge(int u, int v, int weight, int edgeId = 0) override {
        int id = (edgeId == 0) ? ++nextEdgeId : edgeId;
        adjList[u].emplace_back(v, weight, id);
        // Cho đa đồ thị vô hướng, thêm cạnh ngược với cùng ID
        adjList[v].emplace_back(u, weight, id);
    }
    
    void printGraph() const override {
        cout << "\nDanh sách kề của Đa đồ thị:\n";
        for (int i = 0; i < numVertices; i++) {
            cout << "Đỉnh " << i << ": ";
            for (const Edge& edge : adjList[i]) {
                cout << "(" << edge.to << ", " << edge.weight << ", id:" << edge.id << ") ";
            }
            cout << "\n";
        }
    }
    
    // Phương thức phân tích cạnh song song
    void analyzeParallelEdges() {
        cout << "\nPhân tích cạnh song song:\n";
        unordered_map<string, vector<pair<int, int>>> edgeGroups; // khóa: "u-v", giá trị: (trọng số, id)
        
        for (int u = 0; u < numVertices; u++) {
            for (const Edge& edge : adjList[u]) {
                if (u <= edge.to) { // Tránh đếm trùng cho cạnh vô hướng
                    string key = to_string(u) + "-" + to_string(edge.to);
                    edgeGroups[key].emplace_back(edge.weight, edge.id);
                }
            }
        }
        
        for (const auto& group : edgeGroups) {
            if (group.second.size() > 1) {
                cout << "Cạnh song song giữa " << group.first << ": ";
                for (const auto& edge : group.second) {
                    cout << "(trọng số:" << edge.first << ", id:" << edge.second << ") ";
                }
                cout << "\n";
            }
        }
    }
};

// Bài toán 16: Đồ thị tổng quát (cho phép khuyên và cạnh song song)
class GeneralGraph : public Graph {
private:
    int nextEdgeId;
    unordered_set<int> selfLoops; // Theo dõi đỉnh có khuyên
    
public:
    GeneralGraph(int n) : Graph(n), nextEdgeId(0) {}
    
    void addEdge(int u, int v, int weight, int edgeId = 0) override {
        int id = (edgeId == 0) ? ++nextEdgeId : edgeId;
        adjList[u].emplace_back(v, weight, id);
        
        // Xử lý khuyên
        if (u == v) {
            selfLoops.insert(u);
            return; // Không thêm cạnh ngược cho khuyên
        }
        
        // Cho đồ thị tổng quát vô hướng, thêm cạnh ngược
        adjList[v].emplace_back(u, weight, id);
    }
    
    void printGraph() const override {
        cout << "\nDanh sách kề của Đồ thị tổng quát:\n";
        for (int i = 0; i < numVertices; i++) {
            cout << "Đỉnh " << i << ": ";
            for (const Edge& edge : adjList[i]) {
                cout << "(" << edge.to << ", " << edge.weight << ", id:" << edge.id;
                if (edge.to == i) cout << " [KHUYÊN]";
                cout << ") ";
            }
            cout << "\n";
        }
    }
    
    void analyzeSelfLoops() {
        cout << "\nPhân tích khuyên:\n";
        if (selfLoops.empty()) {
            cout << "Không tìm thấy khuyên.\n";
        } else {
            cout << "Đỉnh có khuyên: ";
            for (int vertex : selfLoops) {
                cout << vertex << " ";
            }
            cout << "\n";
        }
    }
    
    void analyzeParallelEdges() {
        cout << "\nPhân tích cạnh song song:\n";
        unordered_map<string, vector<pair<int, int>>> edgeGroups;
        
        for (int u = 0; u < numVertices; u++) {
            for (const Edge& edge : adjList[u]) {
                if (u <= edge.to) { // Tránh đếm trùng
                    string key = to_string(u) + "-" + to_string(edge.to);
                    edgeGroups[key].emplace_back(edge.weight, edge.id);
                }
            }
        }
        
        for (const auto& group : edgeGroups) {
            if (group.second.size() > 1) {
                cout << "Cạnh song song giữa " << group.first << ": ";
                for (const auto& edge : group.second) {
                    cout << "(trọng số:" << edge.first << ", id:" << edge.second << ") ";
                }
                cout << "\n";
            }
        }
    }
};

void demonstrateConversion() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "MINH HỌA CHUYỂN ĐỔI GIỮA CÁC LOẠI ĐỒ THỊ\n";
    cout << string(60, '=') << "\n";
    
    // Dữ liệu gốc: Đơn đồ thị với thử thêm cạnh trùng
    vector<tuple<int, int, int>> edges = {
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5}, {2, 3, 8}, {2, 4, 10}, {3, 4, 2}
    };
    
    cout << "\n1. Chuyển đổi Đơn đồ thị -> Đa đồ thị:\n";
    SimpleGraph sg(5);
    for (auto& e : edges) {
        sg.addEdge(get<0>(e), get<1>(e), get<2>(e));
    }
    
    Multigraph mg(5);
    // Chuyển đổi đơn đồ thị thành đa đồ thị
    for (auto& e : edges) {
        mg.addEdge(get<0>(e), get<1>(e), get<2>(e));
    }
    // Thêm cạnh song song để minh họa tính năng đa đồ thị
    mg.addEdge(0, 1, 3); // Song song với (0,1,4)
    mg.addEdge(1, 3, 7); // Song song với (1,3,5)
    
    cout << "\n2. Chuyển đổi Đa đồ thị -> Đồ thị tổng quát:\n";
    GeneralGraph gg(5);
    // Chuyển đổi đa đồ thị thành đồ thị tổng quát
    for (auto& e : edges) {
        gg.addEdge(get<0>(e), get<1>(e), get<2>(e));
    }
    gg.addEdge(0, 1, 3); // Cạnh song song
    gg.addEdge(1, 3, 7); // Cạnh song song
    gg.addEdge(2, 2, 1); // Khuyên
    gg.addEdge(4, 4, 3); // Khuyên
    
    mg.analyzeParallelEdges();
    gg.analyzeSelfLoops();
    gg.analyzeParallelEdges();
}

int main() {
    cout << string(80, '=') << "\n";
    cout << "CÁCH TRIỂN KHAI THUẬT TOÁN DIJKSTRA CHO CÁC LOẠI ĐỒ THỊ KHÁC NHAU\n";
    cout << "Bài toán 14, 15, 16: Đơn đồ thị, Đa đồ thị, Đồ thị tổng quát\n";
    cout << string(80, '=') << "\n";
    
    // Bài toán 14: Đơn đồ thị
    cout << "\n" << string(50, '-') << "\n";
    cout << "BÀI TOÁN 14: ĐƠN ĐỒ THỊ\n";
    cout << string(50, '-') << "\n";
    
    SimpleGraph simpleGraph(5);
    
    // Thêm cạnh: (u, v, trọng số)
    simpleGraph.addEdge(0, 1, 4);
    simpleGraph.addEdge(0, 2, 2);
    simpleGraph.addEdge(1, 2, 1);
    simpleGraph.addEdge(1, 3, 5);
    simpleGraph.addEdge(2, 3, 8);
    simpleGraph.addEdge(2, 4, 10);
    simpleGraph.addEdge(3, 4, 2);
    
    simpleGraph.printGraph();
    
    int source = 0;
    auto [distSimple, parentSimple] = simpleGraph.dijkstra(source);
    simpleGraph.printShortestPaths(source, distSimple, parentSimple);
    
    // Bài toán 15: Đa đồ thị
    cout << "\n" << string(50, '-') << "\n";
    cout << "BÀI TOÁN 15: ĐA ĐỒ THỊ\n";
    cout << string(50, '-') << "\n";
    
    Multigraph multigraph(5);
    
    // Thêm cùng các cạnh như đơn đồ thị
    multigraph.addEdge(0, 1, 4);
    multigraph.addEdge(0, 2, 2);
    multigraph.addEdge(1, 2, 1);
    multigraph.addEdge(1, 3, 5);
    multigraph.addEdge(2, 3, 8);
    multigraph.addEdge(2, 4, 10);
    multigraph.addEdge(3, 4, 2);
    
    // Thêm cạnh song song (nhiều cạnh giữa cùng hai đỉnh)
    multigraph.addEdge(0, 1, 3); // Đường đi thay thế 0->1 với trọng số 3
    multigraph.addEdge(1, 3, 7); // Đường đi thay thế 1->3 với trọng số 7
    multigraph.addEdge(2, 4, 6); // Đường đi thay thế 2->4 với trọng số 6
    
    multigraph.printGraph();
    multigraph.analyzeParallelEdges();
    
    auto [distMulti, parentMulti] = multigraph.dijkstra(source);
    multigraph.printShortestPaths(source, distMulti, parentMulti);
    
    // Bài toán 16: Đồ thị tổng quát
    cout << "\n" << string(50, '-') << "\n";
    cout << "BÀI TOÁN 16: ĐỒ THỊ TỔNG QUÁT\n";
    cout << string(50, '-') << "\n";
    
    GeneralGraph generalGraph(5);
    
    // Thêm cùng các cạnh như đa đồ thị
    generalGraph.addEdge(0, 1, 4);
    generalGraph.addEdge(0, 2, 2);
    generalGraph.addEdge(1, 2, 1);
    generalGraph.addEdge(1, 3, 5);
    generalGraph.addEdge(2, 3, 8);
    generalGraph.addEdge(2, 4, 10);
    generalGraph.addEdge(3, 4, 2);
    generalGraph.addEdge(0, 1, 3); // Cạnh song song
    generalGraph.addEdge(1, 3, 7); // Cạnh song song
    generalGraph.addEdge(2, 4, 6); // Cạnh song song
    
    // Thêm khuyên (cạnh từ đỉnh đến chính nó)
    generalGraph.addEdge(0, 0, 1); // Khuyên tại đỉnh 0
    generalGraph.addEdge(2, 2, 3); // Khuyên tại đỉnh 2
    generalGraph.addEdge(4, 4, 2); // Khuyên tại đỉnh 4
    
    generalGraph.printGraph();
    generalGraph.analyzeSelfLoops();
    generalGraph.analyzeParallelEdges();
    
    auto [distGeneral, parentGeneral] = generalGraph.dijkstra(source);
    generalGraph.printShortestPaths(source, distGeneral, parentGeneral);
    
    // Minh họa chuyển đổi giữa các loại đồ thị
    demonstrateConversion();
    
    // Phân tích toán học
    cout << "\n" << string(60, '=') << "\n";
    cout << "PHÂN TÍCH TOÁN HỌC\n";
    cout << string(60, '=') << "\n";
    
    cout << "\nPhân tích độ phức tạp thời gian:\n";
    cout << "- Sử dụng Binary Heap (Hàng đợi ưu tiên): O((V + E) log V)\n";
    cout << "- V = số đỉnh, E = số cạnh\n";
    cout << "- Độ phức tạp không gian: O(V + E)\n";
    
    cout << "\nTính chất đồ thị:\n";
    cout << "- Đơn đồ thị: Không có cạnh song song, không có khuyên\n";
    cout << "- Đa đồ thị: Cho phép cạnh song song, không có khuyên\n";
    cout << "- Đồ thị tổng quát: Cho phép cả cạnh song song và khuyên\n";
    
    cout << "\nTính chất thuật toán Dijkstra:\n";
    cout << "- Chỉ hoạt động với trọng số cạnh không âm\n";
    cout << "- Tìm đường đi ngắn nhất từ nguồn đến tất cả đỉnh\n";
    cout << "- Sử dụng phương pháp tham lam với cấu trúc con tối ưu\n";
    cout << "- Khuyên không ảnh hưởng đến đường đi ngắn nhất (bỏ qua trong relaxation)\n";
    cout << "- Cạnh song song: chỉ cạnh có trọng số nhỏ nhất quan trọng\n";
    
    return 0;
}