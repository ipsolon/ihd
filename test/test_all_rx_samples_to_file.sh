#!/bin/bash 

if [ $# -ne 3 ]
then
  echo "Usage: $(basename $0) DEST_IP CHAMELEON_IP NUM_SAMPS"
  exit 1
fi
dest_ip=$1
cham_ip=$2
NUM_SAMPS=$3
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
   ./rx_samples_to_file --stream_type=psd --dest_ip="${dest_ip}" --args=addr="${cham_ip}"  --nsamps="${num_samps}" --fft_size="${fft_size}" --channel=${channel}
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

exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 3

exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 3

exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 3

exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 3

exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 1
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 2
exec_rx_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 3

echo "ERRORS = ${errors}"
echo "FILE SIZE ERRORS = ${file_size_errors}"
