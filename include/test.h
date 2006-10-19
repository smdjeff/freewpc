/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

#ifndef _TEST_H
#define _TEST_H

struct window;

void window_push_first (void);
void window_pop_first (void);
void window_start_thread (void);
void window_stop_thread (void);
void window_push (struct window_ops *ops, void *priv);
void window_pop_quiet (void);
void window_pop (void);
void window_init (void);


#define M_MENU 0x1
#define M_ITEM 0x2
#define M_LETTER_PREFIX 0x4

/**
 * A menu descriptor, which could be an actual menu or just a menu item.
 * The flags field determines which it really is.
 *
 * If it is of type M_MENU, then the var union should contain a
 * 'submenus' value, which points to an array of pointers to the menu's
 * items.  The submenus array should be NULL terminated.
 *
 * If it is of type M_ITEM, then the union contains a window operations
 * structure, and optionally a pointer to its instance data.  When the
 * item is selected, a new window will get created.
 */
struct menu
{
	const char *name;
	U8 flags;
	union {
		struct menu **submenus;
		struct {
			struct window_ops *ops;
			void *priv;
		} subwindow;
	} var;
};

extern struct menu main_menu;
extern struct window_ops menu_window;


#define ADJ_BROWSING 0
#define ADJ_EDITING 1
#define ADJ_CONFIRMING 2


/**
 * An adjustment value descriptor, which controls the range of values
 * for a particular adjustment and how it is displayed.
 */
struct adjustment_value
{
	/** The minimum value of the adjustment */
	U8 min;

	/** The maximum value of the adjustment */
	U8 max;

	/** The step (increment or decrement) to be used when the
	 * adjustment is modified with the up or down buttons. */
	U8 step;

	/** A function that renders the value into the print buffer.
	 * This function will always call sprintf(). */
	void (*render) (U8 val);
};


/**
 * An adjustment descriptor, which correlates an adjustment's storage
 * location in non-volatile RAM with a textual name and a value
 * descriptor.
 */
struct adjustment
{
	const char *name;
	struct adjustment_value *values;
	U8 factory_default;
	U8 *nvram;
};

__test__ void adj_reset (struct adjustment *adjs);
__test__ void adj_verify (struct adjustment *adjs);
__test__ void adj_reset_all (void);
__test__ void adj_verify_all (void);


/**
 * An audit descriptor, which correlates an audit's storage location
 * in non-volatile RAM with a textual name and a rendering function.
 */
struct audit
{
	const char *name;
	void (*render) (audit_t);
	audit_t *nvram;
};

void menu_init (void);
void menu_draw (void);
void menu_enter (void);
void menu_up (void);
void menu_down (void);
void menu_start (void);

__test__ void test_init (void);
__test__ void test_up_button (void);
__test__ void test_down_button (void);
__test__ void test_left_flipper_button (void);
__test__ void test_right_flipper_button (void);
__test__ void test_enter_button (void);
__test__ void test_escape_button (void);
__test__ void test_start_button (void);

__test__ void switch_test_add_queue (U8 sw);

#endif /* _TEST_H */
