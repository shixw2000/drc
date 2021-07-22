#!/bin/bash

root_dir=/home/shixw/opt/codes/mycodes/opc_rules
prog=$root_dir/bin_test
data_file=$root_dir/contours3.txt
log_file=$root_dir/log_test_4_1000.txt

export LD_LIBRARY_PATH=$root_dir

#clear log file
>$log_file

echo "=======start test====" >> $log_file

function test_opc {
opt=$1
min_distance=$2
max_distance=$3
distance=$min_distance
while [ $distance -le $max_distance ]
do
    $prog $opt $distance $data_file >> $log_file
    ((distance++))
done
}

test_opc 4 1000 1000

echo "=======end test====" >> $log_file
exit 0

