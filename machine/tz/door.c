/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <eb.h>
/** Index of the panel which is currently slow flashing (next to
 * be awarded) or fast flashing (running) */
__local__ U8 door_index;

/** Number of door panels that have been started */
__local__ U8 door_panels_started;
extern __local__ U8 extra_ball_enable_count;
extern void award_unlit_shot (U8 unlit_called_from);
extern void reset_unlit_shots (void);

U8 door_active_lamp;

/* For testing -- only enables GREED mode */
//#define GREED_ONLY
//#define DOOR_INDEX 12
/** Total number of door panels, not counting the handle */
#define NUM_DOOR_PANELS 14

#define LITZ_DOOR_INDEX NUM_DOOR_PANELS

/** Names of all the door panels, in order */
const char *door_panel_names[] = {
	"TOWN SQUARE MADNESS",
	"LIGHT EXTRA BALL",
	"SUPER SLOT",
	"CLOCK MILLIONS",
	"THE SPIRAL",
	"BATTLE THE POWER",
	"10 MILLION",
	"GREED",
	"THE CAMERA",
	"THE HITCHHIKER",
	"CLOCK CHAOS",
	"SUPER SKILL SHOT",
	"FAST LOCK",
	"LIGHT GUMBALL",
	"RETURN TO THE ZONE",
};

const char *door_award_goals[] = {
	"JET BUMPERS",
	"LOCK LANE",
	"SLOT MACHINE",
	"CLOCK TARGET",
	"THE LOOPS",
	"RIGHT RAMP",
	"ANYTHING",
	"STANDUPS",
	"THE LEFT HOLE",
	"THE SIDE LANE",
	"CLOCK TARGET",
	"LEFT RAMP",
	"LOCK LANE",
	"RIGHT LOOP",
	"PIANO NOW",
};


void door_start_event (U8 id)
{
	switch (id)
	{
		case 0: callset_invoke (door_start_tsm); break;
		case 1: callset_invoke (door_start_eb); break;
		case 2: callset_invoke (door_start_sslot); break;
		case 3: callset_invoke (door_start_clock_millions); break;
		case 4: callset_invoke (door_start_spiral); break;
		case 5: callset_invoke (door_start_battle_power); break;
		case 6: callset_invoke (door_start_10M); break;
		case 7: callset_invoke (door_start_greed); break;
		case 8: callset_invoke (door_start_camera); break;
		case 9: callset_invoke (door_start_hitchhiker); break;
		case 10: callset_invoke (door_start_clock_chaos); break;
		case 11: callset_invoke (door_start_super_skill); break;
		case 12: callset_invoke (door_start_fast_lock); break;
		case 13: callset_invoke (door_start_light_gumball); break;
		case 14: callset_invoke (door_start_litz); break;
	}
}


extern inline lampnum_t door_get_lamp (U8 id)
{
	return lamplist_index (LAMPLIST_DOOR_PANELS_AND_HANDLE, id);
}


extern inline U8 door_get_flashing_lamp (void)
{
	return door_get_lamp (door_index);
}


void door_set_flashing (U8 id)
{
	lamp_flash_off (door_get_flashing_lamp ());
	door_index = id;
	lamp_flash_on (door_get_flashing_lamp ());
}


void door_advance_flashing (void)
{
	U8 new_door_index;

	if (door_panels_started < NUM_DOOR_PANELS)
	{
		new_door_index = door_index;
		do {
			new_door_index++;
			if (new_door_index >= NUM_DOOR_PANELS)
				new_door_index = 0;
		} while (lamp_test (door_get_lamp (new_door_index)));
	}
	else
		/* Light the door handle */
		new_door_index = LITZ_DOOR_INDEX;

	door_set_flashing (new_door_index);
}

void door_award_rotate (void)
{
	task_sleep_sec (2);
	while (in_live_game)
	{
		door_advance_flashing ();
		task_sleep_sec (2);
	}
	task_exit ();
}
/* TODO Crashes in test mode */
void door_award_deff (void)
{
	U8 index = door_index;

	kickout_lock (KLOCK_DEFF);
	//dmd_alloc_low_clean ();
	dmd_alloc_pair_clean ();
	//dmd_alloc_pair ();

	sprintf ("DOOR PANEL %d", door_panels_started);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 21, door_panel_names[index]);
	dmd_show_low ();
	sound_send (SND_NEXT_CAMERA_AWARD_SHOWN);
	task_sleep_sec (2);
	
	//dmd_alloc_pair ();
	dmd_alloc_pair_clean ();

	//dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 9, "SHOOT");
	font_render_string_center (&font_fixed6, 64, 22, door_award_goals[index]);
	dmd_sched_transition (&trans_scroll_left);
	sound_send (SND_SPIRAL_EB_LIT);
	dmd_show_low ();
	task_sleep_sec (1);
	kickout_unlock (KLOCK_DEFF);
	task_sleep_sec (1);
	deff_exit ();
}

