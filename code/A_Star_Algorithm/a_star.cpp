#include <bits/stdc++.h>
#include <thread>
#include <pthread.h>
using namespace std;

int ROW, COL;
typedef pair<int, int> ii;

struct Grid
{
    ii parent = {-1, -1};
    double f = FLT_MAX, g = FLT_MAX, h = FLT_MAX; // f = g + h
};

#define fi first
#define se second
#define mp make_pair
typedef vector<vector<int>> VVI;
typedef vector<vector<bool>> VVB;
typedef vector<vector<Grid>> VVG;
typedef pair<double, pair<int, int>> dii;

auto is_valid = [](int r, int c) { return (r >= 0) && (r < ROW) && (c >= 0) && (c < COL); };
auto not_blocked = [](VVI &grid, int r, int c) { return grid[r][c] != 0; };
auto is_dst = [](int r, int c, ii dst) { return r == dst.fi && c == dst.se; };
auto calc_h = [](int r, int c, ii dst) { return ((double)sqrt((r - dst.fi) * (r - dst.fi) + (c - dst.se) * (c - dst.se))); };

void get_path(const VVG &cells, ii dst)
{
    printf("Path: ");
    int r = dst.fi, c = dst.se;
    stack<ii> path;
    // partent (i, j)
    while (!(cells[r][c].parent.fi == r && cells[r][c].parent.se == c))
    {
        path.push(mp(r, c));
        int tmp_r = cells[r][c].parent.fi, tmp_c = cells[r][c].parent.se;
        r = tmp_r, c = tmp_c;
    }
    path.push(mp(r, c)); // push source
    while (!path.empty())
    {
        ii p = path.top();
        path.pop();
        printf("(%d,%d) ", p.fi, p.se);
    }
    printf("\n");
}

bool try_go_to(VVI &grid, int &i, int &j, int iNew, int jNew, VVG &cells, set<dii> &pq, VVB &visited, ii &dst)
{
    double fNew, hNew, gNew;
    if (is_valid(iNew, jNew))
    {
        if (is_dst(iNew, jNew, dst))
        {
            cells[iNew][jNew].parent = {i, j};
            get_path(cells, dst);
            return true;
        }
        else if (!visited[iNew][jNew] && not_blocked(grid, iNew, jNew))
        {
            gNew = cells[i][j].g + grid[i][j], hNew = calc_h(iNew, jNew, dst);
            fNew = gNew + hNew;
            if (cells[iNew][jNew].f == FLT_MAX || cells[iNew][jNew].f > fNew)
            {
                pq.insert(mp(fNew, mp(iNew, jNew)));
                cells[iNew][jNew] = {{i, j}, fNew, gNew, hNew}; // update cell
            }
        }
    }
    return false;
}

void a_star(VVI &grid, ii src, ii dst)
{
    if (!(is_valid(src.fi, src.se) || is_valid(dst.fi, dst.se)))
    {
        printf("Source or Destination is invalid\n");
        return;
    }

    if (!(not_blocked(grid, src.fi, src.se) && not_blocked(grid, dst.fi, dst.se)))
    {
        printf("Source or Destination is blocked\n");
        return;
    }

    if (is_dst(src.fi, src.se, dst))
    {
        printf("The Source Is The Destination\n");
        return;
    }

    VVG cells(ROW, vector<Grid>(COL));
    VVB visited(ROW, vector<bool>(COL, false));
    int i = src.fi, j = src.se;      // source to i,j
    cells[i][j] = {{i, j}, 0, 0, 0}; // set source
    set<dii> pq;                     // to expand not visited list has f, g and h
    pq.insert(mp(0, mp(i, j)));
    while (!pq.empty())
    {
        dii p = *pq.begin();
        pq.erase(pq.begin());
        i = p.se.fi, j = p.se.se;
        visited[i][j] = true;
        bool finished = try_go_to(grid, i, j, i - 1, j, cells, pq, visited, dst); // North
        if (finished)
            return;
        finished = try_go_to(grid, i, j, i + 1, j, cells, pq, visited, dst); // South
        if (finished)
            return;
        finished = try_go_to(grid, i, j, i, j - 1, cells, pq, visited, dst); // West
        if (finished)
            return;
        finished = try_go_to(grid, i, j, i, j + 1, cells, pq, visited, dst); // East
        if (finished)
            return;
    }
    printf("Failed to find path from (%d,%d) to (%d,%d)\n", src.fi, src.se, dst.fi, dst.se);
}

int main()
{
    // 0 is blocked otherwise is weight
    VVI grid =
        {{1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
         {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
         {1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
         {0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
         {1, 1, 1, 0, 1, 1, 1, 0, 1, 0},
         {1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
         {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
         {1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
         {1, 1, 1, 0, 0, 0, 1, 0, 0, 1}};

    ROW = grid.size(), COL = grid[0].size();
    ii src = {8, 0}, dst = {0, 0};
    // thread t1(a_star, grid, src, dst);
    // thread t2(a_star, grid, dst, src);
    a_star(grid, src, dst);
    return 0;
}