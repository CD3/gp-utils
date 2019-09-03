#include "Benchmark.hpp"
#include "catch.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/filesystem.hpp>

#include "gputils/io.hpp"
#include "gputils/transformations.hpp"

using namespace std;

/**
 * This file is used for developement. As new classes are created, small tests
 * are written here so that we can try to compile and use them.
 */


void MakeLarge3DData(int Nx, int Ny, GP3DData& data)
{
  data.clear();
  for( int i = 0; i < Nx; ++i)
  {
    data.x.push_back(i);
  for( int j = 0; j < Ny; ++j)
  {
    if( i == 0)
      data.y.push_back(j);

    data.f.push_back(i+j);
  }
  }

}

TEST_CASE("General", "[devel]") {}

TEST_CASE("File Input/Output", "[parsing]")
{
  SECTION("Spirit Parsers")
  {
    SECTION("Gnuplot ASCII Data Line Parser")
    {
      std::string line;
      std::string::iterator it;
      bool r;
      GPDataLine dl;

      SECTION("Space Separated")
      {
        GPASCIIDataLineParser<std::string::iterator> lparser;

        dl.clear();
        line = "1.1 2.2";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = "1.1  2.2";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = " 1.1  2.2";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = "1.1  2.2 ";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = "1.1\t2.2 ";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = "1.1 2.2#This is a comment";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 17);

        dl.clear();
        line = "1.1 2.2 #This is a comment";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 17);

        dl.clear();
        line = "1.1 2.2 # This is a comment";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 2);
        CHECK(dl.comment.size() == 18);

        dl.clear();
        line = "# This is a comment";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 0);
        CHECK(dl.comment.size() == 18);

        dl.clear();
        line = "";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 0);
        CHECK(dl.comment.size() == 0);

        dl.clear();
        line = " ";
        it = line.begin();
        r = qi::parse(it, line.end(), lparser, dl);
        CHECK(r);
        CHECK(it == line.end());
        CHECK(dl.data.size() == 0);
        CHECK(dl.comment.size() == 0);
      }

      SECTION("Non-space delimiters")
      {
        SECTION("Comma")
        {
          GPASCIIDataLineParser<std::string::iterator> lparser(qi::lit(","));

          dl.clear();
          line = "1.1,2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);
        }

        SECTION("Comma with optional spaces")
        {
          GPASCIIDataLineParser<std::string::iterator> lparser(
              *qi::blank >> qi::lit(",") >> *qi::blank);

          dl.clear();
          line = "1.1,2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);

          dl.clear();
          line = "1.1 ,2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);

          dl.clear();
          line = "1.1, 2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);

          dl.clear();
          line = "1.1 , 2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);

          dl.clear();
          line = " 1.1 , 2.2 ";
          it = line.begin();
          r = qi::parse(it, line.end(), lparser, dl);
          CHECK(r);
          CHECK(it == line.end());
          CHECK(dl.data.size() == 2);
          CHECK(dl.comment.size() == 0);
        }
      }
    }
  }

  SECTION("File Read Functions")
  {
    ofstream out("test-3d.txt");

    SECTION("ASCII - Single space delimited")
    {
      out << 1 << " " << 10 << " " << 110 << "\n";
      out << 1 << " " << 20 << " " << 110 << "\n";
      out << 1 << " " << 30 << " " << 110 << "\n";

      out << 2 << " " << 10 << " " << 210 << "\n";
      out << 2 << " " << 20 << " " << 210 << "\n";
      out << 2 << " " << 30 << " " << 210 << "\n";

      out.close();

      GP3DData data;
      ReadGPASCII3DDataFile("test-3d.txt", data);

      CHECK(data.x.size() == 2);
      CHECK(data.y.size() == 3);
      CHECK(data.f.size() == 6);
    }

    SECTION("ASCII - Tab delimited")
    {
      out << 1 << "\t" << 10 << "\t" << 110 << "\n";
      out << 1 << "\t" << 20 << "\t" << 110 << "\n";
      out << 1 << "\t" << 30 << "\t" << 110 << "\n";

      out << 2 << "\t" << 10 << "\t" << 210 << "\n";
      out << 2 << "\t" << 20 << "\t" << 210 << "\n";
      out << 2 << "\t" << 30 << "\t" << 210 << "\n";

      out.close();

      GP3DData data;
      ReadGPASCII3DDataFile("test-3d.txt", data);

      CHECK(data.x.size() == 2);
      CHECK(data.y.size() == 3);
      CHECK(data.f.size() == 6);
    }

    SECTION("Binary Matrix")
    {
      GP3DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);

      data.y.push_back(2.1);
      data.y.push_back(2.2);
      data.y.push_back(2.3);

      data.f.push_back(1.1 + 2.1);
      data.f.push_back(1.1 + 2.2);
      data.f.push_back(1.1 + 2.3);
      data.f.push_back(1.2 + 2.1);
      data.f.push_back(1.2 + 2.2);
      data.f.push_back(1.2 + 2.3);

      WriteGPBinary3DDataFile("test-3d.bin", data);

      data.clear();

      ReadGPBinary3DDataFile("test-3d.bin", data);

      CHECK(data.x.size() == 2);
      CHECK(data.y.size() == 3);
      CHECK(data.f.size() == 6);

      CHECK(data.x[0] == Approx(1.1));
      CHECK(data.x[1] == Approx(1.2));

      CHECK(data.y[0] == Approx(2.1));
      CHECK(data.y[1] == Approx(2.2));
      CHECK(data.y[2] == Approx(2.3));

      CHECK(data.f[0] == Approx(1.1+2.1));
      CHECK(data.f[1] == Approx(1.1+2.2));
      CHECK(data.f[2] == Approx(1.1+2.3));
      CHECK(data.f[3] == Approx(1.2+2.1));
      CHECK(data.f[4] == Approx(1.2+2.2));
      CHECK(data.f[5] == Approx(1.2+2.3));


    }

    SECTION("HDF5 Field")
    {
      GP3DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);

      data.y.push_back(2.1);
      data.y.push_back(2.2);
      data.y.push_back(2.3);

      data.f.push_back(1.1 + 2.1);
      data.f.push_back(1.1 + 2.2);
      data.f.push_back(1.1 + 2.3);
      data.f.push_back(1.2 + 2.1);
      data.f.push_back(1.2 + 2.2);
      data.f.push_back(1.2 + 2.3);

      WriteHDF5Field3DDataFile("test-3d.h5", data);

      data.clear();

      ReadHDF5Field3DDataFile("test-3d.h5", data);

      CHECK(data.x.size() == 2);
      CHECK(data.y.size() == 3);
      CHECK(data.f.size() == 6);

      CHECK(data.x[0] == Approx(1.1));
      CHECK(data.x[1] == Approx(1.2));

      CHECK(data.y[0] == Approx(2.1));
      CHECK(data.y[1] == Approx(2.2));
      CHECK(data.y[2] == Approx(2.3));

      CHECK(data.f[0] == Approx(1.1+2.1));
      CHECK(data.f[1] == Approx(1.1+2.2));
      CHECK(data.f[2] == Approx(1.1+2.3));
      CHECK(data.f[3] == Approx(1.2+2.1));
      CHECK(data.f[4] == Approx(1.2+2.2));
      CHECK(data.f[5] == Approx(1.2+2.3));


    }

    SECTION("ASCII - Comments and Blank Lines")
    {
      out << "\n";
      out << 1 << " " << 10 << " " << 110 << "\n";
      out << 1 << " " << 20 << " " << 110 << "\n";
      out << 1 << " " << 30 << " " << 110 << "\n";

      out << "\n";
      out << "# comment\n";
      out << 2 << " " << 10 << " " << 210 << " # comment\n";
      out << 2 << " " << 20 << " " << 210 << "\n";
      out << 2 << " " << 30 << " " << 210 << "\n";

      out.close();

      GP3DData data;
      ReadGPASCII3DDataFile("test-3d.txt", data);

      REQUIRE(data.x.size() == 2);
      REQUIRE(data.y.size() == 3);
      REQUIRE(data.f.size() == 6);

      CHECK(data.x[0] == Approx(1) );
      CHECK(data.x[1] == Approx(2) );

      CHECK(data.y[0] == Approx(10) );
      CHECK(data.y[1] == Approx(20) );
      CHECK(data.y[2] == Approx(30) );
    }

    SECTION("ASCII - Header")
    {
      out << "\n";
      out << "# Comment\n";
      out << "\n";
      out << "# Comment\n";
      out << "\n";
      out << "# Comment\n";
      out << "# Comment\n";
      out << 1 << " " << 10 << " " << 110 << "\n";
      out << 1 << " " << 20 << " " << 110 << "\n";
      out << 1 << " " << 30 << " " << 110 << "\n";

      out << "\n";
      out << "# Comment\n";
      out << "# Comment\n";
      out << 2 << " " << 10 << " " << 210 << " # comment\n";
      out << 2 << " " << 20 << " " << 210 << "\n";
      out << 2 << " " << 30 << " " << 210 << "\n";

      out.close();

      GP3DData data;
      ReadGPASCII3DDataFile("test-3d.txt", data);

      REQUIRE(data.x.size() == 2);
      REQUIRE(data.y.size() == 3);
      REQUIRE(data.f.size() == 6);

      CHECK(data.x[0] == Approx(1) );
      CHECK(data.x[1] == Approx(2) );

      CHECK(data.y[0] == Approx(10) );
      CHECK(data.y[1] == Approx(20) );
      CHECK(data.y[2] == Approx(30) );
    }

    SECTION("Binary Datafile Info")
    {
      GP3DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);
      data.x.push_back(1.3);

      data.y.push_back(2.1);
      data.y.push_back(2.2);
      data.y.push_back(2.3);
      data.y.push_back(2.4);

      data.f.push_back(10);
      data.f.push_back(11);
      data.f.push_back(12);
      data.f.push_back(13);

      data.f.push_back(20);
      data.f.push_back(21);
      data.f.push_back(22);
      data.f.push_back(23);

      data.f.push_back(30);
      data.f.push_back(31);
      data.f.push_back(32);
      data.f.push_back(33);


      WriteGPBinary3DDataFile("test-3d.bin", data);

      GP3DDataInfo info;

      QueryGPBinary3DDataFile("test-3d.bin", info);

      CHECK( info.Nx ); CHECK(info.Nx.get() == 3);
      CHECK( info.Ny ); CHECK(info.Ny.get() == 4);
      CHECK( info.xmin); CHECK(info.xmin.get() == Approx(1.1) );
      CHECK( info.xmax); CHECK(info.xmax.get() == Approx(1.3) );
      CHECK( info.ymin); CHECK(info.ymin.get() == Approx(2.1) );
      CHECK( info.ymax); CHECK(info.ymax.get() == Approx(2.4) );

    }

    SECTION("ASCII 2D Datafile")
    {
      GP2DData odata;

      odata.x.push_back(1.1);
      odata.x.push_back(1.2);
      odata.x.push_back(1.3);
      odata.f.push_back(1.1);
      odata.f.push_back(2.1);
      odata.f.push_back(3.1);

      WriteGPASCII2DDataFile("test-2d.txt", odata);

      GP2DData data;
      ReadGPASCII2DDataFile("test-2d.txt", data);

      CHECK( odata.x.size() == 3);
      CHECK( odata.x[0]  == Approx(1.1));
      CHECK( odata.x[1]  == Approx(1.2));
      CHECK( odata.x[2]  == Approx(1.3));
      CHECK( odata.f[0]  == Approx(1.1));
      CHECK( odata.f[1]  == Approx(2.1));
      CHECK( odata.f[2]  == Approx(3.1));


    }

    SECTION("HDF52D Datafile")
    {
      GP2DData odata;

      odata.x.push_back(1.1);
      odata.x.push_back(1.2);
      odata.x.push_back(1.3);
      odata.f.push_back(1.1);
      odata.f.push_back(2.1);
      odata.f.push_back(3.1);

      WriteHDF5Field2DDataFile("test-2d.h5", odata);

      GP2DData data;
      ReadHDF5Field2DDataFile("test-2d.h5", data);

      CHECK( odata.x.size() == 3);
      CHECK( odata.x[0]  == Approx(1.1));
      CHECK( odata.x[1]  == Approx(1.2));
      CHECK( odata.x[2]  == Approx(1.3));
      CHECK( odata.f[0]  == Approx(1.1));
      CHECK( odata.f[1]  == Approx(2.1));
      CHECK( odata.f[2]  == Approx(3.1));


    }


  }

  SECTION("File Write Functions")
  {
    SECTION("ASCII 2D")
    {
      GP2DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);
      data.x.push_back(1.3);
      data.f.push_back(1.1);
      data.f.push_back(2.1);
      data.f.push_back(3.1);

      WriteGPASCII2DDataFile("test-2d.txt", data);

      ifstream fin;
      fin.open("test-2d.txt", ios::in);
      float buffer;

      fin >> buffer;
      CHECK(buffer == Approx(1.1));
      fin >> buffer;
      CHECK(buffer == Approx(1.1));

      fin >> buffer;
      CHECK(buffer == Approx(1.2));
      fin >> buffer;
      CHECK(buffer == Approx(2.1));

      fin >> buffer;
      CHECK(buffer == Approx(1.3));
      fin >> buffer;
      CHECK(buffer == Approx(3.1));


      CHECK(!(fin >> buffer));
      CHECK(fin.eof());

      fin.close();

    }
    SECTION("ASCII 3D")
    {
      GP3DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);

      data.y.push_back(2.1);
      data.y.push_back(2.2);
      data.y.push_back(2.3);

      data.f.push_back(1.1 + 2.1);
      data.f.push_back(1.1 + 2.2);
      data.f.push_back(1.1 + 2.3);
      data.f.push_back(1.2 + 2.1);
      data.f.push_back(1.2 + 2.2);
      data.f.push_back(1.2 + 2.3);

      WriteGPASCII3DDataFile("test-3d.txt", data);

      ifstream fin;
      fin.open("test-3d.txt", ios::in);
      float buffer;

      fin >> buffer;
      CHECK(buffer == Approx(1.1));
      fin >> buffer;
      CHECK(buffer == Approx(2.1));
      fin >> buffer;
      CHECK(buffer == Approx(1.1 + 2.1));

      fin >> buffer;
      CHECK(buffer == Approx(1.1));
      fin >> buffer;
      CHECK(buffer == Approx(2.2));
      fin >> buffer;
      CHECK(buffer == Approx(1.1 + 2.2));

      fin >> buffer;
      CHECK(buffer == Approx(1.1));
      fin >> buffer;
      CHECK(buffer == Approx(2.3));
      fin >> buffer;
      CHECK(buffer == Approx(1.1 + 2.3));

      fin >> buffer;
      CHECK(buffer == Approx(1.2));
      fin >> buffer;
      CHECK(buffer == Approx(2.1));
      fin >> buffer;
      CHECK(buffer == Approx(1.2 + 2.1));

      fin >> buffer;
      CHECK(buffer == Approx(1.2));
      fin >> buffer;
      CHECK(buffer == Approx(2.2));
      fin >> buffer;
      CHECK(buffer == Approx(1.2 + 2.2));

      fin >> buffer;
      CHECK(buffer == Approx(1.2));
      fin >> buffer;
      CHECK(buffer == Approx(2.3));
      fin >> buffer;
      CHECK(buffer == Approx(1.2 + 2.3));

      CHECK(!(fin >> buffer));
      CHECK(fin.eof());

      fin.close();

    }
    SECTION("Binary Matrix")
    {
      GP3DData data;

      data.x.push_back(1.1);
      data.x.push_back(1.2);

      data.y.push_back(2.1);
      data.y.push_back(2.2);
      data.y.push_back(2.3);

      data.f.push_back(1.1 + 2.1);
      data.f.push_back(1.1 + 2.2);
      data.f.push_back(1.1 + 2.3);
      data.f.push_back(1.2 + 2.1);
      data.f.push_back(1.2 + 2.2);
      data.f.push_back(1.2 + 2.3);

      WriteGPBinary3DDataFile("test-3d.bin", data);

      ifstream fin;
      fin.open("test-3d.bin", ios::in | ios::binary);
      float buffer[10];

      fin.read(reinterpret_cast<char*>(buffer), sizeof(float));
      CHECK(buffer[0] == Approx(3));

      fin.read(reinterpret_cast<char*>(buffer), 3 * sizeof(float));
      CHECK(buffer[0] == Approx(2.1));
      CHECK(buffer[1] == Approx(2.2));
      CHECK(buffer[2] == Approx(2.3));

      fin.read(reinterpret_cast<char*>(buffer), 4 * sizeof(float));
      CHECK(buffer[0] == Approx(1.1));
      CHECK(buffer[1] == Approx(1.1 + 2.1));
      CHECK(buffer[2] == Approx(1.1 + 2.2));
      CHECK(buffer[3] == Approx(1.1 + 2.3));

      fin.read(reinterpret_cast<char*>(buffer), 4 * sizeof(float));
      CHECK(buffer[0] == Approx(1.2));
      CHECK(buffer[1] == Approx(1.2 + 2.1));
      CHECK(buffer[2] == Approx(1.2 + 2.2));
      CHECK(buffer[3] == Approx(1.2 + 2.3));

      // we should be at the end of the file, so one more read
      // should fail and turn on the eof bit.
      CHECK(!fin.read(reinterpret_cast<char*>(buffer), sizeof(float)));
      CHECK(fin.eof());

      fin.close();
    }
  }

  SECTION("File Convert Functions")
  {
    SECTION("3D ASCII to Binary Matrix")
    {
      ofstream out("test-3d-convert.txt");

      out << 1.1 << " " << 2.10 << " " << 1.1 + 2.1 << "\n";
      out << 1.1 << " " << 2.20 << " " << 1.1 + 2.2 << "\n";
      out << 1.1 << " " << 2.30 << " " << 1.1 + 2.3 << "\n";
      out << "\n";
      out << 1.2 << " " << 2.10 << " " << 1.2 + 2.1 << "\n";
      out << 1.2 << " " << 2.20 << " " << 1.2 + 2.2 << "\n";
      out << 1.2 << " " << 2.30 << " " << 1.2 + 2.3 << "\n";

      out.close();

      CHECK_THROWS(ConvertGPASCII2Binary3DDataFile("missing.txt",
                                                   "test-3d-convert.bin"));
      ConvertGPASCII2Binary3DDataFile("test-3d-convert.txt",
                                      "test-3d-convert.bin");

      ifstream fin;
      fin.open("test-3d-convert.bin", ios::in | ios::binary);
      float buffer[10];

      fin.read(reinterpret_cast<char*>(buffer), sizeof(float));
      CHECK(buffer[0] == Approx(3));

      fin.read(reinterpret_cast<char*>(buffer), 3 * sizeof(float));
      CHECK(buffer[0] == Approx(2.1));
      CHECK(buffer[1] == Approx(2.2));
      CHECK(buffer[2] == Approx(2.3));

      fin.read(reinterpret_cast<char*>(buffer), 4 * sizeof(float));
      CHECK(buffer[0] == Approx(1.1));
      CHECK(buffer[1] == Approx(1.1 + 2.1));
      CHECK(buffer[2] == Approx(1.1 + 2.2));
      CHECK(buffer[3] == Approx(1.1 + 2.3));

      fin.read(reinterpret_cast<char*>(buffer), 4 * sizeof(float));
      CHECK(buffer[0] == Approx(1.2));
      CHECK(buffer[1] == Approx(1.2 + 2.1));
      CHECK(buffer[2] == Approx(1.2 + 2.2));
      CHECK(buffer[3] == Approx(1.2 + 2.3));

      // we should be at the end of the file, so one more read
      // should fail and turn on the eof bit.
      CHECK(!fin.read(reinterpret_cast<char*>(buffer), sizeof(float)));
      CHECK(fin.eof());

      fin.close();

    }

    SECTION("3D Binary to ASCII Matrix")
    {
      float tmp;

      ofstream out("test-3d-convert.bin", ios::out | ios::binary);

      tmp = 3;   out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // number of y coordinates
      tmp = 1.1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 1st y coord
      tmp = 1.2; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 2nd y coord
      tmp = 1.3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 3rd y coord
      tmp = 2.1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 1st x coord
      tmp = 11.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f11
      tmp = 12.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f12
      tmp = 13.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f13
      tmp = 2.2; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 2nd x coord
      tmp = 21.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f21
      tmp = 22.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f22
      tmp = 23.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f23
      tmp = 2.3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 3rd x coord
      tmp = 31.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f31
      tmp = 32.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f32
      tmp = 33.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f33
      tmp = 2.4; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // 4th x coord
      tmp = 41.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f41
      tmp = 42.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f42
      tmp = 43.; out.write(reinterpret_cast<char*>(&tmp), sizeof(float)); // f43
      

      out.close();

      CHECK_THROWS(ConvertGPBinary2ASCII3DDataFile("missing.txt",
                                                   "test-3d-convert.bin"));
      ConvertGPBinary2ASCII3DDataFile("test-3d-convert.bin",
                                      "test-3d-convert.txt");

      ifstream fin;
      fin.open("test-3d-convert.txt", ios::in | ios::binary);

      fin >> tmp; CHECK( tmp == Approx(2.1) ) ;  // 1st x coord
      fin >> tmp; CHECK( tmp == Approx(1.1) ) ;  // 1st y coord
      fin >> tmp; CHECK( tmp == Approx(11) ) ;   // f11

      fin >> tmp; CHECK( tmp == Approx(2.1) ) ;  // 1st x coord
      fin >> tmp; CHECK( tmp == Approx(1.2) ) ;  // 2nd y coord
      fin >> tmp; CHECK( tmp == Approx(12) ) ;   // f12

      fin >> tmp; CHECK( tmp == Approx(2.1) ) ;  // 1st x coord
      fin >> tmp; CHECK( tmp == Approx(1.3) ) ;  // 3rd y coord
      fin >> tmp; CHECK( tmp == Approx(13) ) ;   // f13


      fin >> tmp; CHECK( tmp == Approx(2.2) ) ;  // 2nd x coord
      fin >> tmp; CHECK( tmp == Approx(1.1) ) ;  // 1st y coord
      fin >> tmp; CHECK( tmp == Approx(21) ) ;   // f21

      fin >> tmp; CHECK( tmp == Approx(2.2) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.2) ) ;
      fin >> tmp; CHECK( tmp == Approx(22) ) ;

      fin >> tmp; CHECK( tmp == Approx(2.2) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.3) ) ;
      fin >> tmp; CHECK( tmp == Approx(23) ) ;


      fin >> tmp; CHECK( tmp == Approx(2.3) ) ;  // 3rd x coord
      fin >> tmp; CHECK( tmp == Approx(1.1) ) ;  // 1st y coord
      fin >> tmp; CHECK( tmp == Approx(31) ) ;   // f31

      fin >> tmp; CHECK( tmp == Approx(2.3) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.2) ) ;
      fin >> tmp; CHECK( tmp == Approx(32) ) ;

      fin >> tmp; CHECK( tmp == Approx(2.3) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.3) ) ;
      fin >> tmp; CHECK( tmp == Approx(33) ) ;


      fin >> tmp; CHECK( tmp == Approx(2.4) ) ;  // 4th x coord
      fin >> tmp; CHECK( tmp == Approx(1.1) ) ;  // 1st y coord
      fin >> tmp; CHECK( tmp == Approx(41) ) ;   // f41

      fin >> tmp; CHECK( tmp == Approx(2.4) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.2) ) ;
      fin >> tmp; CHECK( tmp == Approx(42) ) ;

      fin >> tmp; CHECK( tmp == Approx(2.4) ) ;
      fin >> tmp; CHECK( tmp == Approx(1.3) ) ;
      fin >> tmp; CHECK( tmp == Approx(43) ) ;

      CHECK( !(fin>>tmp) );
      CHECK( fin.eof() );

      fin.close();

    }

    SECTION("Binary File Corruption")
    {
      float tmp;
      ofstream out("test-corrupt.bin", ios::out | ios::binary);

      SECTION("Wrong Format")
      {
      tmp = -3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -2; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));

      tmp =  1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 10; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 11; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 12; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));

      out.close();

      GP3DData data;
      CHECK_THROWS(ReadGPBinary3DDataFile( "test-corrupt.bin", data ));
      }

      SECTION("Corrupt")
      {
      tmp = 3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -3; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -2; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = -1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));

      tmp =  1; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 10; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 11; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 12; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));

      tmp =  2; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 20; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));
      tmp = 21; out.write(reinterpret_cast<char*>(&tmp), sizeof(float));

      out.close();

      GP3DData data;
      CHECK_THROWS(ReadGPBinary3DDataFile( "test-corrupt.bin", data ));
      }





    }
  }
}

