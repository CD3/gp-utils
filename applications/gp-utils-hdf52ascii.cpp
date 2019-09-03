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
    cout << "gp-utils-hdf52ascii [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to convert.\n";
    cout << R"EOF(
Converts a HDF5 datafile to ASCII Gnuplot format. The hdf5 format is the format used by `libField` (https://github.com/CD3/libField), and
is the format written by `gp-utils-ascii2hdf5`. Both 2 and 3 column ASCII files are supported. By default, 3 columns are assumed. To convert
2 column files, use `--columns 2`.
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
    ConvertHDF5Field2GPASCII3DDataFile(ifn, ofn);
  else if (vm["columns"].as<size_t>() == 2)
    ConvertHDF5Field2GPASCII2DDataFile(ifn, ofn);
  else {
    std::cerr << "ERROR: Only 2 or 3 column files are supported. You specified "
              << vm["columns"].as<size_t>() << "\n";
    return 1;
  }

  return 0;
}
