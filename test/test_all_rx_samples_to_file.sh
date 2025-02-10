#!/bin/bash 

if [[ $# -lt 5  || $# -gt 6 ]]
then
  echo "Usage: $(basename $0) DEST_IP DEST_PORT CHAMELEON_IP NUM_SAMPS STREAM_MASK TEST_MASK(optional)"
  echo "STREAM_MASK: bit1=iq, bit2=psd"
  echo "TEST_MASK: bit1=256, bit2=512 bit3=1024 bit4=2048 bit5=4096"
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

stream_iq=$(($5 & 1))
stream_psd=$(($5 & 2))

if [ $# -eq 6 ]; then
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

function exec_rx_psd_samples_to_file() {
   local dest_ip=$1
   local cham_ip=$2
   local num_samps=$3
   local fft_size=$4
   local channel=$5
   local stream_type=$6
   GREEN='\033[0;32m'
   WHITE='\033[0;37m'

   echo -e "${GREEN} Running psd test with fft_size ${fft_size} channel: ${channel} ${WHITE}\n"

   rm isrp_samples.dat
   ./rx_samples_to_file --stream_type="${stream_type}" --dest_ip="${dest_ip}" --dest_port="${dest_port}" --args=addr="${cham_ip}"  --nsamps="${num_samps}" --fft_size="${fft_size}" --channel=${channel}

   ret_code=$?
   if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: fft: ${fft_size} channel: ${channel}\n"; fi
   ls -al isrp_samples.dat

   FILESIZE=$(stat -c%s "$FILENAME")
   # calculate the correct size
   # total_bytes = num_samps * 4
   total_bytes=$((num_samps * 4))
   # file size = total_bytes - (total_bytes % fft_size*4)
   fft_bytes=$((fft_size * 4))
   CORRECT_FILE_SIZE=$((total_bytes-(total_bytes % fft_bytes)))

   if [ $FILESIZE != $CORRECT_FILE_SIZE ]; then
     let "file_size_errors=file_size_errors+1";
     echo -e "\n***ERROR file size ${FILESIZE} ne correct size ${CORRECT_FILE_SIZE}\n\n";
   fi
} # end function exec_rx_psd_samples_to_file


function exec_rx_iq_samples_to_file() {
   local dest_ip=$1
   local cham_ip=$2
   local num_samps=$3
   local fft_size=$4
   local channel=$5
   local stream_type=$6
   GREEN='\033[0;32m'
   WHITE='\033[0;37m'

   echo -e "${GREEN} Running iq test  channel: ${channel} ${WHITE}\n"

   rm isrp_samples.dat
   ./rx_samples_to_file --stream_type="${stream_type}" --dest_ip="${dest_ip}" --dest_port="${dest_port}" --args=addr="${cham_ip}" --channel=${channel}

   ret_code=$?
   if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: fft: ${fft_size} channel: ${channel}\n"; fi
   ls -al isrp_samples.dat

   FILESIZE=$(stat -c%s "$FILENAME")
   CORRECT_FILE_SIZE=682687824

   if [ $FILESIZE != $CORRECT_FILE_SIZE ]; then
     let "file_size_errors=file_size_errors+1";
     echo -e "\n***ERROR file size ${FILESIZE} ne correct size ${CORRECT_FILE_SIZE}\n\n";
   fi
} # end function exec_rx_samples_to_file



if [ $stream_iq == 1 ];
then
    exec_rx_iq_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 1 iq
    sleep 1
    exec_rx_iq_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 2 iq
    sleep 1
fi


if [ $stream_psd == 2 ];
then
  if [ $test_256 == 1 ];
  then
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 1 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 2 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 4 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 8 psd
    sleep 1
  fi

  if [ $test_512 == 2 ];
  then
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 1 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 512 2 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 4 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 8 psd
    sleep 1
  fi

  if [ $test_1024 == 4 ];
  then
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 1 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 1024 2 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 4 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 256 8 psd
    sleep 1
  fi

  if [ $test_2048 == 8 ];
  then
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 1 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 2 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 4 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 2048 8 psd
    sleep 1
  fi

  if [ $test_4096 == 16 ];
  then
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 1 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 2 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 4 psd
    sleep 1
    exec_rx_psd_samples_to_file "${dest_ip}" "${cham_ip}" "${NUM_SAMPS}" 4096 8 psd
  fi
fi

echo "ERRORS = ${errors}"
echo "FILE SIZE ERRORS = ${file_size_errors}"
