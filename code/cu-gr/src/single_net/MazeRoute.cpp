#include "MazeRoute.h"

ostream &operator<<(ostream &os, const Solution &sol) {
    // modified
    os << "cost=" << sol.g_cost << ", vertex=" << sol.vertex << ", prev=" << (sol.prev ? sol.prev->vertex : -1);
    return os;
}

void MazeRoute::constructGridGraph(const vector<gr::GrBoxOnLayer> &guides) {
    GuideGridGraphBuilder graphBuilder(grNet, graph, guides);
    mergedPinAccessBoxes = grNet.getMergedPinAccessBoxes(
        [](const gr::GrPoint &point) { return gr::PointOnLayer(point.layerIdx, point[X], point[Y]); });
    graphBuilder.run(mergedPinAccessBoxes);
}

void MazeRoute::constructGridGraph(const CongestionMap &congMap) {
    CoarseGridGraphBuilder graphBuilder(grNet, graph, congMap);
    mergedPinAccessBoxes =
        grNet.getMergedPinAccessBoxes([&](const gr::GrPoint &point) { return graphBuilder.grPointToPoint(point); });
    graphBuilder.run(mergedPinAccessBoxes);
}

db::RouteStatus MazeRoute::run() {
    vertexCosts.assign(graph.getNodeNum(), Costs(FLT_MAX, FLT_MAX, FLT_MAX));
    pinSols.assign(mergedPinAccessBoxes.size(), nullptr);
    const int startPin = 0;

    auto status = route(startPin);
    if (!db::isSucc(status)) {
        return status;
    }

    getResult();

    return status;
}

db::RouteStatus MazeRoute::route(int startPin) {
    // define std::priority_queue
    auto solComp = [](const std::shared_ptr<Solution> &lhs, const std::shared_ptr<Solution> &rhs) {
        return rhs->f_cost < lhs->f_cost;
    };
    std::priority_queue<std::shared_ptr<Solution>, vector<std::shared_ptr<Solution>>, decltype(solComp)> solQueue(
        solComp);

    auto updateSol = [&](const std::shared_ptr<Solution> &sol) {
        solQueue.push(sol);
        if (sol->f_cost < vertexCosts[sol->vertex].f_cost)
            vertexCosts[sol->vertex] = Costs(sol->g_cost, sol->h_cost, sol->f_cost);
    };
    const double HEURISTIC_CONST1 = 1;
    const double HEURISTIC_CONST2 = 0.3;
    auto heuristic = [&HEURISTIC_CONST1, &HEURISTIC_CONST2](
                         const gr::PointOnLayer &p1, const int &layer1, const gr::PointOnLayer &p2, const int &layer2) {
        // return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
        return (std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y)) * HEURISTIC_CONST1 +
               std::abs(layer1 - layer2) * HEURISTIC_CONST2;
    };

    // init from startPin
    for (auto vertex : graph.getVertices(startPin)) {
        gr::PointOnLayer s = graph.getPoint(startPin);
        updateSol(std::make_shared<Solution>(0, vertex, nullptr));
    }

    std::unordered_set<int> visitedPin = {startPin};
    int nPinToConnect = mergedPinAccessBoxes.size() - 1;

    while (nPinToConnect != 0) {
        std::shared_ptr<Solution> dstVertex;

        int dstPinIdx = -1;

        // A star
        while (!solQueue.empty()) {
            auto current = solQueue.top();
            int u = current->vertex;
            solQueue.pop();

            // reach a pin?
            dstPinIdx = graph.getPinIdx(u);
            if (dstPinIdx != -1 && visitedPin.find(dstPinIdx) == visitedPin.end()) {
                dstVertex = current;
                break;
            }

            // pruning by upper bound
            if (vertexCosts[u].g_cost < current->g_cost) continue;

            const db::MetalLayer &uLayer = database.getLayer(graph.getPoint(u).layerIdx);

            for (auto direction : directions) {
                if (!graph.hasEdge(u, direction) ||
                    (current->prev && graph.getEdgeEndPoint(u, direction) == current->prev->vertex))
                    continue;

                // from u to v
                int v = graph.getEdgeEndPoint(u, direction);
                db::CostT cost = graph.getEdgeCost(u, direction);

                gr::PointOnLayer nextPoint = graph.getPoint(v);
                gr::PointOnLayer dst = graph.getPoint(dstPinIdx);

                vertexCosts[v].h_cost = heuristic(nextPoint, nextPoint.layerIdx, dst, dst.layerIdx);
                db::CostT newCost_g = current->g_cost + cost;
                db::CostT newCost_f = vertexCosts[v].h_cost + newCost_g;  // vertexCosts[v].first = neighbour_h_cost

                if (newCost_f < vertexCosts[v].f_cost)
                    updateSol(std::make_shared<Solution>(newCost_g, vertexCosts[v].h_cost, newCost_f, v, current));
            }
        }

        if (!dstVertex) {
            printWarnMsg(db::RouteStatus::FAIL_DISCONNECTED_GRID_GRAPH, grNet.dbNet);
            printlog(visitedPin, nPinToConnect, mergedPinAccessBoxes.size(), mergedPinAccessBoxes);
            printlog(graph.checkConn());
            graph.writeDebugFile(grNet.getName() + ".graph");
            getchar();
            return db::RouteStatus::FAIL_DISCONNECTED_GRID_GRAPH;
        }

        // update pinSols
        pinSols[dstPinIdx] = dstVertex;

        // mark the path to be zero
        auto tmp = dstVertex;
        while (tmp && tmp->g_cost != 0) {
            updateSol(std::make_shared<Solution>(0, tmp->vertex, tmp->prev));
            tmp = tmp->prev;
        }

        // mark all the accessbox of the pin to be almost zero
        for (auto vertex : graph.getVertices(dstPinIdx)) {
            if (vertex == dstVertex->vertex) continue;
            updateSol(std::make_shared<Solution>(0, vertex, nullptr));
        }

        visitedPin.insert(dstPinIdx);
        nPinToConnect--;
    }

    return db::RouteStatus::SUCC_NORMAL;
}

