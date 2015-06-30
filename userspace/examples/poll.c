#include <stdio.h>
#include <unistd.h>
#include "libpru.h"
void mycallback(int p)
{
    printf("Callback executed\n");
    return;
}

int main(void)
{	
	int err;
	char *fw = "/lib/rproc-pru0-fw";
	
	/* Boot PRU 0 */
	err = pruss_boot(fw,PRU0);
	if(err)
		return err;
	
	printf("waiting for event....\n");
	err = hostevt_poll(EVENTOUT1,mycallback);
	if(err)
		return err;		

	/* Shutdown PRU core */
	err = pruss_shutdown(PRU0);
	if(err)
		return err;

	return 0;
}

