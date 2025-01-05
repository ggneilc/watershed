# Watershed Simulation

This program serves as a singular cellular automaton, representing a cubic square of terrain and how fluid would flow over said terrain and how it would collect. 

## Part A

For this specific program, we are to implement a method `stat1`, which is defined in the header file `pointcloud.h`, to take a redirected input from `stdin` in the form `x.0 y.0 z.0` and calculate the minimum, maximum, and average `z` value, and display the `x,y` where the min and max are located.

### Running & Compiling

To compile the program, simply type `make parta` while in the directory. The makefile contains a `CC` variable at the top to change the compiler if using something different than gcc. 

To run the program, pipe in the targetted file; if the file is named `test.txt`, run `cat test.txt | ./parta`

If the program is ran without a file being piped in, then it will wait for user input then exit. 


## Part B

This program expands on part a by:
1. Reading from a file's contents instead of just redirection. 
2. Creating structs to store the line contents (`x y z`)
3. Graphing the structs to a bitmap partitioned by their z value
4. outputting the bitmap with their respective color values 

There is a slight issue with mapping pointcloud x,y coordinates to their pixel position on the bitmap; some pixel x,y coordinates gets skipped and create black streaks across the image. 

### Running & Compiling

To compile the program, simply type `make` in the directory. 

To run the program, execute `./partb file.xyz`: Simply pass a pointcloud file as an argument to the executable and it will parse through it and run. If no file is given the program will display a message asking for a file input. 

# Part C


This part implements the water simulation onto the terrain in the pointcloud file. It creates a pointcloud data structure to house all the individual terrain cells, initialize them with a given amount of water, then simulations for a given amount of iterations of how water would flow over the terrain, shading the map more blue for areas that have more water.

The program follows the specifications, but there is a known issue is the `watershedStep()` function. The actual cause of the issue is unknown, but the formula for determining waterflow was not well described and had many changes. The current implementation adheres to the doc, but all water always drains out of all cells. I have had some iterations that produce terrain maps that had blue shading, but they wouldn't update consistently and did not follow the design doc. # watershed
