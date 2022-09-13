/*
    Lean Editor -- a small text editor for programmers.
    Copyright (C) 2017-2022 Lars Lindehaven

    Work based on the Program Text editor (te) v1.08 from 05 Jul 2017.
    Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2, or (at your option) any
    later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 */

/* Terminal settings ------------------------------------------------------ */
#define TERM_ROWS    25     /* Number of rows on terminal screen.           */
#define TERM_COLS    80     /* Number of columns on terminal screen.        */
#define TERM_TABS     8     /* Number of spaces for each tab (tab width).   */

/* Cursor movement -------------------------------------------------------- */
#define KEY_RUP       5     /* ^E       Cursor one row up                   */
#define KEY_RDN      24     /* ^X       Cursor one row down                 */
#define KEY_CLT      19     /* ^S       Cursor one column left              */
#define KEY_CRT       4     /* ^D       Cursor one column right             */
#define KEY_WLT       1     /* ^A       Cursor one word left                */
#define KEY_WRT       6     /* ^F       Cursor one word right               */
#define KEY_LBEG     10     /* ^J       Cursor to beginning of line         */
#define KEY_LEND     11     /* ^K       Cursor to end of line               */
#define KEY_PUP      18     /* ^R       Cursor one page up                  */
#define KEY_PDN       3     /* ^C       Cursor one page down                */
#define KEY_FBEG     20     /* ^T       Cursor to beginning of file         */
#define KEY_FEND     22     /* ^V       Cursor to end of file               */
#define KEY_CENTER   26     /* ^Z       Center current row                  */
#define KEY_SEARCH   12     /* ^L       Search string incrementally (look)  */
#define KEY_SCREEN   17     /* ^Q       Unpause screen output               */

/* Editing ---------------------------------------------------------------- */
#define KEY_TABW     21     /* ^U       Change tab width                    */
#define KEY_TAB       9     /* ^I       Insert tab width spaces             */
#define KEY_LINS     13     /* ^M       Insert new line                     */
#define KEY_DEL       7     /* ^G       Delete character to the right       */
#define KEY_RUB       8     /* ^H       Delete character to the left        */
#define KEY_BS      127     /* BACK     Delete character to the left        */
#define KEY_LCUT     15     /* ^O       Move line to paste buffer           */
#define KEY_PASTE    16     /* ^P       Insert line(s) from paste buffer    */
#define KEY_LUP      25     /* ^Y       Move line up                        */
#define KEY_LDN       2     /* ^B       Move line down                      */

/* File commands ---------------------------------------------------------- */
#define KEY_FSAVE    23     /* ^W       Write to file                       */
#define KEY_FQUIT    27     /* ESC      Escape (save and quit)              */
