#include <stdio.h>
#include "libpru.h"
void mycallback(int p)
{
    printf("Callback executed\n");
    return;
}

int main(void)
{
	
	hostevt_poll(EVENTOUT1,mycallback);
	
	return 0;
}