TEST_CASE("File Transformations")
{
  SECTION("Every Spec Parser")
  {
    // clang-format off
    GP3DDataEverySpec spec("1:2:3:4:5:6");
    CHECK( spec.y_inc  );   CHECK( spec.y_inc.get()   == 1 );
    CHECK( spec.x_inc  );   CHECK( spec.x_inc.get()   == 2 );
    CHECK( spec.y_start  ); CHECK( spec.y_start.get() == 3 );
    CHECK( spec.x_start  ); CHECK( spec.x_start.get() == 4 );
    CHECK( spec.y_end  );   CHECK( spec.y_end.get()   == 5 );
    CHECK( spec.x_end  );   CHECK( spec.x_end.get()   == 6 );


    spec = GP3DDataEverySpec(":1:2:3:4:5");
    CHECK(!spec.y_inc  );
    CHECK( spec.x_inc  );   CHECK( spec.x_inc.get()   == 1 );
    CHECK( spec.y_start  ); CHECK( spec.y_start.get() == 2 );
    CHECK( spec.x_start  ); CHECK( spec.x_start.get() == 3 );
    CHECK( spec.y_end  );   CHECK( spec.y_end.get()   == 4 );
    CHECK( spec.x_end  );   CHECK( spec.x_end.get()   == 5 );

    spec = GP3DDataEverySpec("::1:2:3:4");
    CHECK(!spec.y_inc  );
    CHECK(!spec.x_inc  ); 
    CHECK( spec.y_start  ); CHECK( spec.y_start.get() == 1 );
    CHECK( spec.x_start  ); CHECK( spec.x_start.get() == 2 );
    CHECK( spec.y_end  );   CHECK( spec.y_end.get()   == 3 );
    CHECK( spec.x_end  );   CHECK( spec.x_end.get()   == 4 );

    spec = GP3DDataEverySpec("::1::2");
    CHECK(!spec.y_inc  );
    CHECK(!spec.x_inc  ); 
    CHECK( spec.y_start  ); CHECK( spec.y_start.get() == 1 );
    CHECK(!spec.x_start  );
    CHECK( spec.y_end  );   CHECK( spec.y_end.get()   == 2 );
    CHECK(!spec.x_end  );

    spec = GP3DDataEverySpec("::1::2:");
    CHECK(!spec.y_inc  );
    CHECK(!spec.x_inc  ); 
    CHECK( spec.y_start  ); CHECK( spec.y_start.get() == 1 );
    CHECK(!spec.x_start  );
    CHECK( spec.y_end  );   CHECK( spec.y_end.get()   == 2 );
    CHECK(!spec.x_end  );

    spec = GP3DDataEverySpec(":::1::2");
    CHECK(!spec.y_inc  );
    CHECK(!spec.x_inc  );
    CHECK(!spec.y_start  );
    CHECK( spec.x_start  ); CHECK( spec.x_start.get() == 1 );
    CHECK(!spec.y_end  );
    CHECK( spec.x_end  );   CHECK( spec.x_end.get()   == 2 );

    spec = GP3DDataEverySpec("1:2::::");
    CHECK( spec.y_inc  );   CHECK( spec.y_inc.get()   == 1 );
    CHECK( spec.x_inc  );   CHECK( spec.x_inc.get()   == 2 );
    CHECK(!spec.y_start  );
    CHECK(!spec.x_start  );
    CHECK(!spec.y_end  );
    CHECK(!spec.x_end  );
    // clang-format on
  }

  SECTION("Block Extraction")
  {
    GP3DData idata,odata;
    idata.x.resize(10);
    idata.y.resize(20);
    idata.f.resize(10*20);

    double dx = 0.1;
    double xmin = 0.0;
    double dy = 0.1;
    double ymin = 10;
    for( int i = 0; i < idata.x.size(); ++i )
      idata.x[i] = xmin+dx*i;
    for( int j = 0; j < idata.y.size(); ++j )
      idata.y[j] = ymin+dy*j;
    for( int i = 0; i < idata.x.size(); ++i )
    {
      for( int j = 0; j < idata.y.size(); ++j )
        idata.f[i*idata.y.size() + j] = sin(idata.x[i])*cos(idata.y[j]);
    }



    if( boost::filesystem::exists("test-small-3d.bin"))
    {
      boost::filesystem::remove("test-small-3d.bin");
    }
    WriteGPBinary3DDataFile("test-large-3d.bin", idata);
    FilterGPBinary3DDataFile("test-large-3d.bin", "test-small-3d.bin", "::4:2:12:5");
    ReadGPBinary3DDataFile("test-small-3d.bin", odata);

    for(int j = 4; j <= 12; ++j)
    {
      CHECK(odata.y[j-4] == Approx(idata.y[j]));
    }

    for(int i = 2; i <= 5; ++i)
    {
      CHECK(odata.x[i-2] == Approx(idata.x[i]));
    }
    
    for(int i = 2; i <= 5; ++i)
    {
    for(int j = 4; j <= 12; ++j)
      CHECK(odata.f[(i-2)*odata.y.size() + (j-4)] == Approx(idata.f[i*idata.y.size() + j]));
    }



  }

  SECTION("Polar -> Cartesian")
  {
    GP3DData data;
    double dx = 0.1;
    double dy = 0.2;

    for(int i = 0; i < 3; ++i)
    {
      data.x.push_back(dx*i);
      for(int j = 0; j < 10; ++j)
      {
        if( i == 0 )
          data.y.push_back(dy*j);

        data.f.push_back( exp(-dy*j) );
      }
    }

    WriteGPBinary3DDataFile( "test-3d-cyl.bin", data);
    ConvertGPBinary3DDataFileCylindrical2Cartesian("test-3d-cyl.bin", "test-3d-car.bin");
    ReadGPBinary3DDataFile( "test-3d-car.bin.0", data);



    CHECK(data.x.size() == 19);
    CHECK(data.y.size() == 19);
    CHECK(data.f.size() == 19*19);

    // (0,0) should be at index 9,9
    CHECK(data.x[0] == Approx(-dy*9));
    CHECK(data.x[9] == Approx(0));
    CHECK(data.x[18] == Approx(dy*9));

    CHECK(data.y[0] == Approx(-dy*9));
    CHECK(data.y[9] == Approx(0));
    CHECK(data.y[18] == Approx(dy*9));

    // along x = 0 and y = 0 lines, the function should be the same as z = 0
    // line of the input.
    for(int i = 0; i < 10; ++i)
    {
      CHECK(data.f[9*19 + 9+i] == Approx(exp(-i*dy)));
      CHECK(data.f[9*19 + 9-i] == Approx(exp(-i*dy)));

      CHECK(data.f[(9+i)*19 + 9] == Approx(exp(-i*dy)));
      CHECK(data.f[(9-i)*19 + 9] == Approx(exp(-i*dy)));
    }

    // at all other points, the function is interpolated, but should be close...
    for(int i = 0; i < 19; ++i)
    {
    for(int j = 0; j < 19; ++j)
    {
      double x = dy*(i-9);
      double y = dy*(j-9);
      double r = sqrt(x*x+y*y);
      if( r > dy*9) // interpolator will not extrapolate
      {
        CHECK(data.f[i*19 + j] == Approx(0));
      }
      else
      {
        CHECK(data.f[i*19 + j] == Approx(exp(-r)).epsilon(0.01));
      }
    }
    }

  }
}

