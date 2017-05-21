/* @(#)text.c 1.19 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  text.c - part of SM/Editor
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
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
#include <Xm/Text.h>
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>
#include <Xm/ToggleBG.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/RepType.h>
#include <Xm/DragDrop.h>

#include "list.h"
#include "search.h"
#include "strctrl.h"

#define MAXTAB		80

char	*FileName;
LinkPtr	undo = (LinkPtr)NULL, redo = (LinkPtr)NULL;
XmTextPosition	last_position;		/* the last cursor Position  */
int	BlockIndentSize = 2;		/* Block Indent		*/
int	TabSize = 8;			/* Tab Size		*/
Boolean	insert_mode = True;	        /* Toggle Insert Mode   */
Boolean	indent_mode = True;		/* Toggle Indent Mode	*/
Boolean	show_cursor_position = True;	/* Display Line Column	*/
Boolean	undo_after_save = True;		/* Undo After Save 	*/
Boolean	ignoreModify = False;		/* Ignore Modify	*/
Boolean	fill_mode = True;		/* Toggle Tab Mode	*/
Boolean	file_saved = True;		/* Editing File Modified     */
Boolean	show_match = False;	        /* Auto Matching	*/

Widget	undo_button, redo_button;
/*
Widget	undo_b, redo_b;
*/
Widget	popup_undo_button, popup_redo_button;
Widget	separator, frame_w[4], rl_form,
	line_label, line_text, col_label, col_text;
Widget	msg_line, env_line_1, env_line_2;

Widget	toggle[8];

#ifdef _NO_PROTO
Widget smCreateStatusLine(parent, name)
Widget parent;
char *name;
#else
Widget smCreateStatusLine(Widget parent, char *name)
#endif
{
	static	Widget form_status;

	form_status = XtVaCreateWidget(name,
		xmFormWidgetClass,	parent,
		XmNmarginHeight,	0,
		XmNrightAttachment,	XmATTACH_FORM,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNbottomAttachment,    XmATTACH_FORM,
		XmNtopOffset,		0,
                XmNbottomOffset,	0,
		XmNleftOffset,		0,
		XmNrightOffset,		0,
                NULL);


       frame_w[0] = XtVaCreateWidget("frame_w0",
                xmFrameWidgetClass,     form_status,
		XmNshadowType,		XmSHADOW_IN,
		XmNshadowThickness,	1,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		2,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		1,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	1,
                NULL);

	env_line_2 = XtVaCreateManagedWidget("env_line_2",
                xmLabelWidgetClass,     frame_w[0],
                XmNalignment,           XmALIGNMENT_BEGINNING,
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		NULL);

	XtManageChild(frame_w[0]);

       frame_w[1] = XtVaCreateWidget("frame_w1",
                xmFrameWidgetClass,     form_status,
		XmNshadowType,		XmSHADOW_IN,
		XmNshadowThickness,	1,
		XmNrightAttachment,	XmATTACH_WIDGET,
		XmNrightWidget,		frame_w[0],
		XmNrightOffset,		2,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		1,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	1,
		NULL);

	env_line_1 = XtVaCreateManagedWidget("env_line_1",
                xmLabelWidgetClass,     frame_w[1],
                XmNalignment,           XmALIGNMENT_BEGINNING,
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
                NULL);

	XtManageChild(frame_w[1]);

	frame_w[2] = XtVaCreateWidget("frame_w2",
                xmFrameWidgetClass,     form_status,
		XmNshadowType,		XmSHADOW_IN,
		XmNshadowThickness,	1,
		XmNchildHorizontalAlignment,	XmALIGNMENT_CENTER,
                XmNmarginWidth,         0,
                XmNmarginHeight,        0,
                XmNrightAttachment,     XmATTACH_WIDGET,
                XmNrightWidget,         frame_w[1],
                XmNrightOffset,         2,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		1,
                XmNbottomAttachment,    XmATTACH_FORM,
                XmNbottomOffset,        1,
                NULL);

	rl_form = XtVaCreateWidget("rl_form",
		xmFormWidgetClass,	frame_w[2],
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		NULL);

	line_label = XtVaCreateManagedWidget("line_label",
                xmLabelWidgetClass,     rl_form,
                XmNalignment,           XmALIGNMENT_END,
                XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		0,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	0,
		NULL);

	line_text = XtVaCreateManagedWidget("line_text",
                xmLabelWidgetClass,     rl_form,
                XmNalignment,           XmALIGNMENT_END,
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		line_label,
		XmNleftOffset,		0,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	0,
		NULL);

	col_label = XtVaCreateManagedWidget("col_label",
                xmLabelWidgetClass,     rl_form,
                XmNalignment,           XmALIGNMENT_END,
                XmNmarginWidth,		0,
		XmNmarginHeight,	0,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		line_text,
		XmNleftOffset,		4,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	0,
		NULL);

	col_text = XtVaCreateManagedWidget("col_text",
                xmLabelWidgetClass,     rl_form,
                XmNalignment,           XmALIGNMENT_END,
                XmNmarginWidth,         0,
		XmNmarginHeight,	0,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		col_label,
		XmNleftOffset,		0,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	0,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		0,
		NULL);

	XtManageChild(rl_form);
	XtManageChild(frame_w[2]);

	frame_w[3] = XtVaCreateWidget("frame_w3",
                xmFrameWidgetClass,     form_status,
		XmNshadowType,		XmSHADOW_IN,
		XmNshadowThickness,	1,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		2,
		XmNrightAttachment,	XmATTACH_WIDGET,
		XmNrightWidget,		frame_w[2],
		XmNrightOffset,		2,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		1,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	1,
		NULL);

	msg_line = XtVaCreateManagedWidget("msg_line",
                xmLabelWidgetClass,     frame_w[3],
                XmNalignment,           XmALIGNMENT_BEGINNING,
		XmNmarginWidth,		0,
		XmNmarginHeight,	0,
                NULL);

	XtManageChild(frame_w[3]);

	return (form_status);
}

