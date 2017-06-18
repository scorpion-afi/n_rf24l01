#!/bin/bash

# the purpose of this script is loading of linux kernel module image (*.ko file)
# to the odroid target, also auxilary script (load_module.sh), used on target,
# gets loaded to target.

#TODO: ODROID_IP is statically defined in ~/.bashrc file,
#      it'd be good to have it automatically discovered

odroid_dir="/home/odroid/n_rf24l01"
module_name="n_rf24l01.ko"

echo ""; echo "loading module..."; echo ""
ssh odroid@${ODROID_IP} "mkdir -p ${odroid_dir}"
scp src/load_module.sh ${module_name} odroid@${ODROID_IP}:${odroid_dir} || (echo ""; echo "...fail"; exit 1)
echo ""; echo "...ok."
