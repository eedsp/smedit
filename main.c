/* @(#)main.c 1.8 01/01/99 */
/*
 * Copyright (c) 1991-1998 Shiwon Cho.  All rights reserved.
 *
 *  main.c - part of SM/Editor
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
#include <string.h>
#ifdef hpux
#include <strings.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <Xm/Protocols.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RepType.h>
#include <Xm/DragDrop.h>
#include <Xm/PanedW.h>

#include "version.h"
#include "search.h"
#include "help.h"
#include "strctrl.h"
#include "dialog.h"
#include "text.h"

#include "icons/new.xpm"
#include "icons/open.xpm"
#include "icons/save.xpm"
#include "icons/undo.xpm"
#include "icons/redo.xpm"
#include "icons/cut.xpm"
#include "icons/copy.xpm"
#include "icons/paste.xpm"
#include "icons/delete.xpm"
#include "icons/search.xpm"
#include "icons/blocki.xpm"
#include "icons/blocku.xpm"

#define APP_NAME	"SM/Editor"
#define	RES_PATH	".smed"
#define	INIT_FILE	"init"
#define HIST_FILE	"history"

#define WARNING_NEW	0
#define WARNING_OPEN	1
#define WARNING_LIST	2
#define WARNING_QUIT	3

#define SAVE_DEFAULT	0
#define SAVE_NEW	1
#define SAVE_OPEN	2
#define SAVE_LIST	3
#define SAVE_QUIT	4

#define FILE_SAVE	0
#define FILE_OPEN	1

#define MAXLIST		10

/*	Global Widgets		*/
Widget	toplevel;
Widget	main_w, menubar, form_main;
Widget	paned_w, copy_text_w;
Widget	file_menu, edit_menu, search_menu, option_menu, help_menu;
Widget	pick_submenu, pick_button[MAXLIST], option_editor, option_file;
Widget	popup_menu, popup_submenu;
Widget	text_w;
Widget	form_status;

Widget	OpenDialog, SaveAsDialog;
Widget	OverwriteDialog, QuestionDialog, ErrorDialog;
Widget	go_dialog;
Widget	SearchDialog;
Widget	tab_dialog, block_dialog, backup_dialog;
Widget	help_dialog, about_dialog;
Widget	toolbar;

static Atom wm_quit_Atom, wm_delw_Atom = 0;
XmTextSource text_source;

struct ToolbarRec {
	char	*name;
	int	leftOffset;
	char	**xpm;
};

/*	Global Variables	*/
char	*OpenName = NULL;
char	*backupExt;
static	XmTextPosition bookmark[10];	/* Place Mark Position */
Boolean create_backup = True;		/* Create BackUp File	*/
Boolean optimize_text = True;		/* Optimize Text Mode	*/
Boolean pending_delete = True;		/* Pending delete Mode	*/
Boolean show_toolbar = True;		/* Show Toolbar		*/
Boolean autosave_env = True;		/* Autosave Environment*/
int	SaveNum;			/* case of Save As...	*/
int	WarningNum;			/* case of Waring DialogBox */
int	file_count = 0;			/* No. of  file list */

/*	Global Functions	*/
void	usage(char *);
void	add_recent_file(char *);
void	set_title(char *);
Boolean open_file(char *);
Boolean save_file(char *);

void	load_option();
void	load_Env();

Widget	CreateMenuBar(Widget);
Widget	CreateToolbar(Widget);

/*	XtEventHandler	*/
void	buttonEvnet_proc(Widget, XtPointer, XButtonPressedEvent *, Boolean *);

/*	MenuBar Callback Proc	*/
void	FileMenu_cb(Widget, XtPointer, XtPointer);
void	EditMenu_cb(Widget, XtPointer, XtPointer);
void	SearchMenu_cb(Widget, XtPointer, XtPointer);
void	OptionMenu_cb(Widget, XtPointer, XtPointer);
void	EnvMenu_cb(Widget, XtPointer, XtPointer);
void	PrefMenu_cb(Widget, XtPointer, XtPointer);
void	PopupMenu_cb(Widget, XtPointer, XtPointer);
void	PopupSubMenu_cb(Widget, XtPointer, XtPointer);
void	HelpMenu_cb(Widget, XtPointer, XtPointer);

void	eventProc_cb(Widget, XtPointer, XEnterWindowEvent *, Boolean *);
void	active_cb(Widget, XtPointer, XtPointer);

void	pick_cb();
void	pick_cb(Widget, XtPointer, XtPointer);

/*	Widget Callback Proc	*/
void	CloseBox(Widget, XtPointer, XtPointer);
void	open_cb(Widget, XtPointer, XtPointer);
void	SaveAs_cb(Widget, XtPointer, XtPointer);
void	OpenDir_cb(Widget, XtPointer, XtPointer);
void	OverWrite_cb(Widget, XtPointer, XtPointer);
void	yes_cb(Widget, XtPointer, XtPointer);
void	no_cb(Widget, XtPointer, XtPointer);
void	go_cb(Widget, XtPointer, XtPointer);
void	settab_cb(Widget, XtPointer, XtPointer);
void	setblock_cb(Widget, XtPointer, XtPointer);
void	setbackup_cb(Widget, XtPointer, XtPointer);

/*	Translation Action Proc */
void	with_Ctrl_K(Widget, XEvent *, String *, Cardinal *);
void	with_Ctrl_Q(Widget, XEvent *, String *, Cardinal *);
void	proc_Options(Widget, XEvent *, String *, Cardinal *);

/*	Translation Action Table		*/
static XtActionsRec text_actions[] = {
	{"replace-find-string", (XtActionProc) ReplaceFindString},
	{"find-string", (XtActionProc) findString},
	{"refresh-text", (XtActionProc) RefreshTextWidget},
	{"UndoLast", (XtActionProc) UndoLast},
	{"UndoAll", (XtActionProc) UndoAll},
	{"RedoAction", (XtActionProc) RedoAction},
	{"process-Ctrl-K", (XtActionProc) with_Ctrl_K},
	{"process-Ctrl-Q", (XtActionProc) with_Ctrl_Q},
	{"process-Options", (XtActionProc) proc_Options},
	{"Process-tab", (XtActionProc) process_tab},
	{"block-indent", (XtActionProc) BlockIndent},
	{"block-unindent", (XtActionProc) BlockUnindent},
	{"move-top", (XtActionProc) move_top},
	{"move-bottom", (XtActionProc) move_bottom},
	{"delete-line", (XtActionProc) delete_line},
	{"Delete-next-word", (XtActionProc) delete_next_word},
	{"Delete-previous-word", (XtActionProc) delete_previous_word},
	{"word-left", (XtActionProc) word_left},
	{"word-right", (XtActionProc) word_right},
	{"show-info", (XtActionProc) show_info},
	{NULL, NULL}
};

static char *set_unindent = "#override \n\
	<Key>Return:		newline()\n\
";

static char *set_indent = "#override \n\
	<Key>Return:		newline-and-indent()\n\
";

static char *optimal_fill = "#override \n\
	<Key>Tab:		process-tab()\n\
";

static char *normal_fill = "#override \n\
	<Key>Tab:		Process-tab()\n\
";

