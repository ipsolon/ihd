#!/bin/bash

if [[ $# -ne 1 ]]
then
  echo "Usage: $(basename $0) ARGS CHAN FREQ GAIN"
  exit 1
fi

args=$1


GREEN='\033[0;32m'
WHITE='\033[0;37m'
echo -e  " $GREEN Running with --args=$args  $WHITE"


function exec_ipsolon_timed_jammer() {
   local args=$1
   local chan=$2
   local freq=$3
   local gain=$4

   ./ipsolon_timed_jammer --args=${args} --channel=$chan --freq=$freq --gain=$gain
    ret_code=$?
    if [ $ret_code != 0 ]; then  let "errors=errors+1";  echo -e "***ERROR: args: $args channel: $chan freq: $freq gain: $gain \n"; fi
}


exec_ipsolon_timed_jammer  $args 1 2450000000 0
exec_ipsolon_timed_jammer  $args 1 2450000000 10
exec_ipsolon_timed_jammer  $args 1 2450000000 20
exec_ipsolon_timed_jammer  $args 1 2450000000 40

exec_ipsolon_timed_jammer  $args 1 5250000000 0
exec_ipsolon_timed_jammer  $args 1 5250000000 10
exec_ipsolon_timed_jammer  $args 1 5250000000 20
exec_ipsolon_timed_jammer  $args 1 5250000000 40

exec_ipsolon_timed_jammer  $args 2 2450000000 0
exec_ipsolon_timed_jammer  $args 2 2450000000 10
exec_ipsolon_timed_jammer  $args 2 2450000000 20
exec_ipsolon_timed_jammer  $args 2 2450000000 40

exec_ipsolon_timed_jammer  $args 2 5250000000 0
exec_ipsolon_timed_jammer  $args 2 5250000000 10
exec_ipsolon_timed_jammer  $args 2 5250000000 20
exec_ipsolon_timed_jammer  $args 2 5250000000 40


exec_ipsolon_timed_jammer  $args 3 2450000000 0
exec_ipsolon_timed_jammer  $args 3 2450000000 10
exec_ipsolon_timed_jammer  $args 3 2450000000 20
exec_ipsolon_timed_jammer  $args 3 2450000000 40

exec_ipsolon_timed_jammer  $args 3 5250000000 0
exec_ipsolon_timed_jammer  $args 3 5250000000 10
exec_ipsolon_timed_jammer  $args 3 5250000000 20
exec_ipsolon_timed_jammer  $args 3 5250000000 40

exec_ipsolon_timed_jammer  $args 4 2450000000 0
exec_ipsolon_timed_jammer  $args 4 2450000000 10
exec_ipsolon_timed_jammer  $args 4 2450000000 20
exec_ipsolon_timed_jammer  $args 4 2450000000 40

exec_ipsolon_timed_jammer  $args 4 5250000000 0
exec_ipsolon_timed_jammer  $args 4 5250000000 10
exec_ipsolon_timed_jammer  $args 4 5250000000 20
exec_ipsolon_timed_jammer  $args 4 5250000000 40
