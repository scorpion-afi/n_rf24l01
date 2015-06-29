#!/bin/bash

module_name=n_rf24l01

lsmod | grep ${module_name} > /dev/null

if [ $? == 0 ]
then
  rmmod ${module_name} && echo "module ${module_name} has been unloded." || exit 1
fi

insmod ${module_name}.ko

if [ $? == 0 ]
then
  echo "module ${module_name} has been loaded."
  
  rm -f /dev/${module_name}
  mknod -m 606 /dev/${module_name} c 248 0 
  
  echo "device file /dev/${module_name} has been created." 
fi




