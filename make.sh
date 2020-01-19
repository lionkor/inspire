#!/bin/bash

mkdir -p bin
cd bin 
cmake .. -S ..
make -j9
cd ..

