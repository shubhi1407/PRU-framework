#include <stdio.h>
#include "libpru.h"
void mycallback(int p)
{
    printf("Callback executed\n");
    return;
}

int main(void)
{
	printf("waiting for event....\n");
	hostevt_poll(EVENTOUT1,mycallback);
	
	/* Shutdown PRU core */
	pruss_shutdown(PRU0);
	return 0;
}