#ifdef _NO_PROTO
void smStautsLineSetMessage(str)
char *str;
#else
void smStautsLineSetMessage(char *str)
#endif
{
        XmString label;

	label = XmStringCreateLocalized(str);
	XtVaSetValues(msg_line,
		    XmNlabelString,       label,
		    NULL);
	XmStringFree(label);
}

#ifdef _NO_PROTO
void smStatusLineShowPosition(w, pos)
Widget w;
XmTextPosition pos;
#else
void smStatusLineShowPosition(Widget w, XmTextPosition pos)
#endif
{
        int line, col;
	static int o_line, o_col;
	char buf[10];
        XmString label;

        smPosToLineCol(w, pos, TabSize, &line, &col);

	if (o_line != line) {
	    (void)sprintf(buf, "%4d", line);
	    label = XmStringCreateLocalized(buf);
	    XtVaSetValues(line_text,
		    XmNlabelString,       label,
		    NULL);
	    XmStringFree(label);
	    o_line = line;
	}

	if (o_col != col) {
	    (void)sprintf(buf, "%3d", col);
	    label = XmStringCreateLocalized(buf);
	    XtVaSetValues(col_text,
		    XmNlabelString,       label,
		    NULL);
	    XmStringFree(label);
	    o_col = col;
	}
}

#ifdef _NO_PROTO
void smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode)
Boolean file_saved;
Boolean insert_mode;
Boolean indent_mode;
Boolean fill_mode;
#else
void smStatusLineUpdateStatus(Boolean file_saved,
	Boolean insert_mode, Boolean indent_mode, Boolean fill_mode)
#endif
{
        char buf[BUFSIZ];
        XmString label;

	(void)sprintf(buf, "%s %s",
		(!file_saved) ? "*" : " ",
		(insert_mode) ? "Insert   " : "Overwrite");
	label = XmStringCreateLocalized(buf);
	XtVaSetValues(env_line_1,
		    XmNlabelString,       label,
		    NULL);
	XmStringFree(label);

	(void)sprintf(buf, "%s %s",
		(indent_mode) ? " Indent " : "Unindent",
		(fill_mode) ? "Tab" : "   ");
	label = XmStringCreateLocalized(buf);
	XtVaSetValues(env_line_2,
		    XmNlabelString,       label,
		    NULL);
	XmStringFree(label);
}

