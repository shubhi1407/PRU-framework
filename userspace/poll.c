#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <poll.h>
#define TIMEOUT -1
enum pruss_mem {
	EVENTOUT0 = 0,
	EVENTOUT1,
	EVENTOUT2,
	EVENTOUT3,
	EVENTOUT4,
	EVENTOUT5,
	EVENTOUT6,
	EVENTOUT7,
	MAXEVENTOUT,
};

void hostevt_poll(int hostevt, void (*callback)(int))
{
	if(hostevt < 0 || hostevt > MAXEVENTOUT) {
		printf("Incorrect host event number\n");
		return;
	}
	int cnt;
	char firstread[1];
	char data[2];
	int fd = open("/sys/devices/ocp.3/4a300000.pruss/hostevt", O_RDWR);
	/* Have to perform a dummy read for poll to work */
	cnt = read( fd, firstread, 1 ); 
	lseek(fd,0,0);
	/* Send host event number we are waiting for to kernel */
	write(fd,&hostevt,sizeof(int)); 
	
	if (-1 != fd) {
		struct pollfd host_poll = { .fd = fd, .events = POLLPRI|POLLERR };
	    int rv = poll(&host_poll, 1, TIMEOUT);    /* block endlessly */
	    if (rv > 0) {
	        if (host_poll.revents & POLLPRI) {
	        	int n = pread(host_poll.fd, data, sizeof(data),0);
	        	if(n>0) {   /*IRQ happened*/
	        		hostevt = atoi(data);
	        		printf("irq recieved: %d\n",hostevt);
	        		callback(hostevt);
	          	}	
	        }
	    }
	    else {
	    	printf("Timeout\n");
	    }
	}
	close(fd);
}
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

