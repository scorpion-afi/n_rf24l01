#!/bin/bash

# this script must be executed with root priveleges

module_name=n_rf24l01

# if the target also has the ${module_name} driver at first unload it
lsmod | grep ${module_name} > /dev/null

if [ $? == 0 ]
then
  modprobe -r ${module_name} && echo "module ${module_name} has been unloded." || exit 1
fi

# load aux spi master driver and our driver
modprobe spi-s3c64xx
modprobe ${module_name}

# create device file to comunicate from user space
if [ $? == 0 ]
then
  echo "module ${module_name} has been loaded."
  
  rm -f /dev/${module_name}
  mknod -m 606 /dev/${module_name} c 248 0 
  
  echo "device file /dev/${module_name} has been created."
else
  echo "error during modprobe ${module_name}."
fi
