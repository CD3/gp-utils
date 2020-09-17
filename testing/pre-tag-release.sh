#! /bin/bash

set -e
root=$(git rev-parse --show-toplevel)

cd $root

bindir="$PWD/build-and-test"
function cleanup()
{
  rm -r $bindir
}
trap cleanup EXIT

function error_exit()
{
  cp -r $bindir $bindir.error
  echo "There was an error."
  echo "Tapped on '$BASH_COMMAND'"
}
trap error_exit ERR

mkdir $bindir
cd $bindir
conan install .. --build missing
veval cmake .. -DCMAKE_INSTALL_PREFIX=$bindir/install
cmake --build .
cmake --build . --target test



# test install

cmake --build . --target install


# TEST APP
cd $bindir
echo
echo "Building test app"
rm -rf app
mkdir app
cd app

cat << EOF > main.cpp
#include <iostream>
#include <gputils/io.hpp>

int main()
{
  return 0;
}
EOF

cat << EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
add_executable( main main.cpp )
find_package( gputils REQUIRED )
target_link_libraries(main gputils::gputils)
set_target_properties(main PROPERTIES CXX_STANDARD 14)
EOF

mkdir build1
cd build1
conan install ${root} --build missing
veval cmake .. -Dgputils_DIR=${bindir}/install/cmake/
cmake --build .

cd ..
cat << EOF > main.cpp
#include <iostream>
#include <gputils/io.hpp>

int main()
{
  return 0;
}
EOF

cd build1
conan install ${root} --build missing
veval cmake .. -Dgputils_DIR=${bindir}/install/cmake/
cmake --build .
./main










echo "PASSED"

exit 0
