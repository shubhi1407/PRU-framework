#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <poll.h>

#define PRU_DRAM0 0
#define PRU_DRAM1 1
#define PRU_SHRAM 2

int pruss_write(unsigned const int mem_name, int wordoffset, const unsigned int *data, size_t bytelength)
{
	int i=0;
	char *memtype = "/sys/devices/ocp.3/4a300000.pruss/memtype";
	char *offset = "/sys/devices/ocp.3/4a300000.pruss/offset";
	char *data_file = "/sys/devices/ocp.3/4a300000.pruss/datafile";
	int fd[3] = {open(memtype,  O_WRONLY,  S_IWUSR),
				 open(offset,  O_WRONLY,  S_IWUSR),
				 open(data_file,  O_WRONLY,  S_IWUSR) };
	if(!fd[0] || !fd[1] || !fd[2]) {
		printf("Error: Failed to open sysfs\n");
		return -1;
	}

	printf("memory name:%d word offset:%d data:%d size:%zu\nnumber of elements %d address %x\n",
		mem_name,wordoffset,*data,bytelength,bytelength/sizeof(int),(unsigned)data);
	
	write(fd[0], &mem_name, sizeof(mem_name));
	write(fd[1], &wordoffset, sizeof(wordoffset));
	write(fd[2], data, bytelength);

	for(i=0;i<3;i++)
		close(fd[i]);
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
	if(!fd[0] || !fd[1] || !fd[2] || !fd[3]) {
		printf("Error: Failed to open sysfs\n");
		return -1;
	}
	printf("size to be read %zu\n",bytelength);
	write(fd[0], &mem_name, sizeof(mem_name));
	write(fd[1], &wordoffset, sizeof(wordoffset));
	write(fd[2], &bytelength, sizeof(int));
	read(fd[3], data, bytelength);
	printf("size is:%zu\n",bytelength);
	for(i=0;i<4;i++)
		close(fd[i]);
}
void interrupt(int sysevent)
{
	char *sysevt = "/sys/devices/ocp.3/4a300000.pruss/sysevt";
	int fd= open(sysevt, O_WRONLY, S_IWUSR);
	write(fd,&sysevent,sizeof(int));
	close(fd);
}

int main(void)
{
	int i=0;
	int p[6]={102,-4,2,457,4325,757};
	int read[6]={0};
	//pruss_write(PRU_DRAM0,0,&i,sizeof(i));
	pruss_write(PRU_DRAM0,0,p,sizeof(p));
	pruss_read(PRU_DRAM0,0,read,sizeof(read));
	interrupt(16);
	//hostevt_poll();
	for(i=0;i<6;i++)
		printf("%d ", *(read+i));
	printf("\n");
	
	return 0;
}