#ifdef _NO_PROTO
void main(argc, argv)
int argc;
char *argv[];
#else		/* _NO_PROTO */
void main(int argc, char *argv[])
#endif
{
	XtAppContext app_context;
	Arg	args[10];
	int	arg;
	XmString str;
	char *buf;
	int i, line;
	Boolean is_Empty;

	TabSize = 8;
	backupExt = XtNewString("~");
	load_option();		/* Load Option data file */

	setlocale(LC_ALL, "");
	XtSetLanguageProc((XtAppContext)NULL, (XtLanguageProc)NULL,
                   (XtPointer)NULL );

	XmRepTypeInstallTearOffModelConverter();

	toplevel = XtAppInitialize(&app_context, "smedit",
			(XrmOptionDescList)NULL, 0,
			&argc, argv,
			(String *)NULL, (ArgList)NULL, 0);

	main_w = XtCreateWidget("main_w",
		xmMainWindowWidgetClass, toplevel, (ArgList)NULL, 0);

	menubar = CreateMenuBar(main_w);
	XtManageChild(menubar);

	toolbar = CreateToolbar(main_w);

/*	undo_b = XtNameToWidget(toolbar, "undo_b");
	redo_b = XtNameToWidget(toolbar, "redo_b");*/

	XtManageChild(toolbar);

	form_main = XtVaCreateWidget("form_main",
		xmFormWidgetClass,	main_w,
		XmNmarginHeight,        0,
                XmNmarginWidth,         0,
		NULL);

	form_status = smCreateStatusLine(form_main, "form_status");
	XtManageChild(form_status);

	paned_w = XtVaCreateWidget("paned_w",
		xmPanedWindowWidgetClass, form_main,
		XmNmarginWidth,		0,
		XmNmarginHeight,	1,
		XmNspacing,		0,
		XmNseparatorOn,		True,
		XmNsashHeight,		5,
		XmNsashWidth,		12,
		XmNsashIndent,		-1,
		XmNpaneMinimum,		0,
		XmNleftAttachment,  	XmATTACH_FORM,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNtopAttachment,   	XmATTACH_FORM,
		XmNtopOffset,       	0,
		XmNbottomAttachment,    XmATTACH_WIDGET,
		XmNbottomWidget,    	form_status,
		XmNbottomOffset,    	0,
		NULL);

	arg = 0;
	XtSetArg(args[arg], XmNeditMode,		XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNheight,			1); arg++;
	XtSetArg(args[arg], XmNscrollHorizontal,	False); arg++;
	XtSetArg(args[arg], XmNhighlightThickness,	1); arg++;

	copy_text_w = XmCreateScrolledText(paned_w, "text_w", args, arg);

	XtAddCallback(copy_text_w, XmNmotionVerifyCallback, Motion_cb, (XtPointer)NULL);
	XtAddCallback(copy_text_w, XmNmodifyVerifyCallback, Modify_cb, (XtPointer)NULL);

	XtVaSetValues(XtParent(copy_text_w),
		XmNspacing,	0,
		NULL);

	arg = 0;
	XtSetArg(args[arg], XmNeditMode,	XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNhighlightThickness,	1); arg++;

	text_w = XmCreateScrolledText(paned_w, "text_w", args, arg);

	XtAddCallback(text_w, XmNmotionVerifyCallback, Motion_cb, (XtPointer)NULL);
	XtAddCallback(text_w, XmNmodifyVerifyCallback, Modify_cb, (XtPointer)NULL);

	XtVaSetValues(XtParent(text_w),
		XmNspacing,	0,
		NULL);

	text_source = XmTextGetSource(text_w);
	XmTextSetSource(copy_text_w, text_source, 0, 0);

	XtAppAddActions(app_context, text_actions, XtNumber(text_actions));
	/*
	 *	Create Popup Menu
	 */

	popup_menu = XmVaCreateSimplePopupMenu(form_main,
		"popup_menu", PopupMenu_cb,
		XmVaTITLE,	NULL,
		XmVaSEPARATOR,
		XmVaCASCADEBUTTON, NULL, NULL,		/* Undo 	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Redo		*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Cut		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Copy		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Paste	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Clear	*/
		XmNpopupEnabled,	False,
		NULL);

	popup_undo_button = XtNameToWidget(popup_menu, "button_0");
	popup_redo_button = XtNameToWidget(popup_menu, "button_1");

	popup_submenu = XmVaCreateSimplePulldownMenu(popup_menu,
		"popup_submenu", 0, PopupSubMenu_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Undo Last	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Undo All	*/
		NULL);

	XtVaSetValues(XtNameToWidget(popup_menu, "button_0"),
		XmNsubMenuId, popup_submenu,
		NULL);

	XtAddEventHandler(text_w, ButtonPressMask, False,
		(XtEventHandler)buttonEvnet_proc,
		(XtPointer)popup_menu);

	/*
	 *	Manage TextWidget
	 */

	XtManageChild(text_w);
	XtManageChild(copy_text_w);
	XtManageChild(paned_w);

	XtVaSetValues(main_w,
		 XmNshowSeparator, True,
		 NULL);

	if (show_toolbar) {
	    XmMainWindowSetAreas(main_w, menubar, toolbar, NULL, NULL, form_main);
	}
	else {
	    XtUnmanageChild(toolbar);
	    XmMainWindowSetAreas(main_w, menubar, NULL, NULL, NULL, form_main);
	}

	XtVaSetValues(main_w,
		 XmNshowSeparator, False,
		 NULL);


	/*
	 *	Manage FormWidget
	 */

	XtManageChild(form_main);

	/*
	 *	File Open DialogBox
	 */

	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNautoUnmanage,	False); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	OpenDialog = XmCreateFileSelectionDialog(form_main,
		"OpenDialog", args, arg);

	XtUnmanageChild(XmFileSelectionBoxGetChild(OpenDialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(OpenDialog, XmNokCallback, open_cb, (XtPointer)NULL);
	XtAddCallback(OpenDialog, XmNcancelCallback,
			CloseBox, (XtPointer)NULL);

	/*
	 *	Save As... DialogBox
	 */
	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNautoUnmanage,	False); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	SaveAsDialog = XmCreateFileSelectionDialog(form_main,
		"SaveAsDialog", args, arg);

	XtUnmanageChild(XmFileSelectionBoxGetChild(SaveAsDialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(SaveAsDialog, XmNokCallback, SaveAs_cb,
			(XtPointer)NULL);
	XtAddCallback(SaveAsDialog, XmNapplyCallback, OpenDir_cb,
			(XtPointer)NULL);
	XtAddCallback(SaveAsDialog, XmNcancelCallback,
			CloseBox, (XtPointer)NULL);

	/*
	 *	Overwrite Question DialogBox
	 */
	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNautoUnmanage,	False); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	OverwriteDialog = XmCreateQuestionDialog(form_main, "overwriteQuestionDialog",
			args, arg);

	XtUnmanageChild(XmMessageBoxGetChild(OverwriteDialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(OverwriteDialog, XmNokCallback, OverWrite_cb,
			(XtPointer)NULL);
	XtAddCallback(OverwriteDialog, XmNcancelCallback,
			CloseBox, (XtPointer)NULL);

	/*
	 *	Question DialogBox
	 */
	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNautoUnmanage,	False); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	QuestionDialog = XmCreateQuestionDialog(form_main, "QuestionDialog",
			args, arg);

	XtAddCallback(QuestionDialog, XmNokCallback, yes_cb, (XtPointer)NULL);
	XtAddCallback(QuestionDialog, XmNcancelCallback, no_cb, (XtPointer)NULL);
	XtAddCallback(QuestionDialog, XmNhelpCallback,
			CloseBox, (XtPointer)NULL);

	/*
	 *	Error Dialog
	 */
	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	ErrorDialog = XmCreateErrorDialog(form_main, "ErrorDialog",
		args, arg);

	XtUnmanageChild(XmMessageBoxGetChild(ErrorDialog,
			XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(ErrorDialog,
			XmDIALOG_HELP_BUTTON));

	/*
	 *	Go Dialog
	 */
	arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage,	True); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;

	go_dialog = XmCreatePromptDialog(form_main, "GoDialog",
			args, arg);

	XtUnmanageChild(XmSelectionBoxGetChild(go_dialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(go_dialog, XmNokCallback, go_cb, (XtPointer)NULL);
	XtAddCallback(go_dialog, XmNcancelCallback,
			CloseDialog_cb, (XtPointer)NULL);
	XtAddCallback(XmSelectionBoxGetChild(go_dialog, XmDIALOG_TEXT),
		XmNmodifyVerifyCallback, SBText_cb,
		(XtPointer)XmSelectionBoxGetChild(go_dialog, XmDIALOG_OK_BUTTON));
	XtSetSensitive(XmSelectionBoxGetChild(go_dialog, XmDIALOG_OK_BUTTON),
		False);

	/*
	 *	Find  & Replace Dialog
	 */

	SearchDialog = smCreateSearchDialog(form_main, "SearchDialog", text_w);

	/*
	 *	TabSize Dialog
	 */

	buf = XtMalloc(BUFSIZ);
	(void)sprintf(buf, "%d", TabSize);
	str = XmStringCreateLocalized(buf);
	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;
	XtSetArg(args[arg], XmNtextString,	str); arg++;

	tab_dialog = XmCreatePromptDialog(form_main, "TabSizeDialog",
			args, arg);

	XtUnmanageChild(XmSelectionBoxGetChild(tab_dialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(tab_dialog, XmNokCallback, settab_cb, (XtPointer)NULL);
	XtAddCallback(tab_dialog, XmNcancelCallback,
			CloseDialog_cb, (XtPointer)NULL);
	XtAddCallback(XmSelectionBoxGetChild(tab_dialog, XmDIALOG_TEXT),
		XmNmodifyVerifyCallback, SBText_cb,
		(XtPointer)XmSelectionBoxGetChild(tab_dialog,
			XmDIALOG_OK_BUTTON));

	XtFree(buf);
	XmStringFree(str);

	/*
	 *	Block Indent Dialog
	 */
	buf = XtMalloc(BUFSIZ);
	(void)sprintf(buf, "%d", BlockIndentSize);
	str = XmStringCreateLocalized(buf);

	arg = 0;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;
	XtSetArg(args[arg], XmNtextString,	str); arg++;

	block_dialog = XmCreatePromptDialog(form_main, "BlockDialog",
			args, arg);

	XtUnmanageChild(XmSelectionBoxGetChild(block_dialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(block_dialog, XmNokCallback, setblock_cb, (XtPointer)NULL);
	XtAddCallback(block_dialog, XmNcancelCallback,
			CloseDialog_cb, (XtPointer)NULL);
	XtAddCallback(XmSelectionBoxGetChild(block_dialog, XmDIALOG_TEXT),
			XmNmodifyVerifyCallback, SBText_cb,
			(XtPointer)XmSelectionBoxGetChild(block_dialog,
			XmDIALOG_OK_BUTTON));
	XtFree(buf);
	XmStringFree(str);

	/*
	 *	Backup Extension Dialog
	 */

	backup_dialog = XmCreatePromptDialog(form_main, "BackupDialog",
			args, arg);

	XtUnmanageChild(XmSelectionBoxGetChild(backup_dialog,
			XmDIALOG_HELP_BUTTON));
	XtAddCallback(backup_dialog, XmNokCallback, setbackup_cb, (XtPointer)NULL);
	XtAddCallback(backup_dialog, XmNcancelCallback,
			CloseDialog_cb, (XtPointer)NULL);
	XtAddCallback(XmSelectionBoxGetChild(backup_dialog, XmDIALOG_TEXT),
			XmNmodifyVerifyCallback, SBText_cb,
			(XtPointer)XmSelectionBoxGetChild(backup_dialog,
			XmDIALOG_OK_BUTTON));

	/*
	 *	Help Dialog
	 */

	help_dialog = CreateHelpDialog(form_main, "HelpDialog");

	/*
	 *	About Dialog
	 */
	arg = 0;
	str = XmStringCreateLtoR(ABOUT_STR, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[arg], XmNmessageString,	str); arg++;
	XtSetArg(args[arg], XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	XtSetArg(args[arg], XmNnoResize,	True); arg++;
	XtSetArg(args[arg], XmNresizePolicy,	XmRESIZE_NONE); arg++;
	XtSetArg(args[arg], XmNmessageAlignment,	XmALIGNMENT_CENTER); arg++;

	about_dialog = XmCreateInformationDialog(form_main, "AboutDialog",
			args, arg);

	XmStringFree(str);

	XtUnmanageChild(XmMessageBoxGetChild(about_dialog,
			XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(about_dialog,
			XmDIALOG_HELP_BUTTON));

	FileName = XtNewString("(NONE)");

	smStatusLineShowPosition(text_w, 0);
	smStautsLineSetMessage("");
	smTextSetTabsize(text_w, TabSize);

	if (autosave_env)
	    load_Env();
	line = -1;
	is_Empty = True;
	for (i = 1; i < argc; i++) {
	    buf = XtNewString(argv[i]);
	    if (buf[0] == '+') {
		if (!strcmp(buf + 1, "insert"))		/* Insert Mode Off */
		    insert_mode = False;
		else if (!strcmp(buf + 1, "indent"))	/* Indent Mode Off */
		    indent_mode = False;
		else if (!strcmp(buf + 1, "tab"))	/* Use tab character Mode Off */
		    fill_mode = False;
		else if (!strcmp(buf + 1, "undo"))	/* Undo after Save Off */
		    undo_after_save = False;
		else if (!strcmp(buf + 1, "showmatch")) /* Show Match Off */
		    show_match = False;
		else if (!strcmp(buf + 1, "backup"))	/* Create Bkacup Off */
		    create_backup = False;
		else if (!strcmp(buf + 1, "rw"))	/* Remove Whitespace Off */
		    optimize_text = False;
		else if (is_num(buf + 1) == True)
		    line = atoi(buf + 1);
		else usage(argv[0]);
	    }
	    else if (buf[0] == '-') {
		if (!strcmp(buf + 1, "insert"))		/* Insert Mode On */
		    insert_mode = True;
		else if (!strcmp(buf + 1, "indent"))	/* Indent Mode On */
		    indent_mode = True;
		else if (!strcmp(buf + 1, "tab"))	/* Use tab character Mode On */
		    fill_mode = True;
		else if (!strcmp(buf + 1, "undo"))	/* Undo after Save On */
		    undo_after_save = True;
		else if (!strcmp(buf + 1, "showmatch")) /* Show Match On */
		    show_match = True;
		else if (!strcmp(buf + 1, "backup"))	/* Create Bkacup On */
		    create_backup = True;
		else if (!strcmp(buf + 1, "rw"))	/* Remove Whitespace On */
		    optimize_text = True;
		else if (!strcmp(buf + 1, "tabs") || !strcmp(buf + 1, "ts")) {
			/* Set Tab size */
		    i++;
		    TabSize = atoi(argv[i]);
		}
		else if (!strcmp(buf + 1, "blocki") || !strcmp(buf + 1, "bi")) {
			/* Set Block indent */
		    i++;
		    smSetBlockIndentSize(atoi(argv[i]));
		}
		else usage(argv[0]);	/* Help */
	    }
	    else {
		if (is_Empty) {
		    is_Empty = False;
		    ignoreModify = True;
		    XtFree(FileName);
		    FileName = XtNewString(buf);
		    if (!open_file(FileName))
			smStautsLineSetMessage("New File");
		    add_recent_file(FileName);
		    ignoreModify = False;
		}
	    }
	    XtFree(buf);
	}

	if (line > 0)
	    smTextSetInsertionLine(text_w, line);

	/* Editor Preference */

	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_0"),
		insert_mode, True);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_1"),
		indent_mode, True);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_2"),
		fill_mode, True);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_3"),
		undo_after_save, False);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_4"),
		show_match, False);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_editor, "button_5"),
		pending_delete, True);

	/* File Preference */

	XmToggleButtonGadgetSetState(XtNameToWidget(option_file, "button_0"),
		create_backup, False);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_file, "button_1"),
		autosave_env, False);
	XmToggleButtonGadgetSetState(XtNameToWidget(option_file, "button_2"),
		optimize_text, False);

	smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);


	smSetSensitiveUndo(False);
	smSetSensitiveRedo(False);

	XtManageChild(main_w);
	XtRealizeWidget(toplevel);

	/* add a delete window protocol callback instead */
	wm_quit_Atom = XmInternAtom(XtDisplay(toplevel), "WM_PROTOCOLS", False);
	wm_delw_Atom = XmInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", False);
    	XmAddWMProtocolCallback(toplevel, wm_quit_Atom,
        	(XtCallbackProc)FileMenu_cb, (XtPointer)5);
    	XmAddWMProtocolCallback(toplevel, wm_delw_Atom,
        	(XtCallbackProc)FileMenu_cb, (XtPointer)5);
	XtVaSetValues(toplevel,
		XmNdeleteResponse, XmDO_NOTHING,
		NULL);

	set_title(FileName);
	XmProcessTraversal(text_w, XmTRAVERSE_CURRENT);

	XtAppMainLoop(app_context);
}

