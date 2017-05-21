/* @(#)strctrl.c 1.87 01/01/99 */
/*
 * Copyright (c) 1991-1998 Si-Won Cho.  All rights reserved.
 *
 *  strctrl.c - part of SM/Editor
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
 * Writen by Si-Won Cho		(stsolaris_at_gmail.com)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef hpux
#include <strings.h>
#endif
#include <X11/Intrinsic.h>
#include <Xm/Text.h>
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>	/* only for NOLINE constant */

#define SHIFT_RIGHT     0
#define SHIFT_LEFT      1

Boolean textBlock;

#ifdef _NO_PROTO
void parseFilename(FullName, PathName, FileName)
char *FullName;
char **PathName;
char **FileName;
#else
void parseFilename(char *FullName, char **PathName, char **FileName)
#endif
{
        int pathlen, filelen;
	char *pPos;

        /* find the last slash */
	pPos = strrchr(FullName, '/');
	pathlen = (pPos + 1) - FullName;
	filelen = strlen(FullName) - pathlen;

        *PathName = XtMalloc((Cardinal)pathlen + 1);
        strncpy(*PathName, FullName, pathlen);
        *(*PathName + pathlen) = '\0';

        *FileName = XtMalloc((Cardinal)filelen + 1);
        strncpy(*FileName, FullName + pathlen, filelen);
        *(*FileName + filelen) = '\0';
}

#ifdef _NO_PROTO
void smTextReplace(w, start, end, str)
Widget w;
XmTextPosition start;
XmTextPosition end;
char *str;
#else
void smTextReplace(Widget w, XmTextPosition start, XmTextPosition end, char *str)
#endif
{
        textBlock = True;
        XmTextReplace(w, start, end, str);
        textBlock = False;
}

#ifdef _NO_PROTO
int textPosToLineNum(w, position)
Widget w;
XmTextPosition position;
#else
int textPosToLineNum(Widget w, XmTextPosition position)
#endif
{
        int lineNum;
        Boolean found = True;
        XmTextPosition pos;

        lineNum = _XmTextPosToLine((XmTextWidget)w, position);
        if (lineNum == NOLINE) {
            pos = XmTextGetLastPosition(w) >> 1;
            if (pos >= position) {
                pos = 0;
                lineNum = 1;
                while (found && pos < position) {
                    found = XmTextFindString(w, pos, "\n", XmTEXT_FORWARD, &pos);
                    if (found) {
                        pos++;
                        lineNum++;
                    }
                }
                return (lineNum);
            }
            else {
                pos = XmTextGetLastPosition(w);
                lineNum = ((XmTextWidget)w)->text.total_lines + 1;
                while (found && pos > position) {
                    found = XmTextFindString(w, pos - 1, "\n", XmTEXT_BACKWARD, &pos);
                    if (found) lineNum--;
                    else lineNum = 1;
                }
                return (lineNum);
            }
        }
        return (((XmTextWidget)w)->text.top_line + lineNum + 1);
}

#ifdef _NO_PROTO
void smPosToLineCol(w, position, tab_size, line, column)
Widget w;
XmTextPosition position;
int tab_size;
int *line;
int *column;
#else         /* _NO_PROTO */
void smPosToLineCol(Widget w, XmTextPosition position, int tab_size, int *line, int *column)
#endif
{
#if (XmVersion >= 1002)
	int col_count, i;
        int status;
        char *buffer;
        Boolean found;
        XmTextPosition pos;

        if (position == 0)
            *line = 1;
        *column = 1;
        if (position > 0L) {
            *line = textPosToLineNum(w, position);
            found = XmTextFindString(w, position - 1, "\n", XmTEXT_BACKWARD, &pos);
            if (found) pos++;
            else pos = 0;
            col_count = 1;
            if (position > pos) {
                buffer = XtMalloc((Cardinal)(position - pos + 1));
                status = XmTextGetSubstring(w, pos, (int)(position - pos),
                        (int)(position - pos + 1), buffer);
	        if (status != XmCOPY_FAILED)
                    for (i = 0; i < position - pos; i++) {
	                if (buffer[i] == '\t')
	                    col_count = ((col_count + tab_size - 1) / tab_size) * tab_size + 1;
                        else col_count++;
                    }
	        XtFree(buffer);
	    }
	    *column = col_count;
        }
#else
	int col_count, i;
	int last_line = 0;
	char *str;

	str = XmTextGetString(w);
	col_count = 1;
	*line = 1;
	*column = 1;
	if (position > 0L || XmTextGetLastPosition(w) > 0L) {
            col_count = 1;
	    for (i = 0; i < (int)position; i++)
	        if (str[i] == '\n') {
	            (*line)++;
	            last_line = i + 1;
	        }
	    for (i = last_line; i < (int)position; i++)
	        if (str[i] == '\t')
	            col_count = ((col_count + tab_size - 1) / tab_size) * tab_size + 1;
	        else col_count++;
	    *column = col_count;
	}
	XtFree(str);
#endif
}

