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
#include <sys/stat.h>
#include <sys/types.h>

extern int errno ;
/* Global Variables */
const char *pru0 = "4a334000.pru0";
const char *pru1 = "4a338000.pru1";
const char *rproc_unbind = "/sys/bus/platform/drivers/pru-rproc/unbind";
const char *rproc_bind = "/sys/bus/platform/drivers/pru-rproc/bind";
const char *fw0 = "/lib/firmware/rproc-pru0-fw";
const char *fw1 = "/lib/firmware/rproc-pru1-fw";
	
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

/* Return TRUE if PRU core is running and FALSE if powered down */
bool check_device_status(int pru_num)
{
	const char *pru0_uevent = "/sys/bus/platform/devices/4a334000.pru0/uevent";
	const char *pru1_uevent = "/sys/bus/platform/devices/4a338000.pru1/uevent"; 
  	const char *filename;
  	const char *cmp_string = "DRIVER=pru-rproc";
  	
  	filename=(pru_num==0)?pru0_uevent:pru1_uevent;
  	
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

/* Wait for interrupt from PRU */
int hostevt_poll(int hostevt, void (*callback)(int))
{
	char firstread[1];
	char data[2];
	int fd = open("/sys/devices/ocp.3/4a300000.pruss/hostevt", O_RDWR);

	if(!check_device_status(0) && !check_device_status(1))
		return -1;

	if(hostevt < 0 || hostevt > MAXEVENTOUT) 
		return -EINVAL;
		
	/* Have to perform a dummy read for poll to work */
	read( fd, firstread, 1 ); 
	lseek(fd,0,0);

	/* Send host event number we are waiting for to kernel */
	write(fd,&hostevt,sizeof(int)); 
	
	if (fd != -1) {

		struct pollfd host_poll = { .fd = fd, .events = POLLPRI|POLLERR };
	    
	    int rv = poll(&host_poll, 1, TIMEOUT);    /* block until timeout */
	    if (rv > 0) {
	        if (host_poll.revents & POLLPRI) {
	        	int n = pread(host_poll.fd, data, sizeof(data),0);
	        	if(n>0) {   /*IRQ happened*/
	        		hostevt = atoi(data);
	        		//printf("irq recieved: %d\n",hostevt);
	        		callback(hostevt);
	        		close(fd);
	        		return 0;
	          	}	
	        }
	    }
	    /* Timeout occured */
	    else {
	    	return -ETIME;
	    }
	}
	close(fd);
	return -EACCES;
}

/* Shutdown the PRU core */
int pruss_shutdown(int pru_num)
{	
		
	if(pru_num == PRU0 || pru_num == PRU1) {
		
		if(!check_device_status(pru_num)) {
			//fprintf(stderr,"PRU%d already powered down\n",pru_num);
			return -1;
		}

		/*Shutdown pru*/
		FILE *fp;
   		fp = fopen(rproc_unbind, "w");
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
	else {
		return -1;
	}
	return 0;
}

int pruss_boot(char *fwname,int pru_num)
{

	int err;
	const char *fw;
	FILE *fp;
	struct stat file_info;
	
	/* If pru core is already booted shut it down */
	if(check_device_status(pru_num))
		pruss_shutdown(pru_num);

	fw = (pru_num==0)?fw0:fw1;	

	/* Check if previous symlink exist. If yes, remove them */
	lstat(fw,&file_info);
	if(S_ISLNK(file_info.st_mode))
		remove(fw);

	/* Create symlink between user fw path and /lib/firware/rproc-pru01-fw */
	err = symlink(fwname,fw);
	if(err) {
		return err;
	}
		

	/* Finally boot pru core */
	fp = fopen(rproc_bind, "w");
   	if(fp!=NULL) {
   		
   		if(pru_num == PRU0)
			fputs(pru0,fp);
		else
			fputs(pru1,fp);
		fclose(fp);
   	}
   	else {
   		return -EACCES;
   	}

	return 0;
}



