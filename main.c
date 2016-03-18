#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "socal/hps.h"
#include "socal/socal.h"
#include "socal/alt_sdr.h"
#include "socal/alt_rstmgr.h"
#include "hwlib.h" 
#include "test_trustzone.h"
#include "main.h"

//Pointer to Bridge RSTMGR
static volatile ALT_RSTMGR_BRGMODRST_t *rstmgr = (ALT_RSTMGR_BRGMODRST_t *)(RSTMGR_BRGMODRST_ADDRESS);

//Pointers for UI
volatile int * user_input = (int *)(SHARED_ADDRESS_INPUT);
volatile int * user_address = (int *)(SHARED_ADDRESS_ADDRESS);
volatile int * user_data = (int *)(SHARED_ADDRESS_DATA);

//Rule 0 configuration
static volatile uint32_t rule_0_sdram_start = RULE_0_START_ADDRESS;
static volatile uint32_t rule_0_sdram_end = RULE_0_END_ADDRESS;
static volatile uint32_t rule_0_port_mask = RULE_0_PORT_MASK;
static volatile uint32_t rule_0_rule_number = 0;

//Rule 1 configuration
static volatile uint32_t rule_1_sdram_start = RULE_1_START_ADDRESS;
static volatile uint32_t rule_1_sdram_end = RULE_1_END_ADDRESS;
static volatile uint32_t rule_1_port_mask = RULE_1_PORT_MASK;
static volatile uint32_t rule_1_rule_number = 1;

//Error handling variable
volatile unsigned int exception_flag;

extern void *__cs3_interrupt_vector;

void *__arm_vector_table = (void*)0xffff0000;

int main()
{
	ALT_STATUS_CODE status = ALT_E_SUCCESS;   	

	//Disables the interrupts
	disable_interrupts();
	//Copies the exception table from SDRAM into the correct location of OCRAM
	memcpy(__arm_vector_table, &__cs3_interrupt_vector, ARM_VECTOR_TABLE_SIZE);

	printf("Welcome to the memory protection rule test program!\r\n");
	printf("The F2H bridge is being enabled...\r\n");
	 
	status = f2h_bridge_enable();
	if (status == ALT_E_SUCCESS)
	{
		printf("F2H Bridge has been enabled.\r\n");
	}	
	else 
	{
		printf("An error has occured.\r\n");
	}
	printf("Configuring the memory protection rules...\r\n");	
	
	//Set protection rules	
	tz_set_configuration(rule_0_sdram_start, rule_0_sdram_end, rule_0_port_mask, rule_0_rule_number);
	tz_set_configuration(rule_1_sdram_start, rule_1_sdram_end, rule_1_port_mask, rule_1_rule_number);

	printf("Memory protection rules have been configured properly.\r\n");
	printf("Memory Protection Table:\r\n");
	printf("-----------------------------------------------\r\n");
	printf("|HPS Access Range:     0x00000000 - 0x1FFFFFFF|\r\n");
	printf("|L3 Access Range:      0x13F80000 - 0x3FF00000|\r\n");
	printf("|Shared Access Range:  0x13F80000 - 0x1FFFFFFF|\r\n");	
	printf("-----------------------------------------------\r\n");
	printf("****************\r\n");
	printf("Memory diagram:\r\n");
	printf("----------\r\n");
	printf("| L3     |\r\n");
	printf("|--------|\r\n");
	printf("| Shared |\r\n");	
	printf("|--------|\r\n");
	printf("| HPS    |\r\n");
	printf("----------\r\n");
	printf("***************\r\n");

	while(1)
	{
		rule_test();
	}
	
	return 0;
}

ALT_STATUS_CODE f2h_bridge_enable(void)
{
	ALT_STATUS_CODE status = ALT_E_SUCCESS;	
	rstmgr->fpga2hps = 0x0;
	if (rstmgr->fpga2hps == 0)
	{
		status = ALT_E_SUCCESS;
	}
	else
	{
		status = ALT_E_ERROR;
	}
	return status;
}

void rule_test(void)
{
	//Reset values to zero	
	*user_input = 0;
	*user_data = 0;
	*user_address = 0;
	exception_flag = 0;

	printf("\r\nMain menu.\r\n");
	printf("Enter a '1' at address %p if you would like to read from an address.\r\n", user_input);
	printf("Enter a '2' at address %p if you would like to write to an address.\r\n", user_input);
	while(1)
	{	
		if (*user_input == 1)
		{
			read();
			break;
		}
		else if (*user_input == 2)
		{
			write();
			break;
		}
	}
	return;
}

void read(void)
{
	
	volatile unsigned int * read_data;

	printf("You have choosen to read from an address.\r\n");
	printf("Please enter the address you wish to read from at location %p.\r\n", user_address);

	while(1)
	{
		if(*user_address != 0)
		{		
			read_data = (unsigned int*) *user_address; 			
			printf("The value at address %p is %p.\r\n",read_data ,*read_data);
			if (exception_flag != 0)
			{
			printf("Exception flag has been thrown due to an access request to a restricted area.\r\n");
			}
			break;
		}
	}
	return;
}


void disable_interrupts(void)
{
	__asm(
			"mrs r3, CPSR\n"
			"orr r3, #(0x80 | 0x40)\n"
			"msr CPSR_c, r3\n"
			: :
			);
}

void write(void)
{
	volatile unsigned int * write_data;	
	
	printf("You have choosen to write to an address.\r\n");
	printf("Please enter the address you wish to write to at location %p.\r\n", user_address);
	printf("Please enter the data you wish to write at location %p.\r\n", user_data);

	while(1)
	{
		if(*user_data != 0 && *user_address != 0)
		{
			write_data = (unsigned int *) *user_address;
			*write_data = *user_data; //Dereferences the pointer with the value entered
			printf("You have written %p to location %p.\r\n", *write_data, write_data);
			if (exception_flag != 0)
			{
			printf("Exception flag has been thrown due to an access request to a restricted area.\r\n");
			}
			break;
		}
	}	
	return;
}

