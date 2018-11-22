#ifndef gputils_transformations_hpp
#define gputils_transformations_hpp


/** @file transformations.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 11/20/18
  */

#include <string>
#include <string>
#include "./structs.hpp"


/**
 * Converts a 3D data
 */
int ConvertGP3DDataCylindrical2Cartesian( const GP3DData& idata, std::vector<GP3DData>& odata );
int FilterGP3DData( const GP3DData& idata, GP3DData& odata, const std::string& every_spec );

#endif // include protector
