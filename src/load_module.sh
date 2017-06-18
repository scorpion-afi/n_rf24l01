#!/bin/bash

# this script must be executed on the target with root priveleges
# this script (re)load the kernel module and create the device file to
# comunicate with this loaded module.

# TODO: this value has to be got from kernel log, after module has been loaded
dev_maj_number=248

module_name=n_rf24l01

# if the target also has the ${module_name} driver at first unload it
lsmod | grep ${module_name} > /dev/null

if [ $? == 0 ]
then
  modprobe -r ${module_name}
  
  if [ $? == 0 ]; then
    echo "module ${module_name} has been unloded."
  else
    echo "fail to remove module."; exit 1
  fi
fi

# load linux kernel module
modprobe ${module_name}

# create device file to comunicate from user space
if [ $? == 0 ]
then
  echo "module ${module_name} has been loaded."
  
  rm -f /dev/${module_name}
  mknod -m 606 /dev/${module_name} c ${dev_maj_number} 0 
  
  echo "device file /dev/${module_name} has been created."
else
  echo "fail to load module ${module_name}."
fi
