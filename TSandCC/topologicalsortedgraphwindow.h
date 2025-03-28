#ifndef TOPOLOGICALSORTEDGRAPHWINDOW_H
#define TOPOLOGICALSORTEDGRAPHWINDOW_H

#include <QWidget>
#include "graph.h"
#include <QPaintEvent>
#include <QPainter>

#include <unordered_map>
#include <unordered_set>



class TopologicalSortedGraphWindow
    : public QWidget
{
    Q_OBJECT

public:
    TopologicalSortedGraphWindow(Graph sortedGraph, QWidget* parent = nullptr)
        : QWidget(parent), m_graph(sortedGraph)
    {
        setWindowTitle("Topologically Sorted Graph");
        resize(800, 600);
        m_graph.makeDirected();
    }

protected:

    void paintEvent(QPaintEvent* ev) override;



private:
    Graph m_graph;
    int m_radius = 30;

    std::pair<int, int> calculateIntersection(int x1, int y1, int x2, int y2, int radius);



    std::array<int,4> calculateTriangle(int x1, int y1, int x2, int y2, int radius);
};

#endif // TOPOLOGICALSORTEDGRAPHWINDOW_H
