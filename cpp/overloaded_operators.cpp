//code demonstrating overloaded operators

#include<cstdio>
using std::printf

#define EXIT_SUCCESS 1

class Point{
    public:
        int x,y;
        Point & operator += (const Point & rhs){
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
};

int main(void){
    Point p1;
    Point p2;

    p1.x = 3;
    p1.y = 4;
    p2.x = 5;
    p2.y = 6;
    p1 += p2;
    printf("p1.x: %d,p1.y: %d\n",p1.x,p1.y);

    return EXIT_SUCCESS;
}