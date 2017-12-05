rm ~/VSSIM/VSSIM1/QEMU/x86_64-softmmu/hash.dat

cd ~/VSSIM/VSSIM2/QEMU/x86_64-softmmu
gnome-terminal -x sh -c "./run_linux.sh"

# VSSIM1 must come last
cd ~/VSSIM/VSSIM1/QEMU/x86_64-softmmu
gnome-terminal -x sh -c "./run_linux.sh"