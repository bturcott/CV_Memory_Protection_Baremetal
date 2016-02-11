#ifndef MAIN_H
#define MAIN_H

typedef struct
{
	unsigned int hps2fpga_val;
	unsigned int lwhps2fpga_val;
	unsigned int fpga2hps_val;
} RSTMGR_BRGMODRST_T;

int bridge_config(void);

#endif
