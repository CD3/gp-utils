#include <libInterpolate/Interpolate.hpp>
#include "./transformations.hpp"

int ConvertGP3DDataCylindrical2Cartesian( const GP3DData& idata, std::vector<GP3DData>& odata )
{
  // interpret input as cylindrical coordinates
  // x -> z
  // y -> r
  
  size_t Nz = idata.x.size();
  size_t Nr = idata.y.size();

  float rmin = idata.y[0];
  float rmax = idata.y[Nr-1];

  if( rmin < 0 )
    throw std::runtime_error("ConvertGP3DDataCylindrical2Cartesian minimum r coordinate (y column) cannot be less than 0.");

  size_t Nx = 2*Nr;
  size_t Ny = 2*Nr;

  if( rmin == 0 )
  {
    Nx -= 1;
    Ny -= 1;
  }

  float dx = 2*rmax/(Nx-1);
  float dy = 2*rmax/(Ny-1);

  odata.resize(Nz);


  for(int k = 0; k < Nz; ++k)
  {
    odata[k].x.resize(Nx);
    odata[k].y.resize(Ny);
    odata[k].f.resize(Nx*Ny);

    for(int i = 0; i < Nx; ++i)
    {
      odata[k].x[i] = -rmax + i*dx;
    }

    for(int j = 0; j < Ny; ++j)
    {
      odata[k].y[j] = -rmax + j*dy;
    }

    // create an interpolator to interpolate this z slice
    // r coordinates are stored consecutively
    _1D::CubicSplineInterpolator<float> interp;
    interp.setData(Nr, idata.y.data(), idata.f.data()+k*Nz);
    float r;

    for(int i = 0; i < Nx; ++i)
    {
    for(int j = 0; j < Ny; ++j)
    {
      r = sqrt( odata[k].x[i]*odata[k].x[i] + odata[k].y[j]*odata[k].y[j] );
      odata[k].f[Ny*i + j] = interp( r );
    }
    }

  }

  return 0;
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
      throw std::runtime_error("FilterGP3DData increments can be less than 1. Spec string:"+every_spec);

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


    return 0;

  }
