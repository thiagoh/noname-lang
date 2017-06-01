#!/bin/bash

verbose=0
to_be_translated_directory=$(pwd)/accessory-src/to-be-translated
clang_directory=/Users/thiagoh/dev/llvm-src/build-3.8/bin

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

cd $clang_directory

rm -rf $to_be_translated_directory/translated/*.s
rm -rf $to_be_translated_directory/translated/*.ll
rm -rf $to_be_translated_directory/translated/*.cpp

for file in $(ls $to_be_translated_directory/*.cpp 2>/dev/null); do 
  echo file: $file
  filename=$(basename $file)
  filename="${filename%.*}"

  ./clang -S -emit-llvm $file -o $to_be_translated_directory/translated/$filename.ll \
    && ./llc -march=cpp $to_be_translated_directory/translated/$filename.ll \
    -o $to_be_translated_directory/translated/$filename.generated.cpp

  llc $to_be_translated_directory/translated/$filename.ll -o $to_be_translated_directory/translated/$filename.s
  g++ $to_be_translated_directory/translated/$filename.s -o $to_be_translated_directory/translated/$filename.out
done
  
cd $to_be_translated_directory