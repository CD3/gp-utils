#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <gputils/io.hpp>

using namespace std;
namespace po = boost::program_options;

void print_manual() {
std::cout << "Extract sub-blocks of data from gnuplot binary datafiles.\n";
}

int main(int argc, char *argv[])
{
  // clang-format off
  po::options_description opt_options("Options");
  opt_options.add_options()
    // these are simple flag options, they do not have an argument.
    ("help,h",    "print help message.")
    //("version",   "print library version.")
    //("manual",    "print manual.")
    //("verbose,v"  , po::value<int>()->implicit_value(0)          , "verbose level.") // an option that takes an argument, but has a default value.
    ("output,o"   , po::value<string>(), "file to write output to.")
    ("every,e"   , po::value<string>()->default_value(""), "gnuplot 'every' string to apply.")
    ("overwrite,x", "overwrite existing output files.")
    ;
  // clang-format on

  // now define our arguments.
  po::options_description arg_options("Arguments");
  arg_options.add_options()("datafile",
                            "data file to extract from.");

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
    cout << "gp-utils-extract [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to read.\n";
    cout << "\n\n";
    cout << R"EOF(

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

)EOF";
    return 0;
  }


  string ifn = vm["datafile"].as<string>();
  string ofn = boost::filesystem::change_extension(ifn,".extracted.bin").string();

  if( vm.count("output") > 0 )
  {
    ofn = vm["output"].as<string>();
  }

  if( !boost::filesystem::exists(ifn) )
  {
    cerr << "ERROR: input file '"<<ifn<<"' does not exist. Check spelling.\n";
    return 1;
  }

  if( boost::filesystem::exists(ofn) && vm.count("overwrite") < 1 )
  {
    cerr << "ERROR: output file '"<<ofn<<"' exists. Use -x to overwrite.\n";
    return 1;
  }


  FilterGPBinary3DDataFile(ifn,ofn,vm["every"].as<string>());
  



  return 0;
}
