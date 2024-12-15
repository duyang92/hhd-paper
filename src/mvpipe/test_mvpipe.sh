#!/bin/bash
echo compiling mvpipe algorithm and testing programme
make all
bit_thresholds=(10000 20000 30000 40000 50000)
bit_memory=(128 256 512 1024)
byte_thresholds=(2000 4000 6000 8000 10000)
byte_memory=(64 128 256 512 1024)
echo check if files to be compared are present
for i in "${byte_thresholds[@]}"
do
    if [ ! -f "flow_actual_size_byte_$i" ]
    then
        echo "flow_actual_size_byte_$i not exist"
        ./dataset_process_byte $i
    fi
done
for i in "${bit_thresholds[@]}"
do
    if [ ! -f "flow_actual_size_bit_$i" ]
    then
        echo "flow_actual_size_bit_$i not exist"
        ./dataset_process_bit $i
    fi
done
echo testing mvpipe algorithm
echo running mvpipe algorithm for byte lattice 
for i in "${byte_thresholds[@]}"
do
    for j in "${byte_memory[@]}"
    do
        echo "--------------------"
        echo "threshold: $i,memory: $j"
        ./mvpipe_byte $i $j
        ./check_byte $i
    done
done
echo ---------------------
echo byte lattice test done
# echo running mvpipe algorithm for bit lattice
# for i in "${bit_thresholds[@]}"
# do
#     for j in "${bit_memory[@]}"
#     do
#         echo "--------------------"
#         echo "threshold: $i,bucket_num: $j"
#         ./mvpipe $i $j
#         ./check_bit $i
#     done
# done
# echo ---------------------
echo test done
