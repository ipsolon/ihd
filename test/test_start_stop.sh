#!/bin/bash 

if [[ $# -ne 5 ]]
then
  echo "Usage: $(basename $0) DEST_IP DEST_PORT CHAMELEON_IP CHAN_MASK LOOPS DELAY[optional]"
  exit 1
fi
dest_ip=$1
dest_port=$2
cham_ip=$3
chan_mask=$4
loops=$5

if [[ $# -ne 6 ]];
then
  delay=0
else
  delay=$4
fi

echo "Running with dest_ip=$dest_ip cham_ip=$cham_ip dlay=$delay"

let "errors=0"

function exec_packet_check() {
   local dest_ip=$1
   local cham_ip=$2
   local chan_mask=$3
   local stream_type=$4

   GREEN='\033[0;32m'
   WHITE='\033[0;37m'
   echo -e "${GREEN} Running test with chan_mask ${chan_mask} ${WHITE}\n"
   ./packet_check --stream_type="${stream_type}" --dest_ip="${dest_ip}" --dest_port="${dest_port}" --args=addr="${cham_ip}"  --chan_mask="${chan_mask}"
   ret_code=$?
   if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: fft: ${fft_size} channel: ${channel}\n"; fi

}

for (( i=0; i<$loops; i+=1 ))
do  
   exec_packet_check "${dest_ip}" "${cham_ip}" "${chan_mask}" psd
   sleep ${delay}
done

for (( i=0; i<$loops; i+=1 ))
do  
   exec_packet_check "${dest_ip}" "${cham_ip}" "${chan_mask}" iq
   sleep ${delay}
done


echo "ERRORS = ${errors}"


