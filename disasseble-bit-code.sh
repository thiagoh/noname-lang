#!/bin/bash

verbose=0
output_directory=bc-output

for i in "$@"; do
  key="$1"
  case $key in
      -v|-p|--verbose|--print)
      verbose=1
      shift
      ;;
      -d)
      output_directory=$2
      shift
      shift
      ;;
      --output-directory=*)
      output_directory="${key#*=}"
      shift # past argument=value
      ;;
      *)
      shift
      ;;
  esac
done

echo $output_directory
exit

for file in *.bc; do 
  llvm-dis $file -o $file.ll

  if [ $verbose -eq 1 ]; then
    cat $file.ll
  fi

done