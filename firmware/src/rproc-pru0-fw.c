/*
 * Copyright (c) 2014, Texas Instruments, Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

 /*As of now this program does nothing other than sitting in an endless loop*/

#include <stdint.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include "resource_table_0.h"
#include <pru_vring.h>
// PRU Core 1
#define PRU1

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Mapping Constant table register to variable */
volatile pruCfg CT_CFG __attribute__((cregister("PRU_CFG", near), peripheral));
volatile far pruIntc CT_INTC __attribute__((cregister("PRU_INTC", far), peripheral));

/* Defines */
#define PRU0

/* PRU0-to-PRU1 interrupt */
#define PRU0_PRU1_EVT		(16)
#define PRU0_PRU1_TRIGGER	(__R31 = (PRU0_PRU1_EVT - 16) | (1 << 5))


void main(){
	
	/* Configure GPI and GPO as Mode 0 (Direct Connect) */
	CT_CFG.GPCFG0 = 0x0000;

	/* Clear GPO pins */
	__R30 |= 0xFFFFFFFF;

	/* Configure INTC. From kernel driver */
	//configIntc();

	while(1){
		/* Loop endlessly for now */
		__delay_cycles(5);
	}

	/* Halt the PRU core - shouldn't get here */
	//__halt();
}

