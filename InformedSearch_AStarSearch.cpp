#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <set>
#include <math.h>

#define N 8
using namespace std;

int ManhattanDistance(int cy, int cx, int gy, int gx) {
    // h = abs (current_cell.x – goal.x) +  abs(current_cell.y – goal.y)
    return abs(cx - gx) + abs(cy - gy);
}

int DiagonalDistance(int cy, int cx, int gy, int gx) {
    // h = max { abs(current_cell.x – goal.x), abs(current_cell.y – goal.y) }
    return max(abs(cx - gx), abs(cy - gy));
}

int EuclideanDistance(int cy, int cx, int gy, int gx) {
    // h = sqrt((current_cell.x – goal.x)^2 + (current_cell.y – goal.y)^2)
    return sqrt(pow((cx - gx), 2) + pow((cy - gy), 2));
}

bool isValidPosition(int row, int col) {
    return (row >= 0) && (row < N) && (col >= 0) && (col < N);
}

void Search(int maze[N][N], bool closedList[N][N], set<pair<int, pair<int, int>>>& openList, unordered_map<string, string>& prev, unordered_map<string, int>& f, unordered_map<string, int>& g, pair<int, int>& goal, pair<int, int> SearchPosition, string current) {
    if (isValidPosition(SearchPosition.first, SearchPosition.second) == false) return; // if out of boundary return
    if (closedList[SearchPosition.first][SearchPosition.second] == true || maze[SearchPosition.first][SearchPosition.second] == 0) return; // if already been search or not a path return
    int temp_cost = g[current] + 1; // + 1 because this map is a grid path from current node (g[current]) to next node
    string next = to_string(SearchPosition.first) + ',' + to_string(SearchPosition.second); // create new string
    if (temp_cost >= g[next] && g[next]) return; // if cost_so_far[next] is exist and new cost >= cost_so_far[next] return
    if (closedList[SearchPosition.first][SearchPosition.second] == false) closedList[SearchPosition.first][SearchPosition.second] = true; // mark as explored
    prev[next] = current; // parent
    g[next] = temp_cost; // new cost
    f[next] = g[next] + EuclideanDistance(SearchPosition.first, SearchPosition.second, goal.first, goal.second); // heuristic, if you want to use another heuristic funtion just change this function (Euclidean Distance) with other such as Diagonal Distance or Manhattan Distance
    openList.insert(make_pair(g[next], SearchPosition)); // next position to search
}

bool AStarSearch(int maze[N][N], pair<int, int> start, pair<int, int> goal) {
    // V = N^2 = 8^2 = 64
    // Time Complexity: O(|V| * log |V| + |E|)
    // Format Y , X
    bool closedList[N][N];
    memset(closedList, false, sizeof(closedList));
    set<pair<int, pair<int, int>>> openList; // create min Heap using C++ set, O(log n)
    openList.insert(make_pair(0, make_pair(start.first, start.second)));
    unordered_map<string, string> prev; // you can use STL map as well but is faster using STL unordered_map
    unordered_map<string, int> g; // STL map is implemented using tree so operation will be O(log n)
    unordered_map<string, int> f; // STL unordered_map using hash table so operation will be O(1)
    string src = to_string(start.first) + ',' + to_string(start.second);
    prev[src] = "None";
    g[src] = 0;
    f[src] = EuclideanDistance(start.first, start.second, goal.first, goal.second); // heuristic

    while (!openList.empty()) {
        pair<int, pair<int, int>> current = *openList.begin();
        string strt = to_string(current.second.first) + ',' + to_string(current.second.second);

        cout << strt << endl; // check searching path

        if (current.second.first == goal.first && current.second.second == goal.second) {

            cout << "\n Backtrack from goal to start, format Y,X" << endl;
            cout << " Start Y: " << start.first << ", X: " << start.second << " | Goal Y: " << goal.first << ", X: " << goal.second << endl << endl;
            string trace_path = strt;
            while (prev[trace_path] != "None") {
                cout << trace_path << endl;
                trace_path = prev[trace_path];
            }
            cout << trace_path << endl;

            // Free Memory
            prev.clear();
            f.clear();
            g.clear();

            return true;
        }

        // from heap delete the first item, fyi openList.begin() point to the first element in set which is always the smallest element
        openList.erase(openList.begin());

        // search arah jarum jam
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first - 1, current.second.second), strt); // North
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first - 1, current.second.second + 1), strt); // Nort-East
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first, current.second.second + 1), strt); // East
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first + 1, current.second.second + 1), strt); // South-East
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first + 1, current.second.second), strt); // South
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first + 1, current.second.second - 1), strt); // South-West
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first, current.second.second - 1), strt); // West
        Search(maze, closedList, openList, prev, f, g, goal, make_pair(current.second.first - 1, current.second.second - 1), strt); // North-West

        if (closedList[current.second.first][current.second.second] == false) closedList[current.second.first][current.second.second] = true;
    }

    // Free Memory
    prev.clear();
    f.clear();
    g.clear();

    return false;
}

void main() {

    // Making I/O (Input Output) more fast
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // There are 3 Possible Paths from start to goal in this maze
    int maze[N][N]
        = { { 1, 1, 1, 1, 1, 1, 1, 1 },
            { 1, 0, 1, 0, 0, 1, 0, 1 },
            { 1, 0, 0, 1, 1, 0, 0, 1 },
            { 1, 0, 0, 1, 0, 0, 0, 1 },
            { 1, 0, 1, 0, 1, 1, 0, 1 },
            { 1, 0, 1, 0, 1, 0, 0, 1 },
            { 1, 0, 1, 0, 0, 1, 0, 1 },
            { 1, 1, 0, 0, 0, 0, 1, 1 } };

    if (AStarSearch(maze, { 0, 0 }, { 7, 7 }))
        cout << "\nFound Path from start to end!\n";
    else
        cout << "\nNot found!\n";
}