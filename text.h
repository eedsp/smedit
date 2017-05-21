/* @(#)text.h 1.12 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  text.h - part of SM/Editor
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
#ifndef _TEXT_H
#define _TEXT_H

#define MAXTAB		80

extern	char	*FileName;

extern	XmTextPosition	last_position;		/* the last cursor Position  */
extern	int	BlockIndentSize;		/* Block Indent		*/
extern	int	TabSize;			/* Tab Size		*/
extern	Boolean	insert_mode;	        	/* Toggle Insert Mode   */
extern	Boolean	indent_mode;			/* Toggle Indent Mode	*/
extern	Boolean show_cursor_position;		/* Display Line Column	*/
extern	Boolean	undo_after_save; 		/* Undo After Save 	*/
extern	Boolean	ignoreModify;			/* Ignore Modify	*/
extern	Boolean	fill_mode;			/* Toggle Tab Mode	*/
extern	Boolean	file_saved;			/* Editing File Modified     */
extern	Boolean	show_match;		        /* Auto Matching	*/

extern	Widget	undo_button, redo_button;
/*extern	Widget	undo_b, redo_b;*/
extern	Widget	popup_undo_button, popup_redo_button;
extern	int	smStatusLineGetMsgStrLen();
extern	void 	FreeUndoBuffer();
extern	void	FreeRedoBuffer();

#ifdef _NO_PROTO
extern	Widget	smCreateStatusLine();
extern	void	smStautsLineSetMessage();
extern	void	smStatusLineShowPosition();
extern	void	smStatusLineUpdateStatus();
extern	XmTextPosition	smTextGetBottomCharacter();
extern	void	smTextSetInsertionLine();
extern	void	smTextSetTabsize();
extern	void	smSetBlockIndentSize();
extern	void	smSetSensitiveUndo();
extern	void	smSetSensitiveRedo();
extern	void	RefreshTextWidget();
extern	void	UndoLast();
extern	void	UndoAll();
extern	void	RedoAction();
extern	void 	delete_line();
extern	void	process_tab();
extern	void	BlockIndent();
extern	void	BlockUnindent();
extern	void	move_top();
extern	void	move_bottom();
extern	void	delete_next_word();
extern	void	delete_previous_word();
extern	void	word_right();
extern	void	word_left();
extern	void	Modify_cb();
extern	void	Motion_cb();
extern	void 	show_info();
extern	void	SetPBPixmap();
#else
extern	Widget	smCreateStatusLine(Widget, char *);
extern	void	smStautsLineSetMessage(char *);
extern	void	smStatusLineShowPosition(Widget, XmTextPosition);
extern	void	smStatusLineUpdateStatus(Boolean, Boolean, Boolean, Boolean);
extern	XmTextPosition	smTextGetBottomCharacter(Widget w);
extern	void	smTextSetInsertionLine(Widget, int);
extern	void	smTextSetTabsize(Widget, int);
extern	void	smSetBlockIndentSize(int);
extern	void	smSetSensitiveUndo(Boolean);
extern	void	smSetSensitiveRedo(Boolean);
extern	void	RefreshTextWidget(Widget, XEvent *, String *, Cardinal *);
extern	void	UndoLast(Widget, XEvent *, String *, Cardinal *);
extern	void	UndoAll(Widget, XEvent *, String *, Cardinal *);
extern	void 	delete_line(Widget, XEvent *, String *, Cardinal *);
extern	void	process_tab(Widget, XEvent *, String *, Cardinal *);
extern	void	BlockUnindent(Widget, XEvent *, String *, Cardinal *);
extern	void	BlockIndent(Widget, XEvent *, String *, Cardinal *);
extern	void	RedoAction(Widget, XEvent *, String *, Cardinal *);
extern	void	move_top(Widget, XEvent *, String *, Cardinal *);
extern	void	move_bottom(Widget, XEvent *, String *, Cardinal *);
extern	void	delete_next_word(Widget, XEvent *, String *, Cardinal *);
extern	void	delete_previous_word(Widget, XEvent *, String *, Cardinal *);
extern	void	word_right(Widget, XEvent *, String *, Cardinal *);
extern	void	word_left(Widget, XEvent *, String *, Cardinal *);
extern	void	Motion_cb(Widget, XtPointer, XtPointer);
extern	void	Modify_cb(Widget, XtPointer, XtPointer);
extern	void 	show_info(Widget, XEvent *, String *, Cardinal *);
extern	void	SetPBPixmap(Widget, char **);
#endif

#endif
