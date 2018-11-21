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
std::cout << "Display information about gnuplot binary datafiles.\n";
}

int main(int argc, char *argv[])
{
  // clang-format off
  po::options_description opt_options("Options");
  opt_options.add_options()
    // these are simple flag options, they do not have an argument.
    ("help,h",    "print help message.")
    ("version",   "print library version.")
    ("manual",    "print manual.")
    ("verbose,v"  , po::value<int>()->implicit_value(0)          , "verbose level.") // an option that takes an argument, but has a default value.
    ;
  // clang-format on

  // now define our arguments.
  po::options_description arg_options("Arguments");
  arg_options.add_options()("datafile,f",
                            po::value<string>()->default_value("-"),
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
    // print out a usage statement and summary of command line options
    cout << "gp-utils-ascii2bin [options] <file>"
         << "\n\n";
    cout << opt_options << "\n";
    return 0;
  }

  if (vm.count("manual")) {
    // print the manual
    print_manual();
    // print out a usage statement and summary of command line options
    cout << "gp-utils-ascii2bin [options] <file>"
         << "\n\n";
    cout << opt_options << "\n";
    return 0;
  }

  if (vm.count("version")) {
    // print the version number for the library
    cout << "No version information" << endl;
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