int smStatusLineGetMsgStrLen()
{
        XmString label;
	char *str;
	int len;

        XtVaGetValues(msg_line,
            XmNlabelString,     &label,
            NULL);
        XmStringGetLtoR(label, XmSTRING_DEFAULT_CHARSET, &str);
        XmStringFree(label);
	len = strlen(str);
	XtFree(str);

	return (len);
}

#ifdef _NO_PROTO
XmTextPosition smTextGetBottomCharacter(w)
Widget w;
#else
XmTextPosition smTextGetBottomCharacter(Widget w)
#endif
{
#if (XmVersion >= 1002) /* Motif 1.2 */
        int page, i = 0;
        short row;
        XmTextPosition pos;
        Boolean found;

        XtVaGetValues(w,
                XmNrows,                &row,
                XmNtopCharacter,        &pos,
                NULL);
        page = (int)row - 1;
        do {
            found = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &pos);
            if (found) {
                i++;
                pos++;
            } /* if */
        } while (found && i < page);
#else
        int page, len, i = 0;
        XmTextPosition pos;
        short row;
        char *str;

        XtVaGetValues(w,
                XmNrows,                &row,
                XmNvalue,               &str,
                XmNtopCharacter,        &pos,
                NULL);
        page = (int)row - 1;
        len = strlen(str);
        while (pos < len && i < page)
            if (str[pos++] == '\n') i++;
        XtFree(str);
        if (pos > len) pos = len;
#endif
        return (pos);
}

#ifdef _NO_PROTO
void smTextSetInsertionLine(w, line)
Widget w;
int line;
#else
void smTextSetInsertionLine(Widget w, int line)
#endif
{
        int count_line;
        Boolean found;
        XmTextPosition pos;

        pos = 0;
        count_line = 1;
        found = True;
        while (found && count_line < line) {
            found = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &pos);
            if (found) {
                count_line++;
                pos++;
            } /* if */
        }

        smTextSetInsertionPosition(w, pos);
}

#ifdef _NO_PROTO
void smTextSetTabsize(w, size)
Widget w;
int size;
#else
void smTextSetTabsize(Widget w, int size)
#endif
{
        OutputData data = ((XmTextWidget)w)->text.output->data;
        static int check = 0;
        static int tabwidth;
        int avgwidth;

        TabSize = (size > MAXTAB || size <= 0) ? 8 : size;
        if (!check) {
            check = 1;
            tabwidth = data->tabwidth / 8;
        }
        avgwidth = (data->averagecharwidth / 2) - 1;
        data->tabwidth = (tabwidth >= data->averagecharwidth) ?
                        TabSize * data->averagecharwidth :
                        TabSize * avgwidth;
        ignoreModify = True;
        XmTextInsert(w, 0, "\n");
        XmTextReplace(w, 0, 1, "");
        ignoreModify = False;
}

#ifdef _NO_PROTO
void smSetSensitiveUndo(sensitive)
Boolean sensitive;
#else
void smSetSensitiveUndo(Boolean sensitive)
#endif
{

	XtSetSensitive(undo_button, sensitive);
	XtSetSensitive(popup_undo_button, sensitive);

/*	XtSetSensitive(undo_b, sensitive);*/

}

#ifdef _NO_PROTO
void smSetSensitiveRedo(sensitive)
Boolean sensitive;
#else
void smSetSensitiveRedo(Boolean sensitive)
#endif
{
	XtSetSensitive(redo_button, sensitive);
	XtSetSensitive(popup_redo_button, sensitive);

/*	XtSetSensitive(redo_b, sensitive);*/

}