TEST_CASE("Performance Benchmarks", "[!hide][benchmarks]")
{
      SECTION("3D ASCII vs Binary Writer")
      {

        std::string name = "3D-ascii-vs-binary-writer";
        std::string fn = BM::PerformanceBenchmark::getStorageFilename(name);
        if( boost::filesystem::exists(fn) )
        {
          boost::filesystem::remove(fn);
        }
        BM::PerformanceBenchmark bm(name);
        BM::Benchmark meter;

        GP3DData data;
        MakeLarge3DData(100,200,data);

        meter.run([&]() { WriteGPASCII3DDataFile("test-3d.txt", data); });
        bm(meter);
        std::cout << "ASCII:" << meter.get_measurement() << "\n";

        meter.run([&]() { WriteGPBinary3DDataFile("test-3d.bin", data); });
        auto result = bm(meter);
        std::cout << "Binary:" << meter.get_measurement() << "\n";

        std::cout << name
                  << "\n";
        std::cout << "faster than baseline? " << result.is_faster_than_baseline
                  << "\n";
        std::cout << "faster than minimum? " << result.is_faster_than_minimum
                  << "\n";
        std::cout << "speedup over baseline: "
                  << result.speedup_over_baseline << "\n";
        std::cout << "speedup over minimum: "
                  << result.speedup_over_minimum << "\n";
        std::cout << std::endl;

      SECTION("3D ASCII vs Binary Reader")
      {

        std::string name = "3D-ascii-vs-binary-reader";
        std::string fn = BM::PerformanceBenchmark::getStorageFilename(name);
        if( boost::filesystem::exists(fn) )
        {
          boost::filesystem::remove(fn);
        }
        BM::PerformanceBenchmark bm(name);
        BM::Benchmark meter;

        GP3DData data;
        MakeLarge3DData(100,200,data);

        meter.run([&]() { ReadGPASCII3DDataFile("test-3d.txt", data); });
        bm(meter);
        std::cout << "ASCII:" << meter.get_measurement() << "\n";

        meter.run([&]() { ReadGPBinary3DDataFile("test-3d.bin", data); });
        auto result = bm(meter);
        std::cout << "Binary:" << meter.get_measurement() << "\n";

        std::cout << name
                  << "\n";
        std::cout << "faster than baseline? " << result.is_faster_than_baseline
                  << "\n";
        std::cout << "faster than minimum? " << result.is_faster_than_minimum
                  << "\n";
        std::cout << "speedup over baseline: "
                  << result.speedup_over_baseline << "\n";
        std::cout << "speedup over minimum: "
                  << result.speedup_over_minimum << "\n";
        std::cout << std::endl;
      }

      }

      SECTION("3D ASCII -> Binary Conversion")
      {
        std::string fn = BM::PerformanceBenchmark::getStorageFilename("ascii2binary-conversion");
        if( boost::filesystem::exists(fn) )
        {
          boost::filesystem::remove(fn);
        }
        BM::PerformanceBenchmark bm("ascii2binary-conversion");
        BM::Benchmark meter;

        GP3DData data;
        MakeLarge3DData(500,500,data);
        WriteGPASCII3DDataFile("test-3d-convert-benchmark.txt",data);

        meter.run([]() {
          ConvertGPASCII2Binary3DDataFile("test-3d-convert-benchmark.txt",
                                          "test-3d-convert-benchmark.bin",
                                          ConvertMethod::ReadThenWrite);
        });
        bm(meter);

        meter.run([]() {
          ConvertGPASCII2Binary3DDataFile("test-3d-convert-benchmark.txt",
                                          "test-3d-convert-benchmark.bin",
                                          ConvertMethod::SimultaneousReadWrite);
        });
        auto result = bm(meter);
        std::cout << "ascii -> binary 3D conversion"
                  << "\n";
        std::cout << "Benchmark:\n";
        meter.print_results(std::cout);
        std::cout << "\n:";
        std::cout << "Measurement:" << meter.get_measurement() << "\n";
        std::cout << "faster than baseline? " << result.is_faster_than_baseline
                  << "\n";
        std::cout << "faster than minimum? " << result.is_faster_than_minimum
                  << "\n";
        std::cout << "speedup over baseline: "
                  << result.speedup_over_baseline << "\n";
        std::cout << "speedup over minimum: "
                  << result.speedup_over_minimum << "\n";
        std::cout << std::endl;
      }
}

