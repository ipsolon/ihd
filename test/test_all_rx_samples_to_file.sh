#!/bin/bash 

if [[ $# -lt 4  || $# -gt 5 ]]
then
  echo "Usage: $(basename $0) DEST_IP DEST_PORT CHAMELEON_IP NUM_SAMPS TEST_MASK(optional)"
  exit 1
fi
dest_ip=$1
dest_port=$2
cham_ip=$3
NUM_SAMPS=$4

test_256=0
test_512=0
test_1024=0
test_2048=0
test_4096=0

echo "arg 5 is -> $5\n"
if [ $# -eq 5 ]; then
   test_256=$(($5 & 0x1))
   test_512=$(($5 & 0x2))
   test_1024=$(($5 & 0x4))
   test_2048=$(($5 & 0x8))
   test_4096=$(($5 & 0x10))
else
   # test mask 1=256, 2=512, 4=1024, 8=2048, 16=4096
   test_256=1
   test_512=2
   test_1024=4
   test_2048=8
   test_4096=16
fi

echo "Running with dest_ip=$dest_ip cham_ip=$cham_ip num_samps=$NUM_SAMPS"

FILENAME=isrp_samples.dat
let "errors=0"
let "file_size_errors=0"

function exec_rx_samples_to_file() {
   local dest_ip=$1
   local cham_ip=$2
   local num_samps=$3
   local fft_size=$4
   local channel=$5
   echo -e "Running test with fft_size ${fft_size} channel: ${channel}\n"
   rm isrp_samples.dat
   ./rx_samples_to_file --stream_type=psd --dest_ip="${dest_ip}" --dest_port="${dest_port}" --args=addr="${cham_ip}"  --nsamps="${num_samps}" --fft_size="${fft_size}" --channel=${channel}
   ret_code=$?
   if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: fft: ${fft_size} channel: ${channel}\n"; fi
   ls -al isrp_samples.dat
   FILESIZE=$(stat -c%s "$FILENAME")
   # calculate the correct size
   # total_bytes = num_samps * 4
   total_bytes=$((num_samps * 4))
   echo "total_bytes ${total_bytes}"
   # file size = total_bytes - (total_bytes % fft_size*4)
   fft_bytes=$((fft_size * 4))
   echo "fft_bytes ${fft_bytes}"

   CORRECT_FILE_SIZE=$((total_bytes-(total_bytes % fft_bytes)))
   echo "FILESIZE: ${FILESIZE} CORRECT: ${CORRECT_FILE_SIZE}"
   if [ $FILESIZE != $CORRECT_FILE_SIZE ]; then
     let "file_size_errors=file_size_errors+1";
     echo -e "\n***ERROR file size ${FILESIZE} ne correct size ${CORRECT_FILE_SIZE}\n\n";
   fi
}

if [ $test_256 == 1 ];
then
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 3
fi

if [ $test_512 == 2 ];
then
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 3
fi

if [ $test_1024 == 4 ];
then
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 3
fi

if [ $test_2048 == 8 ];
then
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 3
fi

if [ $test_4096 == 16 ];
then
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 3
fi

echo "ERRORS = ${errors}"
echo "FILE SIZE ERRORS = ${file_size_errors}"
