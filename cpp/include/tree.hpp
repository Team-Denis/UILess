#pragma once

#include "node.hpp"
// Contains nodes

class Tree {
public:
    Tree();
    ~Tree();

    void insertNode(Node* node);
    void deleteNode(Node* node);
    Node* searchNode(Node* node);

private:
    Node* root;
};