#ifdef _NO_PROTO
Widget CreateMenuBar(parent)
Widget parent;
#else
Widget CreateMenuBar(Widget parent)
#endif
{
	Widget menu;

	menu = XmVaCreateSimpleMenuBar(parent, "MainMenu",
		XmVaCASCADEBUTTON, NULL, NULL,		/* File		*/
		XmVaCASCADEBUTTON, NULL, NULL,		/* Edit		*/
		XmVaCASCADEBUTTON, NULL, NULL,		/* Search	*/
		XmVaCASCADEBUTTON, NULL, NULL,		/* Options	*/
		XmVaCASCADEBUTTON, NULL, NULL,		/* Help		*/
		NULL);

	file_menu = XmVaCreateSimplePulldownMenu(menu,
		"file_menu", 0, FileMenu_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* New		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Open		*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Save		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Save As..	*/
		XmVaSEPARATOR,
		XmVaCASCADEBUTTON, NULL, NULL,		/* Recent Files...*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Quit		*/
		NULL);

	pick_submenu = XmCreatePulldownMenu(file_menu,
		"pick_submenu", (ArgList)NULL, 0);

	XtVaSetValues(XtNameToWidget(file_menu, "button_4"),
		XmNsubMenuId, pick_submenu,
		NULL);

	XtSetSensitive(XtNameToWidget(file_menu, "button_4"), False);

	edit_menu = XmVaCreateSimplePulldownMenu(menu,
		"edit_menu", 1, EditMenu_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Undo		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Redo		*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Cut		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Copy		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Paste	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Clear	*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Select All	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Unselect	*/
		NULL);

	undo_button = XtNameToWidget(edit_menu, "button_0");
	redo_button = XtNameToWidget(edit_menu, "button_1");

	search_menu = XmVaCreateSimplePulldownMenu(menu,
		"search_menu", 2, SearchMenu_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Find		*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Replace	*/
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Find next	*/
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Go to line	*/
		NULL);

	option_menu = XmVaCreateSimplePulldownMenu(menu,
		"option_menu", 3, OptionMenu_cb,
		XmVaCASCADEBUTTON, NULL, NULL,		/* File Preference */
		XmVaCASCADEBUTTON, NULL, NULL,		/* Editor Preference */
		XmVaSEPARATOR,
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Show Toolbar */
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Save Options */
		NULL);

	XtVaSetValues(XtNameToWidget(option_menu, "button_2"),
		XmNset, show_toolbar,
		NULL);

	option_editor = XmVaCreateSimplePulldownMenu(option_menu,
		"option_editor", 0, EnvMenu_cb,
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Insert mode */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Indent mode */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Use tab character */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Undo after save */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Show match */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Pending delete */
		XmVaSEPARATOR,
		XmVaPUSHBUTTON,	 NULL, NULL, NULL, NULL, /* Tab size */
		XmVaPUSHBUTTON,	 NULL, NULL, NULL, NULL, /* Block Indent size */
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_0"),
		XmNset, insert_mode,
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_1"),
		XmNset, indent_mode,
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_2"),
		XmNset, fill_mode,
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_3"),
		XmNset, undo_after_save,
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_4"),
		XmNset, show_match,
		NULL);

	XtVaSetValues(XtNameToWidget(option_editor, "button_5"),
		XmNset, pending_delete,
		NULL);

	XtVaSetValues(XtNameToWidget(option_menu, "button_0"),
		XmNsubMenuId,	option_editor,
		NULL);

	option_file = XmVaCreateSimplePulldownMenu(option_menu,
		"option_file", 1, PrefMenu_cb,
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Create backup file */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Autosave Environment */
		XmVaCHECKBUTTON, NULL, NULL, NULL, NULL, /* Remove whitespace */
		XmVaSEPARATOR,
		XmVaCASCADEBUTTON, NULL, NULL,		/* Backup Extension */
		NULL);

	XtVaSetValues(XtNameToWidget(option_file, "button_0"),
		XmNset, create_backup,
		NULL);

	XtVaSetValues(XtNameToWidget(option_file, "button_1"),
		XmNset, optimize_text,
		NULL);

	XtVaSetValues(XtNameToWidget(option_menu, "button_1"),
		XmNsubMenuId,	option_file,
		NULL);

	help_menu = XmVaCreateSimplePulldownMenu(menu,
		"help_menu", 4, HelpMenu_cb,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* Contents... */
		XmVaSEPARATOR,
		XmVaPUSHBUTTON, NULL, NULL, NULL, NULL, /* About... */
		NULL);

	XtVaSetValues(menu,
		XmNmenuHelpWidget, XtNameToWidget(menu, "button_4"),
		NULL);

	return (menu);
}

#ifdef _NO_PROTO
Widget CreateToolbar(parent)
Widget parent;
#else
Widget CreateToolbar(Widget parent)
#endif
{
	Widget form_tool;
	Widget t_b[12];
	Cardinal i;

	static struct ToolbarRec toolbtn[] = {
		{"new_b",	10, new_xpm},
		{"open_b",	1, open_xpm},
		{"save_b",	1, save_xpm},
		{"cut_b",	8, cut_xpm},
		{"copy_b",	1, copy_xpm},
		{"paste_b",	1, paste_xpm},
		{"delete_b",	1, delete_xpm},
		{"undo_b",	8, undo_xpm},
		{"redo_b",	1, redo_xpm},
		{"search_b",	8, search_xpm},
		{"bu_b",	8, blocku_xpm},
		{"bi_b",	1, blocki_xpm}
	};

	form_tool = XtVaCreateWidget("toolbar",
		xmFormWidgetClass,	parent,
		XmNmarginHeight,	1,
		XmNmarginWidth,		1,
		NULL);

	for (i = 0; i < XtNumber(t_b); i++) {
	    t_b[i] = XtVaCreateManagedWidget(toolbtn[i].name,
		xmPushButtonWidgetClass, form_tool,
		XmNtopAttachment,		XmATTACH_FORM,
		XmNleftAttachment,		(i) ? XmATTACH_WIDGET : XmATTACH_FORM,
		XmNbottomAttachment,		XmATTACH_FORM,
		XmNtopOffset,			1,
		XmNleftWidget,			(i) ? t_b[i - 1] : NULL,
		XmNleftOffset,			toolbtn[i].leftOffset,
		XmNbottomOffset,		0,
		XmNtraversalOn,			False,
		XmNshadowThickness,		0,
		XmNhighlightThickness,		0,
		XmNmarginHeight,		2,
		XmNmarginWidth,			3,
		NULL);

		SetPBPixmap(t_b[i], toolbtn[i].xpm);

		XtAddCallback(t_b[i],
			XmNactivateCallback, active_cb, (XtPointer)i);
		XtAddEventHandler(t_b[i], EnterWindowMask, False,
			(XtEventHandler)eventProc_cb, (XtPointer)0);
		XtAddEventHandler(t_b[i], LeaveWindowMask, False,
			(XtEventHandler)eventProc_cb, (XtPointer)1);
	}

	return (form_tool);
}

