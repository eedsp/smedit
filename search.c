/* @(#)search.c 1.9 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  search.c - part of SM/Editor
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
#include <sys/types.h>
#ifndef _GNU_RX
#include <regex.h>
#endif
#include <string.h>
#ifdef hpux
#include <strings.h>
#endif
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include "strctrl.h"
#include "text.h"
#include "dialog.h"

#ifdef _GNU_RX
#include "rx.h"
#endif

#define BYTEWIDTH 8

struct _searchDialog_rec {
	Widget	form;
	Widget	frame_input;
	Widget	form_input;
	Widget	find_w, label_f;
	Widget	replace_w, label_r;
	Widget	set_origin;
	Widget	frame[2], label[2];
	Widget	rowcol_toggle;
	Widget	toggle[3];
	Widget	radio_box;
	Widget	radio_t[2];
	Widget	Find, Replace, ReplaceAll, Close;
/*
	Widget	message_dialog;
*/
} searchDialog;	/* Find & Replace Dialog Struct */

/*      Global Variables        */
Boolean	found = False;

/*      Common Variables in Find & Replace Part  */
static char *search_str;
static char *regext_str;
static char *replace_str;
static XmTextPosition search_len;
static XmTextPosition replace_len;
static char *str;
static XmTextPosition str_len;
XmTextPosition pos = -2L;
XmTextPosition old_pos = -1L;
XmTextPosition start, end;

#ifdef _GNU_RX
struct re_pattern_buffer p_buf;
struct re_registers regs;
char fastmap[(1 << BYTEWIDTH)];
XmTextPosition endpos;
#else
regex_t re;
regmatch_t pm;
int re_status;
int r_so = -1, r_eo = -1;
int n_ptr;
#endif

#ifdef _KOR
char *notFound = "Ã£?? ???Ú¿??? ???À´Ï´?.";
#else
char *notFound = "Search string not found.";
#endif

Boolean case_sensitive = True;		/* Case sensitive */
Boolean whole_words_only = False;	/* Whole words only */
Boolean regular_exp = False;		/* Regular exporession */
Boolean forward = True;			/* Forward  or Backward */
int	SearchStart = 0;
Boolean Start_new = False;
Widget  text_w;

#ifdef _NO_PROTO
void toggle_cb();
void setStart_cb();
void search_cb();
void closedialog_cb();
void valueChange_cb();
#else /* _NO_PROTO */
void toggle_cb(Widget, XtPointer, XtPointer);
void setStart_cb(Widget, XtPointer, XtPointer);
void search_cb(Widget, XtPointer, XtPointer);
void closedialog_cb(Widget, XtPointer, XtPointer);
void valueChange_cb(Widget, XtPointer, XtPointer);
#endif

