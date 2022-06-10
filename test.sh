#!/bin/bash
# Author: qushijie

logFile=./error_log.txt

cmd_list=("touch $logFile" "make" "sudo insmod kernel_test.ko" "sudo dmesg -c" "gcc user_test.c -o run" "sudo ./run")
for ((i=0;i<${#cmd_list[@]};i++));do  # ${#cmd_list[@]}  is array lenth
    ${cmd_list[$i]}
    if [ $? -ne 0 ];then
	for ((j=0;j<5;j++));do
	    ${cmd_list[$i]}
        done
	if [ $? -ne 0 ];then
	    ${cmd_list[$i]} > $logFile 2>&1
	    exit
	fi
    fi
done

#IFS="$OLDIFS"
