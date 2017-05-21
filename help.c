/* @(#)help.c 1.28 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  help.c - part of SM/Editor
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
#include <string.h>
#include <stdlib.h>
#ifdef hpux
#include <strings.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>

#define HELP_ENV	"SMED_HELP"
#define HELP_FILE	"smed.help"

Widget	help_text;

#ifdef _NO_PROTO
void closeHelpDialog_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void closeHelpDialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	Widget dialog = (Widget)client_data;

	XtUnmanageChild(dialog);
}

#ifdef _NO_PROTO
Widget CreateHelpDialog(parent, name)
Widget parent;
char *name;
#else
Widget CreateHelpDialog(Widget parent, char *name)
#endif
{
	static	Widget	form, seperator_w, ok_button;
	Arg args[10];
	int arg;

	arg = 0;
	XtSetArg(args[arg], XmNfractionBase,	5); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	form = XmCreateFormDialog(parent, name, args, arg);

	arg = 0;
	XtSetArg(args[arg], XmNeditMode,	XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNeditMode,	False); arg++;
	XtSetArg(args[arg], XmNeditable,	False); arg++;
	XtSetArg(args[arg], XmNtopAttachment,	XmATTACH_FORM); arg++;
        XtSetArg(args[arg], XmNtopOffset,	5); arg++;
	XtSetArg(args[arg], XmNleftAttachment,	XmATTACH_FORM); arg++;
        XtSetArg(args[arg], XmNleftOffset,	5); arg++;
	XtSetArg(args[arg], XmNrightAttachment,	XmATTACH_FORM); arg++;
        XtSetArg(args[arg], XmNrightOffset,	5); arg++;
        XtSetArg(args[arg], XmNtraversalOn,	False); arg++;

	help_text = XmCreateScrolledText(form, "helpText", args, arg);

	XtManageChild(help_text);

	seperator_w = XtVaCreateManagedWidget("Separator",
                xmSeparatorGadgetClass, form,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNtopWidget,		help_text,
                XmNtopOffset,		2,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                NULL);

	ok_button = XtVaCreateManagedWidget("helpOk",
		xmPushButtonWidgetClass, form,
		XmNshowAsDefault,	True,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		seperator_w,
		XmNtopOffset,		5,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	2,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	3,
                XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	5,
		NULL);

	XtAddCallback(ok_button,
	        XmNactivateCallback, closeHelpDialog_cb, (XtPointer)form);

	return (form);
}

void loadHelpText()
{
	char *str, buf[BUFSIZ];
	FILE *fp;
	struct stat statb;
	char *env;

	env = getenv(HELP_ENV);
	if (env != NULL)
	    (void)sprintf(buf, "%s/%s", env, HELP_FILE);
	else
	    (void)sprintf(buf, "/usr/lib/X11/%s", HELP_FILE, HELP_FILE);

	if ((fp = fopen(buf, "r"))) {
	    (void)stat(buf, &statb);
	    str = XtMalloc((Cardinal)statb.st_size + 1);
	    if (fread(str, sizeof(char), (int)statb.st_size, fp) != (size_t)statb.st_size)
	        (void)sprintf(buf, "\nWarning: did not read entire file!\n");
	    str[statb.st_size] = '\0';
	    fclose(fp);
	    XmTextSetString(help_text, str);
	    XtFree(str);
	}
	else {
	    str = XtMalloc((Cardinal)BUFSIZ);
	    (void)sprintf(str, "\n\t\tHelp file : %s not found", HELP_FILE);
	    XmTextSetString(help_text, str);
	    XtFree(str);
	}
}
