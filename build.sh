cd build
if [ $0 = "release" ]; then
  echo "Build release version"
  cmake -DCMAKE_BUILD_TYPE=$0 ../src
else
  echo "Build debug version"
  cmake ../src
fi