#ifdef _NO_PROTO
Widget smCreateSearchDialog(parent, name, text_widget)
Widget parent;
char *name;
Widget text_widget;
#else
Widget smCreateSearchDialog(Widget parent, char *name, Widget text_widget)
#endif
{
	Arg args[4];
	int arg;

	text_w = text_widget;

	arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage,	False); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	searchDialog.form = XmCreateFormDialog(parent, name, args, arg);

        searchDialog.frame_input = XtVaCreateWidget("Frame",
                xmFrameWidgetClass,     searchDialog.form,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		10,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		10,
                NULL);

	searchDialog.form_input = XtVaCreateWidget("form_input",
		xmFormWidgetClass,	searchDialog.frame_input,
		NULL);

	searchDialog.find_w = XtVaCreateManagedWidget("FindText",
		xmTextWidgetClass, searchDialog.form_input,
		XmNeditMode,		XmSINGLE_LINE_EDIT,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		3,
		NULL);

        searchDialog.label_f = XtVaCreateManagedWidget("FindLabel",
                xmLabelGadgetClass,     searchDialog.form_input,
		XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,		searchDialog.find_w,
		XmNrightAttachment,	XmATTACH_WIDGET,
		XmNrightWidget,		searchDialog.find_w,
		XmNrightOffset,		3,
		XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget,	searchDialog.find_w,
		NULL);

        searchDialog.replace_w = XtVaCreateManagedWidget("ReplaceText",
		xmTextWidgetClass,	searchDialog.form_input,
		XmNeditMode,		XmSINGLE_LINE_EDIT,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		searchDialog.find_w,
		XmNtopOffset,		3,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		3,
		XmNbottomAttachment,	XmATTACH_FORM,
		NULL);

        searchDialog.label_r = XtVaCreateManagedWidget("ReplaceLabel",
                xmLabelGadgetClass,     searchDialog.form_input,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		3,
		XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,		searchDialog.replace_w,
		XmNrightAttachment,	XmATTACH_WIDGET,
		XmNrightWidget,		searchDialog.replace_w,
		XmNrightOffset,		3,
		XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget,	searchDialog.replace_w,
                NULL);

	XtManageChild(searchDialog.form_input);
	XtManageChild(searchDialog.frame_input);

	searchDialog.set_origin = XmVaCreateSimpleOptionMenu(searchDialog.form,
		"set_origin",
		(XmString)NULL, (KeySym)NULL, 0, setStart_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		45,
                XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		searchDialog.frame_input,
		NULL);

	XtManageChild(searchDialog.set_origin);

	searchDialog.label[0] = XtVaCreateManagedWidget("FindOptionsLabel",
		xmLabelGadgetClass,	searchDialog.form,
                XmNleftAttachment,	XmATTACH_FORM,
                XmNleftOffset,		10,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNtopWidget,		searchDialog.set_origin,
                XmNtopOffset,		2,
		NULL);

        searchDialog.frame[0] = XtVaCreateWidget("Frame",
                xmFrameWidgetClass,     searchDialog.form,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNleftAttachment,	XmATTACH_FORM,
                XmNleftOffset,		10,
                XmNtopWidget,		searchDialog.label[0],
                XmNbottomAttachment,	XmATTACH_FORM,
                XmNbottomOffset,	10,
                NULL);

	searchDialog.rowcol_toggle = XtVaCreateWidget("FindRowCol",
		xmRowColumnWidgetClass,	searchDialog.frame[0],
		XmNorientation,		XmVERTICAL,
		XmNentryAlignment,	XmALIGNMENT_BEGINNING,
		NULL);

        searchDialog.toggle[0] = XtVaCreateManagedWidget("Toggle1",
                xmToggleButtonGadgetClass, searchDialog.rowcol_toggle,
                XmNset,			case_sensitive,
                NULL);

	XtAddCallback(searchDialog.toggle[0], XmNvalueChangedCallback,
			toggle_cb, (XtPointer)NULL);

        searchDialog.toggle[1] = XtVaCreateManagedWidget("Toggle2",
                xmToggleButtonGadgetClass, searchDialog.rowcol_toggle,
                XmNset,			regular_exp,
                NULL);

	XtAddCallback(searchDialog.toggle[1], XmNvalueChangedCallback,
			toggle_cb, (XtPointer)NULL);

        searchDialog.toggle[2] = XtVaCreateManagedWidget("Toggle3",
                xmToggleButtonGadgetClass, searchDialog.rowcol_toggle,
                XmNset,			whole_words_only,
                NULL);

	XtAddCallback(searchDialog.toggle[2], XmNvalueChangedCallback,
			toggle_cb, (XtPointer)1);

	XtManageChild(searchDialog.rowcol_toggle);
	XtManageChild(searchDialog.frame[0]);

	searchDialog.label[1] = XtVaCreateManagedWidget("FindDirectionLabel",
		xmLabelGadgetClass,	searchDialog.form,
                XmNleftAttachment,	XmATTACH_WIDGET,
                XmNleftWidget,       	searchDialog.frame[0],
                XmNleftOffset,		10,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNtopWidget,		searchDialog.set_origin,
                XmNtopOffset,		2,
		NULL);

        searchDialog.frame[1] = XtVaCreateWidget("Frame",
                xmFrameWidgetClass,     searchDialog.form,
                XmNleftAttachment,	XmATTACH_WIDGET,
                XmNleftWidget,		searchDialog.frame[0],
                XmNleftOffset,		10,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNtopWidget,		searchDialog.label[1],
                XmNrightAttachment,	XmATTACH_OPPOSITE_WIDGET,
                XmNrightWidget,       	searchDialog.frame_input,
                NULL);

        searchDialog.radio_box = XmCreateRadioBox(searchDialog.frame[1], "RadioBox",
                        (ArgList)NULL, 0);

        searchDialog.radio_t[0] = XtVaCreateManagedWidget("Radio1",
                xmToggleButtonGadgetClass, searchDialog.radio_box,
                XmNset,			True,
                NULL);

	XtAddCallback(searchDialog.radio_t[0], XmNvalueChangedCallback,
			toggle_cb,	(XtPointer)NULL);

        searchDialog.radio_t[1] = XtVaCreateManagedWidget("Radio2",
                xmToggleButtonGadgetClass, searchDialog.radio_box,
                XmNset,			False,
                NULL);

	XtAddCallback(searchDialog.radio_t[1], XmNvalueChangedCallback,
			toggle_cb,	(XtPointer)NULL);

        XtManageChild(searchDialog.radio_box);
        XtManageChild(searchDialog.frame[1]);

	searchDialog.Find = XtVaCreateManagedWidget("FindB",
		xmPushButtonWidgetClass, searchDialog.form,
		XmNmarginWidth,		20,
		XmNsensitive,         	False,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNtopOffset,		10,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		searchDialog.frame_input,
		XmNleftOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		NULL);

	XtAddCallback(searchDialog.Find,
	        XmNactivateCallback, search_cb, (XtPointer)0);

        searchDialog.Replace = XtVaCreateManagedWidget("ReplaceB",
		xmPushButtonWidgetClass, searchDialog.form,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,         	searchDialog.Find,
		XmNtopOffset,		0,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		searchDialog.frame_input,
		XmNleftOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		NULL);

        XtAddCallback(searchDialog.Replace,
                XmNactivateCallback, search_cb, (XtPointer)1);

        searchDialog.ReplaceAll = XtVaCreateManagedWidget("ReplaceAllB",
		xmPushButtonWidgetClass, searchDialog.form,
                XmNtopWidget,           searchDialog.Replace,
                XmNleftWidget,          searchDialog.frame_input,
                XmNtopAttachment,	XmATTACH_WIDGET,
                XmNtopWidget,           searchDialog.Replace,
                XmNtopOffset,		0,
                XmNleftAttachment,	XmATTACH_WIDGET,
                XmNleftWidget,          searchDialog.frame_input,
                XmNleftOffset,	        10,
                XmNrightAttachment,	XmATTACH_FORM,
                XmNrightOffset,	        10,
                NULL);

        XtAddCallback(searchDialog.ReplaceAll,
                XmNactivateCallback, search_cb, (XtPointer)2);

	searchDialog.Close = XtVaCreateManagedWidget("CloseB",
		xmPushButtonWidgetClass, 	searchDialog.form,
		XmNtopAttachment,	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget,         	searchDialog.frame[1],
		XmNtopOffset,		0,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,        	searchDialog.frame_input,
		XmNleftOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		NULL);

	XtAddCallback(searchDialog.Close,
		XmNactivateCallback, closedialog_cb, (XtPointer)NULL);

        XtAddCallback(searchDialog.find_w, XmNmodifyVerifyCallback,
                valueChange_cb, (XtPointer)NULL);

	XtVaSetValues(searchDialog.form,
		XmNdefaultButton,	searchDialog.Find,
		NULL);
