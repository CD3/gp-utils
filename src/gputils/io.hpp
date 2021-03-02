#ifndef Parsers_hpp
#define Parsers_hpp

/** @file Parsers.hpp
 * @brief
 * @author C.D. Clark III
 * @date 11/17/18
 */

#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include <string>

#include "./structs.hpp"

namespace spt = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

BOOST_FUSION_ADAPT_STRUCT(GPDataLine,
                          (std::vector<float>, data)(std::string, comment))

template <typename Iterator>
struct GPASCIIDataLineParser : spt::qi::grammar<Iterator, GPDataLine()> {
  qi::rule<Iterator, std::string()> comment;
  qi::rule<Iterator, std::vector<float>()> data;
  qi::rule<Iterator, GPDataLine()> line;
  qi::rule<Iterator> whitespace;
  qi::rule<Iterator> delimiter;

  GPASCIIDataLineParser(qi::rule<Iterator> del = +qi::ascii::blank)
      : GPASCIIDataLineParser::base_type(line)
  {
    whitespace = qi::ascii::blank;
    delimiter = del;
    comment = qi::lit("#") >> qi::lexeme[*(qi::char_ - qi::eol)] >> *qi::eol;
    data = qi::float_ % delimiter;
    line = *whitespace >> +data >> *whitespace >>
               *comment  // data with optional comment
           | *whitespace >> *data >> *whitespace >>
                 comment   // comment with optional data
           | *whitespace;  // blank line
  }
};

int ReadGPASCII2DDataFile(std::string filename, GP2DData& data);
int WriteGPASCII2DDataFile(std::string filename, const GP2DData& data);
int ReadHDF5Field2DDataFile(std::string filename, GP2DData& data);
int WriteHDF5Field2DDataFile(std::string filename, const GP2DData& data);

int ReadGPASCII3DDataFile(std::string filename, GP3DData& data);
int WriteGPASCII3DDataFile(std::string filename, const GP3DData& data);
int ReadGPBinary3DDataFile(std::string filename, GP3DData& data);
int WriteGPBinary3DDataFile(std::string filename, const GP3DData& data);
int ReadHDF5Field3DDataFile(std::string filename, GP3DData& data);
int WriteHDF5Field3DDataFile(std::string filename, const GP3DData& data);

int QueryGPBinary3DDataFile(std::string filename, GP3DDataInfo& info);


int ConvertGPASCII2HDF5Field2DDataFile(std::string ifilename,
                                       std::string ofilename);
int ConvertHDF5Field2GPASCII2DDataFile(std::string ifilename,
                                       std::string ofilename);


enum class ConvertMethod { ReadThenWrite, SimultaneousReadWrite };
int ConvertGPASCII2Binary3DDataFile(std::string ifilename,
                                    std::string ofilename,
                                    ConvertMethod method = ConvertMethod::ReadThenWrite);

int ConvertGPBinary2ASCII3DDataFile(std::string ifilename,
                                    std::string ofilename);

int ConvertGPASCII2HDF5Field3DDataFile(std::string ifilename,
                                       std::string ofilename);
int ConvertHDF5Field2GPASCII3DDataFile(std::string ifilename,
                                       std::string ofilename);

enum class FilterMethod { ReadThenWrite, SimultaneousReadWrite };
int FilterGPBinary3DDataFile(std::string ifilename,
                             std::string ofilename,
                             std::string every_spec,
                                   FilterMethod method = FilterMethod::ReadThenWrite);

int ConvertGPBinary3DDataFileCylindrical2Cartesian(std::string ifilename,
                                                   std::string ofilename);
#endif  // include protector
