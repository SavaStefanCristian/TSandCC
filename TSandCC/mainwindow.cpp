#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isFirstNodeSelected = false;
    isDragging = false;
    m_graph.setName("mainGraph");
    showConnectedComponents = false;
    srand(time(NULL));
    topologicalSortedGraphWindow = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
    if(topologicalSortedGraphWindow!=nullptr){
        topologicalSortedGraphWindow->close();
        delete topologicalSortedGraphWindow;
        topologicalSortedGraphWindow = nullptr;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *m)
{
    if(!isFirstNodeSelected) return;
    if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes = m_graph.getNodes();
        for(Node* n : nodes) {
            if((n->getPos().x() - m->pos().x())*(n->getPos().x() - m->pos().x())+
                    (n->getPos().y() - m->pos().y())*(n->getPos().y() - m->pos().y())
                < m_radius*m_radius) {
                if(isFirstNodeSelected)
                {
                    if(m_selectedNode==n) {
                        isDragging = true;
                        m_selectedNodePreviusCoords = m_selectedNode->getPos();
                    }
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *m) {
    if(isDragging) {
        m_selectedNode->setPos(m->pos());
        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *m)
{
    if (m->button() == Qt::RightButton)
    {
        std::vector<Node*> nodes = m_graph.getNodes();
        for(Node* n : nodes)
        {
            if((n->getPos().x() - m->pos().x())*(n->getPos().x() - m->pos().x())+
                    (n->getPos().y() - m->pos().y())*(n->getPos().y() - m->pos().y())
                        < 2*2*m_radius*m_radius) {
                return;
            }
        }
        m_graph.addNode(m->pos());
        m_colors.clear();
        showConnectedComponents = false;
        update();
    }
    else if(m->button() == Qt::LeftButton)
    {
        std::vector<Node*> nodes;
        if(isDragging) {
            nodes = m_graph.getNodes();
            for(Node* n : nodes) {
                if(n==m_selectedNode) continue;
                if((n->getPos().x() - m_selectedNode->getPos().x())*(n->getPos().x() - m_selectedNode->getPos().x())+
                    (n->getPos().y() - m_selectedNode->getPos().y())*(n->getPos().y() - m_selectedNode->getPos().y())
                        < 2*2*m_radius*m_radius) {
                    m_selectedNode->setPos(m_selectedNodePreviusCoords);
                    break;
                }
            }
            isDragging =false;
            isFirstNodeSelected = false;
            m_selectedNode = nullptr;
            update();
            return;
        }
        nodes = m_graph.getNodes();
        for(Node* n : nodes) {
            if((n->getPos().x() - m->pos().x())*(n->getPos().x() - m->pos().x())+
                    (n->getPos().y() - m->pos().y())*(n->getPos().y() - m->pos().y())
                        < m_radius*m_radius) {
                if(!isFirstNodeSelected) {

                    isFirstNodeSelected = true;
                    m_selectedNode =n;
                }
                else
                {
                    if(m_selectedNode!=n)
                        m_graph.addEdge(m_selectedNode,n);
                    isFirstNodeSelected = false;
                    m_selectedNode = nullptr;
                    m_colors.clear();
                    showConnectedComponents = false;
                }
                update();
                break;
            }
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    QPainter p{this};
    std::vector<Node*> nodes = m_graph.getNodes();
    for(auto& n : nodes)
    {
        QRect r{n->getPos().x()-m_radius,n->getPos().y()-m_radius, m_radius*2, m_radius*2};
        if(m_selectedNode == n) {
            p.setPen(QPen(Qt::green, m_radius/10));
        }
        if(showConnectedComponents) {
            p.setBrush(QBrush(m_colors[n]));
        }
        p.drawEllipse(r);
        p.setBrush(QBrush(Qt::NoBrush));
        if(m_selectedNode != nullptr && m_selectedNode == n) {
            p.setPen(Qt::white);
        }
        QString str = QString::number(n->getValue()+1);
        p.drawText(r, Qt::AlignCenter, str);
    }
    std::unordered_map<Node*,std::unordered_set<Node*>> edges = m_graph.getEdges();
    for(auto& [node,connections] : edges)
    {
        int x1 = node->getPos().x(), y1 = node->getPos().y();
        for(Node* currCon : connections) {
            int x2 = currCon->getPos().x(), y2 = currCon->getPos().y();
            float x1final, y1final, x2final, y2final;
            std::pair<int,int> results1 = calculateIntersection(x1,y1,x2,y2,m_radius);
            x1final = results1.first;
            y1final = results1.second;
            std::pair<int,int> results2 = calculateIntersection(x2,y2,x1,y1,m_radius);
            x2final = results2.first;
            y2final = results2.second;
            p.drawLine(x1final,y1final,x2final,y2final);
            if(m_graph.isDirectedGraph()) {
                std::array<int,4> triangleEnds = calculateTriangle(x2,y2,x1,y1,m_radius*2);
                QPolygon arrowTriangle;
                arrowTriangle << QPoint(x2final, y2final)
                              << QPoint(triangleEnds[0], triangleEnds[1])
                              << QPoint(triangleEnds[2], triangleEnds[3]);
                p.setBrush(QBrush(Qt::white));
                p.drawPolygon(arrowTriangle);
                p.setBrush(QBrush(Qt::NoBrush));
            }
        }
    }
}


std::pair<int, int> MainWindow::calculateIntersection(int x1, int y1, int x2, int y2, int radius)
{
    float x1final, y1final;
    if (x1 == x2) {
        float yv1 = y1 + radius;
        float yv2 = y1 - radius;

        if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
            y1final = yv1;
        } else {
            y1final = yv2;
        }
        x1final = x1;
    }
    else {
        float m = float(y2 - y1) / float(x2 - x1);
        float b = y1 - m * x1;

        float A = 1 + m * m;
        float B = 2 * (m * (b - y1) - x1);
        float C = (x1 * x1) + (b - y1) * (b - y1) - radius * radius;

        float delta = B * B - 4 * A * C;

        if (delta < 0) {
            float yv1 = y1 + radius;
            float yv2 = y1 - radius;

            if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
                y1final = yv1;
            } else {
                y1final = yv2;
            }
            x1final = x1;
            return std::pair<int, int>(x1final, y1final);
        }

        float sqrtDelta = std::sqrt(delta);
        float xv1 = (-B + sqrtDelta) / (2 * A);
        float xv2 = (-B - sqrtDelta) / (2 * A);

        float yv1 = m * xv1 + b;
        float yv2 = m * xv2 + b;

        float dist1 = std::pow(x2 - xv1, 2) + std::pow(y2 - yv1, 2);
        float dist2 = std::pow(x2 - xv2, 2) + std::pow(y2 - yv2, 2);

        if (dist1 < dist2) {
            x1final = xv1;
            y1final = yv1;
        } else {
            x1final = xv2;
            y1final = yv2;
        }
    }

    return std::pair<int,int>(x1final,y1final);
}



std::array<int,4> MainWindow::calculateTriangle(int x1, int y1, int x2, int y2, int radius)
{
    float x1final, y1final;
    int xfirst, yfirst, xsecond, ysecond;

    if (x1 == x2) {
        float yv1 = y1 + radius;
        float yv2 = y1 - radius;

        if (std::abs(y2 - yv1) < std::abs(y2 - yv2)) {
            y1final = yv1;
        } else {
            y1final = yv2;
        }
        x1final = x1;
        yfirst = y1final;
        ysecond = y1final;
        xfirst = x1final - radius/8;
        xsecond = x1final +radius/8;
        return std::array<int,4>{xfirst,yfirst,xsecond,ysecond};
    }
    else {
        if(y1 == y2) {
            float xv1 = x1 + radius;
            float xv2 = x1 - radius;
            if (std::abs(x2 - xv1) < std::abs(x2 - xv2)) {
                x1final = xv1;
            } else {
                x1final = xv2;
            }
            yfirst = y1 + radius/8;
            ysecond = y1 - radius/8;
            return std::array<int, 4>{(int)x1final, yfirst, (int)x1final, ysecond};
        }
        else{
            float m = float(y2 - y1) / float(x2 - x1);
            float b = y1 - m * x1;

            float A = 1 + m * m;
            float B = 2 * (m * (b - y1) - x1);
            float C = (x1 * x1) + (b - y1) * (b - y1) - radius * radius;

            float delta = B * B - 4 * A * C;
            if (delta < 0) {
                return std::array<int,4>{(int)x1, (int)y1 + radius/4,(int)x1,(int)y1 - radius/4};
            }

            float xv1 = (-B + std::sqrt(delta)) / (2 * A);
            float xv2 = (-B - std::sqrt(delta)) / (2 * A);

            float yv1 = m * xv1 + b;
            float yv2 = m * xv2 + b;

            float dist1 = std::pow(x2 - xv1, 2) + std::pow(y2 - yv1, 2);
            float dist2 = std::pow(x2 - xv2, 2) + std::pow(y2 - yv2, 2);

            if (dist1 < dist2) {
                x1final = xv1;
                y1final = yv1;
            } else {
                x1final = xv2;
                y1final = yv2;
            }
            float m2 = -1/m;

            double dx = m_radius/4 / sqrt(1 + m2 * m2);
            double dy = m2 * dx;

            int xfirst = x1final + dx;
            int yfirst = y1final + dy;

            int xsecond = x1final - dx;
            int ysecond = y1final - dy;

            return std::array<int,4>{xfirst,yfirst,xsecond,ysecond};
        }
    }


}

QColor MainWindow::generateRandomColor() {
    int red = rand()%256;
    int green = rand()%256;
    int blue = rand()%256;
    return QColor(red, green, blue);
}



void MainWindow::on_pushButton_released()
{
    if(m_graph.isDirectedGraph()) return;
    showConnectedComponents = true;
    std::vector<std::unordered_set<Node*>> connectedComponents = this->m_graph.getConnectedComponents();
    for(const std::unordered_set<Node*>& conComp : connectedComponents) {
        QColor currColor = generateRandomColor();
        for(Node* curr : conComp) m_colors[curr] = currColor;
    }
    update();
}


void MainWindow::on_radioButton_released()
{
    m_graph.makeDirected();
    m_graph.saveGraph();
    showConnectedComponents =false;
    m_colors.clear();
    update();
}


void MainWindow::on_radioButton_2_released()
{
    m_graph.makeUndirected();
    m_graph.saveGraph();
    showConnectedComponents =false;
    m_colors.clear();
    update();
}


void MainWindow::on_pushButton_2_released()
{
    if (!m_graph.isDirectedGraph()) return;

    auto sortedResult = m_graph.getTopologicalSort();
    if (sortedResult.first.empty()) {
        qDebug() << "The graph contains a cycle, cannot perform topological sort.\n";
        QMessageBox::information(this, "Warning", "The graph contains a cycle, cannot perform topological sort.");
        return;
    }

    Graph sortedGraph = sortedResult.second;
    sortedGraph.setName("sortedGraph");
    sortedGraph.saveGraph();

    std::ofstream outputFile("topologicalSort.txt");
    if(!outputFile.is_open()){
        qDebug() << "Could not open file!\n";
    }
    for(int curr : sortedResult.first) {
        outputFile<<curr<<" ";
    }
    outputFile.flush();
    outputFile.close();

    if(topologicalSortedGraphWindow!=nullptr){
        topologicalSortedGraphWindow->close();
        delete topologicalSortedGraphWindow;
        topologicalSortedGraphWindow = nullptr;
    }

    topologicalSortedGraphWindow = new TopologicalSortedGraphWindow(sortedGraph);
    topologicalSortedGraphWindow->show();

    update();
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    if(topologicalSortedGraphWindow!=nullptr){
        topologicalSortedGraphWindow->close();
        delete topologicalSortedGraphWindow;
        topologicalSortedGraphWindow = nullptr;
    }
    event->accept();
}


