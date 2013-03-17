#!/bin/bash

perl -e 'print pack("a4 a12 a12 a48", "NOTM", "ControlNode", "WhoDat", "192.168.1.1");' |nc 192.168.1.100 2300