TEST_CASE("Data Transformations")
{
  SECTION("Filter (Gnuplot Every)")
  {
    GP3DData idata, odata;
    double dx = 0.1;
    double dy = 0.2;

    for(int i = 0; i < 100; ++i)
    {
      idata.x.push_back(dx*i);
      for(int j = 0; j < 50; ++j)
      {
        if( i == 0 )
          idata.y.push_back(dy*j);

        idata.f.push_back( sin(i*dx)*sin(j*dy) );
      }
    }

    FilterGP3DData( idata, odata, "::40:20:45:30" );

    CHECK(odata.y.size() == 6);
    CHECK(odata.x.size() == 11);
    CHECK(odata.f.size() == 66);

    CHECK(odata.y[0] == Approx(dy*40));
    CHECK(odata.y[1] == Approx(dy*41));
    CHECK(odata.y[4] == Approx(dy*44));
    CHECK(odata.y[5] == Approx(dy*45));

    CHECK(odata.x[0] == Approx(dx*20));
    CHECK(odata.x[5] == Approx(dx*25));
    CHECK(odata.x[10] == Approx(dx*30));

    CHECK(odata.f[0] == Approx(sin(dx*20)*sin(dy*40)));
    CHECK(odata.f[1] == Approx(sin(dx*20)*sin(dy*41)));
    CHECK(odata.f[64] == Approx(sin(dx*30)*sin(dy*44)));
    CHECK(odata.f[65] == Approx(sin(dx*30)*sin(dy*45)));



  }


  SECTION("Polar -> Cartesian")
  {
    GP3DData idata;
    std::vector<GP3DData> odata_slices;
    double dx = 0.1;
    double dy = 0.2;

    for(int i = 0; i < 3; ++i)
    {
      idata.x.push_back(dx*i);
      for(int j = 0; j < 10; ++j)
      {
        if( i == 0 )
          idata.y.push_back(dy*j);

        idata.f.push_back( exp(-dy*j) );
      }
    }

    ConvertGP3DDataCylindrical2Cartesian( idata, odata_slices);

    CHECK(odata_slices.size() == 3);
    CHECK(odata_slices[0].x.size() == 19);
    CHECK(odata_slices[0].y.size() == 19);
    CHECK(odata_slices[0].f.size() == 19*19);

    // (0,0) should be at index 9,9
    CHECK(odata_slices[0].x[0] == Approx(-dy*9));
    CHECK(odata_slices[0].x[9] == Approx(0));
    CHECK(odata_slices[0].x[18] == Approx(dy*9));

    CHECK(odata_slices[0].y[0] == Approx(-dy*9));
    CHECK(odata_slices[0].y[9] == Approx(0));
    CHECK(odata_slices[0].y[18] == Approx(dy*9));

    // along x = 0 and y = 0 lines, the function should be the same as z = 0
    // line of the input.
    for(int i = 0; i < 10; ++i)
    {
      CHECK(odata_slices[0].f[9*19 + 9+i] == Approx(exp(-i*dy)));
      CHECK(odata_slices[0].f[9*19 + 9-i] == Approx(exp(-i*dy)));

      CHECK(odata_slices[0].f[(9+i)*19 + 9] == Approx(exp(-i*dy)));
      CHECK(odata_slices[0].f[(9-i)*19 + 9] == Approx(exp(-i*dy)));
    }

    // at all other points, the function is interpolated, but should be close...
    for(int i = 0; i < 19; ++i)
    {
    for(int j = 0; j < 19; ++j)
    {
      double x = dy*(i-9);
      double y = dy*(j-9);
      double r = sqrt(x*x+y*y);
      if( r > dy*9) // interpolator will not extrapolate
      {
        CHECK(odata_slices[0].f[i*19 + j] == Approx(0));
      }
      else
      {
        CHECK(odata_slices[0].f[i*19 + j] == Approx(exp(-r)).epsilon(0.01));
      }
    }
    }

  }
}
