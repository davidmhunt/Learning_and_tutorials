/*
Code to test the implementation of the square class
*/

#include "square.h"
#include <cstdio>
using std::printf;
using square::Square;

#define EXIT_FAILURE 0
#define EXIT_SUCCESS 1

int main(void){
    Square squares[4]; //declare an array of 4 square classes
    for (int i = 0; i<4; i++){
        squares[i].setEdgeLength(2 * i + 2);
    }
    for (int i = 0; i < 4; i ++){
        printf("Square %d has edge length %f\n",i,squares[i].getEdgeLength());
        printf("\tand area %f\n",squares[i].getArea());
        printf("\tand perimeter %f\n",squares[i].getPerimeter());
    }
    printf("trying to set a negative edge length (should abort) \n");
    squares[0].setEdgeLength(-1);
    return EXIT_FAILURE;
}