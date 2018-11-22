#include "./transformations.hpp"

int ConvertGP3DDataCylindrical2Cartesian( const GP3DData& idata, std::vector<GP3DData>& odata )
  {
  }
int FilterGP3DData( const GP3DData& idata, GP3DData& odata, const std::string& every_spec )
  {
    GP3DDataEverySpec slice(every_spec);

    size_t iNx, iNy; // old sizes
    size_t oNx, oNy; // new sizes
    size_t yi,xi, ys,xs, ye, xe; // increment, start, stop

    iNx = idata.x.size();
    iNy = idata.y.size();

    yi = slice.y_inc.get_value_or(1);
    xi = slice.x_inc.get_value_or(1);
    ys = slice.y_start.get_value_or(0);
    xs = slice.x_start.get_value_or(0);
    ye = slice.y_end.get_value_or(iNy-1);
    xe = slice.x_end.get_value_or(iNx-1);


    if(ys >= iNy)
      ys = iNy-1;
    if(ye >= iNy)
      ye = iNy-1;

    if(xs >= iNx)
      xs = iNx-1;
    if(xe >= iNx)
      xe = iNx-1;

    if( yi < 1 || xi < 1 )
      throw std::runtime_error("FilterGP3DData inrements can be less than 1. Spec string:"+every_spec);

    if( ye < ys )
      throw std::runtime_error("FilterGP3DData: y (point) end cannot be less than start. Spec string: "+every_spec);

    if( xe < xs )
      throw std::runtime_error("FilterGP3DData: x (block) end cannot be less than start. Spec string: "+every_spec);

    // start and end indices are *inclusive*.
    // if end == start, then there will be 1 piont
    // if end == start + 1, then there will be 2 points
    // etc
    oNy = (ye-ys+1)/yi;
    oNx = (xe-xs+1)/xi;

    odata.y.resize(oNy);
    odata.x.resize(oNx);
    odata.f.resize(oNx*oNy);

    int ij,oj,ii,oi;

    for(ij = ys, oj = 0; ij <= ye; ij += yi, ++oj)
    {
      odata.y[oj] = idata.y[ij];
    }
    for(ii = xs, oi = 0; ii <= xe; ii += xi, ++oi)
    {
      odata.x[oi] = idata.x[ii];
    }

    for(ii = xs, oi = 0; ii <= xe; ii += xi, ++oi)
    {
    for(ij = ys, oj = 0; ij <= ye; ij += yi, ++oj)
    {
      odata.f[oi*oNy+oj] = idata.f[ii*iNy+ij];
    }
    }



  }
