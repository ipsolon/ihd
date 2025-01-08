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

function exec_rx_samples_to_file()
{
   local tmp_dest_ip=$1
   local tmp_cham_ip=$2
   local tmp_num_samps=$3
   local tmp_fft_size=$4
   local tmp_channel=$5

   echo "parms ${1} ${2} ${3} ${4} ${5}"

   echo "HUH? ${tmp_dest_ip} ${tmp_cham_ip} ${tmp_num_samps} ${tmp_fft_size} ${tmp_channel}"
   rm isrp_samples.dat
   ./rx_samples_to_file --stream_type=psd --dest_ip="${tmp_dest_ip}" --args=addr="${tmp_cham_ip}"  --nsamps="${tmp_num_samps}" --fft_size="${tmp_fft_size}" #--channel=${channel}
   ret_code=$?
   if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: fft: ${fft_size} channel: ${channel}\n"; fi
   ls -al isrp_samples.dat
   FILESIZE=$(stat -c%s "$FILENAME")
   let "CORRECT_FILE_SIZE=$(($tmp_num_samps * 4))"
   echo "correct size ${CORRECT_FILE_SIZE}"
   if [ $FILESIZE != $CORRECT_FILE_SIZE ]; then
     let "file_size_errors=file_size_errors+1";
     echo -e "\n***ERROR file size ${FILESIZE} ne correct size ${CORRECT_FILE_SIZE}\n\n";
   fi
}

echo "BEFORE call echo ${dest_ip} ${cham_ip} ${NUM_SAMPS}"
exec_rx_samples_to_file ${dest_id} ${cham_ip} ${NUM_SAMPS} 256 1

exit 1

let "FILE_SIZE_256_FFT=4000000"
echo -e "\n\n\n***** psd fft_size=256 channel = 1"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=256 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR fft256 channel1\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_256_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=256 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=256 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "***ERROR3\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_256_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=256 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=256 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR5\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_256_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=512 channel = 1"
FILE_SIZE_512_FFT=4000000
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=512 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR7\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_512_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=512 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=512 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "*** ERROR9\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_512_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=512 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=512 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "*** ERROR11\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_512_FFT}\n\n"; fi


echo -e "\n***** psd fft_size=1024 channel = 1"
FILE_SIZE_1024_FFT=4000000
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=1024 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "*** ERROR13\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_1024_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=1024 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=1024 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "*** ERROR15 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_1024_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=1024 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=1024 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "*** ERROR17 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_1024_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=2048 channel = 1"
FILE_SIZE_2048_FFT=4000000
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=2048 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR19 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_2048_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=2048 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=2048 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR21 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_2048_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=2048 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=2048 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR23 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_2048_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=4096 channel = 1"
FILE_SIZE_4096_FFT=4000000
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=4096 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR25 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_4096_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=4096 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=4096 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR27 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_4096_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=4096 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=${dest_ip} --args=addr=${cham_ip} --stream_type=psd --nsamps=${NUM_SAMPS} --fft_size=4096 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then  let "errors=errors+1"; echo -e "**** ERROR29 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then  let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FILE_SIZE_4096_FFT}\n\n"; fi


echo "ERRORS = ${errors}"
echo "FILE SIZE ERRORS = ${file_size_errors}"