void MazeRoute::getResult() {
    grNet.gridTopo.clear();

    std::unordered_map<int, std::shared_ptr<gr::GrSteiner>> visited;

    // back track from pin to source
    for (unsigned p = 0; p < mergedPinAccessBoxes.size(); p++) {
        std::unordered_map<int, std::shared_ptr<gr::GrSteiner>> curVisited;
        auto cur = pinSols[p];
        std::shared_ptr<gr::GrSteiner> prevS;
        while (cur) {
            auto it = visited.find(cur->vertex);
            if (it != visited.end()) {
                // graft to an existing node
                if (prevS) {
                    gr::GrSteiner::setParent(prevS, it->second);
                }
                break;
            } else {
                // get curS
                auto point = graph.getPoint(cur->vertex);
                auto curS = std::make_shared<gr::GrSteiner>(gr::GrPoint(point.layerIdx, point[X], point[Y]),
                                                            graph.getPinIdx(cur->vertex));
                if (prevS) {
                    gr::GrSteiner::setParent(prevS, curS);
                }
                if (curVisited.find(cur->vertex) != curVisited.end()) {
                    printlog("Warning: self loop found in a path for net", grNet.getName(), "for pin", p);
                }
                curVisited.emplace(cur->vertex, curS);
                // store tree root
                if (!(cur->prev)) {
                    grNet.gridTopo.push_back(curS);
                    break;
                }
                // prep for the next loop
                prevS = curS;
                cur = cur->prev;
            }
        }
        for (const auto &v : curVisited) visited.insert(v);
    }

    // remove redundant Steiner nodes
    for (auto &tree : grNet.gridTopo) {
        gr::GrSteiner::mergeNodes(tree);
    }
}
