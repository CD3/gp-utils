#! /bin/bash


if [[ ! -e build ]]
then
  mkdir build
  cd build
  cmake ../../applications
  cd ..
fi

cd build
cmake --build .
cd ..

for file in build/gp-utils-*
do
  echo "### ${file/build\//}"
  echo "\`\`\`bash"
  $file -h
  echo "\`\`\`"
  echo
done > help-messages.txt

