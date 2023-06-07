#pragma once

#include "./bot.hpp"
#include <vector>
#include <algorithm>

struct NodeData
{
    int n;
    std::vector<float> reward;
    int player;
    int n_avail;
    bool avail;
};


class Node {
    public:
        Node(Card, int, bool root = false);
    public:
        std::vector<Node*> children;
        Node* parent;
        NodeData data;
        Card action;
    public:
        void setChild(Node*);
        

};