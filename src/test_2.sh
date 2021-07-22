#!/bin/bash

root_dir=/home/shixw/opt/codes/mycodes/opc_rules
prog=$root_dir/bin_test
data_file=$root_dir/contours3.txt
log_file=$root_dir/log_test_2.txt

export LD_LIBRARY_PATH=$root_dir

#clear log file
>$log_file

echo "=======start test====" >> $log_file

function test_opc {

distance_beg=1
distance_end=400

min_polygon_beg=1
min_polygon_end=100

min_width_beg=1
min_width_end=400

distance=$distance_beg
min_polygon=$min_polygon_beg
min_width=$min_width_beg

ret_1=0
ret_2=0
while [ $distance -le $distance_end ]
do
    $prog 3 $distance $data_file 0
    ret_1=$?

    min_polygon=$min_polygon_beg
    while [ $min_polygon -le $min_polygon_end ]
    do
    	if [ $min_width_beg -le $distance ]
    	then
    		min_width=$distance
    	else
    		min_width=$min_width_beg
    	fi

    	if [ $min_width_end -le $distance ]
    	then
    		min_width_loop=$distance
    	else
    		min_width_loop=$min_width_end
    	fi
    	
        while [ $min_width -le $min_width_loop ]
        do
            $prog 2 $distance $data_file $min_polygon $min_width 0 >> $log_file 2>&1
            ret_2=$?

            if [ $ret_1 -ne $ret_2 ]
            then
                echo "distance=$distance| min_polygon=$min_polygon| min_width=$min_width| ret1=$ret_1| ret2=$ret_2| err=invalid ret|" >> $log_file
                exit -1
            fi

            ((min_width++))
        done

        ((min_polygon++))
    done

    ((distance++))
done
}

test_opc

echo "=======end test====" >> $log_file
exit 0

