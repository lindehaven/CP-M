/*
 *  Lean Editor -- a small text editor for programmers
 *
 *  Copyright (C) 2017 Lars Lindehaven <lars dot lindehaven at gmail dot com>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Terminal settings ------------------------------------------------------ */
#define TERM_ROWS   25      /* Number of rows on terminal screen.           */
#define TERM_COLS   80      /* Number of columns on terminal screen.        */
#define TERM_TABS   4       /* Number of spaces for each tab.               */

/* Cursor movement -------------------------------------------------------- */
#define KEY_RUP       5     /* ^E       Cursor one row up                   */
#define KEY_RDN      24     /* ^X       Cursor one row down                 */
#define KEY_CLT      19     /* ^S       Cursor one column left              */
#define KEY_CRT       4     /* ^D       Cursor one column right             */
#define KEY_WLT       1     /* ^A       Cursor one word left                */
#define KEY_WRT       6     /* ^F       Cursor one word right               */
#define KEY_PUP      18     /* ^R       Cursor one page up                  */
#define KEY_PDN       3     /* ^C       Cursor one page down                */
#define KEY_LBEG      2     /* ^B       Cursor to beginning of line         */
#define KEY_LEND     11     /* ^K       Cursor to end of line               */
#define KEY_FBEG     16     /* ^P       Cursor to beginning of file         */
#define KEY_FEND     12     /* ^L       Cursor to end of file               */

/* Editing ---------------------------------------------------------------- */
#define KEY_TAB       9     /* ^I       Insert tab                          */
#define KEY_LNEW     13     /* ^M       Insert CR/LF                        */
#define KEY_DEL       7     /* ^G       Delete character to the right       */
#define KEY_RUB       8     /* ^H       Delete character to the left        */
#define KEY_BS      127     /* BACK     Delete character to the left        */
#define KEY_CUT     255     /* --       Cut the current line to buffer      */
#define KEY_COPY    254     /* --       Copy the current line to buffer     */
#define KEY_PASTE   253     /* --       Paste the buffer to a new line      */

/* File commands ---------------------------------------------------------- */
#define KEY_FILE     27     /* ESC      Opens file commands                 */
#define KEY_FNEW    'N'     /* ESC N    New file                            */
#define KEY_FOPEN   'O'     /* ESC O    Open file                           */
#define KEY_FSAVE   'S'     /* ESC S    Save file                           */
#define KEY_FSAS    'A'     /* ESC A    Save file as                        */
#define KEY_FEXIT   'X'     /* ESC X    Save file and exit                  */
#define KEY_FQUIT   'Q'     /* ESC Q    Exit without saving                 */

/* Quick commands --------------------------------------------------------- */
#define KEY_QUICK    17     /* ^Q       Starts quick commands               */
#define KEY_QLBEG   'S'     /* ^Q S     Cursor to beginning of line         */
#define KEY_QLEND   'D'     /* ^Q D     Cursor to end of line               */
#define KEY_QFBEG   'R'     /* ^Q R     Cursor to beginning of file         */
#define KEY_QFEND   'C'     /* ^Q C     Cursor to end of file               */