#ifdef _NO_PROTO
int smTextGetRows(w)
Widget w;
#else         /* _NO_PROTO */
int smTextGetRows(Widget w)
#endif
{
	short row;

	XtVaGetValues(w,
	        XmNrows, &row,
	        NULL);

	return ((int)(row - 1));
}

#ifdef _NO_PROTO
void smTextSetInsertionPosition(w, pos)
Widget w;
XmTextPosition pos;
#else         /* _NO_PROTO */
void smTextSetInsertionPosition(Widget w, XmTextPosition pos)
#endif
{
	int row;
	XmTextPosition top_1, top_2;

	row = smTextGetRows(w) >> 1;

	top_1 = XmTextGetTopCharacter(w);
	XmTextSetInsertionPosition(w, pos);
	top_2 = XmTextGetTopCharacter(w);

	if (top_2 < top_1) XmTextScroll(w, -row);
	else if (top_2 > top_1) XmTextScroll(w, row);
}

#ifdef _NO_PROTO
Boolean is_space(ch)
char ch;
#else         /* _NO_PROTO */
Boolean is_space(char ch)
#endif
{
	static char *str = " \t";

	return (strchr(str, ch) ? True : False);
}

#ifdef _NO_PROTO
Boolean is_num(str)
char *str;
#else         /* _NO_PROTO */
Boolean is_num(char *str)
#endif
{
	Boolean isnumber = True;

	while(*str) {
	    if (!isdigit((int)*str)) isnumber = False;
	    str++;
	}

	return (isnumber);
}

#ifdef _NO_PROTO
Boolean stob(str)
char *str;
#else         /* _NO_PROTO */
Boolean stob(char *str)
#endif
{
	return ((strcmp(str, "True")) ? False : True);
}

#ifdef _NO_PROTO
int search_pair(w, start, position)
Widget w;
XmTextPosition start;
XmTextPosition *position;
#else
int search_pair(Widget w, XmTextPosition start, XmTextPosition *position)
#endif
{
	int status;
	XmTextPosition i;
	char buf[3];
	int pair_matching = -1;
	char pair[2];
	Boolean search_forward = True;

        i = start;
	status = XmTextGetSubstring(w, i, 2, 3, buf);
	if (status != XmCOPY_FAILED) {
	    if (!strcmp(buf, "/*")) {
	        do {
       	            i++;
       	            status = XmTextGetSubstring(w, i, 2, 3, buf);
       	            if (status != XmCOPY_FAILED) {
       	                if (!strcmp(buf, "/*")) pair_matching--;
       	                else if (!strcmp(buf, "*/")) pair_matching++;
       	            } /* if */
       	        } while (status != XmCOPY_FAILED && pair_matching != 0);
	    }
	    else if (!strcmp(buf, "*/")) {
	        do {
                    i--;
                    status = XmTextGetSubstring(w, i, 2, 3, buf);
                    if (status != XmCOPY_FAILED) {
	                if (!strcmp(buf, "*/")) pair_matching--;
                        else if (!strcmp(buf, "/*")) pair_matching++;
                    } /* if */
                } while (i >= 0 && pair_matching != 0);
	    }
	    else if (strchr("{}()[]<>", buf[0])) {
	        switch (buf[0]) {
	            case '{' :
	                pair[0] = '{';
	                pair[1] = '}';
	                search_forward = True;
	                break;
	            case '}' :
	                pair[0] = '}';
	                pair[1] = '{';
	                search_forward = False;
	                break;
	            case '(' :
	                pair[0] = '(';
	                pair[1] = ')';
	                search_forward = True;
	                break;
	            case ')' :
	                pair[0] = ')';
	                pair[1] = '(';
	                search_forward = False;
	                break;
	            case '[' :
	                pair[0] = '[';
	                pair[1] = ']';
	                search_forward = True;
	                break;
	            case ']' :
	                pair[0] = ']';
	                pair[1] = '[';
	                search_forward = False;
	                break;
	            case '<' :
	                pair[0] = '<';
	                pair[1] = '>';
	                search_forward = True;
	                break;
	            case '>' :
	                pair[0] = '>';
	                pair[1] = '<';
	                search_forward = False;
	                break;
	        }
	        if (search_forward) {
	            do {
		        i++;
		        status = XmTextGetSubstring(w, i, 2, 3, buf);
		        if (status != XmCOPY_FAILED) {
		            if (buf[0] == pair[0]) pair_matching--;
		            else if (buf[0] == pair[1]) pair_matching++;
		        } /* if */
	            } while (i <= XmTextGetLastPosition(w) && pair_matching != 0);
	        }
	        else {
	            do {
		        i--;
		        status = XmTextGetSubstring(w, i, 2, 3, buf);
		        if (status != XmCOPY_FAILED) {
		            if (buf[0] == pair[0]) pair_matching--;
		            else if (buf[0] == pair[1]) pair_matching++;
		        } /* if */
	            } while (i >= 0 && pair_matching != 0);
	        } /* if (search_forward) */
	    } /* if (strchr("{}()[]<>", buf[0])) */
	} /* if (status != XmCOPY_FAILED) */

	if (pair_matching == 0)
	    *position = i;
	else *position = -1;

	return (pair_matching);
}

