/* @(#)dialog.c 1.5 Software Membership 01/01/99 */
/*
 *  dialog.c - part of SM/Editor program
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

#include <stdio.h>
#include <string.h>
#ifdef hpux
#include <strings.h>
#endif
#include <errno.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>

#define FILE_SAVE	0
#define FILE_OPEN	1

/* external variable declarations */
extern char *sys_errlist[];
extern int errno;

#ifdef _NO_PROTO
void setDialogMessage(w, str)
Widget w;
char *str;
#else
void setDialogMessage(Widget w, char *str)
#endif
{
	XmString label;

	label = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues(w,
		XmNmessageString,	label,
		XmNnoResize,		False,
		XmNresizePolicy,	XmRESIZE_ANY,
		NULL);
	XmStringFree(label);
	XtVaSetValues(w,
		XmNnoResize,		True,
		XmNresizePolicy,	XmRESIZE_NONE,
		NULL);
}

#ifdef _NO_PROTO
void set_ErrorDialog_message(w, str, tag)
Widget w;
char *str;
int tag;
#else
void set_ErrorDialog_message(Widget w, char *str, int tag)
#endif
{
	char *buf;

	buf = XtMalloc((Cardinal)strlen(str) + strlen(sys_errlist[errno]) + 40);
	switch (tag) {
	    case FILE_SAVE :
		(void)sprintf(buf, "File:%s\n\nUnable to save file.\n%s", str, sys_errlist[errno]);
		break;
	    case FILE_OPEN :
		(void)sprintf(buf, "File:%s\n\nUnable to open file.\n%s", str, sys_errlist[errno]);
		break;
	}
	setDialogMessage(w, buf);
	XtFree(buf);
}

#ifdef _NO_PROTO
void set_QuestionDialog_message(w, str)
Widget w;
char *str;
#else
void set_QuestionDialog_message(Widget w, char *str)
#endif
{
	char *buf;

	buf = XtMalloc((Cardinal)strlen(str) + 50);
	(void)sprintf(buf, "File:%s\n\nFile has been modfied.\nSave it now ?", str);
	setDialogMessage(w, buf);
	XtFree(buf);
}

#ifdef _NO_PROTO
void set_OverwriteDialog_message(w, str)
Widget w;
char *str;
#else
void set_OverwriteDialog_message(Widget w, char *str)
#endif
{
	char *buf;

	buf = XtMalloc((Cardinal)strlen(str) + 60);
	(void)sprintf(buf, "File:%s\n\nThat file exits and has data in it.\nReplace it ?", str);
	setDialogMessage(w, buf);
	XtFree(buf);
}

#ifdef _NO_PROTO
void CloseDialog_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void CloseDialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XtUnmanageChild(w);
}

#ifdef _NO_PROTO
void setPromptDialogText(w, buf)
Widget w;
char *buf;
#else
void setPromptDialogText(Widget w, char *buf)
#endif
{
        XmString str;

	str = XmStringCreateLocalized(buf);
	XtVaSetValues(w,
		XmNtextString,	str,
	    	NULL);
	XmStringFree(str);
	XmTextSetSelection(XmSelectionBoxGetChild(w, XmDIALOG_TEXT),
		0,
	    	XmTextGetLastPosition(XmSelectionBoxGetChild(w, XmDIALOG_TEXT)),
	    	CurrentTime);
}

#ifdef _NO_PROTO
void SBText_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void SBText_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
        Boolean In_string = True;
        XmTextVerifyCallbackStruct *cbs =
        	(XmTextVerifyCallbackStruct *)call_data;
	char *str;
	XmTextPosition len;

	str = XmTextGetString(w);
	len = XmTextGetLastPosition(w);
	if (cbs->text->length > 0) In_string = True;
	else if ((cbs->endPos - cbs->startPos) == len || len == 0)
	    In_string = False;
	XtFree(str);

	XtSetSensitive((Widget)client_data, In_string);
}
