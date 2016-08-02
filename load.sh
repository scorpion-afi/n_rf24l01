#!/bin/bash

odroid_dir="/home/odroid/n_rf24l01"

echo ""; echo "loading module..."; echo ""
ssh odroid@${ODROID_IP} "mkdir -p ${odroid_dir}"
scp src/load_module.sh n_rf24l01.ko odroid@${ODROID_IP}:${odroid_dir} || (echo ""; echo "...fail"; exit 1)
echo ""; echo "...ok."
