/*
 *  Syntax Highlighter -- prints highlighted source code
 *  Copyright (C) 2017 Lars Lindehaven
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

#define SHL_FAIL    -1
#define SHL_DONE    0

/* int shl_language(char *lang_file);
 * Sets the desired language for parsing and highlighting.
 *  @lang_file File name with file extension for the desired language.
 *  Returns SHL_DONE if language is supported or SHL_FAIL if language is
 *  not supported.
 */
extern int shl_language();

/* int shl_tab_stop(int tab_stop);
 * Sets the desired tab stop for emitting highlighted strings.
 * @tab_stop Desired tab stop (tab width) in the range {1..8}. Default 8.
 * Returns the set tab stop.
 */
extern int shl_tab_stop();

/* int shl_width(int width);
 * Sets the desired screen width for emitting highlighted strings.
 * @width Desired width in the range {1..32767}. Default 32767.
 * Returns the set width.
 */
extern int shl_width();

/* int shl_column(int column);
 * Sets the desired column for emitting highlighted strings.
 * @column Desired column in the range {0..32767}. Default 32767.
 * Returns the set column.
 */
extern int shl_column();

/* int shl_highlight(char *buf_b, char *buf_e, char *buf_p, int rows);
 * Parses buffer and prints highlighted string on screen.
 *  @buf_b Points to beginning of buffer.
 *  @buf_e Points to end of buffer.
 *  @buf_p Points to part of buffer where the printing shall start.
 *  @rows  Maximum number of rows to print.
 *  Returns number of parsed characters or SHL_FAIL if failed.
 */
extern int shl_highlight();
