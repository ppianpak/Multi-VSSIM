## VSSIM Linux re-run script
## Copyright(c)2014
## Hanyang University, Seoul, Korea
## Embedded Software Systems Laboratory. All right reserved

#!/bin/bash

# rm -rf data/*.dat
./qemu-system-x86_64 -m 2048M -cpu core2duo -hda ../../../RAMDISK/rd/ssd_hda_1.img -usbdevice tablet -redir tcp:2221::22
