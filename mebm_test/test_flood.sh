#!/bin/bash

declare -a senders=(test_send_bad_magic.sh test_send_magstripe.sh test_send_open_door.sh test_send_who_dat.sh)

while true; do
	./${senders[$(($RANDOM % ${#senders[@]}))]}
	sleep 1
done
