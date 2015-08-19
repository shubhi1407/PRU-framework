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

###Library functions

`int pruss_write(unsigned const int mem_name, int wordoffset, int *data, size_t bytelength)` 

Writes buffer pointed at by 'data' pointer to PRU memory.  
@mem_name : can take  PRU_DRAM0 / PRU_DRAM1 / PRU_SHRAM as values  
@wordoffset : offset within specified memory type.  
@bytelength : length of data to be copied (in bytes).  

Returns 0 on success.  
----

`int pruss_read(unsigned const int mem_name, int wordoffset, int *data, size_t bytelength)` 

Reads into 'data' pointer from PRU memory.  
@mem_name : can take  PRU_DRAM0 / PRU_DRAM1 / PRU_SHRAM as values  
@wordoffset : offset within specified memory type.  
@bytelength : length of data to be copied (in bytes).  

Returns 0 on success.  
----

`int pruss_interrupt(int sysevent)`  

Send user provided sysevent to PRU INTC.  

@sysevent : Sysevent number ( 0 – 63 )  

Returns 0 on success.  
----

`bool check_device_status(int pru_num)` 

@pru_num : PRU core id ( 0 or 1 )  

Returns true if specified core is powered up. Otherwise false.  
----

`int hostevt_poll( int hostevt, void (*callback)(int) )` 

This function can either be blocking or non-blocking depending on timeout provided by user ( in libpru.h )  

@hostevt : EVTOUT0 – EVTOUT7  
@callback : user provided callback function. Host event int is argument. No return.  
@TIMEOUT (in libpru.h): -1 for indefinite block  
						>0 wait time before releasing poll  
----

`int pruss_boot(char *fwname,int pru_num)`

Boots the PRU core.  

@fwname : path to PRU firmware  
@pru_num: PRU0 / PRU1  

Returns 0 on success
----

`int pruss_shutdown(int pru_num)`

Shutdown PRU core.

@pru_num: PRU0 / PRU1  

Returns 0 on success
