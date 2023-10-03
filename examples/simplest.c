// This is one of the simplest C programs possible

#include <stdio.h>

int func() {
    return 9;
}

int main() {
    int x = func();
    
    if (x > 5) {
        printf("x is greater than five\n");

    } else {
        /* 
            Multi
            line
            comment!
        */
        if ( (x) == (5) ) {
               printf("x is five\n");
        } else printf("x is less than five\n");
    }

    printf("using commas is fun %d %d %d\n", (5), (6), (7));

    return 0;
}