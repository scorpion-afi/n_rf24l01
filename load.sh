#!/bin/bash

module_name=n_rf24l01

lsmod | grep ${module_name} > /dev/null

if [ $? == 0 ]
then
  rmmod ${module_name}
  echo "module ${module_name} has been unloded."
fi

insmod ${module_name}.ko
echo "module ${module_name} has been loaded."
