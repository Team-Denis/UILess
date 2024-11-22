#pragma once

class Node {
public:
    Node();
    virtual ~Node();

    void execute();
    int getType() const;

private:
    int type;
};