/*
 *	XtEnevtHandler
 */
#ifdef _NO_PROTO
void buttonEvnet_proc(w, client_data, event, continue_to_dispatch_return)
Widget w;
XtPointer client_data;
XButtonPressedEvent *event;
Boolean *continue_to_dispatch_return;
#else
void buttonEvnet_proc(Widget w, XtPointer client_data,
		XButtonPressedEvent *event,
		Boolean *continue_to_dispatch_return)
#endif
{
	Widget popup = (Widget)client_data;
	Boolean is_selected;
	XmTextPosition start, end;

	if (event->button == 3) {
	    is_selected = XmTextGetSelectionPosition(w, &start, &end);
	    if (is_selected && start != end) {
		XtSetSensitive(XtNameToWidget(popup_menu, "button_2"), True);
		XtSetSensitive(XtNameToWidget(popup_menu, "button_3"), True);
		XtSetSensitive(XtNameToWidget(popup_menu, "button_5"), True);
	    }
	    else {
		XtSetSensitive(XtNameToWidget(popup_menu, "button_2"), False);
		XtSetSensitive(XtNameToWidget(popup_menu, "button_3"), False);
		XtSetSensitive(XtNameToWidget(popup_menu, "button_5"), False);
	    }
	    XmMenuPosition(popup, event);
	    XtManageChild(popup);
	}
}

#ifdef _NO_PROTO
void usage(name)
char *name;
#else
void usage(char *name)
#endif
{
	printf("usage: %s [+line] [-/+insert] [-/+indent ] [-/+tab] [-/+undo] [-/+showmatch]\n", name);
	printf("\t[-tabs n] [-blocki n] [-/+backup] [-/+rw] [filename]\n");
	exit(0);
}

void checkResDir()
{
	char *pDirName;

	pDirName = XtMalloc((Cardinal)(strlen(getenv("HOME")) + strlen(RES_PATH) + 1));
	(void)sprintf(pDirName, "%s/%s", getenv("HOME"), RES_PATH);
	if (access(pDirName, R_OK | W_OK | F_OK) == -1) {
	    if (mkdir(pDirName, S_IRWXU) == -1)
		fprintf(stderr, "Can not make directories: %s", pDirName);
	}
	XtFree(pDirName);
}

void load_option()	/* Load Option Function */
{
	char *pInitFile, buf[BUFSIZ];
	FILE *fp;
	int	tmp_value;

	pInitFile = XtMalloc((Cardinal)(strlen(getenv("HOME")) + strlen(RES_PATH) + strlen(INIT_FILE)) + 1);
	(void)sprintf(pInitFile, "%s/%s/%s", getenv("HOME"), RES_PATH, INIT_FILE);

	if ((fp = fopen(pInitFile, "r")) != NULL) {
	    fgets(buf, BUFSIZ, fp);
/*	    if (!strcmp("+3\n", buf)) {*/
		if (fscanf(fp, "Insert Mode=%s\n", buf))
		    insert_mode = stob(buf);
		if (fscanf(fp, "Auto indent=%s\n", buf))
		    indent_mode = stob(buf);
		if (fscanf(fp, "Use tab character=%s\n", buf))
		    fill_mode = stob(buf);
		if (fscanf(fp, "Undo after save=%s\n", buf))
		    undo_after_save = stob(buf);
		if (fscanf(fp, "Show match=%s\n", buf))
		    show_match = stob(buf);
		if (fscanf(fp, "Pending delete=%s\n", buf))
		    pending_delete = stob(buf);

		if (fscanf(fp, "Tab size=%d\n", &tmp_value))
		    TabSize = tmp_value;
		if (fscanf(fp, "Block indent=%d\n", &tmp_value))
		    smSetBlockIndentSize(tmp_value);
		if (fscanf(fp, "Create backup file=%s\n", buf))
		    create_backup = stob(buf);
		if (fscanf(fp, "Autosave Environment=%s\n", buf))
		    autosave_env = stob(buf);
		if (fscanf(fp, "Remove whitespace=%s\n", buf))
		    optimize_text = stob(buf);
		if (fscanf(fp, "Backup extension=%s\n", buf)) {
		    XtFree(backupExt);
		    backupExt = XtNewString(buf);
		}
		if (fscanf(fp, "Show Toolbar=%s\n", buf))
		    show_toolbar = stob(buf);
/*	    }*/
	    fclose(fp);
	}
	XtFree(pInitFile);
}

void save_init()
{
	char *pInitFile;
	FILE *fp;

	checkResDir();
	pInitFile = XtMalloc((Cardinal)(strlen(getenv("HOME")) + strlen(RES_PATH) + strlen(INIT_FILE)) + 1);
	(void)sprintf(pInitFile, "%s/%s/%s", getenv("HOME"), RES_PATH, INIT_FILE);
	if ((fp = fopen(pInitFile, "w")) != NULL) {
/*	    fprintf(fp, "+3\n");*/
	    if (insert_mode)
		(void)fprintf(fp, "Insert Mode=True\n");
	    else (void)fprintf(fp, "Insert Mode=False\n");
	    if (indent_mode)
		(void)fprintf(fp, "Auto indent=True\n");
	    else (void)fprintf(fp, "Auto indent=False\n");
	    if (fill_mode)
		(void)fprintf(fp, "Use tab character=True\n");
	    else (void)fprintf(fp, "Use tab character=False\n");
	    if (undo_after_save)
		(void)fprintf(fp, "Undo after save=True\n");
	    else (void)fprintf(fp, "Undo after save=False\n");
	    if (show_match)
		(void)fprintf(fp, "Show match=True\n");
	    else (void)fprintf(fp, "Show match=False\n");
	    if (pending_delete)
		(void)fprintf(fp, "Pending delete=True\n");
	    else (void)fprintf(fp, "Pending delete=False\n");

	    (void)fprintf(fp, "Tab size=%d\n", TabSize);
	    (void)fprintf(fp, "Block indent=%d\n", BlockIndentSize);

	    if (create_backup)
		(void)fprintf(fp, "Create backup file=True\n");
	    else (void)fprintf(fp, "Create backup file=False\n");
	    if (autosave_env)
		(void)fprintf(fp, "Autosave Environment=True\n");
	    else (void)fprintf(fp, "Autosave Environment=False\n");
	    if (optimize_text)
		(void)fprintf(fp, "Remove whitespace=True\n");
	    else (void)fprintf(fp, "Remove whitespace=False\n");

	    (void)fprintf(fp, "Backup extension=%s\n", backupExt);

	    if (show_toolbar)
		(void)fprintf(fp, "Show Toolbar=True\n");
	    else (void)fprintf(fp, "Show Toolbar=False\n");

	    fclose(fp);
	}
	XtFree(pInitFile);
}

void save_history()
{
	char *pHistFile, *str;
	XmString label_str;
	FILE *fp;
	int i;

	checkResDir();
	pHistFile = XtMalloc((Cardinal)(strlen(getenv("HOME")) + strlen(RES_PATH) + strlen(HIST_FILE)) + 1);
	(void)sprintf(pHistFile, "%s/%s/%s", getenv("HOME"), RES_PATH, HIST_FILE);
	if ((fp = fopen(pHistFile, "w")) != NULL) {
/*	    fprintf(fp, "+1\n");*/
	    for (i = 0; i < 10; i++)
	    	fprintf(fp, " %ld", bookmark[i]);
	    fprintf(fp, "\n");
	    for (i = 0; i < file_count; i++) {
		XtVaGetValues(pick_button[i],
		    XmNlabelString,	    &label_str,
		    NULL);
		XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &str);
		fprintf(fp, "%s\n", str + 3);
		XmStringFree(label_str);
		XtFree(str);
	    }
	    fclose(fp);
	}
	XtFree(pHistFile);
}

void load_Env()
{
	char *pHistFile, *str;
	char buf[BUFSIZ], fname[BUFSIZ];
	XmString label_str;
	FILE *fp;

	pHistFile = XtMalloc((Cardinal)(strlen(getenv("HOME")) + strlen(RES_PATH) + strlen(HIST_FILE)) + 1);
	(void)sprintf(pHistFile, "%s/%s/%s", getenv("HOME"), RES_PATH, HIST_FILE);
	if ((fp = fopen(pHistFile, "r")) != NULL) {
	    fgets(buf, BUFSIZ, fp);
	        fscanf(fp, " %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
	    		&bookmark[0],
	    		&bookmark[1],
	    		&bookmark[2],
	    		&bookmark[3],
	    		&bookmark[4],
	    		&bookmark[5],
	    		&bookmark[6],
	    		&bookmark[7],
	    		&bookmark[8],
	    		&bookmark[9]);
		file_count = 0;
		while (fgets(fname, BUFSIZ, fp) != NULL) {
		    fname[strlen(fname) - 1] = '\0';
		    str = XtMalloc(strlen(fname) + 4);
		    if (file_count < 10)
			sprintf(str, "%d. %s", file_count, fname);
		    else
			sprintf(str, "   %s", fname);
		    label_str = XmStringCreateLocalized(str);
		    (void)sprintf(buf, "button_%d", file_count);
		    pick_button[file_count] = XtVaCreateManagedWidget(buf,
			    xmPushButtonGadgetClass, pick_submenu,
			    XmNlabelString, label_str,
			    NULL);
		    XtAddCallback(pick_button[file_count],
			    XmNactivateCallback, pick_cb, (XtPointer)NULL);
		    XtVaSetValues(pick_button[file_count],
			XmNlabelString,     label_str,
			XmNmnemonic,	(file_count < 10) ? str[0] : NULL,
			NULL);
		    XmStringFree(label_str);
		    XtFree(str);
		    file_count = (file_count < MAXLIST) ? file_count + 1 : file_count;
		}
		if (file_count)
	    	    XtSetSensitive(XtNameToWidget(file_menu, "button_4"), True);
	}
	XtFree(pHistFile);
}