/*
	arg = 0;
	XtSetArg(args[arg], XmNmessageString,	str); arg++;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	searchDialog.message_dialog = XmCreateWarningDialog(searchDialog.form, "MessageDialog",
		args, arg);

	XtUnmanageChild(XmMessageBoxGetChild(searchDialog.message_dialog,
                        XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(searchDialog.message_dialog,
                        XmDIALOG_HELP_BUTTON));
*/
	return (searchDialog.form);
}

#ifdef _NO_PROTO
void smManageReplaceText(is_manage)
Boolean is_manage;
#else /* _NO_PROTO */
void smManageReplaceText(Boolean is_manage)
#endif
{
	XtVaSetValues(searchDialog.form,
		XmNnoResize,		False,
		XmNresizePolicy,	XmRESIZE_ANY,
		NULL);
	if (found == True)
	    found = False;
	if (is_manage == True) {
	    if (!XtIsManaged(searchDialog.replace_w)) {
/*	        XtUnmanageChild(searchDialog.form);*/
		if (XtIsSensitive(searchDialog.Find) == False) {
		    XtSetSensitive(searchDialog.Replace, False);
		    XtSetSensitive(searchDialog.ReplaceAll, False);
		}
		XtManageChild(searchDialog.replace_w);
		XtManageChild(searchDialog.label_r);
		XtManageChild(searchDialog.ReplaceAll);
/*		XtManageChild(searchDialog.form);*/
            }
	}
	else {
	    if (XtIsManaged(searchDialog.replace_w)) {
/*	        XtUnmanageChild(searchDialog.form);*/
		XtUnmanageChild(searchDialog.replace_w);
		XtUnmanageChild(searchDialog.label_r);
		XtUnmanageChild(searchDialog.ReplaceAll);
		if (XtIsSensitive(searchDialog.Replace) == False) {
		    XtSetSensitive(searchDialog.Replace, True);
		}
/*		XtManageChild(searchDialog.form);*/
            }
	}
	XtVaSetValues(searchDialog.form,
		XmNnoResize,		True,
		XmNresizePolicy,	XmRESIZE_NONE,
		NULL);
}

