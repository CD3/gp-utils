#ifndef gputils_exceptions_hpp
#define gputils_exceptions_hpp

/** @file exceptions.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 11/24/18
  */

#include <stdexcept>

class corrupt_binary_matrix_file_error : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};


#endif // include protector
