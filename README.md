# PRU-framework
PRUSS support for newer kernels (3.14 and above). A complete communication framework between ARM and PRUSS.

##How to build.

###Kernel Module.
1. Install the correct kernel headers.

	`sudo apt-get install linux-headers-uname -r`

2. Make backup of existing pruss_remoteproc driver (if any)

	`sudo cp /lib/modules/$(uname -r)/kernel/drivers/remoteproc/pruss_remoteproc.ko /lib/modules/$(uname -r)/kernel/drivers/remoteproc/pruss_remoteproc.ko.back`

3. cd to /drivers/remoteproc in cloned repo and run 'make' command
	
	`make`

###User Library and Examples.

1. cd to /userspace and run 'make' command. This will compile the library and examples
	
	`make`

###How to use library.

(Assumes you have followed previous steps correctly)

1. Copy pru firmware to /lib/firmware.
Firmware name for pru0 must be 'rproc-pru0-fw'
and for pru1 it must be 'rproc-pru1-fw'. (without '')

2. Load driver by using modprobe pruss_remoteproc.

3. This will load the pru firmwares and boot the remote processors.

4. cd to /userspace/examples

5. Execute ./poll ( program waits for interrupt from PRU )

6. In another terminal execute ./write