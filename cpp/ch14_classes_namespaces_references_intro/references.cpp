//review of pointers and references

#include <cstdio>

#define EXIT_SUCCESS 1

void swap (int & x, int & y) {
    int temp = x;
    x = y;
    y = temp;
}

int main(void){
    int a = 3;
    int b = 4;
    swap(a,b);
    printf("a = %d, b = %d\n", a, b);
    return EXIT_SUCCESS;
}