#ifndef MAIN_H
#define MAIN_H

#define L3_RESTRICTED_ADDRESS			0x2FFFFFF4
#define HPS_RESTRICTED_ADDRESS			0x12F81114
#define SHARED_ADDRESS_INPUT			0x1FFFFFF4
#define SHARED_ADDRESS_ADDRESS			0x1FFFFFE4
#define SHARED_ADDRESS_DATA				0x1FFFFFD4

//Settings for Rule 0
#define RULE_0_START_ADDRESS		0x0 //Base address of zero
#define RULE_0_END_ADDRESS 		0x1FF80000 // ~512MB of CPU Memory Space
#define RULE_0_PORT_MASK		0x280 //CPU Access Only
 
//Settings for Rule 1
#define RULE_1_START_ADDRESS 		0x13F80000 //Overlap from 0x13F80000 to 0x1FF80000 ~192MB of shared memory space
#define RULE_1_END_ADDRESS 		0x3FF00000 //(Max address for 1GB of SDRAM)
#define RULE_1_PORT_MASK  		0x140 //L3 Access Only

//Address for RSTMGR necessary to activate F2H bridge
#define SOCFPGA_RSTMGR_ADDRESS 		0xFFD05000
#define RSTMGR_BRGMODRST_ADDRESS	(SOCFPGA_RSTMGR_ADDRESS + 0x01C)

//Required for Interrupt Vector
#define ARM_VECTOR_TABLE_SIZE  128

typedef struct
{
	unsigned int hps2fpga_val;
	unsigned int lwhps2fpga_val;
	unsigned int fpga2hps_val;
} RSTMGR_BRGMODRST_T;

ALT_STATUS_CODE f2h_bridge_enable(void);
void read(void);
void write(void);
void rule_test(void);
#endif
