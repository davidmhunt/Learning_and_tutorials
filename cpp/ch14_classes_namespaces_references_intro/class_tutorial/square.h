/*
Header file for a square class
*/

#pragma once //only include this header file once

namespace square {
    class Square {
        private:
            double edge_length;
        public:
            void setEdgeLength(double new_edge_length);
            double getEdgeLength () const;
            double getArea() const;
            double getPerimeter() const;
    };
}