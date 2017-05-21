/* @(#)list.h 1.15 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  list.h - part of SM/Editor
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

#ifndef _LIST_H
#define _LIST_H

typedef struct _Link {
        struct _Link    *next, *prev;
        long	length;
        long	pos;
        char 	*buffer;
} *LinkPtr;

#ifdef _NO_PROTO
extern LinkPtr NewLink();
extern LinkPtr DeleteLink();
#else
extern LinkPtr NewLink(LinkPtr, long);
extern LinkPtr DeleteLink(LinkPtr);
#endif /*  _NO_PROTO  */

#endif


