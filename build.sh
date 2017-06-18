#!/bin/bash

module_name="n_rf24l01.ko"

rm -f ${module_name}

rm -rf temp; mkdir -p temp
cp src/* temp/

cd temp && make && cp ${module_name} ../ && cd ../ && rm -rf temp
