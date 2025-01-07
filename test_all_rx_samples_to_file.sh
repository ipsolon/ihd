#!/bin/bash 
// FIXME - make functions maybe pass in NUM_SAMPS and calculate file size
let "NUM_SAMPS=100000"

let "FILE_SIZE_256_FFT=399360"
FILENAME=isrp_samples.dat
let "errors=0"
let "file_size_errors=0"
echo -e "\n\n\n***** psd fft_size=256 channel = 1"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=256 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1";  echo -e "***ERROR1\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
echo "FILESIZE FFT FILE $FILESIZE SHOULD BE $FILE_SIZE_256_FFT"
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_256_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=256 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=256 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "***ERROR3\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
echo "FILESIZE = ${FILESIZE}"
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_256_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=256 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=256 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1";  echo -e "***ERROR5\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_256_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_256_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=512 channel = 1"
FILE_SIZE_512_FFT=399360
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=512 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1";  echo -e "***ERROR7\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_512_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=512 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=512 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1";  echo -e "*** ERROR9\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_512_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=512 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=512 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "*** ERROR11\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_512_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_512_FFT}\n\n"; fi


echo -e "\n***** psd fft_size=1024 channel = 1"
FILE_SIZE_1024_FFT=397312
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=1024 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1";  echo -e "*** ERROR13\n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_1024_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=1024 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=1024 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "*** ERROR15 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_1024_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=1024 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=1024 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "*** ERROR17 \n\n"; fi
ls -al isrp_samples.dat
rm isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_1024_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_1024_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=2048 channel = 1"
FILE_SIZE_2048_FFT=393216
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=2048 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR19 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_2048_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=2048 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=2048 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR21 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_2048_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=2048 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=2048 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR23 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_2048_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_2048_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=4096 channel = 1"
FILE_SIZE_4096_FFT=393216
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=4096 --channel=1
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR25 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_4096_FFT}\n\n"; fi

echo -e "\n\n\n***** psd fft_size=4096 channel = 2"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=4096 --channel=2
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR27 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_4096_FFT}\n\n"; fi


echo -e "\n\n\n***** psd fft_size=4096 channel = 3"
rm isrp_samples.dat
./rx_samples_to_file --dest_ip=10.75.42.248 --args=addr=10.75.42.202 --stream_type=psd --nsamps="$NUM_SAMPS" --fft_size=4096 --channel=3
ret_code=$?
if [ $ret_code != 0 ]; then let "errors=errors+1"; echo -e "**** ERROR29 \n\n"; fi
ls -al isrp_samples.dat
FILESIZE=$(stat -c%s "$FILENAME")
if [[ $FILESIZE != $FILE_SIZE_4096_FFT ]]; then let "file_size_errors=file_size_errors+1"; echo -e "\n***ERROR file size ${FILESIZE} ne fft size ${FFT_FILES_4096_FFT}\n\n"; fi


echo "ERRORS = ${errors}"
echo "FILE SIZE ERRORS = ${file_size_errors}"
