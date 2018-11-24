
#include <fstream>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scope_exit.hpp>

#include "./io.hpp"
#include "./transformations.hpp"


int ReadGPASCII3DDataFile(const std::string& ifilename, GP3DData& data)
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }

  std::ifstream fin;
  std::string line;
  GPASCIIDataLineParser<std::string::iterator> lparser;
  bool r;
  std::string::iterator it;

  data.x.clear();
  data.y.clear();
  data.f.clear();

  fin.open(ifilename.c_str());
  BOOST_SCOPE_EXIT(&fin) { fin.close(); }
  BOOST_SCOPE_EXIT_END

  size_t linenumber = 0;
  while (getline(fin, line)) {
    linenumber++;
    it = line.begin();
    GPDataLine ldata;
    r = qi::parse(it, line.end(), lparser, ldata);
    if (r) {
      if (ldata.data.size() > 3)
        throw std::runtime_error("Too many columns found on line " +
                                 boost::lexical_cast<std::string>(linenumber));

      if (ldata.data.size() != 0 && ldata.data.size() < 3)
        throw std::runtime_error("Too few columns found on line " +
                                 boost::lexical_cast<std::string>(linenumber));

      if (ldata.data.size() == 3) {
        if (data.x.size() < 1) {  // no data has been pushed yet
          data.x.push_back(ldata.data[0]);
          data.y.push_back(ldata.data[1]);
          data.f.push_back(ldata.data[2]);
        } else {
          data.f.push_back(ldata.data[2]);

          // only push x value if it is different than the last one
          if (ldata.data[0] != data.x[data.x.size() - 1])
            data.x.push_back(ldata.data[0]);

          // only push y value if we are still on the first x value
          if (data.x.size() == 1) data.y.push_back(ldata.data[1]);
        }
      }
    } else {
      throw std::runtime_error("There was an error parsing the line '" + line +
                               "'.");
    }
  }

  return 0;
}


int ReadGPBinary3DDataFile(const std::string& ifilename, GP3DData& data)
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }
  // get size of file so we can compute how many elements it holds
  auto fs = boost::filesystem::file_size(ifilename);
  size_t N = fs/sizeof(float);
  std::ifstream fin;

  data.x.clear();
  data.y.clear();
  data.f.clear();

  fin.open(ifilename.c_str(), std::ios::in | std::ios::binary);
  BOOST_SCOPE_EXIT(&fin) { fin.close(); }
  BOOST_SCOPE_EXIT_END

  float buffer;
  // first number is Ny
  fin.read(reinterpret_cast<char*>(&buffer),sizeof(float));
  size_t Ny = buffer;
  data.y.resize(Ny);
  // read in y coordinates
  fin.read(reinterpret_cast<char*>(data.y.data()),Ny*sizeof(float));
  // file holds a total of N elements.
  // - one of these is the number of y corrdinates
  // - Ny of these are the y coordinates
  // - That leaves N - 1 - Ny for the x coordinates and function values.
  //
  // N will also be: N = 1 + Ny + Nx + Nx*Ny
  // so N - 1 - Ny = Nx + Nx*Ny = Nx*(1 + Ny)
  // which means Nx = (N - 1 - Ny)/1 + Ny)
  size_t Nx = (N - 1 - Ny)/(1 + Ny);
  data.x.resize(Nx);
  data.f.resize(Nx*Ny);
  // read in data
  for(int i = 0; i < Nx; ++i)
  {
    fin.read(reinterpret_cast<char*>(data.x.data()+i),sizeof(float));
    fin.read(reinterpret_cast<char*>(data.f.data()+i*Ny),Ny*sizeof(float));
  }



  return 0;
}

int QueryGPBinary3DDataFile(const std::string& ifilename, GP3DDataInfo& info)
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }
  info.clear();

  auto fs = boost::filesystem::file_size(ifilename);
  size_t N = fs/sizeof(float);
  float tmp;

  info.size_in_bytes = fs;

  std::ifstream fin;

  fin.open(ifilename.c_str(), std::ios::in | std::ios::binary);
  BOOST_SCOPE_EXIT(&fin) { fin.close(); }
  BOOST_SCOPE_EXIT_END


  fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
  
  info.Ny = tmp;
  info.Nx = (N - 1 - info.Ny.get())/(1 + info.Ny.get());

  fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
  info.ymin = tmp;

  if( info.Ny.get() > 1 )
  {
    fin.ignore((info.Ny.get()-2)*sizeof(float));
    fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
    info.ymax = tmp;
  }
  else
  {
    info.ymax = info.ymin.get();
  }

  fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
  info.xmin = tmp;

  if( info.Nx.get() > 1)
  {
    fin.ignore((info.Ny.get())*sizeof(float));
    if( info.Nx.get() > 2 )
    {
      fin.ignore(((info.Ny.get()+1)*(info.Nx.get()-2))*sizeof(float));
    }
    fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
    info.xmax = tmp;
  }
  else
  {
  info.xmax = info.xmin.get();
  }

}


