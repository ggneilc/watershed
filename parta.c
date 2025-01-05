#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pointcloud.h"

int stat1();

int main(int argc, char const *argv[]){
    stat1();
    return 0;
}

/**
 * Input a stream of characters in format (x, y, z)
 * Find minimum and maximum z, print x,y location of min and max
 * Find average of all z  
 */
int stat1(){
    float max, avg = 0;
    float min = 9999;
    int count = 0; // for computing avg
    float minCoords[2];
    float maxCoords[2];

    char buf[256];
    /* read input file line by line */
    while (fgets(buf, sizeof(buf), stdin)){
        // Break line into floats, delimited by spaces
        float x, y, h = 0;
        sscanf(buf, "%f %f %f", &x, &y, &h);
        avg += h;
        min = MIN(min, h);
        if (min == h){
            minCoords[0] = x; minCoords[1] = y;
        }
        max = MAX(max, h);
        if (max == h){
            maxCoords[0] = x; maxCoords[1] = y;
        }
        count++;
    }
    printf("Maximum Height: %f x:%f y:%f\nMinimum Height: %f x:%f y:%f\nAverage Height: %f\n",
     max, maxCoords[0], maxCoords[1],
     min, minCoords[0], minCoords[1], 
     avg/count);

    return 0;
}