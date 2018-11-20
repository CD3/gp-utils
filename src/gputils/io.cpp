#include "./io.hpp"


int ReadGPASCII3DDataFile(const std::string& filename, GP3DData& data)
{
  std::ifstream fin;
  std::string line;
  GPASCIIDataLineParser<std::string::iterator> lparser;
  bool r;
  std::string::iterator it;

  data.x.clear();
  data.y.clear();
  data.f.clear();

  fin.open(filename.c_str());
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


int WriteGPBinary3DDataFile(const std::string& filename, const GP3DData& data)
{
  std::ofstream fout;

  fout.open(filename.c_str(), std::ios::out | std::ios::binary);
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

int WriteGPASCII3DDataFile(const std::string& filename, const GP3DData& data)
{
  std::ofstream fout;

  fout.open(filename.c_str(), std::ios::out);
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
