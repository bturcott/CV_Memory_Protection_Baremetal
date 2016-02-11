#ifndef TEST_TRUSTZONE_H
#define TEST_TRUSTZONE_H

typedef struct
{
	unsigned int lo_addr;
	unsigned int hi_addr;
	unsigned int lo_id;
	unsigned int hi_id;
	unsigned int security;
	unsigned int portmask;
	unsigned int result;
	unsigned int valid;
} TZ_RULE_T;


void tz_display_configuration(void);
void tz_get_rule(unsigned int ruleno, TZ_RULE_T * rule);
void tz_set_rule(unsigned int ruleno, TZ_RULE_T * rule);
void tz_set_configuration(uint32_t sdram_start, uint32_t sdram_end, uint32_t port_mask);

#endif