#ifdef _NO_PROTO
char *make_regexp(case_sensitive, whole_words_only, str)
Boolean case_sensitive;
Boolean whole_words_only;
char *str;
#else
char *make_regexp(Boolean case_sensitive, Boolean whole_words_only, char *str)
#endif
{
	int i, str_len, tmp_i;
	char *ret_str;
	char *tmp_str, tmp_buf[BUFSIZ];

	str_len = strlen(str);
	tmp_str = XtMalloc(BUFSIZ);
        tmp_i = 0;
	strcpy(tmp_str, "");
	for (i = 0; i < str_len; i++) {
	    if (case_sensitive == True) {
		switch (str[i]) {
		    case '[' : case ']' :
		    case '{' : case '}' :
		    case '$' : case '?' : case '+' :
		    case '.' : case '^' :
			sprintf(tmp_buf, "\\%c", str[i]);
			break;
		    case '\\' :
			sprintf(tmp_buf, "%c%c", str[i], str[i]);
			break;
		    default :
			sprintf(tmp_buf, "%c", str[i]);
		}
	    }
	    else {
		switch (str[i]) {
		    case '^' :
			sprintf(tmp_buf, "\\%c", str[i]);
			break;
		    default :
			sprintf(tmp_buf, "[%c%c]",
		    	    toupper((int)str[i]), tolower((int)str[i]));
			break;
		}
	    }
	    strcat(tmp_str + tmp_i, tmp_buf);
	    tmp_i += strlen(tmp_buf);
	    tmp_str = XtRealloc(tmp_str, BUFSIZ + tmp_i);
	}
	tmp_str[tmp_i] = '\0';
	str_len = (whole_words_only == True) ? tmp_i + 10 : tmp_i;
	ret_str = XtMalloc(str_len);
	sprintf(ret_str, "%s%s%s",
		(whole_words_only == True) ? "[ \t]" : "",
		tmp_str,
		(whole_words_only == True) ? "[ \t]" : "");
	XtFree(tmp_str);

	return (ret_str);
}

