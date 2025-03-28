#ifndef NODE_H
#define NODE_H
#pragma once
#include <QPoint>

class Node
{
public:
    Node();
    Node(int val, QPoint pos) : m_value{val}, m_pos{pos}{}
    void setValue(int val) {m_value = val;}
    void setPos(QPoint pos) {m_pos = pos;}
    QPoint getPos() {return m_pos;}
    int getValue() {return m_value;}
private:
    int m_value;
    QPoint m_pos;
};

#endif // NODE_H
