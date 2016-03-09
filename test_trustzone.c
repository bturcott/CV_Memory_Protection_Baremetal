#include <stdio.h>
#include <inttypes.h>
#include "socal/hps.h"
#include "socal/socal.h"
#include "socal/alt_sdr.h"
#include "test_trustzone.h"

static volatile ALT_SDR_CTL_raw_t *sdr = (ALT_SDR_CTL_raw_t *)(ALT_SDR_OFST + ALT_SDR_CTL_OFST);
volatile int rule_count = 0;

#define TZ_ALL_PORTS 0x3FF
#define TZ_ALL_IDS   0xFFF

void tz_display_configuration(void)
{
	TZ_RULE_T rule;

	printf("SDRAM Protection Rules:\r\n");
	printf("  sdr->protportdefault = 0x%08x\r\n", (unsigned int)sdr->protportdefault);

	for(int i=0; i<20; i++)
	{
		tz_get_rule(i, &rule);
		if(!rule.valid)
		{
		}
		else
		{
			printf("  Valid rule %d: \r\n", i);
			printf("    lo_addr  = 0x%08x (~%dMB)\r\n", rule.lo_addr, rule.lo_addr >> 20);
			printf("    hi_addr  = 0x%08x (~%dMB)\r\n", rule.hi_addr, (rule.hi_addr >> 20) + 1);
			printf("    lo_id    = 0x%03x\r\n", rule.lo_id);
			printf("    hi_id    = 0x%03x\r\n", rule.hi_id);
			printf("    security = 0x%01x\r\n", rule.security);
			printf("    portmask = 0x%03x\r\n", rule.portmask);
			printf("    result   = 0x%01x\r\n", rule.result);
			//printf("   [protruleaddr] = 0x%08x\r\n", (unsigned int) sdr->protruleaddr);
			//printf("   [protruleid]   = 0x%08x\r\n", (unsigned int) sdr->protruleid);
			//printf("   [protruledata] = 0x%08x\r\n", (unsigned int) sdr->protruledata);
		}
	}
}

void tz_set_rule(unsigned int ruleno, TZ_RULE_T * rule)
{
	// select the rule
	sdr->protrulerdwr = ruleno;

	// obtain the address bits
	uint32_t lo_addr_bits = (rule->lo_addr >> 20) & 0xFFF;
	uint32_t hi_addr_bits = (rule->hi_addr >> 20) & 0xFFF;

	// set rule addresses
	sdr->protruleaddr = lo_addr_bits | (hi_addr_bits << 12);

	// set rule IDs
	sdr->protruleid = rule->lo_id | (rule->hi_id << 12);

	// set rule data
	sdr->protruledata = rule->security | (rule->valid << 2) | (rule->portmask << 3) | (rule->result << 13);

	// write the rule
	sdr->protrulerdwr = ruleno | (1L << 5);
    
	// sdr->protrulerdwr = 0;
}

void tz_get_rule(unsigned int ruleno, TZ_RULE_T * rule)
{
	// read the rule
	sdr->protrulerdwr = ruleno;
	sdr->protrulerdwr = ruleno | (1L << 6);

	// get the addresses
	rule->lo_addr = (sdr->protruleaddr & 0xFFF) << 20;
	rule->hi_addr = ((sdr->protruleaddr >> 12) & 0xFFF) << 20;

	// get IDs
	rule->lo_id = sdr->protruleid & 0xFFF;
	rule->hi_id = (sdr->protruleid >> 12) & 0xFFF;

	// get data
	uint32_t data = sdr->protruledata;
	//printf("[data=0x%08x]", (int)data);
	rule->security = data & 0x3;
	rule->valid = (data >> 2) & 0x1;
	rule->portmask = (data >> 3) & 0x3FF;
	rule->result = (data >> 13) & 0x1;
}

void tz_set_configuration(uint32_t sdram_start, uint32_t sdram_end, uint32_t port_mask, uint32_t rule_number)
{
	TZ_RULE_T rule;

	// default rule: reject everything
	sdr->protportdefault = 0x3ff;

	// new rule: accept SDRAM
	rule.lo_addr = sdram_start;
	rule.hi_addr = sdram_end;
	rule.lo_id = 0x0;
	rule.hi_id = 0xFFF; //Set all high to allow all AxIDs
	rule.portmask = port_mask; //0x3FF applies the rule to all ports
	rule.security = 0x3; // Applies to secure and non-secure transactions
	rule.result = 0; //Allows the access to succeed
	rule.valid = 1; //Sets the rule as valid

	// set new rule
	tz_set_rule(rule_number, &rule);
}
