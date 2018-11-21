#ifndef gputils_transformations_hpp
#define gputils_transformations_hpp

/** @file transformations.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 11/20/18
  */


stuct GP3DDataSlice
{
  int xmin,xmax,dx;
  int ymin,ymax,dy;
};

int SliceGPBinary3DData(const std::string& ifilename, std::string& ofilename, Slice slice)
{
}


#endif // include protector
