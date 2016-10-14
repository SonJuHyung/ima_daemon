#!/bin/bash

check_reboot="ima_appraise_check_reboot"
check_working="ima_appraise_check_working"
log_path_init="/usr/ima-appraisal/log.txt"

dmesg_checking=$( pgrep 'dmesg_checking' | wc -l )
pid=$(pgrep 'dmesg_checking')

if [ -f $log_path_init  ]
then
    rm -rf $path
fi

if [ $dmesg_checking -eq 0 ]
then
    echo $check_reboot
    cd /usr/ima-appraisal/
    ./dmesg_checking
else
    daenom_count=$dmesg_checking
    echo $dmesg_checking" "$check_working", pid : "$pid
fi


