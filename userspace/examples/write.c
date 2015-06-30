#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <poll.h>
#include "libpru.h"
#define PRU_DRAM0 0
#define PRU_DRAM1 1
#define PRU_SHRAM 2

int main(void)
{
	int i=0;
	int data[2]={40,33};
	int read[1]={0};
	int err;

	err = pruss_boot("/lib/rproc-pru1-fw",PRU1);
	if(err)
		return err;

	err = pruss_write(PRU_DRAM1,0,data,sizeof(data));
	if(err){
		perror("Write error");
		return err;
	}
	
	err = pruss_interrupt(16);
	if(err){
		perror("Interrupt error");
		return err;
	}

	err = pruss_read(PRU_DRAM1,2,read,sizeof(read));
	if(err){
		perror("Read error");
		return err;
	}
	
	printf("Data read\n");
	for(i=0;i<sizeof(read)/sizeof(int);i++)
		printf("%d ", *(read+i));
	printf("\n");
	
	err = pruss_shutdown(PRU1);
	if(err)
		return err;

	return 0;
}

