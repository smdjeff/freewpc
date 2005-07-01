
#include <freewpc.h>


__fastram__ struct {
	uint8_t high;
	uint8_t low;
	uint8_t flash1;
	uint8_t flash2;
	uint8_t aux1;
	uint8_t aux2;
	uint8_t cksum;
} sol_state;


uint8_t *pulse_queue_head;
uint8_t *pulse_queue_tail;
uint8_t pulse_queue[8];


uint8_t sol_calc_cksum (void)
{
	return 0;
}


void sol_update_cksum (void)
{
	sol_state.cksum = sol_calc_cksum ();
}


int sol_verify_cksum (void)
{
	uint8_t cksum = sol_calc_cksum ();
	return (cksum == sol_state.cksum);
}


void sol_rtt (void)
{
	/* TODO : add cksum verify here */
	*(uint8_t *)WPC_SOL_LOWPOWER_OUTPUT = sol_state.low;
	*(uint8_t *)WPC_SOL_HIGHPOWER_OUTPUT = sol_state.high;
	*(uint8_t *)WPC_SOL_FLASH1_OUTPUT = sol_state.flash1;
	*(uint8_t *)WPC_SOL_FLASH2_OUTPUT = sol_state.flash2;
	*(uint8_t *)WPC_EXTBOARD1 = sol_state.aux2;
}


void sol_on (solnum_t sol)
{
	register bitset p = (bitset)&sol_state;
	register uint8_t v = sol;
	__setbit(p, v);
	sol_update_cksum ();
}

void sol_off (solnum_t sol)
{
	register bitset p = (bitset)&sol_state;
	register uint8_t v = sol;
	__clearbit(p, v);
	sol_update_cksum ();
}


void sol_serve (void)
{
	sol_on (SOL_BALL_SERVE);
	task_sleep (TIME_33MS);
	sol_off (SOL_BALL_SERVE);
}


void sol_init (void)
{
	memset (&sol_state, 0, sizeof (sol_state));
	sol_update_cksum ();
}

