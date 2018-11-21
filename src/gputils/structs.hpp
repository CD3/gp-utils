#ifndef gputils_structs_hpp
#define gputils_structs_hpp
/** @file structs.hpp
 * @brief
 * @author C.D. Clark III
 * @date 11/20/18
 */

#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <string>
#include <vector>

struct GPDataLine {
  std::vector<float> data;
  std::string comment;
  void clear()
  {
    data.clear();
    comment.clear();
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

struct GP3DDataInfo {
  boost::optional<size_t> Nx;
  boost::optional<size_t> Ny;

  boost::optional<float> xmin;
  boost::optional<float> xmax;

  boost::optional<float> ymin;
  boost::optional<float> ymax;

  boost::optional<size_t> size_in_bytes;

  void clear()
  {
    Nx = boost::none;
    xmin = boost::none;
    xmax = boost::none;
    Ny = boost::none;
    ymin = boost::none;
    ymax = boost::none;
    size_in_bytes = boost::none;
  }
};


struct GP3DDataEverySpec {
  GP3DDataEverySpec(std::string spec)
  {
    using boost::spirit::qi::int_;
    using boost::spirit::qi::uint_;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::ascii::blank;

    auto it = std::begin(spec);
    bool r;
    //clang-format off

    phrase_parse(it, std::end(spec), -uint_, blank, y_inc);
    phrase_parse(it, std::end(spec), ":", blank);
    phrase_parse(it, std::end(spec), -uint_, blank, x_inc);
    phrase_parse(it, std::end(spec), ":", blank);
    phrase_parse(it, std::end(spec), -uint_, blank, y_start);
    phrase_parse(it, std::end(spec), ":", blank);
    phrase_parse(it, std::end(spec), -uint_, blank, x_start);
    phrase_parse(it, std::end(spec), ":", blank);
    phrase_parse(it, std::end(spec), -uint_, blank, y_end);
    phrase_parse(it, std::end(spec), ":", blank);
    phrase_parse(it, std::end(spec), -uint_, blank, x_end);
    
    //clang-format on
    if(it != std::end(spec))
    {
      throw std::runtime_error("Could not parse gnuplot every spec string: "+spec);
    }
  }

  boost::optional<size_t> y_inc;
  boost::optional<size_t> x_inc;
  boost::optional<size_t> y_start;
  boost::optional<size_t> x_start;
  boost::optional<size_t> y_end;
  boost::optional<size_t> x_end;
};

#endif  // include protector
