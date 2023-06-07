#include "./node.hpp"

Node::Node(Card card, int player, bool root) {
    action = card;
    if(root)
        parent = nullptr;
    data.n = 0;
    data.reward = {0,0,0,0};
    data.avail = true;
    data.n_avail = 0;
    data.player = player;
}

void Node::setChild(Node* child) {
    children.push_back(child);
    child->parent = this;
}