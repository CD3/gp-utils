#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options.hpp>
#include <boost/spirit/include/qi.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <gputils/io.hpp>
#include <gputils/exceptions.hpp>

using namespace std;
namespace po = boost::program_options;

namespace qi = boost::spirit::qi;

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
  arg_options.add_options()("datafile", "data file to extract from.");

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
    cout << "gp-utils-cyl2rec [options] <datafile>";
    cout << "\n\n";
    cout << opt_options << "\n";
    cout << "Arguments:\n";
    cout << "  <datafile>      the datafile to read.\n";
    cout << "\n\n";
    cout << R"EOF(
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


)EOF";
    return 0;
  }

  string ifn = vm["datafile"].as<string>();
  string ofn =
      boost::filesystem::change_extension(ifn, ".cyl2rec.bin").string();

  if (vm.count("output") > 0) {
    ofn = vm["output"].as<string>();
  }

  if (!boost::filesystem::exists(ifn)) {
    cerr << "ERROR: input file '" << ifn
         << "' does not exist. Check spelling.\n";
    return 1;
  }

  if (vm.count("overwrite") < 1) {
    boost::filesystem::path opath(ofn);
    if( !opath.is_absolute() && opath.parent_path().empty() )
    { // prepend current directory is no directory was given.
      opath = boost::filesystem::path(".") / opath;
    }
    vector<string> files;
    for (auto &x : boost::filesystem::directory_iterator(opath.parent_path())) {
      string fn = x.path().generic_string();
      auto it = fn.begin();
      if( qi::parse(it, fn.end(), qi::lit(opath.generic_string()) >> *(qi::lit(".") >> qi::int_) ) && it == fn.end() )
        files.push_back(fn);
    }

    if(files.size() > 0)
    {
       cerr << "ERROR: output file '"<<ofn<<"' could cause the following files to be overwritten:\n";
       for( auto f : files )
       {
         std::cerr << "  " << f << std::endl;
       }
       cerr << "Use -x to overwrite.\n";
      return 1;
    }
  }

  try{
  ConvertGPBinary3DDataFileCylindrical2Cartesian(ifn,ofn);
  }
  catch(corrupt_binary_matrix_file_error& e)
  {
    std::cerr << "There was a problem reading the input file '"<<ifn<<"'. It is either corrupt, or not a binary matrix file." << std::endl;
    return 1;
  }

  return 0;
}
