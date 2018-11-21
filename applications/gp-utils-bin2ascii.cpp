#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <gputils/io.hpp>

using namespace std;
namespace po = boost::program_options;

void print_manual() {
std::cout << "Convert gnuplot ASCII data file to binary.\n";
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
    ("output,o"   , po::value<string>(), "file to write output to.")
    //("memory-efficient,m", "use memory efficient method. required if dataset cannot fit into memory at once.")
    ("overwrite,x", "overwrite existing output files.")
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
