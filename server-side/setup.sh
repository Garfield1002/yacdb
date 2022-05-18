#!/usr/bin/env bash

if [ ! -d _build ]
then
mkdir _build
fi

cd _build
cmake ..
cd ..