#ifdef _NO_PROTO
void add_recent_file(name)
char *name;
#else
void add_recent_file(char *name)
#endif
{
	char buf[BUFSIZ], *str;
	XmString label_str;
	Boolean is_new;
	int i, ret;

	is_new = True;
	for (i = 0; i < file_count; i++) {
	    XtVaGetValues(pick_button[i],
		XmNlabelString,		&label_str,
		NULL);
	    XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &str);
	    ret = strcmp(str + 3, name);
	    XmStringFree(label_str);
	    XtFree(str);
	    if (ret == 0) {
		is_new = False;
		break;
	    }
	}

	if (is_new) {
	    if (file_count < MAXLIST) {
		str = XtMalloc(strlen(name) + 4);
		if (file_count < 10)
		    sprintf(str, "%d. %s", file_count, name);
		else
		    sprintf(str, "   %s", name);
		label_str = XmStringCreateLocalized(str);
		(void)sprintf(buf, "button_%d", file_count);

		pick_button[file_count] = XtVaCreateManagedWidget(buf,
			xmPushButtonGadgetClass, pick_submenu,
			XmNlabelString, label_str,
			NULL);
		XtAddCallback(pick_button[file_count],
			XmNactivateCallback, pick_cb, (XtPointer)NULL);

		XtVaSetValues(pick_button[file_count],
		    XmNlabelString,	label_str,
		    XmNmnemonic,        (file_count < 10) ? str[0] : NULL,
		    NULL);

		XmStringFree(label_str);
		XtFree(str);
		file_count = (file_count < MAXLIST) ? file_count + 1 : file_count;
	    }
	    else {
		for (i = 1; i < file_count; i++) {
		    XtVaGetValues(pick_button[i],
			XmNlabelString,		&label_str,
			NULL);
		    XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &str);
		    XmStringFree(label_str);
		    if (i < 11)
			(void)sprintf(buf, "%d. %s", i - 1, str + 3);
		    else
			(void)sprintf(buf, "   %s", str + 3);
		    label_str = XmStringCreateLocalized(buf);

		    XtVaSetValues(pick_button[i - 1],
			XmNlabelString,	label_str,
			XmNmnemonic,        (i < 11) ? buf[0] : NULL,
			NULL);

		    XmStringFree(label_str);
		    XtFree(str);
		}
		str = XtMalloc(strlen(name) + 4);
		if (file_count < 11)
		    sprintf(str, "%d. %s", file_count - 1, name);
		else
		    sprintf(str, "   %s", name);

		label_str = XmStringCreateLocalized(str);

		XtVaSetValues(pick_button[file_count - 1],
		    XmNlabelString,	label_str,
		    XmNmnemonic,        (file_count < 11) ? str[0] : NULL,
		    NULL);

		XmStringFree(label_str);
		XtFree(str);
	    }
	}

	if (XtIsSensitive(XtNameToWidget(file_menu, "button_4")) == False)
	    XtSetSensitive(XtNameToWidget(file_menu, "button_4"), True);
}

#ifdef _NO_PROTO
void update_SaveAsDialog_data(w, str)
Widget w;
char *str;
#else
void update_SaveAsDialog_data(Widget w, char *str)
#endif
{
	XmString dirSpec, dirPath, dirMask, dirFile;
	char *f_name, *f_path;

	if (strcmp(str, "(NONE)")) {
	    parseFilename(str, &f_path, &f_name);

	    if (!strcmp(f_path, ""))
		dirMask = XmStringCreateLocalized("*");
	    else dirMask = XmStringCreateLocalized(f_path);

	    if (!strcmp(f_name, ""))
		dirFile = XmStringCreateLocalized(str);
	    else dirFile = XmStringCreateLocalized(f_name);

	    XtFree(f_path);
	    XtFree(f_name);
	}
	else {
	    dirMask = XmStringCreateLocalized("*");
	    dirFile = XmStringCreateLocalized("UNNAMED");
	}
	XmFileSelectionDoSearch(w, dirMask);
	XmStringFree(dirMask);

	XtVaGetValues(w,
		XmNdirectory, &dirPath,
		NULL);

	dirSpec = XmStringConcat(dirPath, dirFile);
	XmStringFree(dirPath);
	XmStringFree(dirFile);

	XtVaSetValues(w,
		XmNdirSpec,	dirSpec,
		NULL);
	XmStringFree(dirSpec);
}

#ifdef _NO_PROTO
void set_title(name)
char *name;
#else
void set_title(char *name)
#endif
{
	char *buf;

	buf = XtMalloc((Cardinal)strlen(name) + 15);
	(void)sprintf(buf, "%s - %s", APP_NAME, name);
	XStoreName(XtDisplay(toplevel), XtWindow(toplevel), buf);
	XSetIconName(XtDisplay(toplevel), XtWindow(toplevel), buf);
	XtFree(buf);
}