int WriteGPBinary3DDataFile(const std::string& ofilename, const GP3DData& data)
{
  std::ofstream fout;

  fout.open(ofilename.c_str(), std::ios::out | std::ios::binary);
  BOOST_SCOPE_EXIT(&fout) { fout.close(); }
  BOOST_SCOPE_EXIT_END

  float tmp;
  tmp = data.y.size();
  fout.write(reinterpret_cast<const char*>(&tmp), sizeof(float));
  fout.write(reinterpret_cast<const char*>(data.y.data()),
             data.y.size() * sizeof(float));
  for (int i = 0; i < data.x.size(); ++i) {
    tmp = data.x[i];
    fout.write(reinterpret_cast<const char*>(&tmp), sizeof(float));
    fout.write(reinterpret_cast<const char*>(data.f.data() + i * data.y.size()),
               data.y.size() * sizeof(float));
  }

  return 0;
}

int WriteGPASCII3DDataFile(const std::string& ofilename, const GP3DData& data)
{
  std::ofstream fout;

  fout.open(ofilename.c_str(), std::ios::out);
  BOOST_SCOPE_EXIT(&fout) { fout.close(); }
  BOOST_SCOPE_EXIT_END

  size_t Nx = data.x.size();
  size_t Ny = data.y.size();
  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      fout << data.x[i] << " " << data.y[j] << " " << data.f[i * Ny + j]
           << "\n";
    }
    fout << "\n";
  }

  return 0;
}

int ConvertGPASCII2Binary3DDataFile(const std::string& ifilename,
                                    const std::string& ofilename,
                                    ConvertMethod method )
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }

  if (method == ConvertMethod::ReadThenWrite) {
    // this method is simple, but it requires that
    // there be enough memory to store the entire
    // data set.
    GP3DData data;

    ReadGPASCII3DDataFile(ifilename, data);
    WriteGPBinary3DDataFile(ofilename, data);

    return 0;
  }

  if (method == ConvertMethod::SimultaneousReadWrite) {
    // this is a memory efficient method.
    std::ifstream fin;
    std::ofstream fout;

    GPASCIIDataLineParser<std::string::iterator> lparser;
    bool r;
    std::string::iterator it;

    std::string iline;
    std::array<std::vector<float>, 2> oline;

    fin.open(ifilename.c_str(), std::ios::in);
    fout.open(ofilename.c_str(), std::ios::out | std::ios::binary);
    BOOST_SCOPE_EXIT(&fin, &fout)
    {
      fin.close();
      fout.close();
    }
    BOOST_SCOPE_EXIT_END

    size_t linenumber = 0;
    bool first_block = true;
    bool new_block = false;
    int i;
    size_t N;
    while (getline(fin, iline)) {
      linenumber++;
      it = iline.begin();
      GPDataLine ldata;
      r = qi::parse(it, iline.end(), lparser, ldata);
      if (r) {
        if (ldata.data.size() > 3)
          throw std::runtime_error(
              "Too many columns found on line " +
              boost::lexical_cast<std::string>(linenumber));

        if (ldata.data.size() != 0 && ldata.data.size() < 3)
          throw std::runtime_error(
              "Too few columns found on line " +
              boost::lexical_cast<std::string>(linenumber));

        if (ldata.data.size() == 3) {
          // the first line in the binary file needs to be
          // the number of y coordinates followed by all y coordinates.
          // the second will be the first x coordinate, followed by all function
          // values for that x coordinate
          //
          // Ny y1  y2  ... yN
          // x1 f11 f12 ... f1N
          // ...
          //
          // so we need to store all y coordinates and function values until we
          // reach the second x coordinate
          if (first_block) {
            if (oline[0].size() == 0) {  // first line of data
              oline[1].push_back(0);     // Ny. just set to zero for now
              oline[1].push_back(ldata.data[1]);  // first y coordinate
              oline[0].push_back(ldata.data[0]);  // first x coordinates
              oline[0].push_back(ldata.data[2]);  // first function value
            } else {
              if (ldata.data[0] != oline[0][0]) {  // found new block
                first_block = false;
                new_block = true;
                oline[1][0] = oline[1].size() - 1;
                // write first line. second line will be written in !first_block
                // block
                fout.write(reinterpret_cast<char*>(oline[1].data()),
                           oline[1].size() * sizeof(float));
              } else {
                oline[1].push_back(ldata.data[1]);  // y coordinate
                oline[0].push_back(ldata.data[2]);  // function value
              }
            }
          }

          if (!first_block) {
            if (ldata.data[0] != oline[0][0]) {
              new_block = true;
            }

            if (new_block) {
              new_block = false;
              // write line to output
              fout.write(reinterpret_cast<char*>(oline[0].data()),
                         oline[0].size() * sizeof(float));
              // reset data index
              i = 0;
              oline[0][i] = ldata.data[0];  // x coorinates
              i++;
            }

            oline[0][i] = ldata.data[2];  // function value
            i++;
          }
        }
      } else {
        throw std::runtime_error("There was an error parsing the line '" +
                                 iline + "'.");
      }
    }
    // write last line
    fout.write(reinterpret_cast<char*>(oline[0].data()),
               oline[0].size() * sizeof(float));

    return 0;
  }

  return 1;
}

