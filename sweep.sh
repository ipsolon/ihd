#!/bin/bash
set -x
# Initialize the integer variable
counter=0
CHANNEL=1
GAIN=31.5
NSAMPS=24375000
CHAM_IP=192.168.60.2
DEST_IP=192.168.60.100

CAPTURE_EXE=/qwtwaterfallplot/build/ihd/rx_samples_to_file

DIR=$PWD/samples_$(date +%Y_%m_%d-%H_%M_%S)

mkdir -p ${DIR}

# Loop from 0 to 4 (inclusive), incrementing 'i' by 1 in each iteration
for (( i=300000000; i<=5800000000; i+=25000000 )); do
    echo "Current value of i: $i"
    # You can also increment 'counter' within the loop
    ((counter++))
    FILENAME="samples_counter_${counter}_freq_${i}_channel_${CHANNEL}_gain_${GAIN}.dat"
    echo "Current value of counter: $counter filename:$FILENAME"
    ${CAPTURE_EXE} --args="addr=${CHAM_IP}" --dest_ip="${DEST_IP}" --stream_type=iq --channel ${CHANNEL}  --nsamps=${NSAMPS} --dest_port=14090 --freq=${i} --gain=${GAIN} --file ${DIR}/${FILENAME}
done

echo "Loop finished. Final value of counter: $counter"