#ifdef _NO_PROTO
void with_Ctrl_K(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void with_Ctrl_K(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XEvent	theEvent;
	char	kbuf[2];
	static int     length;
	KeySym	key;
	XComposeStatus	cs;
	static XmTextPosition begin, end;

	do {
	    XNextEvent(event->xkey.display, &theEvent);
	    if (theEvent.type == KeyPress) {
		length = XLookupString(&(theEvent.xkey), kbuf, 2, &key, &cs);
		kbuf[length] = '\0';
	    }
	} while (theEvent.type != KeyPress && theEvent.type != ButtonPress &&
		theEvent.type != FocusOut);

	switch (key) {
	    case XK_B : case XK_b :	/* <Ctrl><K><B> Mark block-begin */
		begin = XmTextGetInsertionPosition(w);
		break;
	    case XK_K : case XK_k :	/* <Ctrl><K><K> Mark block-end*/
		end = XmTextGetInsertionPosition(w);
		XmTextSetSelection(w, begin, end, theEvent.xkey.time);
		break;
	    case XK_H : case XK_h :	/* <Ctrl><K><H> Hide block */
		XmTextClearSelection(w, theEvent.xkey.time);
		if (found)
		  found = False;
		break;
	    case XK_C : case XK_c :	/* <Ctrl><K><C> Copy Block */
		if (XmTextCopy(w, theEvent.xkey.time) == True)
		  XmTextClearSelection(w, theEvent.xkey.time);
		break;
	    case XK_Y : case XK_y :	/* <Ctrl><K><Y> Delete Block */
		if (XmTextRemove(w) == True)
		  XFlush(event->xkey.display);
		break;
	    case XK_V : case XK_v :	/* <Ctrl><K><V> Paste Block */
		XmTextClearSelection(w, theEvent.xkey.time);
		if (XmTextPaste(w) == True)
		  XFlush(event->xkey.display);
		break;
	    case XK_X : case XK_x :	/* <Ctrl><K><X> Cut Block */
		if (XmTextCut(w, theEvent.xkey.time) == True)
		  XFlush(event->xkey.display);
		break;
	    case XK_I : case XK_i :	  /* <Ctrl><K><I> Block Indent */
		XtCallActionProc(text_w, "block-indent", event, (String *)NULL, 0);
		break;
	    case XK_U : case XK_u :	  /* <Ctrl><K><U> Block Unindent */
		XtCallActionProc(text_w, "block-unindent", event, (String *)NULL, 0);
		break;
	    case XK_S : case XK_s :	/* <Ctrl><K><S> Save FIle */
		if (strcmp(FileName, "(NONE)")) {
		    if (!save_file(FileName)) {
			set_ErrorDialog_message(ErrorDialog, FileName, FILE_SAVE);
			XtManageChild(ErrorDialog);
		    }
		}
		else {
		    SaveNum = SAVE_DEFAULT;
		    update_SaveAsDialog_data(SaveAsDialog, FileName);
		    XtManageChild(SaveAsDialog);
		}
		break;
	    case XK_P : case XK_p :	/* Upper Case */
		UpperCase(text_w);
		break;
	    case XK_L : case XK_l :	/* Lower Case */
		LowerCase(text_w);
		break;
	    case XK_0 : case XK_1 : case XK_2 : case XK_3 : case XK_4 :
	    case XK_5 : case XK_6 : case XK_7 : case XK_8 : case XK_9 :
		bookmark[atoi(kbuf)] = XmTextGetInsertionPosition(w);
		break;		/* Set Place Marker */
	}
}

#ifdef _NO_PROTO
void with_Ctrl_Q(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void with_Ctrl_Q(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XEvent	theEvent;
	char	kbuf[2];
	int	length;
	KeySym	key;
	XComposeStatus	cs;
	XmTextPosition pos, i;
	int page;
	int status;
	Boolean found_str;

	do {
	    XNextEvent(event->xkey.display, &theEvent);
	    if (theEvent.type == KeyPress) {
		length = XLookupString(&(theEvent.xkey), kbuf, 2, &key, &cs);
		kbuf[length] = '\0';
	    }
	} while (theEvent.type != KeyPress && theEvent.type != ButtonPress &&
		theEvent.type != FocusOut);

	switch (key) {
	    case XK_Y : case XK_y :	/* <Ctrl><Q><Y> Delete end of line */
		XtCallActionProc(w, "delete-to-end-of-line", event, (String *)NULL, 0);
		break;
	    case XK_P : case XK_p :	/* goto the last cursor Position */
		smTextSetInsertionPosition(w, last_position);
		break;
	    case XK_B : case XK_b :	/* Toggle Create Backup File */
		create_backup = !create_backup;
		break;
	    case XK_S : case XK_s :	/* Beginning of line */
		XtCallActionProc(w, "beginning-of-line", event, (String *)NULL, 0);
		break;
	    case XK_D : case XK_d :	/* End of line */
		XtCallActionProc(w, "end-of-line", event, (String *)NULL, 0);
		break;
	    case XK_E : case XK_e :	/* Top of window */
		pos = XmTextGetTopCharacter(w);
		XmTextSetInsertionPosition(w, pos);
		break;
	    case XK_X : case XK_x :	/* Bottom of window */
		pos = smTextGetBottomCharacter(w);
		XmTextSetInsertionPosition(w, pos);
		break;
	    case XK_M : case XK_m :	/* Mid of window */
		i = 0;
		pos = XmTextGetTopCharacter(w);
		page = smTextGetRows(w) >> 1;
		do {
		    found_str = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &pos);
		    if (found_str) {
			i++;
			pos++;
		    } /* if */
		} while (found_str && i < page);
		XmTextSetInsertionPosition(w, pos);
		break;
	    case XK_R : case XK_r :	  /* Beginning of file */
		XtCallActionProc(w, "beginning-of-file", event, (String *)NULL, 0);
		break;
	    case XK_C : case XK_c :	  /* End of file */
		XtCallActionProc(w, "end-of-file", event, (String *)NULL, 0);
		break;
	     case XK_bracketleft  : case XK_braceleft  :
	     case XK_bracketright : case XK_braceright :
		i = XmTextGetInsertionPosition(w);
		status = search_pair(w, i, &pos);
		if (status == 0)
		    XmTextSetInsertionPosition(w, pos);
		break;		/*	pair matching  {}()[]	*/
	    case XK_F : case XK_f :
		smManageReplaceText(False);
		if (!XtIsManaged(SearchDialog)) {
		    SelectionToFind(text_w);
		    XtManageChild(SearchDialog);
		}
		else SelectionToFind(text_w);
		break;		/* Manage Find Dialog */
	    case XK_A : case XK_a :
		smManageReplaceText(True);
		if (!XtIsManaged(SearchDialog)) {
		    SelectionToFind(text_w);
		    XtManageChild(SearchDialog);
		}
		else SelectionToFind(text_w);
		break;		/* Manage Find & Replace Dialog */
	    case XK_0 : case XK_1 : case XK_2 : case XK_3 : case XK_4 :
	    case XK_5 : case XK_6 : case XK_7 : case XK_8 : case XK_9 :
		smTextSetInsertionPosition(w, bookmark[atoi(kbuf)]);
		break;		/* Find Place Marker */
	} /* switch */
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void proc_Options(w, event, params, num)
Widget w;
XEvent *event;
String *params;
Cardinal *num;
#else
void proc_Options(Widget w, XEvent *event, String *params, Cardinal *num)
#endif
{
	XEvent	theEvent;
	char	kbuf[2];
	static int     length;
	KeySym	key;
	XComposeStatus	cs;

	do {
	    XNextEvent(event->xkey.display, &theEvent);
	    if (theEvent.type == KeyPress) {
		length = XLookupString(&(theEvent.xkey), kbuf, 2, &key, &cs);
		kbuf[length] = '\0';
	    }
	} while (theEvent.type != KeyPress && theEvent.type != ButtonPress &&
		theEvent.type != FocusOut);

	switch (key) {
	    case XK_O : case XK_o :	/* Toggle Insert Mode */
		insert_mode = !insert_mode;
		XmToggleButtonGadgetSetState(XtNameToWidget(option_editor,
			"button_0"), insert_mode, True);
		break;
	    case XK_I : case XK_i :	/* Toggle Indent Mode */
		indent_mode = !indent_mode;
		XmToggleButtonGadgetSetState(XtNameToWidget(option_editor,
			"button_1"), indent_mode, True);
		break;
	    case XK_T : case XK_t :	/* Toggle Tab Mode */
		fill_mode = !fill_mode;
		XmToggleButtonGadgetSetState(XtNameToWidget(option_editor,
			"button_2"), fill_mode, True);
		break;
	    case XK_U : case XK_u :	/* Toggle undo after save */
		undo_after_save = !undo_after_save;
		XmToggleButtonGadgetSetState(XtNameToWidget(option_editor,
			"button_3"), undo_after_save, True);
		break;
	    case XK_B : case XK_b :	/* Toggle Create Backup File */
		create_backup = !create_backup;
		XmToggleButtonGadgetSetState(XtNameToWidget(option_file,
			"button_0"), create_backup, True);
		break;
	}
	XFlush(event->xkey.display);
}

#ifdef _NO_PROTO
void CloseBox(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void CloseBox(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	if (OpenName != NULL) {
	    XtFree(OpenName);
	    OpenName = NULL;
	}
#ifdef _DEBUG
	printf("Close Box\n");
#endif
	XtUnmanageChild(w);
}

#ifdef _NO_PROTO
void go_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void go_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmSelectionBoxCallbackStruct *cbs =
			(XmSelectionBoxCallbackStruct *)call_data;
	char *line_str;
	int line;

	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &line_str);
	switch (line_str[0]) {
	    case '+' :
		line = textPosToLineNum(text_w, XmTextGetInsertionPosition(text_w));
		if (is_num(line_str + 1))
		    line += atoi(line_str + 1);
		break;
	    case '-' :
		line = textPosToLineNum(text_w, XmTextGetInsertionPosition(text_w));
		if (is_num(line_str + 1))
		    line -= atoi(line_str + 1);
		break;
	    default :
		if (is_num(line_str))
		    line = atoi(line_str);
		break;
	}
	smTextSetInsertionLine(text_w, line);
	XtFree(line_str);
	XFlush(cbs->event->xany.display);
}

#ifdef _NO_PROTO
void settab_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void settab_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmSelectionBoxCallbackStruct *cbs =
			(XmSelectionBoxCallbackStruct *)call_data;
	char *Tab_Value;

	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &Tab_Value);

	TabSize = atoi(Tab_Value);

	smTextSetTabsize(text_w, TabSize);

	XtFree(Tab_Value);
	XFlush(cbs->event->xany.display);
}

#ifdef _NO_PROTO
void setblock_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void setblock_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmSelectionBoxCallbackStruct *cbs =
			(XmSelectionBoxCallbackStruct *)call_data;
	char *Value;

	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &Value);
	BlockIndentSize = atoi(Value);
	XtFree(Value);
	if (BlockIndentSize > MAXTAB || BlockIndentSize <= 0)
	    BlockIndentSize = 8;

	XFlush(cbs->event->xany.display);
}

#ifdef _NO_PROTO
void setbackup_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void setbackup_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmSelectionBoxCallbackStruct *cbs =
			(XmSelectionBoxCallbackStruct *)call_data;
	char *Value;

	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &Value);
	XtFree(backupExt);
	backupExt = XtNewString(Value);
	XtFree(Value);

	XFlush(cbs->event->xany.display);
}

#ifdef _NO_PROTO
Boolean open_file(name)
char *name;
#else
Boolean open_file(char *name)
#endif
{
	struct stat statbuf;
	char buf[BUFSIZ];
	char *str;
	int length, flen;
	FILE *fp;

	if (stat(name, &statbuf) < 0 || !S_ISREG(statbuf.st_mode) ||
		(fp = fopen(name, "r")) == NULL)
	    return (False);

       /* Delete Undo & Redo Buffers */

	FreeUndoBuffer();
	FreeRedoBuffer();

	if (found) {
	    XmTextClearSelection(text_w, CurrentTime);
	    found = False;
	}

/*	pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp, pos, SEEK_SET);*/

	smStautsLineSetMessage("Loading...");
	length = (int)statbuf.st_size;

	str = XtMalloc((Cardinal)length + 1);
	flen = fread(str, sizeof(char), length, fp);
	str[length] = '\0';
	fclose(fp);

	XmTextSetString(text_w, str);
	XtFree(str);

	(void)sprintf(buf, "Load %ld bytes", (long)flen);
	if (access(name, W_OK) < 0)
	    strcat(buf, " [Read Only]");
	smStautsLineSetMessage(buf);
	file_saved = True;

	smStatusLineShowPosition(text_w, 0);
	smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);

	if ((XmTextPosition)length != XmTextGetLastPosition(text_w))
	    XmTextReplace(text_w, (XmTextPosition)length,
		XmTextGetLastPosition(text_w), "");
	XmProcessTraversal(text_w, XmTRAVERSE_CURRENT);

	return (True);
}

#ifdef _NO_PROTO
Boolean save_file(name)
char *name;
#else
Boolean save_file(char *name)
#endif
{
	struct stat statbuf;
	char *buf, *str, *new_str;
	FILE *fp;
	int length, extlen;

	/* Create Backup File */
	extlen = strlen(name) - strlen(backupExt);

	if (!access(name, F_OK | W_OK) && !stat(name, &statbuf) &&
		S_ISREG(statbuf.st_mode) &&
		create_backup &&
		strcmp(name + extlen, backupExt)) {
	    buf = XtMalloc((Cardinal)(strlen(name) * 2) + strlen(backupExt) + 10);
	    (void)sprintf(buf, "cp %s %s%s", name, name, backupExt);
	    system(buf);
	    XtFree(buf);
	}

	if ((fp = fopen(name, "w+")) == NULL)
	    return (False);

	if (!undo_after_save) {
	    /* Delete Undo & Redo Buffers */
	    FreeUndoBuffer();
	    FreeRedoBuffer();
	}

	smStautsLineSetMessage("Saving...");
	str = XmTextGetString(text_w);
	buf = XtMalloc((Cardinal)BUFSIZ);

	if (optimize_text) {
	    new_str = trimSpaces(str);
	    length = strlen(new_str);
	    if (fprintf(fp, "%s", new_str) == EOF)
		strcpy(buf, "Warning: Cannot save entire file!");
	    else {
		if (new_str[length - 1] != '\n')
		    fputc('\n', fp);
		(void)sprintf(buf, "Saved %d bytes", length);
	    }
	    XtFree(new_str);
	}
	else {
	    length = strlen(str);
	    if (fprintf(fp, "%s", str) == EOF)
		strcpy(buf, "Warning: Cannot save entire file!");
	    else {
		if (str[length - 1] != '\n')
		    fputc('\n', fp);
		(void)sprintf(buf, "Saved %d bytes", length);
	    }
	}
	XtFree(str);

	fclose(fp);
	smStautsLineSetMessage(buf);
	XtFree(buf);

	file_saved = True;
	smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);

	return (True);
}

