# File: ram_mount.sh
# Date: 2014. 12. 03.
# Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
# Copyright(c)2014
# Hanyang University, Seoul, Korea
# Embedded Software Systems Laboratory. All right reserved

#!/bin/bash
sudo mount -t tmpfs -o size=10G tmpfs ./rd
cp .gitignore_o rd/.gitignore