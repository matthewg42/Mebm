#!/bin/bash

iterations=${1:-100}
iterations=$(($iterations / 2))

echo "sending $(($iterations*2)) messages"
for ((i=0; i<$iterations; i++)); do 
	./test_send_who_dat.sh
	echo -n "."
	./test_send_open_door.sh
	if [ $(($i % 50)) -eq 49 ]; then
		echo ". $((i+1))"
	else
		echo -n "."
	fi
	msleep 300
done

echo ""

