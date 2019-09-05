#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <gputils/io.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  // clang-format off
  po::options_description opt_options("Options");
  opt_options.add_options()
    // these are simple flag options, they do not have an argument.
    ("help,h",    "print help message.")
    ("output,o"   , po::value<string>(), "file to write output to.")
    ("columns,c"   , po::value<size_t>()->default_value(3), "number of ASCII columns (2 or 3).")
    ("overwrite,x", "overwrite existing output files.")
    ;
  // clang-format on

  // now define our arguments.
  po::options_description arg_options("Arguments");
  arg_options.add_options()("datafile", "data file to convert.");

  // combine the options and arguments into one option list.
  // this is what we will use to parse the command line, but
  // when we output a description of the options, we will just use
  // opt_options
  po::options_description all_options("Options");
  all_options.add(opt_options).add(arg_options);

  // tell boost how to translate positional options to named options
  po::positional_options_description args;
  args.add("datafile", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(all_options)
                .positional(args)
                .run(),
            vm);
  po::notify(vm);

  // boiler plate configuration and help
  // -----------------------------------

  if (argc == 1 || vm.count("help")) {
    cout << "gp-utils-ascii2hdf5 [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to convert.\n";
    cout << R"EOF(
Converts an ASCII Gnuplot datafile to HDF5 format. The format written is the format used by `libField` (https://github.com/CD3/libField).
Each coordinate axis is written to a dataset named "axis <N>", where N is the axis number (starting at zero), and the function
is written to a dataset named field. For example:

A file named "example.txt" containing

```
0 1 10
0 2 20
0 3 30

10 1 11
10 2 21
10 3 31
```

would be written to a HDF5 file with the text representation (run `h5dump` on the file)

```
HDF5 "example.h5" {
GROUP "/" {
   DATASET "axis 0" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
      DATA {
      (0): 0, 10
      }
   }
   DATASET "axis 1" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 3 ) / ( 3 ) }
      DATA {
      (0): 0, 1, 3
      }
   }
   DATASET "field" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 2, 3 ) / ( 2, 3 ) }
      DATA {
      (0): 10, 20, 30, 11, 21, 31
      }
   }
}
}
```

Both 2 and 3 column ASCII files are supported. By default, 3 columns are assumed. To convert 2 column
files, use `--columns 2`.

Examples:

  Convert 3 column ASCII data file named 'data.txt' to HDF5 field data file named 'data.h5'
    
    $ gp-utils-ascii2hdf5 data.txt

  By default, gp-utils-ascii2hdf5 won't overwrite the output file if it exists. Use the -x option to override this.
  Convert 3 column ASCII data file named 'data.txt' to HDF5 field data file named 'data.h5', overwriting it if
  it already exists.

    $ gp-utils-ascii2hdf5 -x data.txt

  Convert 3 column ASCII data file named 'data.txt' to HDF5 field data file named 'binary-data.h5', overwriting it if
  it already exists.

    $ gp-utils-ascii2hdf5 -x -o binary-data.h5 data.txt

)EOF";
    return 0;
  }

  string ifn = vm["datafile"].as<string>();
  string ofn = boost::filesystem::change_extension(ifn, ".h5").string();
  if (vm.count("output") > 0) {
    ofn = vm["output"].as<string>();
  }

  if (!boost::filesystem::exists(ifn)) {
    cerr << "ERROR: input file '" << ifn
         << "' does not exist. Check spelling.\n";
    return 1;
  }

  if (boost::filesystem::exists(ofn) && vm.count("overwrite") < 1) {
    cerr << "ERROR: output file '" << ofn << "' exists. Use -x to overwrite.\n";
    return 1;
  }

  if (vm["columns"].as<size_t>() == 3)
    ConvertGPASCII2HDF5Field3DDataFile(ifn, ofn);
  else if (vm["columns"].as<size_t>() == 2)
    ConvertGPASCII2HDF5Field2DDataFile(ifn, ofn);
  else {
    std::cerr << "ERROR: Only 2 or 3 column files are supported. You specified "
              << vm["columns"].as<size_t>() << "\n";
    return 1;
  }

  return 0;
}