#ifdef _NO_PROTO
void find_string(Widget w,
	Boolean case_sensitive, Boolean whole_words_only, Boolean regular_exp, Boolean forward)
#else
void find_string(w, case_sensitive, whole_words_only, regular_exp, forward)
Widget w;
Boolean case_sensitive;
Boolean whole_words_only;
Boolean regular_exp;
Boolean forward;
#endif
{
	char *buf;
#ifdef _GNU_RX
        const char *compile_ret;
#endif

	search_str = XmTextGetString(searchDialog.find_w);
	search_len = XmTextGetLastPosition(searchDialog.find_w);

	if (search_len == 0) return;
#ifdef _GNU_RX
	p_buf.allocated = 0;
	p_buf.buffer = NULL;
	p_buf.fastmap = fastmap;
#endif
	if (regular_exp)
#ifdef _GNU_RX
	    compile_ret = re_compile_pattern(search_str, strlen(search_str), &p_buf);
#else
	    re_status = regcomp(&re, search_str, REG_EXTENDED);
#endif
	else {
	    regext_str = make_regexp(case_sensitive, whole_words_only, search_str);
#ifdef _GNU_RX
	    compile_ret = re_compile_pattern(regext_str, strlen(regext_str), &p_buf);
#else
	    re_status = regcomp(&re, regext_str, REG_EXTENDED);
#endif
	    XtFree(regext_str);
	}
#ifdef _GNU_RX
	if (compile_ret != NULL) {
	    buf = XtMalloc((Cardinal)strlen(compile_ret) + 1);
	    (void)sprintf(buf, "%s", compile_ret);
	    setDialogMessage(searchDialog.message_dialog, buf);
	    XtManageChild(searchDialog.message_dialog);
	    XtFree(buf);
	    XtFree(search_str);
	    return;
	}
#else
	if (re_status) {
	    buf = XtMalloc((Cardinal)BUFSIZ);
	    regerror(re_status, &re, buf, BUFSIZ);
	    smStautsLineSetMessage(buf);
	    XBell(XtDisplay(w), 100);
/*
	    setDialogMessage(searchDialog.message_dialog, buf);
	    XtManageChild(searchDialog.message_dialog);
*/
	    XtFree(buf);
	    XtFree(search_str);
	    return;
	}
#endif
	if (Start_new) {
	    switch (SearchStart) {
	        case 0 : /* From cursor */
	            pos = XmTextGetInsertionPosition(w);
                    if (found) {
                        if (!forward)
#ifdef _GNU_RX
			    pos -= (regs.end[0] - regs.start[0]) + 1;
#else
			    pos -= (r_eo - r_so) + 1;
#endif
			found = False;
                    }
		    else
			if (!forward) pos -= (r_eo - r_so) - 1;
	            break;
                case 1 : /* From beginning */
		    pos = (forward) ? 0 : XmTextGetLastPosition(w);
 		    break;
	    } /* switch */
	    Start_new = False;
	}
	else {
	    pos = XmTextGetInsertionPosition(w);
	    if (found) {
	        if (!forward)
#ifdef _GNU_RX
		    pos -= (regs.end[0] - regs.start[0]) + 1;
#else
		    pos -= (r_eo - r_so) + 1;
#endif
		found = False;
            }
	    else
		if (!forward) pos -= (r_eo - r_so) - 1;
	}
	r_so = r_eo = -1;
	str = XmTextGetString(w);
	str_len = XmTextGetLastPosition(w);
#ifdef _GNU_RX
	endpos = (forward) ? str_len - 1 : -str_len + 1;
	pos = re_search(&p_buf, str, str_len, pos, endpos, &regs);
	if (pos >= 0) {
	    found = True;
	    start = (regular_exp) ? regs.start[0] :
		(whole_words_only) ? regs.start[0] + 1 : regs.start[0];
	    end = (regular_exp) ? regs.end[0] :
		(whole_words_only) ? regs.end[0] - 1: regs.end[0];
	    smTextSetInsertionPosition(text_w, start);
	    XmTextSetSelection(text_w, start, end, CurrentTime);
	}
#else
	if (forward) {
	    re_status = regexec(&re, str + pos, (size_t)1, &pm, 0);
	    r_so = pm.rm_so;
	    r_eo = pm.rm_eo;
	}
	else {
	    n_ptr = 0;
	    while ((re_status = regexec(&re, str + n_ptr, (size_t)1, &pm, 0)) == 0 &&
		(n_ptr + pm.rm_eo - 1) < pos) {
		r_so = pm.rm_so + n_ptr;
		r_eo = pm.rm_eo + n_ptr;
		n_ptr += pm.rm_eo;
	    }
	    if (r_so >= 0 && r_eo >= 0) re_status = 0;
	    else if (r_so < 0 && r_so < 0) re_status = 1;
	}
	if (re_status == 0) {
	    found = True;
	    start = ((forward) ? pos : 0) + ((regular_exp) ? r_so :
		(whole_words_only) ? r_so + 1 : r_so);
	    end = ((forward) ? pos : 0) + ((regular_exp) ? r_eo :
		(whole_words_only) ? r_eo - 1 : r_eo);
	    smTextSetInsertionPosition(text_w, start);
	    XmTextSetSelection(text_w, start, end, CurrentTime);
	}
	else {
	    smStautsLineSetMessage(notFound);
	    XBell(XtDisplay(w), 100);
/*
	    setDialogMessage(searchDialog.message_dialog, notFound);
	    XtManageChild(searchDialog.message_dialog);
*/
	}
#endif
#ifndef _GNU_RX
	regfree(&re);
#endif
	XtFree(search_str);
	XtFree(str);
}