#ifdef _NO_PROTO
void close_text(w)
Widget w;
#else
void close_text(Widget w)
#endif
{
	ignoreModify = True;
	/* Delete Undo & Redo Buffers */
	FreeUndoBuffer();
	FreeRedoBuffer();

	if (found) {
	    XmTextClearSelection(text_w, CurrentTime);
	    found = False;
	}
	XmTextSetString(w, "");
	ignoreModify = False;

	XtFree(FileName);
	FileName = XtNewString("(NONE)");
	set_title(FileName);

	file_saved = True;
	smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
	smStatusLineShowPosition(w, 0);

	XmProcessTraversal(text_w, XmTRAVERSE_CURRENT);
}

void OpenFileProc()
{
	ignoreModify = True;
	if (open_file(OpenName)) {
	    XtFree(FileName);
	    FileName = XtNewString(OpenName);
	    set_title(FileName);
	    add_recent_file(FileName);
	}
	else {
	    set_ErrorDialog_message(ErrorDialog, OpenName, FILE_OPEN);
	    XtManageChild(ErrorDialog);
	}
	XtFree(OpenName);
	OpenName = NULL;
	ignoreModify = False;
}

void QuitProc()
{
	if (autosave_env) {
	    save_init();
	    save_history();
	}
	exit(0);
}

#ifdef _NO_PROTO
Boolean SaveAsProc(SaveNum, name)
int SaveNum;
char *name;
#else
Boolean SaveAsProc(int SaveNum, char *name)
#endif
{
	if (save_file(name)) {
	    XtFree(FileName);
	    FileName = XtNewString(name);
	    switch (SaveNum) {
		case SAVE_DEFAULT :	/*	Default */
		    set_title(FileName);
		    add_recent_file(FileName);
		    break;
		case SAVE_NEW :		/*	New	*/
		    smStautsLineSetMessage("");
		    close_text(text_w);
		    break;
		case SAVE_OPEN :	/*	Open	*/
		    XtManageChild(OpenDialog);
		    break;
		case SAVE_LIST :	/*	List	*/
		    OpenFileProc();
		    break;
		case SAVE_QUIT :	/*	Quit	*/
		    QuitProc();
		    break;
	    }
	    return (True);
	}
	else return (False);
}

#ifdef _NO_PROTO
void OverWrite_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void OverWrite_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XtUnmanageChild(w);
	if (SaveAsProc(SaveNum, OpenName) == False) {
	    set_ErrorDialog_message(ErrorDialog, OpenName, FILE_SAVE);
	    XtManageChild(ErrorDialog);
	}
	XtFree(OpenName);
	OpenName = NULL;
}

#ifdef _NO_PROTO
void SaveAs_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void SaveAs_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmFileSelectionBoxCallbackStruct *cbs =
			  (XmFileSelectionBoxCallbackStruct *)call_data;
	char *tempName;

	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &tempName);
	XtUnmanageChild(w);
	if (access(tempName, F_OK) == 0) {
	    if (OpenName != NULL) {
		XtFree(OpenName);
		OpenName = NULL;
	    }
	    OpenName = XtNewString(tempName);
	    set_OverwriteDialog_message(OverwriteDialog, OpenName);
	    XtManageChild(OverwriteDialog);
	}
	else if (SaveAsProc(SaveNum, tempName) == False) {
	    set_ErrorDialog_message(ErrorDialog, tempName, FILE_SAVE);
	    XtManageChild(ErrorDialog);
	}
	XtFree(tempName);
}

#ifdef _NO_PROTO
void OpenDir_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void OpenDir_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmFileSelectionBoxCallbackStruct *cbs =
			  (XmFileSelectionBoxCallbackStruct *)call_data;
	XmString newName, str;
	char *f_name, *path_name;

	Boolean listUpdated;

	if (strcmp(FileName, "(NONE)")) {
	    parseFilename(FileName, &path_name, &f_name);

	    if (!strcmp(f_name, ""))
		str = XmStringCreateLocalized(FileName);
	    else str = XmStringCreateLocalized(f_name);

	    XtFree(f_name);
	    XtFree(path_name);
	}
	else str = XmStringCreateLocalized("UNNAMED");

	XtVaGetValues(w,
		XmNlistUpdated, &listUpdated,
		NULL);

	if (listUpdated == True) {
	    newName = XmStringConcat(cbs->dir, str);
	    XtVaSetValues(w,
		XmNdirSpec,	newName,
		NULL);
	    XmStringFree(newName);
	}
	XmStringFree(str);
}


#ifdef _NO_PROTO
void yes_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void yes_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XtUnmanageChild(w);
	if (strcmp(FileName, "(NONE)")) {
	    if (save_file(FileName)) {
		switch (WarningNum) {
		    case WARNING_NEW :	/*	New	*/
			smStautsLineSetMessage("");
			close_text(text_w);
			break;
		    case WARNING_OPEN : /*	Open	*/
			XtManageChild(OpenDialog);
			break;
		    case WARNING_LIST : /*	List... */
			OpenFileProc();
			break;
		    case WARNING_QUIT : /*	Quit	*/
			QuitProc();
			break;
		}
	    }
	    else {
		set_ErrorDialog_message(ErrorDialog, FileName, FILE_SAVE);
		XtManageChild(ErrorDialog);
	    }
	}
	else {
	    SaveNum = WarningNum + 1;
	    update_SaveAsDialog_data(SaveAsDialog, FileName);
	    XtManageChild(SaveAsDialog);
	}
}

#ifdef _NO_PROTO
void no_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void no_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XtUnmanageChild(w);

	switch (WarningNum) {
	    case WARNING_NEW :	/*	New	*/
		smStautsLineSetMessage("");
		close_text(text_w);
		break;
	    case WARNING_OPEN : /*	Open	*/
		XtManageChild(OpenDialog);
		break;
	    case WARNING_LIST : /*	List... */
		OpenFileProc();
		break;
	    case WARNING_QUIT : /*	Quit	*/
		QuitProc();
		break;
	}
}

#ifdef _NO_PROTO
void open_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void open_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmFileSelectionBoxCallbackStruct *cbs =
			  (XmFileSelectionBoxCallbackStruct *)call_data;

	if (OpenName != NULL) {
	    XtFree(OpenName);
	    OpenName = NULL;
	}
	XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &OpenName);
	XtUnmanageChild(w);
	OpenFileProc();
}

#ifdef _NO_PROTO
void FileMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void FileMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	Cardinal item_no = (Cardinal)client_data;

	smStautsLineSetMessage("");
	switch (item_no) {
	    case 0 :	/*	New	*/
		if (file_saved) close_text(text_w);
		else {
		    WarningNum = WARNING_NEW;
		    set_QuestionDialog_message(QuestionDialog, FileName);
		    XtManageChild(QuestionDialog);
		}
		break;
	    case 1 :	/*	Open	*/
		if (file_saved) XtManageChild(OpenDialog);
		else {
		    WarningNum = WARNING_OPEN;
		    set_QuestionDialog_message(QuestionDialog, FileName);
		    XtManageChild(QuestionDialog);
		}
		break;
	    case 2 :	/*	Save	*/
		if (strcmp(FileName, "(NONE)")) {
		    if (!save_file(FileName)) {
			set_ErrorDialog_message(ErrorDialog, FileName, FILE_SAVE);
			XtManageChild(ErrorDialog);
		    }
		}
		else {
		    SaveNum = SAVE_DEFAULT;
		    update_SaveAsDialog_data(SaveAsDialog, FileName);
		    XtManageChild(SaveAsDialog);
		}
		break;
	    case 3 :	/*	Save As...	*/
		update_SaveAsDialog_data(SaveAsDialog, FileName);
		SaveNum = SAVE_DEFAULT;
		XtManageChild(SaveAsDialog);
		break;
	    case 5 :	/*	Quit	*/
		if (file_saved) QuitProc();
		else {
		    WarningNum = WARNING_QUIT;
		    set_QuestionDialog_message(QuestionDialog, FileName);
		    XtManageChild(QuestionDialog);
		}
		break;
	}
}

#ifdef _NO_PROTO
void EditMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void EditMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
	Cardinal num = (Cardinal)client_data;

	switch(num) {
	    case 0 :	/*	Undo	*/
		XtCallActionProc(text_w, "UndoLast", cbs->event, (String *)NULL, 0);
		break;
	    case 1 :	/*	Redo	*/
		XtCallActionProc(text_w, "RedoAction", cbs->event, (String *)NULL, 0);
		break;
	    case 2 :	/*	Cut	*/
		if (XmTextCut(text_w, cbs->event->xbutton.time) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 3 :	/*	Copy	*/
		if (XmTextCopy(text_w, cbs->event->xbutton.time) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 4 :	/*	Paste	*/
		if (XmTextPaste(text_w) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 5 :	/*	Delete	*/
		if (XmTextRemove(text_w) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 6 :	/*	Select All...	*/
		XtCallActionProc(text_w, "select-all", cbs->event, (String *)NULL, 0);
		break;
	    case 7 :	/*	Unselect...	*/
		XmTextClearSelection(text_w, cbs->event->xbutton.time);
		if (found)
		    found = False;
		break;
	}
}

#ifdef _NO_PROTO
void SearchMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void SearchMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	Cardinal item_no = (Cardinal)client_data;

	switch (item_no) {
	    case 0 :	/* Find... */
		smManageReplaceText(False);
		if (!XtIsManaged(SearchDialog)) {
		    SelectionToFind(text_w);
		    XtManageChild(SearchDialog);
		}
		else SelectionToFind(text_w);
		break;
	    case 1 :	/* Find & Replace string */
		smManageReplaceText(True);
		if (!XtIsManaged(SearchDialog)) {
		    SelectionToFind(text_w);
		    XtManageChild(SearchDialog);
		}
		else SelectionToFind(text_w);
		break;
	    case 2 :	/* Find Next */
		findNext(text_w);
		break;
	    case 3 :	/* Go to Line number */
		XmTextSetSelection(XmSelectionBoxGetChild(go_dialog, XmDIALOG_TEXT),
			0,
			XmTextGetLastPosition(XmSelectionBoxGetChild(go_dialog, XmDIALOG_TEXT)),
			CurrentTime);
		XtManageChild(go_dialog);
		break;
	}
}

#ifdef _NO_PROTO
void OptionMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void OptionMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmToggleButtonCallbackStruct *cbs =
				   (XmToggleButtonCallbackStruct *)call_data;
	Cardinal num = (Cardinal)client_data;

	switch (num) {
	    case 2 :
		show_toolbar = cbs->set;
		XtVaSetValues(main_w,
			 XmNshowSeparator, True,
			 NULL);
		if (show_toolbar) {
		    XtManageChild(toolbar);
		    XmMainWindowSetAreas(main_w, menubar, toolbar, NULL, NULL, form_main);
		}
		else {
		    XtUnmanageChild(toolbar);
		    XmMainWindowSetAreas(main_w, menubar, NULL, NULL, NULL, form_main);
		}
		XtVaSetValues(main_w,
			 XmNshowSeparator, False,
			 NULL);
		break;
	    case 3 :	/*	Save Options	 */
		save_init();
		break;
	}
}

#ifdef _NO_PROTO
void pick_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void pick_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmString label_str;
	char *pTempStr;

	if (OpenName != NULL) {
	    XtFree(OpenName);
	    OpenName = NULL;
	}
	XtVaGetValues(w,
	    XmNlabelString,		&label_str,
	    NULL);
	XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &pTempStr);
	XmStringFree(label_str);
	OpenName = XtNewString(pTempStr + 3);

	if (file_saved) OpenFileProc();
	else {
	    WarningNum = WARNING_LIST;
	    set_QuestionDialog_message(QuestionDialog, FileName);
	    XtManageChild(QuestionDialog);
	}
}

