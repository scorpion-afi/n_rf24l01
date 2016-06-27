#!/bin/bash

odroid_ip=10.42.0.116

echo ""; echo "loading module..."; echo ""
scp src/load_module.sh n_rf24l01.ko odroid@${odroid_ip}:/home/odroid || (echo ""; echo "...fail"; exit 1)
ssh root@${odroid_ip} "cd /home/odroid && ./load_module.sh"

if [ $? == 0 ]
then
  echo ""; echo "...ok." 
else
  echo ""; echo "...fail."
fi