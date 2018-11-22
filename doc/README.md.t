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

## Note on Binary File Format

Gnuplot's binary matrix file format changed between versions 4 and 5. See http://gnuplot.sourceforge.net/docs_4.2/node330.html
for Gnuplot version 4, and pg. 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf for Gnuplot version 5. Basically,
the x and y coordinates were swaped.

These utilities currently assume the **VERSION 4 FORMAT**. This means that you will need to swap the x and y coordinates
when using splot if you are using Gnuplot 5.

```
> splot 'binary-data.bin' using 2:1:3
```

This is the only time that the difference will matter. All of the tools know about x and y coordinates, which have not changed in
the ASCII file format.

At some point, the binary format may be switched to Gnuplot 5, or perhaps an option to specify which format to use will be added.
But it simpler to convert from plain-text to version 4 binary, because the matrix is stored in row-major order in binary file, which
means that elements in the same row and different columns are stored next to each other. If you look at the plain-text format, elements
with the same x coordinate and different y coordinates are stored next to each other, which implies that the x coordinates are row indices.

## Usage

\file{./help-messages.txt}


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
