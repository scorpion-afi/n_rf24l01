#!/bin/bash

rm -f n_rf24l01.ko

rm -rf temp; mkdir -p temp
cp src/* temp/

cd temp && make && cp n_rf24l01.ko ../ && cd ../ && rm -rf temp
