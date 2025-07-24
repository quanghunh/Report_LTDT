import heapq
from collections import defaultdict, deque
from typing import List, Tuple, Dict, Set, Optional
import math

class Edge:
    """Biểu diễn một cạnh trong đồ thị."""
    def __init__(self, to: int, weight: int, edge_id: int = 0):
        self.to = to              # đỉnh đích
        self.weight = weight      # trọng số
        self.id = edge_id         # định danh cạnh
    
    def __repr__(self):
        return f"Edge(to={self.to}, weight={self.weight}, id={self.id})"

class Graph:
    """Lớp cơ sở cho tất cả loại đồ thị."""
    
    def __init__(self, num_vertices: int):
        self.num_vertices = num_vertices    # số đỉnh
        self.adj_list = [[] for _ in range(num_vertices)]  # danh sách kề
    
    def add_edge(self, u: int, v: int, weight: int, edge_id: int = 0) -> None:
        """Thêm cạnh vào đồ thị. Cách triển khai khác nhau theo loại đồ thị."""
        raise NotImplementedError("Lớp con phải triển khai add_edge")
    
    def print_graph(self) -> None:
        """In biểu diễn danh sách kề."""
        raise NotImplementedError("Lớp con phải triển khai print_graph")
    
    def dijkstra(self, source: int) -> Tuple[List[int], List[int]]:
        """
        Triển khai thuật toán Dijkstra tìm đường đi ngắn nhất.
        
        Trả về:
            Tuple của (mảng khoảng cách, mảng cha)
        """
        # Khởi tạo mảng khoảng cách và mảng cha
        dist = [math.inf] * self.num_vertices
        parent = [-1] * self.num_vertices
        visited = [False] * self.num_vertices
        
        # Hàng đợi ưu tiên: (khoảng cách, đỉnh)
        pq = [(0, source)]
        dist[source] = 0
        
        print(f"\nCác bước thực hiện thuật toán Dijkstra:")
        print("Bước | Hiện tại | Khoảng cách | Cha | Cập nhật kề")
        print("-----|----------|-------------|-----|---------------")
        
        step = 1
        
        while pq:
            current_dist, u = heapq.heappop(pq)
            
            if visited[u]:
                continue
                
            visited[u] = True
            
            # In bước hiện tại
            print(f"{step:4d} |{u:9d} |{int(current_dist) if current_dist != math.inf else -1:12d} |{parent[u]:4d} | ", end="")
            
            updates = []
            
            # Thực hiện relaxation cho tất cả cạnh kề
            for edge in self.adj_list[u]:
                v = edge.to
                weight = edge.weight
                
                if not visited[v] and current_dist + weight < dist[v]:
                    dist[v] = current_dist + weight
                    parent[v] = u
                    heapq.heappush(pq, (dist[v], v))
                    updates.append(f"({u}->{v}:{int(dist[v])})")
            
            if updates:
                print(", ".join(updates))
            else:
                print("Không có")
                
            step += 1
        
        # Chuyển inf thành -1 để hiển thị
        display_dist = [int(d) if d != math.inf else -1 for d in dist]
        return display_dist, parent
    
    def print_shortest_paths(self, source: int, dist: List[int], parent: List[int]) -> None:
        """In đường đi ngắn nhất từ nguồn đến tất cả đỉnh khác."""
        print(f"\nĐường đi ngắn nhất từ đỉnh {source}:")
        print("Đích | Khoảng cách | Đường đi")
        print("-----|-------------|----------")
        
        for i in range(self.num_vertices):
            if i == source:
                continue
                
            print(f"{i:4d} |", end="")
            
            if dist[i] == -1:  # Vô cực
                print(f"{'VÔ CỰC':12s} | Không có đường đi")
            else:
                print(f"{dist[i]:12d} | ", end="")
                
                # Khôi phục đường đi
                path = []
                curr = i
                while curr != -1:
                    path.append(curr)
                    curr = parent[curr]
                path.reverse()
                
                print(" -> ".join(map(str, path)))

class SimpleGraph(Graph):
    """Bài toán 14: Đơn đồ thị - không có cạnh song song, không có khuyên."""
    
    def __init__(self, num_vertices: int):
        super().__init__(num_vertices)
        self.edges_set = set()  # Để kiểm tra cạnh trùng
    
    def add_edge(self, u: int, v: int, weight: int, edge_id: int = 0) -> None:
        """Thêm cạnh vào đơn đồ thị với kiểm tra trùng lặp."""
        # Kiểm tra cạnh đã tồn tại
        if (u, v) in self.edges_set or (v, u) in self.edges_set:
            print(f"Cảnh báo: Cạnh {u}->{v} đã tồn tại trong đơn đồ thị!")
            return
        
        # Kiểm tra khuyên
        if u == v:
            print(f"Cảnh báo: Khuyên {u}->{v} không được phép trong đơn đồ thị!")
            return
        
        self.adj_list[u].append(Edge(v, weight))
        self.adj_list[v].append(Edge(u, weight))  # Vô hướng
        self.edges_set.add((u, v))
    
    def print_graph(self) -> None