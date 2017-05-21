/* @(#)strctrl.h 1.37 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  strctrl.h - part of SM/Editor
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

#ifndef _STRCTR_H
#define _STRCTR_H

#define SHIFT_RIGHT     0
#define SHIFT_LEFT      1

extern Boolean textBlock;

#ifdef _NO_PROTO
extern void parseFilename();
extern void smTextReplace();
extern int textPosToLineNum();
extern void smPosToLineCol();
extern int smTextGetRows();
extern void smTextSetInsertionPosition();
extern Boolean is_space();
extern Boolean is_num();
extern Boolean stob();
extern int search_pair();
extern int find_matching();
extern XmTextPosition smFindString();
extern char *trimSpaces();
extern void shiftBlock();
extern void UpperCase();
extern void LowerCase();
#else /* _NO_PROTO */
extern void parseFilename(char *, char **, char **);
extern void smTextReplace(Widget, XmTextPosition, XmTextPosition, char *);
extern int textPosToLineNum(Widget, XmTextPosition);
extern void smPosToLineCol(Widget, XmTextPosition, int, int *, int *);
extern int smTextGetRows(Widget);
extern void smTextSetInsertionPosition(Widget, XmTextPosition);
extern Boolean is_space(char);
extern Boolean is_num(char *);
extern Boolean stob(char *);
extern int search_pair(Widget, XmTextPosition, XmTextPosition *);
extern int find_matching(Widget, XmTextPosition, char *, XmTextPosition *);
extern XmTextPosition smFindString(char *, XmTextPosition, char *, XmTextPosition,
	XmTextPosition, XmTextPosition, Boolean, Boolean, Boolean);
extern char *trimSpaces(char *);
extern void shiftBlock(Widget, int, int, int);
extern void UpperCase(Widget);
extern void LowerCase(Widget);
#endif

#endif