int ConvertGPBinary2ASCII3DDataFile(const std::string& ifilename,
                                    const std::string& ofilename)
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }

    // this method is simple, but it requires that
    // there be enough memory to store the entire
    // data set.
    GP3DData data;

    ReadGPBinary3DDataFile(ifilename, data);
    WriteGPASCII3DDataFile(ofilename, data);

    return 0;
}

int FilterGPBinary3DDataFile(const std::string& ifilename,
                             const std::string& ofilename,
                             const std::string& every_spec,
                             FilterMethod method)
{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }

  if( method == FilterMethod::ReadThenWrite)
  {
    GP3DData idata,odata;
    ReadGPBinary3DDataFile(ifilename,idata);
    FilterGP3DData(idata,odata,every_spec);
    WriteGPBinary3DDataFile(ofilename,odata);
  }

  if( method == FilterMethod::SimultaneousReadWrite)
  {
    std::ifstream fin;
    std::ofstream fout;


    fin.open(ifilename.c_str(), std::ios::in | std::ios::binary);
    fout.open(ofilename.c_str(), std::ios::out | std::ios::binary);
    BOOST_SCOPE_EXIT(&fin, &fout)
    {
      fin.close();
      fout.close();
    }
    BOOST_SCOPE_EXIT_END

    size_t N, Nx, Ny;
    float tmp;
    std::vector<float> buffer;

    // get size of file so we can compute how many elements it holds
    auto fs = boost::filesystem::file_size(ifilename);
    N = fs/sizeof(float);

    fin.read(reinterpret_cast<char*>(&tmp),sizeof(float));
    Ny = tmp;
    Nx = (N - 1 - Ny)/(1 + Ny);


    GP3DDataEverySpec slice(every_spec);

    size_t ys,xs, ye, xe, newNx, newNy;
    ys = slice.y_start.get_value_or(0);
    xs = slice.x_start.get_value_or(0);
    ye = slice.y_end.get_value_or(Ny-1);
    xe = slice.x_end.get_value_or(Nx-1);

    if(ys >= Ny)
      ys = Ny-1;
    if(ye >= Ny)
      ye = Ny-1;

    if(xs >= Nx)
      xs = Nx-1;
    if(xe >= Nx)
      xe = Nx-1;

    if( slice.y_inc || slice.x_inc )
      throw std::runtime_error("FilterGPBinary3DDataFile does not support y or x increments yet. Spec string:"+every_spec);

    if( ye < ys )
      throw std::runtime_error("FilterGPBinary3DDataFile: y (point) end cannot be less than start. Spec string: "+every_spec);

    if( xe < xs )
      throw std::runtime_error("FilterGPBinary3DDataFile: x (block) end cannot be less than start. Spec string: "+every_spec);


    // ys is *inclusive*
    newNy = ye - ys + 1;
    newNx = xe - xs + 1;

    buffer.resize(1+newNy);

    // first line with y coordinates
    buffer[0] = newNy;
    fin.ignore( ys*sizeof(float) );  // skip to first y coordinate in slice
    fin.read(reinterpret_cast<char*>(buffer.data()+1), newNy*sizeof(float) ); // read new y coordinates
    fin.ignore((Ny - ye - 1)*sizeof(float));  // skip to next line
    fout.write( reinterpret_cast<char*>(buffer.data()), buffer.size()*sizeof(float) );

    
    fin.ignore( xs*(Ny+1)*sizeof(float) );  // skip to first x coordinate in slice
    for(int i = 0; i < newNx; ++i)
    {
      fin.read(reinterpret_cast<char*>(buffer.data()), sizeof(float) ); // read x coordinate
      fin.ignore( ys*sizeof(float) );  // skip to first y coordinate in slice
      fin.read(reinterpret_cast<char*>(buffer.data()+1), newNy*sizeof(float) ); // read new function values
      fin.ignore((Ny - ye - 1)*sizeof(float));  // skip to next line
      fout.write( reinterpret_cast<char*>(buffer.data()), buffer.size()*sizeof(float) );
    }



  return 0;
  }


  return 1;
}

int ConvertGPBinary3DDataFileCylindrical2Cartesian(const std::string& ifilename,
                                                   const std::string& ofilename)

{
  if (!boost::filesystem::exists(ifilename)) {
    throw std::runtime_error("No such file: " + ifilename);
  }

  GP3DData idata;
  std::vector<GP3DData> odata_slices;

  ReadGPBinary3DDataFile(ifilename,idata);

  ConvertGP3DDataCylindrical2Cartesian(idata,odata_slices);

  if( odata_slices.size() == 1 )
  {
    WriteGPBinary3DDataFile(ofilename,odata_slices[0]);
  }
  else
  {
    for(int i = 0; i < odata_slices.size(); i++)
    {
      WriteGPBinary3DDataFile(ofilename+"."+boost::lexical_cast<std::string>(i),odata_slices[i]);
    }
  }
  return 0;
}

