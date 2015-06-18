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
	int p[6]={102,-4,2,457,4325,757};
	int read[6]={0};
	//pruss_write(PRU_DRAM0,0,&i,sizeof(i));
	pruss_write(PRU_DRAM0,0,p,sizeof(p));
	pruss_read(PRU_DRAM0,0,read,sizeof(read));
	pruss_interrupt(16);
	//hostevt_poll();
	for(i=0;i<6;i++)
		printf("%d ", *(read+i));
	printf("\n");
	
	return 0;
}

