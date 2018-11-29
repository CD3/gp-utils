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

### gp-utils-ascii2bin
```bash
gp-utils-ascii2bin [options] <datafile>

Options:
  -h [ --help ]              print help message.
  -o [ --output ] arg        file to write output to.
  -m [ --memory-efficient ]  use memory efficient method. required if dataset 
                             cannot fit into memory at once.
  -x [ --overwrite ]         overwrite existing output files.

Arguments:
  <datafile>      the datafile to convert.

Converts an ASCII 3D Gnuplot datafile to binary matrix format (see http://gnuplot.sourceforge.net/docs_4.2/node330.html).
Only 3 column files with regularly coordinates are supported (i.e. only files that can be plotted with `splot` with
a mesh.

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3

Examples:

  Convert 3 column ASCII data file named 'data.txt' to binary matrix data file named 'data.bin'
    
    $ gp-utils-ascii2bin data.txt

  By default, gp-utils-ascii2bin won't overwrite the output file if it exists. Use the -x option to override this.
  Convert 3 column ASCII data file named 'data.txt' to binary matrix data file named 'data.bin', overwriting it if
  it already exists.

    $ gp-utils-ascii2bin -x data.txt

  Convert 3 column ASCII data file named 'data.txt' to binary matrix data file named 'binary-data.bin', overwriting it if
  it already exists.

    $ gp-utils-ascii2bin -x -o binary-data.bin data.txt

```

### gp-utils-bin2ascii
```bash
gp-utils-bin2ascii [options] <datafile>

Options:
  -h [ --help ]         print help message.
  -o [ --output ] arg   file to write output to.
  -x [ --overwrite ]    overwrite existing output files.

Arguments:
  <datafile>      the datafile to convert.



Converts a Gnuplot binary matrix datafile (see http://gnuplot.sourceforge.net/docs_4.2/node330.html) to ASCII (plain text).
Only 3 column files with regularly coordinates are supported (i.e. only files that can be plotted with `splot` with
a mesh.

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3

Examples:

  Convert binary matrix data file named 'data.bin' to a 3 column ASCII data file named 'data.txt'.
    
    $ gp-utils-bin2ascii data.bin

  By default, gp-utils-bin2ascii won't overwrite the output file if it exists. Use the -x option to override this.
  Convert binary matrix data file named 'data.bin' to a 3 column ASCII data file named 'data.txt', overwriting it if
  it already exists.

    $ gp-utils-bin2ascii -x data.bin

  Convert binary matrix data file named 'data.bin' to a 3 column ASCII data file named 'plain-text-data.txt', overwriting it if
  it already exists.

    $ gp-utils-bin2ascii -x -o plain-text-data.bin data.bin

```

### gp-utils-cyl2rec
```bash
gp-utils-cyl2rec [options] <datafile>

Options:
  -h [ --help ]         print help message.
  -o [ --output ] arg   file to write output to.
  -e [ --every ] arg    gnuplot 'every' string to apply.
  -x [ --overwrite ]    overwrite existing output files.

Arguments:
  <datafile>      the datafile to read.



Create a Cartesian datafile from cylindrical data.

This tool reads a Gnuplot binary matrix datafile, interprets the x-y coordinates as z-r coordinates, creates an x-y slices
each z coordinates, and writes them to new binary matrix datafiles. This is useful if you are working with
azmuthally symmetric function and want to create a plot of the profile at some z slice. For example, to generate a plot
of the beam profile of a circular Gaussian beam at multiple positions along the propagation axis.

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3


Examples:


```

### gp-utils-extract
```bash
gp-utils-extract [options] <datafile>

Options:
  -h [ --help ]         print help message.
  -o [ --output ] arg   file to write output to.
  -e [ --every ] arg    gnuplot 'every' string to apply.
  -x [ --overwrite ]    overwrite existing output files.

Arguments:
  <datafile>      the datafile to read.




Extracts a sub-block of data from a Gnuplot binary matrix data file (see http://gnuplot.sourceforge.net/docs_4.2/node330.html).
The sub-block to extract is specified using Gnuplot's every option syntax (i.e. ::10:5:20:10).
See http://gnuplot.sourceforge.net/docs_4.2/node121.html

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3

However, Gnuplot's the every option syntax is based on ASCII file format, which assumes
consecutive points in a block have different y coordinates with the same x coordinate.
So Gnuplot's start_point and stop_point correspond to y coordinates, and start_block and
stop_block correspond to x coordinates. See http://gnuplot.sourceforge.net/docs_4.2/node121.html
for documentation on the every option.

Examples:

  Extract all points from the 5'th to 10'th x coordinates (inclusive) from a binary data file
  named 'data.bin' and write a new binary data file named 'data.extracted.bin':

  > gp-utils-extract -e :::5::10 data.bin

  Extract all points from the 5'th to 10'th x coordinates (inclusive), and the
  3rd to 8th y coordinates from a binary data file named 'data.bin' and write a
  new binary data file named 'data.extracted.bin', overwriting if it already
  exists:

  > gp-utils-extract -x -e :::5:3:10:8 data.bin

```

### gp-utils-info
```bash
gp-utils-info [options] <datafile>

Options:
  -h [ --help ]         print help message.

Arguments:
  <datafile>      the datafile to read.



Reads a Gnuplot binary matrix datafile (see http://gnuplot.sourceforge.net/docs_4.2/node330.html) and prints
information about it, such as the number of x coordinates, number of y coordinates, etc. This is useful when
extracting data from a binary file.

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3

```




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
