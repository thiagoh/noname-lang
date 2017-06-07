#!/bin/bash

verbose=0
to_be_translated_directory=$(pwd)/accessory-src/to-be-translated
# clang_dir=/Users/thiagoh/dev/llvm-src/build-3.8/bin
clang_dir=/Users/thiagoh/dev/llvm-src/build-3.9/bin

for i in "$@"; do
  key="$1"
  case $key in
      -v|-p|--verbose|--print)
      verbose=1
      shift
      ;;
      --directory=*)
      to_be_translated_directory="${key#*=}"
      shift # past argument=value
      ;;
      *)
      shift
      ;;
  esac
done

cd $clang_dir

rm -rf $to_be_translated_directory/ll-and-api-output/*.s
rm -rf $to_be_translated_directory/ll-and-api-output/*.ll
rm -rf $to_be_translated_directory/ll-and-api-output/*.cpp

for file in $(ls $to_be_translated_directory/cpp-src/*.cpp 2>/dev/null); do 
  filename=$(basename $file)
  filename="${filename%.*}"

  echo Generating: $to_be_translated_directory/ll-and-api-output/$filename.ll

  $clang_dir/clang -S -emit-llvm $file \
    -o $to_be_translated_directory/ll-and-api-output/$filename.ll \

  echo Generating: $to_be_translated_directory/ll-and-api-output/$filename.generated.cpp
  
  $clang_dir/llc -march=cpp $to_be_translated_directory/ll-and-api-output/$filename.ll \
    -o $to_be_translated_directory/ll-and-api-output/$filename.generated.cpp

  echo 
done
  
cd $to_be_translated_directory