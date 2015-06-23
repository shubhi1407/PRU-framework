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

4. Install the compiled module

	`make install`

###PRU firmware

1. Install TI-PRU Code generation tool from this [link](http://software-dl.ti.com/codegen/non-esd/downloads/download.htm#PRU)

2. Clone TI's PRU software package library from this [link](https://git.ti.com/pru-software-support-package)
	to any suitable directory
	Example:
	`cd /usr/share`
	`git clone git://git.ti.com/pru-software-support-package/pru-software-support-package.git`

3. Edit /firmware/Makefile variable SWDIR to the directory in which you cloned repo in Step 2 (skip this step if you
	cloned in /usr/share) 

4. cd to /firmware

	`make`

5. Install firmware
	
	if above steps are performed on host PC
	`make install-tobb`

	if above steps are perfomed on BBB
	`make install-frombb` 

###User Library and Examples.

1. cd to /userspace and run 'make' command. This will compile the library and examples
	
	`make`

###How to use library.

(Assumes you have followed previous steps correctly)

1. Copy pru firmware to /lib/firmware.
Firmware name for pru0 must be 'rproc-pru0-fw'
and for pru1 it must be 'rproc-pru1-fw'. (without '  ')

2. Load driver by using modprobe pruss_remoteproc.

3. This will load the pru firmwares and boot the remote processors.

4. cd to /userspace/examples

5. Execute ./poll ( program waits for interrupt from PRU )

6. In another terminal execute ./write