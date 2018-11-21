#include "Benchmark.hpp"
#include "catch.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "gputils/io.hpp"

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

  SECTION("ASCII File Read Functions")
  {
    ofstream out("test-3d.txt");

    SECTION("Single space delimited")
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

    SECTION("Tab delimited")
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
  }


  SECTION("File Write Functions")
  {
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
  }
}

TEST_CASE("File Filtering")
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
