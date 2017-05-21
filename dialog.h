/* @(#)dialog.h 1.5 Software Membership 01/01/99 */
/*
 *  dialog.h - part of SM/Editor program
 *
 * Copyright 1991 - 1996, Shiwon Cho, Software Membership, Seoul, Korea
 * All rights reserved.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Writen by Shiwon Cho		(stsolaris_at_gmail.com)
 *
 */

#ifndef _DIALOG_H
#define _DIALOG_H

#ifdef _NO_PROTO

extern	void setDialogMessage();
extern	void set_ErrorDialog_message();
extern	void set_OverwriteDialog_message();
extern	void set_QuestionDialog_message();
extern	void CloseDialog_cb();
extern	void SBText_cb();
extern	void setPromptDialogText();

#else	/*	_NO_PROTO	*/

extern	void setDialogMessage(Widget, char *);
extern	void set_ErrorDialog_message(Widget, char *, int);
extern	void set_OverwriteDialog_message(Widget, char *);
extern	void set_QuestionDialog_message(Widget, char *);
extern	void CloseDialog_cb(Widget, XtPointer, XtPointer);
extern	void SBText_cb(Widget, XtPointer, XtPointer);
extern	void setPromptDialogText(Widget, char *);

#endif

#endif

