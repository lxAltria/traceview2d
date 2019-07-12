# traceview2d
A simple tracer and viewer for 2d vector field



Command line arguments:
./main filepath offset seed_x seed_y nmax step

offset - value of z coordinate to slice the 3D data set

seed_x - x coordinate of seed point

seed_y - y coordinate of seed point

nmax - maximum number of advection steps

step - size of each advection step


Example command line arguments:
./main /path/to/nek5000.nc 22 75.5 75.3 100 0.25