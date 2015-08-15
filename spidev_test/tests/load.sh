#!/bin/bash

app_name=tests
app_sources="${app_name}.c"

echo "reload ${app_name} app and it sources."

echo -n "remove ${app_name} directory..."
ssh odroid@192.168.150.8 "rm -rf ${app_name}" && echo "ok." || echo "fail." 

echo -n "create ${app_name} directory..."
ssh odroid@192.168.150.8 "mkdir ${app_name}" && echo "ok." || echo "fail." 

echo -n "copy ${app_name} to odroid..."
scp ${app_name} odroid@192.168.150.8:/home/odroid/${app_name} && echo "ok." || echo "fail." 

# copy app's sources to use them in gdb
echo -n "copy ${app_sources} to odroid..."
scp ${app_sources} odroid@192.168.150.8:/home/odroid/${app_name} && echo "ok." || echo "fail." 