#ifdef _NO_PROTO
void search_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void search_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
        XmPushButtonCallbackStruct *cbs =
        		(XmPushButtonCallbackStruct *)call_data;
        int item_no = (int)client_data;
	XmTextPosition left, right;
#ifdef _GNU_RX
	const char *compile_ret;
#endif
	char *buf;

	if (XmTextGetSelectionPosition(text_w, &left, &right) == True)
	    XmTextClearSelection(text_w, cbs->event->xbutton.time);

	case_sensitive = XmToggleButtonGadgetGetState(searchDialog.toggle[0]);
	whole_words_only = XmToggleButtonGadgetGetState(searchDialog.toggle[1]);
	regular_exp = XmToggleButtonGadgetGetState(searchDialog.toggle[2]);
	forward = XmToggleButtonGadgetGetState(searchDialog.radio_t[0]);

	switch (item_no) {
	    case 0 :	/* Find */
		find_string(text_w, case_sensitive, whole_words_only, regular_exp, forward);
		break;
	    case 1 :	/* Replace */
	        if (!XtIsManaged(searchDialog.replace_w))
	            smManageReplaceText(True);
	        else {
		    if (found) {
		        replace_str = XmTextGetString(searchDialog.replace_w);
		        smTextReplace(text_w, start, end, replace_str);
		        old_pos = XmTextGetInsertionPosition(text_w);
		        XtFree(replace_str);
		        found = False;
		    }
		    else {
			smStautsLineSetMessage(notFound);
			XBell(XtDisplay(w), 100);
/*
		        setDialogMessage(searchDialog.message_dialog, notFound);
		        XtManageChild(searchDialog.message_dialog);
*/
                    }
		}
		break;
          case 2 :	/* Replace All */

                search_str = XmTextGetString(searchDialog.find_w);
                search_len = XmTextGetLastPosition(searchDialog.find_w);
#ifdef _GNU_RX
		p_buf.allocated = 0;
		p_buf.buffer = NULL;
		p_buf.fastmap = fastmap;
#endif
		if (regular_exp)
#ifdef _GNU_RX
		    compile_ret = re_compile_pattern(search_str, strlen(search_str), &p_buf);
#else
		    re_status = regcomp(&re, search_str, REG_EXTENDED);
#endif
		else {
		    regext_str = make_regexp(case_sensitive, whole_words_only, search_str);
#ifdef _GNU_RX
		    compile_ret = re_compile_pattern(regext_str, strlen(regext_str), &p_buf);
#else
		    re_status = regcomp(&re, regext_str, REG_EXTENDED);
#endif
		    XtFree(regext_str);
		}
#ifdef _GNU_RX
		if (compile_ret != NULL) {
		    buf = XtMalloc((Cardinal)strlen(compile_ret) + 1);
		    (void)sprintf(buf, "%s", compile_ret);
		    setDialogMessage(searchDialog.message_dialog, buf);
		    XtManageChild(searchDialog.message_dialog);
		    XtFree(buf);
		    XtFree(search_str);
		    return;
		}
#else
		if (re_status) {
		    buf = XtMalloc((Cardinal)BUFSIZ);
		    regerror(re_status, &re, buf, BUFSIZ);
		    smStautsLineSetMessage(buf);
		    XBell(XtDisplay(w), 100);
/*
		    setDialogMessage(searchDialog.message_dialog, buf);
		    XtManageChild(searchDialog.message_dialog);
*/
		    XtFree(buf);
		    XtFree(search_str);
		    return;
		}
#endif
                replace_str = XmTextGetString(searchDialog.replace_w);
                replace_len = XmTextGetLastPosition(searchDialog.replace_w);
                if (found) {
                    smTextReplace(text_w, start, end, replace_str);
                    found = False;
                }
                if (Start_new) Start_new = False;
                if (SearchStart == 0)
                    pos = XmTextGetInsertionPosition(text_w);
                else
                    pos = (forward) ? 0 : XmTextGetLastPosition(text_w);
#ifdef __GNU_RX
                while (pos >= 0) {
#else
                while (re_status == 0) {
#endif
                    str = XmTextGetString(text_w);
                    str_len = XmTextGetLastPosition(text_w);
#ifdef _GNU_RX
		    endpos = (forward) ? str_len - 1 : -str_len + 1;
		    pos = re_search(&p_buf, str, str_len, pos, endpos, &regs);
		    if (pos >= 0) {
			found = True;
			start = (regular_exp) ? regs.start[0] :
			    (whole_words_only) ? regs.start[0] + 1 : regs.start[0];
			end = (regular_exp) ? regs.end[0] :
			    (whole_words_only) ? regs.end[0] - 1: regs.end[0];
			smTextReplace(text_w, start, end, replace_str);
			if (forward) pos += replace_len;
		    }
#else
		    re_status = regexec(&re, str + pos, (size_t)1, &pm, 0);
		    if (re_status == 0) {
			found = True;
			start = pos + ((regular_exp) ? pm.rm_so :
			    (whole_words_only) ? pm.rm_so + 1 : pm.rm_so);
			end = pos + ((regular_exp) ? pm.rm_eo :
			    (whole_words_only) ? pm.rm_eo - 1 : pm.rm_eo);
			smTextSetInsertionPosition(text_w, start);
			XmTextSetSelection(text_w, start, end, cbs->event->xbutton.time);
			smTextReplace(text_w, start, end, replace_str);
			if (forward) pos += replace_len;
		    }
#endif
                    XtFree(str);
                }
                smTextSetInsertionPosition(text_w, start);
                XtFree(replace_str);
                XtFree(search_str);
#ifndef _GNU_RX
		regfree(&re);
#endif
		break;
	}
}

