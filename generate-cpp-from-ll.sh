#!/bin/bash

verbose=0
to_be_translated_directory=$(pwd)/accessory-src/to-be-translated
clang_dir=/Users/thiagoh/dev/llvm-src/build-3.8/bin

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

rm -rf $to_be_translated_directory/cpp-and-bin-from-ll-output/*.s
rm -rf $to_be_translated_directory/cpp-and-bin-from-ll-output/*.ll
rm -rf $to_be_translated_directory/cpp-and-bin-from-ll-output/*.cpp
rm -rf $to_be_translated_directory/cpp-and-bin-from-ll-output/*.out

for file in $(ls $to_be_translated_directory/ll-src/*.ll 2>/dev/null); do 
  filename=$(basename $file)
  filename="${filename%.*}"

  echo Generating: $to_be_translated_directory/cpp-and-bin-from-ll-output/$filename.s

  $clang_dir/llc $file \
    -o $to_be_translated_directory/cpp-and-bin-from-ll-output/$filename.s \

  echo Generating: $to_be_translated_directory/cpp-and-bin-from-ll-output/$filename.out

  g++ $to_be_translated_directory/cpp-and-bin-from-ll-output/$filename.s \
    -o $to_be_translated_directory/cpp-and-bin-from-ll-output/$filename.out

  echo
done
  
cd $to_be_translated_directory