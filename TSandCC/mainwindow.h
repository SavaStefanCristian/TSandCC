#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


#include <QMouseEvent>
#include <QPaintEvent>
#include <QMessageBox>
#include "graph.h"

#include "topologicalsortedgraphwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mousePressEvent(QMouseEvent* m) override;
    void mouseReleaseEvent(QMouseEvent* m) override;
    void mouseMoveEvent(QMouseEvent * m) override;
    void paintEvent(QPaintEvent* ev) override;


private slots:
    void on_pushButton_released();

    void on_radioButton_released();

    void on_radioButton_2_released();

    void on_pushButton_2_released();

private:
    Ui::MainWindow *ui;
    TopologicalSortedGraphWindow* topologicalSortedGraphWindow;

    void closeEvent(QCloseEvent *event) override;

    Graph m_graph;
    const int m_radius = 30;

    Node* m_selectedNode;
    bool isFirstNodeSelected;
    bool isDragging;
    QPoint m_selectedNodePreviusCoords;


    bool showConnectedComponents;
    std::unordered_map<Node*,QColor> m_colors;


    std::pair<int,int> calculateIntersection(int x1, int y1, int x2, int y2, int radius);
    std::array<int,4> calculateTriangle(int x1, int y1, int x2, int y2, int radius);
    QColor generateRandomColor();
};
#endif // MAINWINDOW_H