#ifdef _NO_PROTO
int find_matching(w, start, str, position)
Widget w;
XmTextPosition start;
char *str;
XmTextPosition *position;
#else
int find_matching(Widget w, XmTextPosition start, char *str, XmTextPosition *position)
#endif
{
	int status;
	XmTextPosition i = start;
	char buf[3];
	int pair_matching = -1, lineNum;
	char pair[2];
	Boolean search_forward = True;

        if (strchr("{}()[]<>", *str)) {
            switch (*str) {
                case '{' :
                    pair[0] = '{';
                    pair[1] = '}';
                    search_forward = True;
                    break;
                case '}' :
                    pair[0] = '}';
                    pair[1] = '{';
                    search_forward = False;
                    break;
                case '(' :
                    pair[0] = '(';
                    pair[1] = ')';
                    search_forward = True;
                    break;
                case ')' :
                    pair[0] = ')';
                    pair[1] = '(';
                    search_forward = False;
                    break;
                case '[' :
                    pair[0] = '[';
                    pair[1] = ']';
                    search_forward = True;
                    break;
                case ']' :
                    pair[0] = ']';
                    pair[1] = '[';
                    search_forward = False;
                    break;
                case '<' :
                    pair[0] = '<';
                    pair[1] = '>';
                    search_forward = True;
                    break;
                case '>' :
                    pair[0] = '>';
                    pair[1] = '<';
                    search_forward = False;
                    break;
            }
            if (search_forward) {
                do {
                    i++;
                    lineNum = _XmTextPosToLine((XmTextWidget)w, i);
                    if (lineNum != NOLINE) {
                        status = XmTextGetSubstring(w, i - 1, 2, 3, buf);
                        if (status != XmCOPY_FAILED) {
                            if (buf[0] == pair[0]) pair_matching--;
                            else if (buf[0] == pair[1]) pair_matching++;
                        } /* if */
                    } /* if */
                } while (lineNum != NOLINE && i < XmTextGetLastPosition(w) &&
                            pair_matching != 0);
                if (pair_matching == 0)
                    *position = i - 1;
                else *position = -1;
            }
            else {
                do {
                    i--;
                    lineNum = _XmTextPosToLine((XmTextWidget)w, i);
                    status = XmTextGetSubstring(w, i, 2, 3, buf);
                    if (status != XmCOPY_FAILED) {
                        if (buf[0] == pair[0]) pair_matching--;
                        else if (buf[0] == pair[1]) pair_matching++;
                    } /* if */
                } while (i >= 0 && pair_matching != 0);
                if (pair_matching == 0 && lineNum != NOLINE)
                    *position = i;
                else *position = -1;
            } /* if (search_forward) */
        } /* if (strchr("{}()[]<>", buf[0])) */

	return (pair_matching);
}

#ifdef _NO_PROTO
char *trimSpaces(s_str)
char *s_str;
#else
char *trimSpaces(char *s_str)
#endif
{
	FILE *tfp, *pfp;
	int num, position;
	char cmd[BUFSIZ];
	char *t_name;
	char *d_str;

	t_name = tmpnam(NULL);
	if ((tfp = fopen(t_name, "w"))) {
	    fprintf(tfp, "%s", s_str);
	    if (s_str[strlen(s_str) - 1] != '\n')
		fputc('\n', tfp);
	    fclose(tfp);
	}
	(void)sprintf(cmd, "sed -e 's/[ \t]*$//g' %s", t_name);
	if ((pfp = popen(cmd, "r"))) {
	    position = 0;
	    d_str = XtMalloc((Cardinal)BUFSIZ);
	    while ((num = read(fileno(pfp), d_str + position, BUFSIZ)) > 0) {
		position += num;
		d_str = XtRealloc(d_str, position + BUFSIZ);
	   }
	    d_str[position - 1] = '\0';
	    pclose(pfp);
	    remove(t_name);
	}

	return (d_str);
}

