#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include "libpru.h"
#include <stdbool.h>
#include <errno.h>

extern int errno ;

int pruss_write(unsigned const int mem_name, int wordoffset, int *data, size_t bytelength)
{
	int i=0;
	char *memtype = "/sys/devices/ocp.3/4a300000.pruss/memtype";
	char *offset = "/sys/devices/ocp.3/4a300000.pruss/offset";
	char *data_file = "/sys/devices/ocp.3/4a300000.pruss/datafile";
	int fd[3] = {open(memtype,  O_WRONLY,  S_IWUSR),
				 open(offset,  O_WRONLY,  S_IWUSR),
				 open(data_file,  O_WRONLY,  S_IWUSR) };
	
	/*
	printf("memory name:%d word offset:%d \ndata size:%zu\nnumber of elements %d\n",
		mem_name,wordoffset,bytelength,bytelength/sizeof(int));
	printf("-------------------------------------------\n");
	*/
	if(fd[0] && fd[1] && fd[2]) {
		write(fd[0], &mem_name, sizeof(mem_name));
		write(fd[1], &wordoffset, sizeof(wordoffset));
		write(fd[2], data, bytelength);

		for(i=0;i<3;i++)
			close(fd[i]);		
	}
	else{
		return -EACCES;
	}
	
	return 0;
}

int pruss_read(unsigned const int mem_name, int wordoffset, int *data, size_t bytelength)
{
	int i=0;
	char *memtype = "/sys/devices/ocp.3/4a300000.pruss/memtype";
	char *offset = "/sys/devices/ocp.3/4a300000.pruss/offset";
	char *size = "/sys/devices/ocp.3/4a300000.pruss/size";
	char *data_file = "/sys/devices/ocp.3/4a300000.pruss/datafile";

	int fd[4] = {open(memtype,  O_WRONLY,  S_IWUSR),
				 open(offset,  O_WRONLY,  S_IWUSR),
				 open(size,  O_WRONLY,  S_IWUSR),
				 open(data_file,  O_RDWR,  S_IRWXU) };
	
	if(fd[0] && fd[1] && fd[2] && fd[3]) {
		write(fd[0], &mem_name, sizeof(mem_name));
		write(fd[1], &wordoffset, sizeof(wordoffset));
		write(fd[2], &bytelength, sizeof(int));
		read(fd[3], data, bytelength);
	
		for(i=0;i<4;i++)
			close(fd[i]);
	}
	else {
		return -EACCES;
	}

	return 0;
}

/* Send event (0-63) to PRU */
int pruss_interrupt(int sysevent)
{
	char *sysevt = "/sys/devices/ocp.3/4a300000.pruss/sysevt";
	int fd= open(sysevt, O_WRONLY, S_IWUSR);
	if(fd) {
		write(fd,&sysevent,sizeof(int));
		close(fd);	
	}
	else
		return -EACCES;

	return 0;
}

/* Wait for interrupt from PRU */
void hostevt_poll(int hostevt, void (*callback)(int))
{
	if(hostevt < 0 || hostevt > MAXEVENTOUT) {
		printf("Incorrect host event number\n");
		return;
	}
	//int cnt;
	char firstread[1];
	char data[2];
	int fd = open("/sys/devices/ocp.3/4a300000.pruss/hostevt", O_RDWR);
	/* Have to perform a dummy read for poll to work */
	read( fd, firstread, 1 ); 
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
	        		return;
	          	}	
	        }
	    }
	    else {
	    	printf("Timeout\n");
	    }
	}
	close(fd);
}

/* Return TRUE if PRU core is running and FALSE if powered down */
bool check_device_status(int pru_num)
{
	const char *pru0 = "/sys/bus/platform/devices/4a334000.pru0/uevent";
	const char *pru1 = "/sys/bus/platform/devices/4a338000.pru1/uevent"; 
  	const char *filename;
  	const char *cmp_string = "DRIVER=pru-rproc";
  	
  	filename=(pru_num==0)?pru0:pru1;
  	
  	FILE *file = fopen (filename, "r");
  	if (file != NULL) {
    	char line [50];
    	fgets(line,sizeof line,file);  
      	
      	if(strncmp(line,cmp_string,sizeof(cmp_string)/sizeof(char)))
      		return false;

      	/* PRU core is currently running. return TRUE*/
		fclose(file);
    	return true;
    }
    else {
    	return -EACCES;
  	}
    
}

/* Shutdown the PRU core */
int pruss_shutdown(int pru_num)
{	
	const char *pru0="4a334000.pru0";
	const char *pru1="4a338000.pru1";
	const char *filename="/sys/bus/platform/drivers/pru-rproc/unbind";	
	if(pru_num == PRU0 || pru_num == PRU1) {
		
		if(!check_device_status(pru_num)) {
			//fprintf(stderr,"PRU%d already powered down\n",pru_num);
			return -1;
		}

		/*Shutdown pru*/
		FILE *fp;
   		fp = fopen(filename, "w");
   		if(fp!=NULL) {
   			if(pru_num == PRU0)
				fputs(pru0,fp);
			else
				fputs(pru1,fp);
		fclose(fp);
   		}
   		else{
   			return -EACCES;
   		}
		
	}
	else{
		return -1;
	}

	return 0;
}



