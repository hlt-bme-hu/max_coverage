#!/usr/bin/env bash

mkdir -p `hostname`
cd `hostname`
cmake -DCMAKE_BUILD_TYPE=Release .. && make
