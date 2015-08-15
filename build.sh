#!/bin/bash

rm -rf temp; mkdir -p temp
cp src/Makefile src/Kbuild src/n_rf24l01_init.c src/n_rf24l01_sysfs.c src/n_rf24l01.h temp/

cd temp && make && cp n_rf24l01.ko ../ && cd ../ && rm -rf temp