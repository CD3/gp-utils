#ifndef gputils_structs_hpp
#define gputils_structs_hpp
/** @file structs.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 11/20/18
  */

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


#endif // include protector
