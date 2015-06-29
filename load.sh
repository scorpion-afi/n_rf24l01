#!/bin/bash

echo ""; echo "loading module..."; echo ""
scp load_module.sh n_rf24l01.ko odroid@192.168.150.8:/home/odroid || (echo ""; echo "...fail"; exit 1)
ssh root@192.168.150.8 "cd /home/odroid && ./load_module.sh"

if [ $? == 0 ]
then
  echo ""; echo "...ok." 
else
  echo ""; echo "...fail."
fi