#ifdef _NO_PROTO
void findNext(w)
Widget w;
#else
void findNext(Widget w)
#endif
{
	XmTextPosition left, right;

	if (XmTextGetSelectionPosition(w, &left, &right) == True)
	    XmTextClearSelection(w, CurrentTime);

	case_sensitive = XmToggleButtonGadgetGetState(searchDialog.toggle[0]);
	whole_words_only = XmToggleButtonGadgetGetState(searchDialog.toggle[1]);
	regular_exp = XmToggleButtonGadgetGetState(searchDialog.toggle[2]);
	forward = XmToggleButtonGadgetGetState(searchDialog.radio_t[0]);

	find_string(w, case_sensitive, whole_words_only, regular_exp, forward);
}

#ifdef _NO_PROTO
void findString(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void findString(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XmTextPosition left, right;

	if (XmTextGetSelectionPosition(w, &left, &right) == True)
	    XmTextClearSelection(w, CurrentTime);

	case_sensitive = XmToggleButtonGadgetGetState(searchDialog.toggle[0]);
	whole_words_only = XmToggleButtonGadgetGetState(searchDialog.toggle[1]);
	regular_exp = XmToggleButtonGadgetGetState(searchDialog.toggle[2]);

	if (!strcmp(params[0], "forward"))
	    forward = True;
	else
	    forward = False;
	XmToggleButtonGadgetSetState(searchDialog.radio_t[0], forward, True);
	find_string(w, case_sensitive, whole_words_only, regular_exp, forward);
}

#ifdef _NO_PROTO
void ReplaceFindString(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void ReplaceFindString(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	if (found) {
	    XmTextClearSelection(w, CurrentTime);
  	    search_len = XmTextGetLastPosition(searchDialog.find_w);
	    replace_str = XmTextGetString(searchDialog.replace_w);
	    smTextReplace(text_w, start, end, replace_str);
	    XtFree(replace_str);
	    found = False;
	}
	else {
	    smStautsLineSetMessage(notFound);
	    XBell(XtDisplay(w), 100);
/*
	    setDialogMessage(searchDialog.message_dialog, notFound);
	    XtManageChild(searchDialog.message_dialog);
*/
	}
}

#ifdef _NO_PROTO
void closedialog_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else /* _NO_PROTO */
void closedialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmPushButtonCallbackStruct *cbs =
        		(XmPushButtonCallbackStruct *)call_data;

	if (found) {
	    XmTextClearSelection(text_w,
	  	cbs->event->xbutton.time);
	    found = False;
	}
	XtUnmanageChild(XtParent(w));
}

#ifdef _NO_PROTO
void SelectionToFind(w)
Widget w;
#else
void SelectionToFind(Widget w)
#endif
{
	char *string;
	int size;
	Boolean is_selected;
	XmTextPosition start_p, end_p;

	is_selected = XmTextGetSelectionPosition(w, &start_p, &end_p);
	if (is_selected == True && start_p != end_p) {
	    size = (int)(end_p - start_p);
	    string = XtMalloc((Cardinal)size + 1);
	    if (XmTextGetSubstring(w, start_p, size, size + 1, string) == XmCOPY_SUCCEEDED)
                XmTextSetString(searchDialog.find_w, string);
	    XtFree(string);
	}
}

#ifdef _NO_PROTO
void toggle_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void toggle_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	int c_data = (int)client_data;
	XmToggleButtonCallbackStruct *cbs =
        		(XmToggleButtonCallbackStruct *)call_data;

	if (cbs->reason == XmCR_VALUE_CHANGED)
	    Start_new = True;
	if (c_data == 1) {
	    XtSetSensitive(searchDialog.toggle[0], (cbs->set == True) ? False : True);
	    XtSetSensitive(searchDialog.toggle[1], (cbs->set == True) ? False : True);
	}
}

#ifdef _NO_PROTO
void setStart_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void setStart_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	SearchStart = (int)client_data;
	Start_new = True;
}

#ifdef _NO_PROTO
void valueChange_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void valueChange_cb(Widget w, XtPointer client_data, XtPointer call_data)
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
	Start_new = True;

	XtSetSensitive(searchDialog.Find, In_string);
	if (XtIsManaged(searchDialog.replace_w)) {
	    XtSetSensitive(searchDialog.Replace, In_string);
	    XtSetSensitive(searchDialog.ReplaceAll, In_string);
	}
}

