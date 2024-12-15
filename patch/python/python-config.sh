#!/bin/bash

prefix=$(dirname $0)

while [[ $# -gt 0 ]]; do
  case $1 in
    --includes)
      echo "-I$prefix/include/python -DPy_NO_ENABLE_SHARED"
      shift
      ;;
    --ldflags)
      echo "-L$prefix/lib -lpythoncore -lbcrypt -lversion -lws2_32"
      shift
      ;;
    --exec-prefix)
      echo "$prefix"
      shift
      ;;
    *)
      shift
      ;;
  esac
done
