#!/bin/bash

while true; do 
	s=$(nc -p 2300 -l |perl -n -e 'printf "magic=%s from=%s type=%s data=%s\n", unpack("a4 a12 a12 a48");')
	date "+%Y-%m-%d %T %Z : $s"
done
