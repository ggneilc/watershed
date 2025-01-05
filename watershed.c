#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pointcloud.h"


int main(int argc, char const *argv[])
{
    FILE *input_file = NULL;
    // Check if an argument is provided
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <ifile> <iter> <iwater> <wcoef> <ecord> <ofilebase> <seq>\n", argv[0]);
        return 1;
    }

    // Try to open the file
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", argv[1]);
        perror("Reason");
        return 1;
    }
    /* Parse arguments */
    int iter = atoi(argv[2]);
    if (iter == 0){
        fprintf(stderr, "Error: 'iter' not found");
        return 1;
    }
    double iwater = atof(argv[3]);
    if (iwater == 0){
        fprintf(stderr, "Error: 'iter' not found");
        return 1;
    }
    double wcoef = atof(argv[4]);
    if (wcoef == 0){
        fprintf(stderr, "Error: 'wcoef' not found");
        return 1;
    }
    double ecoef = atof(argv[5]);
    if (wcoef == 0){
        fprintf(stderr, "Error: 'ecoef' not found");
        return 1;
    }
    const char* ofilebase = argv[6];
    double maxwd = atof(argv[7]);
    int seq = 0;
    if (argc > 8){
        seq = atoi(argv[8]);
    }


    /* Initialize the pointcloud */
    int cols;
    fscanf(input_file, "%d", &cols);
    pointcloud_t *pointcloud;
    pointcloud = readPointCloudData(input_file);
    pointcloud->cols = cols;
    pointcloud->rows = cols;
    pointcloud->wcoef = wcoef;
    pointcloud->ecoef = ecoef;
    initializeWatershed(pointcloud);
    watershedAddUniformWater(pointcloud, iwater);
    imagePointCloud(pointcloud, cols, "initial.gif");

    /* Perform the simulation */
    for (int i=0; i < iter; i++){
        watershedStep(pointcloud);
        if (seq != 0){
            if (i % seq == 0 && i != 0){
                size_t newSize = strlen(ofilebase) + snprintf(NULL, 0, "%d", i) + 5;
                char* result = (char*)malloc(newSize);
                snprintf(result, newSize, "%s%d.gif", ofilebase, i);
                imagePointCloudWater(pointcloud, maxwd, result);
                free(result);
            }
        }
    }

    size_t newSize = strlen(ofilebase) + 5;
    char* result = (char*)malloc(newSize);
    snprintf(result, newSize, "%s.gif", ofilebase);
    imagePointCloudWater(pointcloud, maxwd, result);
    free(result);

    return 0;
}