/* @(#)list.c 1.16 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  list.c - part of SM/Editor
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

#include <stdio.h>
#include <X11/Intrinsic.h>

typedef struct _Link {
        struct _Link    *next, *prev;
        long	length;
        long	pos;
        char 	*buffer;
} LinkRec, *LinkPtr;

#ifdef _NO_PROTO
LinkPtr NewLink(Link, pos)
LinkPtr Link;
long	pos;
#else
LinkPtr NewLink(LinkPtr Link, long pos)
#endif
{
	LinkPtr newLink;

        newLink = XtNew(LinkRec);
	newLink->pos    = pos;
	newLink->length = 0L;
	newLink->buffer = (char *)NULL;
	newLink->prev   = Link;
	if (Link) {
	    newLink->next = Link->next;
            Link->next = newLink;
	}
	else newLink->next = (LinkPtr)NULL;

	return (newLink);
}

#ifdef _NO_PROTO
LinkPtr DeleteLink(Link)
LinkPtr Link;
#else
LinkPtr DeleteLink(LinkPtr Link)
#endif
{
	LinkPtr newLink;

        if (Link == (LinkPtr)NULL)
            return (Link);

	if (Link->prev) {
	    newLink = Link->prev;
	    Link->prev->next = Link->next;
	}
	else newLink = Link->next;
	if (Link->next)
	    Link->next->prev = Link->prev;
        XtFree((char *)Link);

        return (newLink);
}



