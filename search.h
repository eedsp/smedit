/* @(#)search.h 1.5 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  search.h - part of SM/Editor
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Writen by Shiwon Cho		(stsolaris_at_gmail.com)
 *
 */

#ifndef _PATTERN_H
#define _PATTERN_H

extern Boolean	found;

#ifdef _NO_PROTO
extern Widget smCreateSearchDialog();
extern void smManageReplaceText();
extern void SelectionToFind();
extern void findNext();
extern void findString();
extern void ReplaceFindString();
#else /* _NO_PROTO */
extern Widget smCreateSearchDialog(Widget , char *, Widget);
extern void smManageReplaceText(Boolean);
extern void SelectionToFind(Widget);
extern void findNext(Widget);
extern void findString(Widget, XEvent *, String *, Cardinal *);
extern void ReplaceFindString(Widget, XEvent *, String *, Cardinal *);
#endif

#endif
