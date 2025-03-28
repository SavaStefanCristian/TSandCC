#include "graph.h"


Graph::Graph() {makeUndirected();}

Graph::~Graph()
{
}

void Graph::addNode(QPoint pos)
{
    Node* n = new Node((int)m_nodes.size(), pos);
    m_nodes.push_back(n);
    saveGraph();
}

void Graph::addEdge(Node* first, Node* second) {
    m_edges[first].insert(second);
}

void Graph::saveGraph()
{
    int n = m_nodes.size();
    if(n==0) return;
    std::string fileName =this->name + ".graph";
    std::ofstream outputFile(fileName);
    if (!outputFile.is_open()) {
        return;
    }

    outputFile << n << std::endl;

    int** mat = new int*[n];
    mat[0] = new int[n * n]();

    for (int i = 1; i < n; ++i) {
        mat[i] = mat[0] + i * n;
    }

    if (this->isDirected) {
        for(auto& [node,connections] : m_edges) {
            if(node==nullptr) return;
            for(Node* connection : connections) {
                if(connection == nullptr) return;
                mat[node->getValue()][connection->getValue()] = 1;
            }
        }
    } else {
        for(auto& [node,connections] : m_edges) {
            if(node==nullptr) return;
            for(Node* connection : connections) {
                if(connection == nullptr) return;
                mat[node->getValue()][connection->getValue()] = 1;
                mat[connection->getValue()][node->getValue()] = 1;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            outputFile << mat[i][j] << " ";
        }
        outputFile << '\n';
    }

    delete[] mat[0];
    delete[] mat;
    outputFile.flush();
    outputFile.close();
}

std::vector<std::unordered_set<Node*>> Graph::getConnectedComponents()
{
    std::vector<std::unordered_set<Node*>> components;
    if(isDirected==true) return components;
    std::unordered_map<Node*,std::unordered_set<Node*>> edges = m_edges;
    for(auto&[node , connections] : m_edges) {
        for(Node* con : connections) {
            edges[con].insert(node);
        }
    }
    std::unordered_set<Node*> U(m_nodes.begin(),m_nodes.end());
    int currentComponent = 0;
    std::stack<Node*> openList;
    openList.push(m_nodes[0]);
    U.erase(m_nodes[0]);
    bool isLast = false;
    while(!U.empty() || isLast) {
        if(isLast) isLast = false;
        components.push_back({openList.top()});
        while(!openList.empty()) {
            Node* currNode = openList.top();
            openList.pop();
            if(edges.find(currNode)==edges.end()) continue;
            for(Node* currCon : edges[currNode]) {
                if(U.find(currCon) == U.end()) continue;
                U.erase(currCon);
                components[currentComponent].insert(currCon);
                openList.push(currCon);
            }
        }
        for(Node* selectedNode : U) {
            openList.push(selectedNode);
            U.erase(selectedNode);
            ++currentComponent;
            isLast = true;
            break;
        }
    }
    return components;
}



std::pair<std::list<int>, Graph> Graph::getTopologicalSort() {
    if(!isDirected) return {};
    std::list<int> sorted;
    Graph sortedGraph;
    for(Node* node : m_nodes) sortedGraph.addNode(node->getPos());

    std::unordered_set<int> U;
    for(Node* node : m_nodes) {
        U.insert(node->getValue());
    }
    std::unordered_map<Node*,std::unordered_set<Node*>> edges = m_edges;
    std::vector<int> t2(m_nodes.size(),-1);
    std::stack<int> dfsStack;
    int k = 0;
    while(!U.empty()) {
        dfsStack.push(*U.begin());
        U.erase(*U.begin());
        ++k;
        while(!dfsStack.empty()) {
            int currNode = dfsStack.top();

            if(edges[m_nodes[currNode]].empty()) {
                t2[currNode] = ++k;
                dfsStack.pop();
                edges.erase(m_nodes[currNode]);
                sorted.push_front(currNode);
                continue;
            }
            int neighbour = (*edges[m_nodes[currNode]].begin())->getValue();
            edges[m_nodes[currNode]].erase(m_nodes[neighbour]);
            if(U.find(neighbour) != U.end()) {
                U.erase(neighbour);
                sortedGraph.addEdge(sortedGraph.m_nodes[currNode],sortedGraph.m_nodes[neighbour]);
                dfsStack.push(neighbour);
            }
            else if(t2[neighbour]==-1) {
                qDebug() << "Graph contains a circuit.\n";
                return {};
            }
        }
    }
    return std::make_pair(sorted, sortedGraph);
}


