# * Makefile for PRU firmware
# * Builds only with CLPRU 2.0.0B* and above
# BBB hostname
BBHOST = root@192.168.7.2

# Tools to be used
CC=clpru
LD=lnkpru

# Default installation location for TI code generation tools (TI-CGT PRU)
# Change if you have installed at some other location
CGTDIR = /usr/share/ti/cgt-pru

# PRU software support package by TI
SWDIR = /usr/share/pru-software-support-package

INCLUDEDIR = -I$(SWDIR)/include -I$(CGTDIR)/include -I$(CGTDIR)/lib

# Compiler Options
# -v3				PRU version 3
# --c99 			C99 support
# --gcc 			Enable GCC extensions
# -O3				Optimization level maximum
# --printf_support=minimal 	Minimal printf
# -ppd				Generate dependencies *.pp
# -ppa				Continue after generating deps
# -DDEBUG			Enable debug
CFLAGS= -v3 -s -al -O3 --c99 --gcc --printf_support=minimal --symdebug:none $(INCLUDEDIR)

# Linker Options
# -cr 				Link using RAM auto init model (loader assisted)
# -x				Reread libs until no unresolved symbols found
LDFLAGS=-cr --diag_warning=225 -lam335x_pru.cmd -x 

.PHONY: all clean

all: rproc-pru0-fw  rproc-pru1-fw

%.obj: %.c
	@echo "  CC	$@"
	@$(CC) $(CFLAGS) -c $< -ea=.s

rproc-pru0-fw: rproc-pru0-fw.obj
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@
	
rproc-pru1-fw: rproc-pru1-fw.obj 
	@echo "  LD	$@"
	@$(CC) $(CFLAGS) $^ -q -z $(LDFLAGS) -o $@
	
install-frombb: rproc-pru0-fw rproc-pru1-fw-pru1
	cp -t /lib/firmware rproc-pru0-fw
	cp -t /lib/firmware rproc-pru-fw

install-tobb: rproc-pru0-fw rproc-pru1-fw
	scp -q rproc-pru0-fw $(BBHOST):/lib/firmware
	scp -q rproc-pru1-fw $(BBHOST):/lib/firmware

clean:
	rm -f *.obj *.lst *.s rproc-pru0-fw rproc-pru1-fw

