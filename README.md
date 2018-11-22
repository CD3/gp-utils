# gp-utils

A collection of tools for working with gnuplot data files.

## Description

`gp-utils` is a collection of command line utilities for doing common datafile manipulations. They are particularly useful when
working with large datasets, which was the primary motivation for writing them. For example, there are tools for converting ASCII
files of 3D data (data that would be plotted with `splot`) to Gnuplot's binary matrix format, which can be several times smaller in
size, and are much faster to load in gnuplot.

There are also utilities to apply filters or transformations to datafiles, such as extracting sub-blocks from a 3D dataset. These
tools work on binary files because they are *much* faster to read and write. If you want to apply these filters/transformations to
and ASCII file, convert it to binary first, apply the filters/transformations, and then convert it back to ASCII.



## Building and Installing

Cmake is used to configure, build, and install:

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ../applications
$ cmake --build .
$ cmake --build . --target install
```

Note the "Release" build. You will want to turn this on as the ASCII datafile reader is about 10x slower in debug mode.
