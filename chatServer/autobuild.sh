set -x

if [! -x "$PWD/build"]; then
  mkdir build
fi

 rm -rf $PWD/build/*

 cd build && 
	 cmake .. &&
	 make