#ifdef _NO_PROTO
void EnvMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void EnvMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	Cardinal num = (Cardinal)client_data;
	char buf[BUFSIZ];
	XmToggleButtonCallbackStruct *cbs =
				   (XmToggleButtonCallbackStruct *)call_data;

	switch (num) {
	    case 0 :	    /*	    Insert Mode	    */
		insert_mode = cbs->set;
		XtCallActionProc(text_w, "toggle-overstrike", cbs->event,
			(String *)NULL, 0);
		smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
		break;
	    case 1 :	  /*	  Indent Mode	  */
		indent_mode = cbs->set;
		if (indent_mode)
		    XtOverrideTranslations(text_w,
				XtParseTranslationTable(set_indent));
		else
		    XtOverrideTranslations(text_w,
				XtParseTranslationTable(set_unindent));
		smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
		break;
	    case 2 :	  /*	  Fill Mode	  */
		fill_mode = cbs->set;
		if (fill_mode)
		    XtOverrideTranslations(text_w,
				XtParseTranslationTable(optimal_fill));
		else
		    XtOverrideTranslations(text_w,
				XtParseTranslationTable(normal_fill));
		smStatusLineUpdateStatus(file_saved, insert_mode, indent_mode, fill_mode);
		break;
	    case 3 :	  /*	  Undo After Save		*/
		undo_after_save = cbs->set;
		break;
	    case 4 :	  /*	  Auto Matching		*/
		show_match = cbs->set;
		break;
	    case 5 :	  /*	  Pending delete	*/
		pending_delete = cbs->set;
		XtVaSetValues(text_w,
			XmNpendingDelete,	pending_delete,
			NULL);
		break;
	    case 6 :	  /*	  Set Tab Size	   */
		(void)sprintf(buf, "%d", TabSize);
		setPromptDialogText(tab_dialog, buf);
		XtManageChild(tab_dialog);
		break;
	    case 7 :	  /*	  Set Block Indent Size	    */
		(void)sprintf(buf, "%d", BlockIndentSize);
		setPromptDialogText(block_dialog, buf);
		XtManageChild(block_dialog);
		break;
	}

}

#ifdef _NO_PROTO
void PrefMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void PrefMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmToggleButtonCallbackStruct *cbs =
				   (XmToggleButtonCallbackStruct *)call_data;
	Cardinal num = (Cardinal)client_data;

	switch (num) {
	    case 0 :	  /*	  Create Backup File	  */
		create_backup = cbs->set;
		break;
	    case 1 :	  /* Autosave Environment */
		autosave_env = cbs->set;
		break;
	    case 2 :	  /* Optimize Text */
		optimize_text = cbs->set;
		break;
	    case 3 :	  /* Backupfile extention */
		setPromptDialogText(backup_dialog, backupExt);
		XtManageChild(backup_dialog);
		break;
	}
}

#ifdef _NO_PROTO
void PopupMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void PopupMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
	Cardinal num = (Cardinal)client_data;

	switch(num) {
	    case 1 :	/*	Redo	*/
		XtCallActionProc(text_w, "RedoAction", cbs->event, (String *)NULL, 0);
		break;
	    case 2 :	/*	Cut	*/
		if (XmTextCut(text_w, cbs->event->xbutton.time) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 3 :	/*	Copy	*/
		if (XmTextCopy(text_w, cbs->event->xbutton.time) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	    case 4 :	/*	Paste	*/
		if (XmTextPaste(text_w) == True)
		  XFlush(cbs->event->xbutton.display);
		break;
	    case 5 :	/*	Delete	*/
		if (XmTextRemove(text_w) == True)
		    XFlush(cbs->event->xbutton.display);
		break;
	}
}

#ifdef _NO_PROTO
void PopupSubMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void PopupSubMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
	Cardinal num = (Cardinal)client_data;

	switch(num) {
	    case 0 :	/*	Undo Last Edit	*/
		XtCallActionProc(text_w, "UndoLast", cbs->event, (String *)NULL, 0);
		break;
	    case 1 :	/*	Undo All Edits	*/
		XtCallActionProc(text_w, "UndoAll", cbs->event, (String *)NULL, 0);
		break;
	}
}

#ifdef _NO_PROTO
void HelpMenu_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void HelpMenu_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	Cardinal item_no = (Cardinal)client_data;

	switch (item_no) {
	    case 0 :
		loadHelpText();
		if (XtIsManaged(help_dialog) == False)
		    loadHelpText();
		XtManageChild(help_dialog);

		break;
	    case 1 :
		XtManageChild(about_dialog);
		break;
	}
}


/*
	Toolbar Event Handler
*/
#ifdef _NO_PROTO
void eventProc_cb(w, client_data, event, continue_to_dispatch_return)
Widget w;
XtPointer client_data;
XEnterWindowEvent *event;
Boolean *continue_to_dispatch_return;
#else
void eventProc_cb(Widget w, XtPointer client_data,
		XEnterWindowEvent *event,
		Boolean *continue_to_dispatch_return)
#endif
{
	Cardinal num = (Cardinal)client_data;

	switch (num) {
	    case 0 :
		XtVaSetValues(w,
		    XmNshadowThickness,		1,
		    XmNmarginWidth,		2,
		    NULL);
		break;
	    case 1 :
		XtVaSetValues(w,
		    XmNshadowThickness,		0,
		    XmNmarginWidth,		3,
		    NULL);
		break;
	}
}

/*
	Toolbar Callback Proc
*/
#ifdef _NO_PROTO
void active_cb(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
#else
void active_cb(Widget w, XtPointer client_data, XtPointer call_data)
#endif
{
	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *)call_data;
	Cardinal action_no = (Cardinal)client_data;

	if (cbs->reason == XmCR_ACTIVATE) {
	    switch (action_no) {
		case 0 :	/*	New	*/
		    if (file_saved) close_text(text_w);
		    else {
			WarningNum = WARNING_NEW;
			set_QuestionDialog_message(QuestionDialog, FileName);
			XtManageChild(QuestionDialog);
		    }
		    break;
		case 1 :	/*	Open	*/
		    if (file_saved) XtManageChild(OpenDialog);
		    else {
			WarningNum = WARNING_OPEN;
			set_QuestionDialog_message(QuestionDialog, FileName);
			XtManageChild(QuestionDialog);
		    }
		    break;
		case 2 :	/*	Save	*/
		    if (strcmp(FileName, "(NONE)")) {
			if (!save_file(FileName)) {
			    set_ErrorDialog_message(ErrorDialog, FileName, FILE_SAVE);
			    XtManageChild(ErrorDialog);
			}
		    }
		    else {
			SaveNum = SAVE_DEFAULT;
			update_SaveAsDialog_data(SaveAsDialog, FileName);
			XtManageChild(SaveAsDialog);
		    }
		    break;
		case 3 :	/*	Cut	*/
		    if (XmTextCut(text_w, cbs->event->xbutton.time) == True)
			XFlush(cbs->event->xbutton.display);
		    break;
		case 4 :	/*	Copy	*/
		    if (XmTextCopy(text_w, cbs->event->xbutton.time) == True)
			XFlush(cbs->event->xbutton.display);
		    break;
		case 5 :	/*	Paste	*/
		    if (XmTextPaste(text_w) == True)
			XFlush(cbs->event->xbutton.display);
		    break;
		case 6 :	/*	Delete	*/
		    if (XmTextRemove(text_w) == True)
			XFlush(cbs->event->xbutton.display);
		    break;
		case 7 :	/*	Undo	*/
		    XtCallActionProc(text_w, "UndoLast", cbs->event, (String *)NULL, 0);
		    break;
		case 8 :	/*	Redo	*/
		    XtCallActionProc(text_w, "RedoAction", cbs->event, (String *)NULL, 0);
		    break;
		case 9 :	/*	Search	*/
		    smManageReplaceText(False);
		    if (!XtIsManaged(SearchDialog)) {
			SelectionToFind(text_w);
			XtManageChild(SearchDialog);
		    }
		    else SelectionToFind(text_w);
		    break;
		case 10 :	/* Block Unindent	*/
		    XtCallActionProc(text_w, "block-unindent", cbs->event, (String *)NULL, 0);
		    break;
		case 11 :	/* Block Indent */
		    XtCallActionProc(text_w, "block-indent", cbs->event, (String *)NULL, 0);
		    break;
	    }
	}
}
