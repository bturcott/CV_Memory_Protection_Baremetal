#include <stdio.h>
#include <inttypes.h>
#include "socal/hps.h"
#include "socal/socal.h"
#include "socal/alt_sdr.h"
#include "socal/alt_rstmgr.h" 
#include "test_trustzone.h"

//Curently not in use
#include "alt_generalpurpose_io.h"
#include "alt_globaltmr.h"
#include "alt_clock_manager.h"

#include "main.h"

#define L3_RESTRICTED_ADDRESS	0x14FFFFFF
#define HPS_RESTRICTED_ADDRESS	0x12F81111

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

//Pointer to Bridge RSTMGR
static volatile ALT_RSTMGR_BRGMODRST_t *rstmgr = (ALT_RSTMGR_BRGMODRST_t *)(RSTMGR_BRGMODRST_ADDRESS);

//Pointers to restricted memory space
static volatile int * l3_only_pointer = (int *)(L3_RESTRICTED_ADDRESS);
static volatile int * hps_only_pointer = (int *)(HPS_RESTRICTED_ADDRESS);

//Rule 0 configuration
static volatile uint32_t rule_0_sdram_start = RULE_0_START_ADDRESS;
static volatile uint32_t rule_0_sdram_end = RULE_0_END_ADDRESS;
static volatile uint32_t rule_0_port_mask = RULE_0_PORT_MASK;

//Rule 1 configuration
static volatile uint32_t rule_1_sdram_start = RULE_1_START_ADDRESS;
static volatile uint32_t rule_1_sdram_end = RULE_1_END_ADDRESS;
static volatile uint32_t rule_1_port_mask = RULE_1_PORT_MASK;

//Misc variables
volatile int error_flag;

//Function prototypes
int mem_test(void);
void delay(int seconds);

int main()
{
	printf("\r\nWelcome to the memory protection rule test program!\r\n");
	printf("The program must go through some initialization steps before any tests can be run.\r\n");	
	printf("The fpga2hps bridge is being enabled...\r\n");
	error_flag = bridge_config();
	if (error_flag == 1)
	{
		printf("Bridge was not enabled correctly\r\n");
		printf("Please reset the device and try again\r\n");
		while(1);
	}
	if (error_flag == 0)
	{
		printf("The bridge has been enabled correctly\r\n");
	}
	printf("Setting up the memory protection rules..\r\n");	
	
	//Set protection rules	
	tz_set_configuration(rule_0_sdram_start, rule_0_sdram_end, rule_0_port_mask);
	tz_set_configuration(rule_1_sdram_start, rule_1_sdram_end, rule_1_port_mask);
	
	//Display protection rules
	//tz_display_configuration();
	printf("Memory protection rules have been configured properly.\r\n");
	printf("-----------------------------------------------\r\n");
	printf("|HPS Access Range:     0x00000000 - 0x1FFFFFFF|\r\n");
	printf("|L3 Access Range:      0x13F80000 - 0x3FF00000|\r\n");
	printf("|Shared Access Range:  0x13F80000 - 0x1FFFFFFF|\r\n");	
	printf("-----------------------------------------------\r\n");
	
	//Three cases exist 
	// Case 1 - write/read from a shared memory space (Base case)
	// Case 2 - Try and write/read to a restricted memory space (FPGA to HPS)
	// Case 3 - Try and write/read to a restricted memory space (HPS to FPGA)
	volatile int user_input = 0;

	printf("In order to interact with this program you must use the utilize the System Console Toolkit.\r\n");
	printf("Change the value at address location %p via the System Console Toolkit to select a test you wish to perform.\r\n",&user_input);
	printf("A list of options is illustrated below\r\n");
	printf("|-----------------------------------------------------------------------------|\r\n");
	printf("|Please enter a '1' at %p to try and read/write from the hps only memory space|\r\n",&user_input);
	printf("|Please enter a '2' at %p to try and read from the L3 only memory space       |\r\n",&user_input);	
	printf("|Please enter a '3' at %p to try and write to the L3 only memory space        |\r\n",&user_input);
	printf("|-----------------------------------------------------------------------------|\r\n");		
	//mem_test();
	while(1)
	{
		printf("Main menu\r\n");		
		delay(1);		
		if (user_input == 1)
		{
			printf("You have selected case 1 - read/write from the hps only memory space.\r\n");
			*hps_only_pointer = 7;
			while (user_input == 1)
			{				
				printf("The value %d has been written at address location %p\r\n",*hps_only_pointer,hps_only_pointer);
				printf("Try to update that value via System Console - a successful change will be printed out here\r\n");
				printf("Change the value at address location %p to return the main menu.\r\n",&user_input);		
				delay(1);
			}
		}
		else if (user_input == 2)
		{
			printf("You have selected case 2 - read from the L3 only memory space.\r\n");
			printf("Please write a value to the address location %p via the System Console Toolkit.\r\n",l3_only_pointer);
			printf("The program will continually try to read the data at that memory address.\r\n");
			while (user_input == 2)
			{
				printf("The value at memory location %p is %d.\r\n",l3_only_pointer,*l3_only_pointer);
				printf("Change the value at address location %p to return the main menu.\r\n",&user_input);	
				delay(1);
			}
		}
		else if (user_input == 3)
		{
			printf("You have selected case 3 - write to the L3 only memory space.\r\n");
			printf("The program will now try and write a '7' to the L3 only memory space at address location %p.\r\n",l3_only_pointer);
			*l3_only_pointer = 7;
			while (user_input == 3)
			{
				printf("Please confirm the value '7' has not been written to address location %p via the System Console Toolkit.\r\n",l3_only_pointer);
				printf("The value at memory location %p is %d.\r\n",l3_only_pointer, *l3_only_pointer);
				printf("Change the value at address location %p to return the main menu.\r\n",&user_input);	
				delay(1);
			}
			
		}
	}
	
	return 0;
}

int bridge_config(void)
{
	rstmgr->fpga2hps = 0x0;	//Deasserts the fpga2hps reset
	if (rstmgr->fpga2hps == 0)  //Checks to ensure the value was written properly
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int mem_test(void)
{

	volatile int integer_0 = 5;
	volatile int integer_1 = 2;
	volatile int integer_2 = 7;
	volatile int result;
	volatile int *int_pointer_0 = &integer_0;
	volatile int *int_pointer_1 = &integer_1;	
	volatile int *int_pointer_2 = &integer_2;
	result = integer_0 + integer_1 + integer_2;

	printf("** The address location of the integer 5 is: %p **\r\n",int_pointer_0);
	printf("** The address location of the integer 7 is: %p **\r\n",int_pointer_2);	
	printf("** The address location of the integer 2 is: %p **\r\n",int_pointer_1);	
	printf("** The address location of the result is: %p **\r\n",&result);
	printf("** The value of the result is: %d **\r\n",result);
	return 0;
}

void delay(int seconds)
{
	int i, j, k;
	for (i = 0; i < seconds; i++)
	{
		for(j = 0; j < 10000; j++)
		{
			for(k = 0; k < 600; k++)
			{
				int volatile t = 120 * j * i + k;
				t = t +5;
			}
		}
	}
}
