#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
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
    //("manual",    "print manual.")
    //("verbose,v"  , po::value<int>()->implicit_value(0)          , "verbose level.") // an option that takes an argument, but has a default value.
    ;
  // clang-format on

  // now define our arguments.
  po::options_description arg_options("Arguments");
  arg_options.add_options()("datafile",
                            "data file to read.");

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
    cout << "gp-utils-info [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to read.\n";
    cout << "\n\n";
    cout << R"EOF(
Reads a Gnuplot binary matrix datafile (see http://gnuplot.sourceforge.net/docs_4.2/node330.html) and prints
information about it, such as the number of x coordinates, number of y coordinates, etc. This is useful when
extracting data from a binary file.

NOTE: Gnuplot the x-y coordinates in its binary matrix file format between
versions 4 and 5. The gp-utils tools ASSUME THE VERSION 4 FORMAT. This means
that you will need to swap the x and y axes when using splot if you are using
Gnuplot version 5. See page 196 of http://www.gnuplot.info/docs_5.2/Gnuplot_5.2.pdf
and http://gnuplot.sourceforge.net/docs_4.2/node330.html.

  > splot 'ascii-datafile.txt', 'binary-datafile.bin' using 2:1:3

)EOF";
    return 0;
  }


  string ifn = vm["datafile"].as<string>();


  if( !boost::filesystem::exists(ifn) )
  {
    cerr << "ERROR: input file '"<<ifn<<"' does not exist. Check spelling.\n";
    return 1;
  }



  GP3DDataInfo info;
  QueryGPBinary3DDataFile(ifn,info);

  std::cout << "File Summary:\n";
  std::cout << "\t" << "Name: "<< ifn << "\n";
  std::cout << "\t" << "Size:\n";
  std::cout << "\t\t" << "Bytes: " << info.size_in_bytes << "\n";
  std::cout << "\t\t" << "Floats: " << info.size_in_bytes.get()/sizeof(float) << "\n";
  std::cout << "\t\t" << "Function Values: " << info.Nx.get()*info.Ny.get() << "\n";
  std::cout << "\t" << "X:\n";
  std::cout << "\t\t" << "N:" << info.Nx << "\n";
  std::cout << "\t\t" << "Min:" << info.xmin << "\n";
  std::cout << "\t\t" << "Max:" << info.xmax << "\n";
  std::cout << "\t" << "Y:\n";
  std::cout << "\t\t" << "N:" << info.Ny << "\n";
  std::cout << "\t\t" << "Min:" << info.ymin << "\n";
  std::cout << "\t\t" << "Max:" << info.ymax << "\n";
  



  return 0;
}
