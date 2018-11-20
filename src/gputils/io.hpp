#ifndef Parsers_hpp
#define Parsers_hpp

/** @file Parsers.hpp
 * @brief
 * @author C.D. Clark III
 * @date 11/17/18
 */

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scope_exit.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace spt = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

struct GPDataLine {
  std::vector<float> data;
  std::string comment;
  void clear()
  {
    data.clear();
    comment.clear();
  }
};

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

struct GP3DData {
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> f;
  void clear()
  {
    x.clear();
    y.clear();
    f.clear();
  }
};

int ReadGPASCII3DDataFile(const std::string& filename, GP3DData& data);
int ReadGPBinary3DDataFile(const std::string& filename, GP3DData& data);
int WriteGPBinary3DDataFile(const std::string& filename, const GP3DData& data);
int WriteGPASCII3DDataFile(const std::string& filename, const GP3DData& data);

enum class ConvertMethod { ReadThenWrite, SimultaneousReadWrite };
int ConvertGPASCII2Binary3DDataFile(const std::string& ifilename,
                                    const std::string& ofilename,
                                    ConvertMethod method = ConvertMethod::ReadThenWrite);

#endif  // include protector