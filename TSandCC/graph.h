#ifndef GRAPH_H
#define GRAPH_H
#pragma once

#include "node.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <list>
#include <string>
#include <fstream>
#include <QDebug>

class Graph
{
public:
    Graph();
    ~Graph();
    void addNode(QPoint pos);
    void addEdge(Node* first, Node* second);
    std::vector<Node*> getNodes() {return m_nodes;}
    std::unordered_map<Node*,std::unordered_set<Node*>> getEdges() {return m_edges;}
    void makeUndirected() {isDirected = false;}
    void makeDirected() {isDirected = true;}
    bool isDirectedGraph() {return isDirected;}
    void setName(std::string n) { name = n;}
    void saveGraph();

    std::vector<std::unordered_set<Node*>> getConnectedComponents();
    std::pair<std::list<int>,Graph> getTopologicalSort();
private:
    std::vector<Node*> m_nodes;
    std::unordered_map<Node*,std::unordered_set<Node*>> m_edges;
    bool isDirected;
    std::string name;
};

#endif // GRAPH_H