#ifdef _NO_PROTO
int MoveBlock(w, dir, start, end, size, tab_size)
Widget w;
int dir;
XmTextPosition start;
XmTextPosition end;
int size;
int tab_size;
#else
int MoveBlock(Widget w, int dir, XmTextPosition start, XmTextPosition end,
	    int size, int tab_size)
#endif
{
        int length;
	int status, num, position;
	FILE *tfp, *pfp;
	char cmd[BUFSIZ];
	char *t_name;
	char *s_str, *d_str;

	length = 0;
	s_str = XtMalloc((Cardinal)(end - start + 1));
        status = XmTextGetSubstring(w, start, end - start, end - start + 1, s_str);
        s_str[end - start] = '\0';

        if (status != XmCOPY_FAILED) {
	    t_name = tmpnam(NULL);
            if ((tfp = fopen(t_name, "w"))) {
                fprintf(tfp, "%s", s_str);
                if (s_str[strlen(s_str) - 1] != '\n')
                    fputc('\n', tfp);
                fclose(tfp);
	    }
            if (dir == SHIFT_RIGHT)
                (void)sprintf(cmd, "expand -%d %s | sed -e 's/^/%*s/' | unexpand -a", tab_size, t_name, size, " ");
            else
                (void)sprintf(cmd, "expand -%d %s | sed -e 's/^ \\{1,%d\\}//' | unexpand -a", tab_size, t_name, size);

            if ((pfp = popen(cmd, "r"))) {
                position = 0;
                d_str = XtMalloc((Cardinal)BUFSIZ);
                while ((num = read(fileno(pfp), d_str + position, BUFSIZ)) > 0) {
                    position += num;
                    d_str = XtRealloc(d_str, position + BUFSIZ);
               }
                d_str[position - 1] = '\0';
                pclose(pfp);
                remove(t_name);
                smTextReplace(w, start, end, d_str);
                length = strlen(d_str);
                XtFree(d_str);
            }
	}
	XtFree(s_str);

	return (length);
}

#ifdef _NO_PROTO
void shiftBlock(w, dir, size, tab_size)
Widget w;
int dir;
int size;
int tab_size;
#else
void shiftBlock(Widget w, int dir, int size, int tab_size)
#endif
{
        XmTextPosition start, end;
        Boolean is_selected, found;
        int length;

        is_selected = XmTextGetSelectionPosition(w, &start, &end);
        if (is_selected && start != end) {
            XmTextClearSelection(w, CurrentTime);
            if (start > 0) {
                found = XmTextFindString(w, start - 1, "\n", XmTEXT_BACKWARD, &start);
	        start = (found) ? start + 1 : 0;
	    }
	    else start = 0;
	    found = XmTextFindString(w, end - 1, "\n", XmTEXT_FORWARD, &end);
	    if (!found)
	        end = XmTextGetLastPosition(w);

	    length = MoveBlock(w, dir, start, end, size, tab_size);

            if (found)
                length++;
            XmTextSetSelection(w, start, start + length, CurrentTime);
       	}
       	else {
       	    start = end = XmTextGetInsertionPosition(w);
       	    if (start > 0) {
                found = XmTextFindString(w, start - 1, "\n", XmTEXT_BACKWARD, &start);
	        start = (found) ? start + 1 : 0;
	    }
	    else start = 0;
	    found = XmTextFindString(w, end, "\n", XmTEXT_FORWARD, &end);
	    if (!found)
	        end = XmTextGetLastPosition(w);

	    length = MoveBlock(w, dir, start, end, size, tab_size);
       	}
}

#ifdef _NO_PROTO
void UpperCase(w)
Widget w;
#else
void UpperCase(Widget w)
#endif
{
	XmTextPosition start, end;
	Boolean is_selected;
	char *str, *p, *q, c;

	is_selected = XmTextGetSelectionPosition(w, &start, &end);
	if (is_selected && start != end) {
	    if ((str = XmTextGetSelection(w)) != NULL) {
	        p = q = str;
	        while ((c = *p++))
	            *q++ = ((c >= 'a') && (c <='z')) ? toupper(c) : c;
	        smTextReplace(w, start, end, str);
	    }
	}
}

#ifdef _NO_PROTO
void LowerCase(w)
Widget w;
#else
void LowerCase(Widget w)
#endif
{
	XmTextPosition start, end;
	Boolean is_selected;
	char *str, *p, *q, c;

	is_selected = XmTextGetSelectionPosition(w, &start, &end);
	if (is_selected && start != end) {
	    if ((str = XmTextGetSelection(w)) != NULL) {
	        p = q = str;
	        while ((c = *p++))
	            *q++ = ((c >= 'A') && (c <='Z')) ? tolower(c) : c;
	        smTextReplace(w, start, end, str);
	    }
	}
}
