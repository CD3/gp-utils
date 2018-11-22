#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
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
    //("version",   "print library version.")
    //("verbose,v"  , po::value<int>()->implicit_value(0)          , "verbose level.") // an option that takes an argument, but has a default value.
    ("output,o"   , po::value<string>(), "file to write output to.")
    //("memory-efficient,m", "use memory efficient method. required if dataset cannot fit into memory at once.")
    ("overwrite,x", "overwrite existing output files.")
    ;
  // clang-format on

  // now define our arguments.
  po::options_description arg_options("Arguments");
  arg_options.add_options()("datafile",
                            "data file to convert.");

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
    cout << "gp-utils-bin2ascii [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to convert.\n";
    cout << "\n\n";
    cout << R"EOF(
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

)EOF";
    return 0;
  }

  if (vm.count("version")) {
    // print the version number for the library
    cout << "No version information" << endl;
    return 0;
  }

  string ifn = vm["datafile"].as<string>();
  string ofn = boost::filesystem::change_extension(ifn,".txt").string();
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


  ConvertGPBinary2ASCII3DDataFile(ifn,ofn);
  // no memory efficient method implemented yet
  //if( vm.count("memory-efficient") )
  //{
  //ConvertGPASCII2Binary3DDataFile(ifn,ofn, ConvertMethod::SimultaneousReadWrite);
  //}
  //else
  //{
  //ConvertGPASCII2Binary3DDataFile(ifn,ofn, ConvertMethod::ReadThenWrite);
  //}


  return 0;
}
