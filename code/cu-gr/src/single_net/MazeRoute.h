#pragma once

#include "GridGraph.h"
#include "multi_net/CongestionMap.h"

class Solution {
public:
    db::CostT g_cost;
    db::CostT f_cost;  // g_cost + h_cost
    db::CostT h_cost;
    int vertex;
    std::shared_ptr<Solution> prev;

    Solution(db::CostT gc, int v, const std::shared_ptr<Solution> &p)
        : g_cost(gc), f_cost(FLT_MAX), vertex(v), prev(p) {}

    Solution(db::CostT gc, db::CostT hc, db::CostT fc, int v, const std::shared_ptr<Solution> &p)
        : g_cost(gc), h_cost(hc), f_cost(fc), vertex(v), prev(p) {}

    friend ostream &operator<<(ostream &os, const Solution &sol);
};

class Costs {
public:
    db::CostT g_cost;
    db::CostT f_cost;  // g_cost + h_cost
    db::CostT h_cost;
    Costs(db::CostT gc, db::CostT hc, db::CostT fc) : g_cost(gc), h_cost(hc), f_cost(fc) {}
};
class MazeRoute {
public:
    MazeRoute(gr::GrNet &grNetData) : grNet(grNetData) {}

    void constructGridGraph(const vector<gr::GrBoxOnLayer> &guides);
    void constructGridGraph(const CongestionMap &congMap);
    db::RouteStatus run();

private:
    gr::GrNet &grNet;
    GridGraph graph;

    // h_cost first // f_cost second // min cost upper bound for each vertex
    vector<Costs> vertexCosts;
    vector<std::shared_ptr<Solution>> pinSols;  // best solution for each pin
    vector<vector<gr::PointOnLayer>> mergedPinAccessBoxes;

    db::RouteStatus route(int startPin);
    void getResult();
};