void litz_award_deff (void)
{
	//dmd_alloc_low_clean ();
	dmd_alloc_pair ();
	dmd_show_low ();
	sound_send (SND_FIST_BOOM1);
	task_sleep_sec (1);
	sound_send (SND_FIST_BOOM1);
	task_sleep_sec (1);
	sound_send (SND_FIST_BOOM1);
	task_sleep_sec (1);
	sound_send (SND_FIST_BOOM1);
	task_sleep_sec (1);
	deff_exit ();
}

void door_award_enable (void)
{
#ifndef GREED_ONLY
	task_recreate_gid (GID_DOOR_AWARD_ROTATE, door_award_rotate);
#endif
}


static void door_award_flashing (void)
{
	task_kill_gid (GID_DOOR_AWARD_ROTATE);
	door_active_lamp = door_get_flashing_lamp ();
	lamp_tristate_on (door_active_lamp);
	door_start_event (door_index);
	deff_start (DEFF_DOOR_AWARD);
	score (SC_5M);
	timed_game_extend (10);

	door_panels_started++;
	audit_increment (&feature_audits.door_panels);
	switch (door_panels_started)
	{
		case 3:
			audit_increment (&feature_audits._3_panel_games);
			break;
		case 6:
			audit_increment (&feature_audits._6_panel_games);
			break;
		case 9:
			audit_increment (&feature_audits._9_panel_games);
			break;
		case 12:
			audit_increment (&feature_audits._12_panel_games);
			break;
	}

	leff_start (LEFF_DOOR_STROBE);
	task_sleep (TIME_100MS);
	door_advance_flashing ();
	score (SC_50K);
#ifndef GREED_ONLY
	door_award_enable ();
#endif
	callset_invoke (door_panel_awarded);
}

static void door_award_litz (void)
{
	door_start_event (14);
	audit_increment (&feature_audits.litz_started);
	deff_start (DEFF_LITZ_AWARD);
}

bool can_award_door_panel (void)
{
	/* Panels not awarded during any multiball */
	if (multi_ball_play ())
		return FALSE;

	/* No more panels can be awarded after BTTZ */
	if (flag_test (FLAG_BTTZ_RUNNING))
		return FALSE;

	return TRUE;
}

CALLSET_ENTRY (door, lamp_update)
{
	if (can_award_door_panel () && flag_test (FLAG_PIANO_DOOR_LIT))
		lamp_on (LM_PIANO_PANEL);
	else
		lamp_off (LM_PIANO_PANEL);

	if (can_award_door_panel () && flag_test (FLAG_SLOT_DOOR_LIT) 
		&& !task_find_gid (GID_SSLOT_AWARD_ROTATE))
		lamp_on (LM_SLOT_MACHINE);
	else
		lamp_off (LM_SLOT_MACHINE);
}

void award_door_panel (void)
{
	if (door_index == LITZ_DOOR_INDEX)
	{
		flag_on (FLAG_BTTZ_RUNNING);
		flag_off (FLAG_PIANO_DOOR_LIT);
		flag_off (FLAG_SLOT_DOOR_LIT);
		door_award_litz ();
	}
	else
	{
		door_award_flashing ();
	}
	reset_unlit_shots ();
	door_lamp_update ();
}

void door_award_if_possible (void)
{
	if (can_award_door_panel ())
	{
		/* TODO : When called from the camera award, this always
		causes a crash???  This is probably stack overflow. */
		award_door_panel ();
	}
}

CALLSET_ENTRY (door, door_start_10M)
{
	score (SC_10M);
}

CALLSET_ENTRY (door, ball_count_change)
{
	door_lamp_update ();
}

CALLSET_ENTRY(door, sw_piano)
{
	if (can_award_door_panel () && flag_test (FLAG_PIANO_DOOR_LIT))
	{
		flag_off (FLAG_PIANO_DOOR_LIT);
		flag_on (FLAG_SLOT_DOOR_LIT);
		award_door_panel ();
	}
	else
	{
		award_unlit_shot (SW_PIANO);
		score (SC_5130);
		sound_send (SND_ODD_CHANGE_BEGIN);
	}
}

CALLSET_ENTRY (door, shot_slot_machine)
{
	if (can_award_door_panel () && flag_test (FLAG_SLOT_DOOR_LIT))
	{
		flag_off (FLAG_SLOT_DOOR_LIT);
		flag_on (FLAG_PIANO_DOOR_LIT);
		award_door_panel ();
	}
	else
		award_unlit_shot (SW_PIANO);
		score (SC_5130);
	//TODO else 
		//deff_start (DEFF_SHOOT_PIANO);
}

CALLSET_ENTRY (door, door_start_eb)
{
	sound_send (SND_GET_THE_EXTRA_BALL);	
	light_easy_extra_ball ();
}

CALLSET_ENTRY(door, start_player)
{
#ifdef GREED_ONLY
	door_index = DOOR_INDEX;
#else
	door_index = 0;
#endif
	door_panels_started = 0;
	flag_on (FLAG_PIANO_DOOR_LIT);
	flag_on (FLAG_SLOT_DOOR_LIT);
	door_lamp_update ();
}

CALLSET_ENTRY(door, start_ball)
{
	door_set_flashing (door_index);
	door_award_enable ();
}

