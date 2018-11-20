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

  SECTION("File Read Functions")
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

TEST_CASE("Performance Benchmarks", "[!hide][benchmarks]")
{
      SECTION("3D ASCII Writer")
      {
        BM::PerformanceBenchmark bm("ascii-3d-write");
        BM::Benchmark meter;

        GP3DData data;
        MakeLarge3DData(100,200,data);
        meter.run([&]() { WriteGPASCII3DDataFile("test-3d.txt", data); });
        auto result = bm(meter);
        std::cout << "ascii 3d writer"
                  << "\n";
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