#ifdef _NO_PROTO
void RefreshTextWidget(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void RefreshTextWidget(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	char *buf;

	ignoreModify = True;
	if (found) found = False;
	XmTextClearSelection(w, event->xbutton.time);

	buf = XmTextGetString(w);
	XmTextSetString(w, buf);
	smStatusLineShowPosition(w, 0);
	XtFree(buf);

	ignoreModify = False;
#if (XmVersion >= 1002)	/* Motif 1.2 */
 	XmUpdateDisplay(w);
#endif
}

void FreeUndoBuffer()
{
        while (undo) {
            if (undo->buffer != NULL) XtFree(undo->buffer);
                undo = DeleteLink(undo);
        }
	smSetSensitiveUndo(False);
}

void FreeRedoBuffer()
{
        while (redo) {
            if (redo->buffer != NULL) XtFree(redo->buffer);
                redo = DeleteLink(redo);
        }
	smSetSensitiveRedo(False);
}

#ifdef _NO_PROTO
void smSetBlockIndentSize(size)
int size;
#else
void smSetBlockIndentSize(int size)
#endif
{
	BlockIndentSize = (size > MAXTAB || size <= 0) ? 8 : size;
}

#ifdef _NO_PROTO
void UndoLast(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void UndoLast(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition pos;

	if (undo) {
	    ignoreModify = True;
	    if (found) {
	        XmTextClearSelection(w, event->xkey.time);
	        found = False;
	    }
	    smSetSensitiveRedo(True);
            XmTextDisableRedisplay(w);

	    redo = NewLink(redo, undo->pos);
	    if (undo->length > 0L) {
	        XmTextReplace(w, (XmTextPosition)undo->pos,
	    		undo->pos + undo->length, "");
	    }
	    else {
	        XmTextInsert(w, (XmTextPosition)undo->pos, undo->buffer);
	        redo->length = strlen(undo->buffer);
	    }
	    redo->buffer = XtNewString(undo->buffer);
	    XtFree(undo->buffer);
	    pos = (XmTextPosition)undo->pos;
	    undo = DeleteLink(undo);
	    if (undo && undo->length == -1) {
	        redo = NewLink(redo, undo->pos);
	        XmTextInsert(w, (XmTextPosition)undo->pos, undo->buffer);
	        redo->length = -1;
                redo->buffer = XtNewString(undo->buffer);
                XtFree(undo->buffer);
                pos = (XmTextPosition)undo->pos;
                undo = DeleteLink(undo);
	    }
	    if (!undo)
	        smSetSensitiveUndo(False);

            XmTextEnableRedisplay(w);
	    XmTextSetInsertionPosition(w, pos);

	    ignoreModify = False;
            if (undo_after_save && file_saved) {
                file_saved = False;
                smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
            }
        }

	pos = XmTextGetInsertionPosition(w);
	smStatusLineShowPosition(w, pos);
}

#ifdef _NO_PROTO
void UndoAll(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void UndoAll(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	static XmTextPosition position;
	XmTextPosition pos;

	if (undo) {
	    ignoreModify = True;
	    if (found) {
	        XmTextClearSelection(w, event->xkey.time);
	        found = False;
	    }
	    smSetSensitiveRedo(True);
	    while (undo) {
	        redo = NewLink(redo, undo->pos);
	        if (undo->length > 0L) {
	            XmTextReplace(w, (XmTextPosition)undo->pos,
			(XmTextPosition)undo->pos + undo->length, "");
	        }
	        else {
	            XmTextInsert(w, (XmTextPosition)undo->pos, undo->buffer);
	            redo->length = (undo->length == -1) ? -1 : strlen(undo->buffer);
	        }
                redo->buffer = XtNewString(undo->buffer);
                XtFree(undo->buffer);
	        position = (XmTextPosition)undo->pos;
                undo = DeleteLink(undo);
	    }
	    smSetSensitiveUndo(False);

	    XmTextSetInsertionPosition(w, position);
	    ignoreModify = False;
            if (undo_after_save && file_saved) {
                file_saved = False;
                smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
            }
	}

	pos = XmTextGetInsertionPosition(w);
	smStatusLineShowPosition(w, pos);
}

#ifdef _NO_PROTO
void RedoAction(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void RedoAction(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition pos;

	if (redo) {
	    ignoreModify = True;
	    if (found) {
	        XmTextClearSelection(w, CurrentTime);
	        found = False;
	    }
	    smSetSensitiveUndo(True);
            XmTextDisableRedisplay(w);

    	    undo = NewLink(undo, (long)redo->pos);
	    if (redo->length > 0L) {
	        XmTextReplace(w, (XmTextPosition)redo->pos,
			(XmTextPosition)redo->pos + redo->length, "");
	    }
	    else {
    	        if (redo->length == -1) {
    	             XmTextReplace(w, (XmTextPosition)redo->pos,
			(XmTextPosition)redo->pos + strlen(redo->buffer), "");
    	            undo->length = -1;
                    undo->buffer = XtNewString(redo->buffer);
                    XtFree(redo->buffer);
                    redo = DeleteLink(redo);
    	            undo = NewLink(undo, (long)redo->pos);
    	        }
	        XmTextInsert(w, (XmTextPosition)redo->pos, redo->buffer);
	        undo->length = strlen(redo->buffer);
	    }
	    undo->buffer = XtNewString(redo->buffer);
	    XtFree(redo->buffer);
	    pos = (XmTextPosition)redo->pos;
	    redo = DeleteLink(redo);

	    if (!redo)
		smSetSensitiveRedo(False);

            XmTextEnableRedisplay(w);
	    XmTextSetInsertionPosition(w, pos);

	    ignoreModify = False;
            if (undo_after_save && file_saved) {
                file_saved = False;
                smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
            }
      	}

	pos = XmTextGetInsertionPosition(w);
	smStatusLineShowPosition(w, pos);
}

#ifdef _NO_PROTO
void process_tab(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void process_tab(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	char i_str[MAXTAB];
	XmTextPosition index;
	int tabsiz, status;
	char *buf, *index_str;
	XmTextPosition	pos, s_pos, e_pos;
	Boolean found_CR;

	pos = XmTextGetInsertionPosition(w);
	found_CR = XmTextFindString(w, pos - 1, "\n", XmTEXT_BACKWARD, &s_pos);
	s_pos = (found_CR) ? s_pos + 1 : 0;
	found_CR = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &e_pos);
	e_pos = (found_CR) ? e_pos : XmTextGetLastPosition(w);

	buf = XtMalloc((Cardinal)e_pos - s_pos + 1);
	status = XmTextGetSubstring(w, index, s_pos, e_pos - s_pos, buf);
	buf[e_pos - s_pos] = '\0';
	index_str = buf;
	while (*index_str) {
	    if (*index_str == '\t') s_pos += 8L;
	    index_str++;
	}
	XtFree(buf);
	tabsiz = TabSize - (e_pos - s_pos) % TabSize;
	(void)sprintf(i_str, "%*s", tabsiz, " ");
	XmTextInsert(w, pos, i_str);
	XmTextSetInsertionPosition(w, pos + (XmTextPosition)tabsiz);
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void BlockIndent(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void BlockIndent(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        show_cursor_position = False;
        shiftBlock(w, SHIFT_RIGHT, BlockIndentSize, TabSize);
        show_cursor_position = True;
        smStatusLineShowPosition(w, XmTextGetInsertionPosition(w));
}

#ifdef _NO_PROTO
void BlockUnindent(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void BlockUnindent(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        show_cursor_position = False;
        shiftBlock(w, SHIFT_LEFT, BlockIndentSize, TabSize);
        show_cursor_position = True;
        smStatusLineShowPosition(w, XmTextGetInsertionPosition(w));
}

#ifdef _NO_PROTO
void move_top(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void move_top(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition pos;

	pos = XmTextGetTopCharacter(w);
	XmTextSetInsertionPosition(w, pos);
}

#ifdef _NO_PROTO
void move_bottom(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void move_bottom(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition pos;

	pos = smTextGetBottomCharacter(w);
	XmTextSetInsertionPosition(w, pos);
}

#ifdef _NO_PROTO
void clearShowMatch(client_data, id)
XtPointer client_data;
XtIntervalId *id;
#else
void clearShowMatch(XtPointer client_data, XtIntervalId *id)
#endif
{
        Widget w = (Widget)client_data;

        XmTextClearSelection(w, CurrentTime);
}

#ifdef _NO_PROTO
void Modify_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void Modify_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
        XmTextVerifyCallbackStruct *cbs =
                        (XmTextVerifyCallbackStruct *)call_data;
	XmTextPosition start, end;
	Boolean is_selected;
	int buffer_size;
	int status;

	if (ignoreModify == False) {
	    if (found)
	        found = False;

	    if (smStatusLineGetMsgStrLen() > 0)
	        smStautsLineSetMessage("");
	    FreeRedoBuffer();

            /* Delete Redo Buffers */
/*            if (redo) {
	        while (redo) {
	            if (redo->buffer) XtFree(redo->buffer);
	            redo = DeleteLink(redo);
	        }
		smSetSensitiveRedo(False);
            }*/
	    if (!undo)
		smSetSensitiveUndo(True);

	    if (file_saved) {
	        file_saved = False;
	        smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
	    }

	    undo = NewLink(undo, (long)cbs->startPos);  /* Add New Undo Buffer */

	    if (cbs->text->length > 0) {
	        is_selected = XmTextGetSelectionPosition(w, &start, &end);
                if ((is_selected && start != end) || textBlock || !insert_mode) {
 	            buffer_size = (int)(cbs->endPos - cbs->startPos);
	            undo->buffer = XtMalloc((Cardinal)buffer_size + 1);
                    undo->length = -1;
	            status = XmTextGetSubstring(w, cbs->startPos,
	  		      buffer_size, buffer_size + 1, undo->buffer);
	            if (status == XmCOPY_SUCCEEDED)
	                undo->buffer[buffer_size] = '\0';
	            else XtWarning("Insufficient Undo Buffer");
	            undo = NewLink(undo, (long)cbs->startPos);
                }
                undo->length = (long)cbs->text->length;
                undo->buffer = XtNewString(cbs->text->ptr);
                undo->buffer[cbs->text->length] = '\0';
                if (cbs->text->length == 1 && show_match &&
                        strchr("{}()[]<>", cbs->text->ptr[0])) {
                    status = find_matching(w, cbs->startPos, cbs->text->ptr, &start);
                    if (status == 0) {
                        if (start > -1) {
                            XmTextSetSelection(w, start, start + 1, CurrentTime);
                            (void)XtAppAddTimeOut(XtWidgetToApplicationContext(w),
                                    1000,  clearShowMatch, (XtPointer)w);
                            smTextSetInsertionPosition(w, cbs->startPos);
                        }
                    }
                    else if (strchr("})]>", cbs->text->ptr[0])) {
                        XBell(cbs->event->xkey.display, 100);
                    }
                }
	    }
	    else { /* cbs->text->length <= 0 */
 	        buffer_size = cbs->endPos - cbs->startPos;
 	        if (buffer_size > 0) {
 	            undo->length = 0L;
		    undo->buffer = XtMalloc((Cardinal)buffer_size + 1);
		    status = XmTextGetSubstring(w, cbs->startPos,
				buffer_size, buffer_size + 1, undo->buffer);
		    if (status == XmCOPY_SUCCEEDED)
			undo->buffer[buffer_size] = '\0';
		    else XtWarning("Insufficient Undo Buffer");
	        }

	    }
	    if (show_cursor_position)
	        smStatusLineShowPosition(w, cbs->startPos);
	}
}

#ifdef _NO_PROTO
void Motion_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void Motion_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmTextVerifyCallbackStruct *cbs =
                        (XmTextVerifyCallbackStruct *)call_data;

	if (ignoreModify == False) {
	    last_position = XmTextGetInsertionPosition(w);

	    if (smStatusLineGetMsgStrLen() > 0)
	        smStautsLineSetMessage("");
            if (show_cursor_position)
                smStatusLineShowPosition(w, cbs->newInsert);
	}
}

#ifdef _NO_PROTO
void delete_line(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void delete_line(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition	pos, s_pos, e_pos;
	Boolean found_CR;

	pos = XmTextGetInsertionPosition(w);
	found_CR = XmTextFindString(w, pos - 1, "\n", XmTEXT_BACKWARD, &s_pos);
	s_pos = (found_CR) ? s_pos + 1 : 0;
	found_CR = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &e_pos);
	e_pos = (found_CR) ? e_pos + 1 : XmTextGetLastPosition(w);
	XtRemoveCallback(w, XmNmotionVerifyCallback, Motion_cb, (XtPointer)NULL);
	XmTextSetSelection(w, s_pos, e_pos, event->xkey.time);
	if (XmTextRemove(w) == True)
	    XFlush(event->xkey.display);
	XtAddCallback(w, XmNmotionVerifyCallback, Motion_cb, (XtPointer)NULL);
}


#ifdef _NO_PROTO
void delete_next_word(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void delete_next_word(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        XmTextPosition start, pos, l_pos;
        char buf[3];
        int status;

        start = pos = XmTextGetInsertionPosition(w);
	l_pos = XmTextGetLastPosition(w);
        status = XmTextGetSubstring(w, pos, 2, 3, buf);
	if (status != XmCOPY_FAILED && buf[0] == '\n') pos++;
	else if (status != XmCOPY_FAILED && ispunct((int)buf[0])) pos++;
        else while (status != XmCOPY_FAILED && pos < l_pos &&
		!ispunct((int)buf[0]) && !isspace((int)buf[0])) {
            status = XmTextGetSubstring(w, pos, 2, 3, buf);
            if (status != XmCOPY_FAILED && !ispunct((int)buf[0]) && !isspace((int)buf[0]))
                pos++;
        }
        do {
            status = XmTextGetSubstring(w, pos, 2, 3, buf);
            if (status != XmCOPY_FAILED && is_space(buf[0]))
                pos++;
        } while (status != XmCOPY_FAILED && is_space(buf[0]) && pos < l_pos);
	if (start != pos)
	    XmTextReplace(w, start, pos, "");
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void delete_previous_word(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void delete_previous_word(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        XmTextPosition start, pos;
        char buf[3];
        int status;

        start = pos = XmTextGetInsertionPosition(w);
        status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
	if (status != XmCOPY_FAILED && buf[0] == '\n') pos--;
	else if (status != XmCOPY_FAILED && ispunct((int)buf[0])) pos--;
        else while (status != XmCOPY_FAILED && pos > 0 &&
		!ispunct((int)buf[0]) && !isspace((int)buf[0])) {
            status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
            if (status != XmCOPY_FAILED &&
		!ispunct((int)buf[0]) && !isspace((int)buf[0]))
                pos--;
        }
        do {
            status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
            if (status != XmCOPY_FAILED && is_space(buf[0]))
                pos--;
        } while (status != XmCOPY_FAILED && is_space(buf[0]) && pos > 0);
	if (start != pos)
	    XmTextReplace(w, pos, start, "");
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void word_right(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else	/* _NO_PROTO */
void word_right(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        XmTextPosition pos, l_pos;
        char buf[3];
        int status;

        pos = XmTextGetInsertionPosition(w);
	l_pos = XmTextGetLastPosition(w);
        status = XmTextGetSubstring(w, pos, 2, 3, buf);
        if (status != XmCOPY_FAILED) {
            if (isspace((int)buf[0]))
	        while (status != XmCOPY_FAILED && isspace((int)buf[0]) && pos < l_pos) {
	            status = XmTextGetSubstring(w, pos, 2, 3, buf);
	            if (status != XmCOPY_FAILED && isspace((int)buf[0]))
	                pos++;
	        }
	    else
                while (!isspace((int)buf[0]) && !ispunct((int)buf[0]) && pos < l_pos) {
	            status = XmTextGetSubstring(w, pos, 2, 3, buf);
	            if (status != XmCOPY_FAILED && !isspace((int)buf[0]) && !ispunct((int)buf[0]))
	                pos++;
                }
        }
        do {
	    status = XmTextGetSubstring(w, pos, 2, 3, buf);
	    if (status != XmCOPY_FAILED && (is_space(buf[0]) || ispunct((int)buf[0])))
	        pos++;
        } while (status != XmCOPY_FAILED && (is_space(buf[0]) || ispunct((int)buf[0]))
		&& pos < l_pos);
	XmTextSetInsertionPosition(w, pos);
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void word_left(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void word_left(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
        XmTextPosition pos;
        char buf[3];
        int status;

        pos = XmTextGetInsertionPosition(w);
        do {
	    status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
	    if (status != XmCOPY_FAILED && (is_space(buf[0]) || ispunct((int)buf[0])))
	        pos--;
        } while (status != XmCOPY_FAILED && (is_space(buf[0]) || ispunct((int)buf[0])) && pos > 0);
        status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
        if (status != XmCOPY_FAILED) {
            if (isspace((int)buf[0]))
	        while (status != XmCOPY_FAILED && isspace((int)buf[0]) && pos > 0) {
	            status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
	            if (status != XmCOPY_FAILED && isspace((int)buf[0]))
	                pos--;
	        }
	    else
                while (!isspace((int)buf[0]) && !ispunct((int)buf[0]) && pos > 0) {
	            status = XmTextGetSubstring(w, pos - 1, 2, 3, buf);
	            if (status != XmCOPY_FAILED && !isspace((int)buf[0]) && !ispunct((int)buf[0]))
	                pos--;
                }
        }
	XmTextSetInsertionPosition(w, pos);
	XFlush(event->xkey.display);

}

#ifdef _NO_PROTO
void show_info(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void show_info(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	char buf[BUFSIZ];

	(void)sprintf(buf, " %ld bytes", XmTextGetLastPosition(w));
	if (!access(FileName, F_OK) && access(FileName, W_OK) < 0)
	    strcat(buf, " [Read Only]");

	smStautsLineSetMessage(buf);
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void SetPBPixmap(w, xpm_data)
Widget w;
char **xpm_data;
#else
void SetPBPixmap(Widget w, char **xpm_data)
#endif
{
	Display *display;
	Pixmap pix_returned, pix_mask_returned, pix_armed;
	XpmAttributes xpmatts;
	XpmColorSymbol XpmTransparentColor[1];
	Pixel bg_pixel, armbg_pixel;
	int depth = 1;

	display = XtDisplay(w);

	XtVaGetValues(w, XmNbackground, &bg_pixel,
			XmNarmColor, &armbg_pixel,
			XmNdepth, &depth,
			NULL);

	XpmTransparentColor[0].pixel = bg_pixel;
	XpmTransparentColor[0].value = "none";
	XpmTransparentColor[0].name = NULL;
	xpmatts.valuemask = XpmColorSymbols | XpmCloseness |XpmDepth;
	xpmatts.colorsymbols = XpmTransparentColor;
	xpmatts.numsymbols = 1;
	xpmatts.closeness = 65535;
	xpmatts.depth = depth;			 /* Pixmap for DISARMED  */
	XpmCreatePixmapFromData(XtDisplay(w),
			DefaultRootWindow(display),
			xpm_data,
			&pix_returned,
			&pix_mask_returned,
			&xpmatts);
	if(pix_mask_returned)
	    XFreePixmap(display, pix_mask_returned );

						/* Pixmap for ARMED  */
	XpmTransparentColor[0].pixel = armbg_pixel;
	XpmCreatePixmapFromData(XtDisplay(w),
			DefaultRootWindow(display),
			xpm_data,
			&pix_armed,
			&pix_mask_returned,
			&xpmatts);
	if(pix_mask_returned)
	    XFreePixmap(display, pix_mask_returned );

	XtVaSetValues(w,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, pix_returned,
			XmNarmPixmap, pix_armed,
			NULL);
}
