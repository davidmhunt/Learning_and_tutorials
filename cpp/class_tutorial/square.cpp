/*
Code for the squre function
*/

#include <cassert>

#include "square.h"

using square::Square;

void Square::setEdgeLength(double new_edge_length){
    assert(new_edge_length > 0);
    edge_length = new_edge_length;
}

double Square::getEdgeLength() const {
    return edge_length;
}

double Square::getPerimeter() const {
    return edge_length * 4;
}

double Square::getArea() const {
    return edge_length * edge_length;
}