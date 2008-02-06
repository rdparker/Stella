//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2008 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: ConsoleFont.hxx,v 1.7 2008-02-06 13:45:23 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef CONSOLE_FONT_DATA_HXX
#define CONSOLE_FONT_DATA_HXX

#include "bspf.hxx"

namespace GUI {
/* Font information:
   name: 8x13
   facename: -Misc-Fixed-Medium-R-Normal--13-120-75-75-C-80-ISO10646-1
   w x h: 8x13
   size: 127
   ascent: 11
   descent: 2
   first char: 0 (0x00)
   last char: 126 (0x7e)
   default char: 0 (0x00)
   proportional: no
   Public domain font.  Share and enjoy.
*/

/* Font character bitmap data. */
static const uInt16 _console_font_bits[] = {

/* Character 0 (0x00):
   width 8
   +--------+
   |        |
   |        |
   |* * * * |
   |        |
   |*     * |
   |        |
   |*     * |
   |        |
   |*     * |
   |        |
   |* * * * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0xaa00,
0x0000,
0x8200,
0x0000,
0x8200,
0x0000,
0x8200,
0x0000,
0xaa00,
0x0000,
0x0000,

/* Character 32 (0x20):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 33 (0x21):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |        |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x0000,
0x1000,
0x0000,
0x0000,

/* Character 34 (0x22):
   width 8
   +--------+
   |        |
   |        |
   |  *  *  |
   |  *  *  |
   |  *  *  |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x2400,
0x2400,
0x2400,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 35 (0x23):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |  *  *  |
   |  *  *  |
   | ****** |
   |  *  *  |
   | ****** |
   |  *  *  |
   |  *  *  |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x2400,
0x2400,
0x7e00,
0x2400,
0x7e00,
0x2400,
0x2400,
0x0000,
0x0000,
0x0000,

/* Character 36 (0x24):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |  ****  |
   | * *    |
   | * *    |
   |  ***   |
   |   * *  |
   |   * *  |
   | ****   |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x3c00,
0x5000,
0x5000,
0x3800,
0x1400,
0x1400,
0x7800,
0x1000,
0x0000,
0x0000,

/* Character 37 (0x25):
   width 8
   +--------+
   |        |
   |        |
   |  *   * |
   | * *  * |
   |  *  *  |
   |    *   |
   |    *   |
   |   *    |
   |  *  *  |
   |  * * * |
   | *   *  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x2200,
0x5200,
0x2400,
0x0800,
0x0800,
0x1000,
0x2400,
0x2a00,
0x4400,
0x0000,
0x0000,

/* Character 38 (0x26):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |  **    |
   | *  *   |
   | *  *   |
   |  **    |
   | *  * * |
   | *   *  |
   |  *** * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x3000,
0x4800,
0x4800,
0x3000,
0x4a00,
0x4400,
0x3a00,
0x0000,
0x0000,

/* Character 39 (0x27):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |   *    |
   |   *    |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x1000,
0x1000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 40 (0x28):
   width 8
   +--------+
   |        |
   |        |
   |     *  |
   |    *   |
   |    *   |
   |   *    |
   |   *    |
   |   *    |
   |    *   |
   |    *   |
   |     *  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0400,
0x0800,
0x0800,
0x1000,
0x1000,
0x1000,
0x0800,
0x0800,
0x0400,
0x0000,
0x0000,

/* Character 41 (0x29):
   width 8
   +--------+
   |        |
   |        |
   |  *     |
   |   *    |
   |   *    |
   |    *   |
   |    *   |
   |    *   |
   |   *    |
   |   *    |
   |  *     |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x2000,
0x1000,
0x1000,
0x0800,
0x0800,
0x0800,
0x1000,
0x1000,
0x2000,
0x0000,
0x0000,

/* Character 42 (0x2a):
   width 8
   +--------+
   |        |
   |        |
   |  *  *  |
   |   **   |
   | ****** |
   |   **   |
   |  *  *  |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x2400,
0x1800,
0x7e00,
0x1800,
0x2400,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 43 (0x2b):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |   *    |
   |   *    |
   | *****  |
   |   *    |
   |   *    |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x1000,
0x1000,
0x7c00,
0x1000,
0x1000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 44 (0x2c):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ***   |
   |  **    |
   | *      |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3800,
0x3000,
0x4000,
0x0000,

/* Character 45 (0x2d):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   | *****  |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x7c00,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 46 (0x2e):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |   *    |
   |  ***   |
   |   *    |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x1000,
0x3800,
0x1000,
0x0000,

/* Character 47 (0x2f):
   width 8
   +--------+
   |        |
   |        |
   |      * |
   |      * |
   |     *  |
   |    *   |
   |   *    |
   |  *     |
   | *      |
   |*       |
   |*       |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0200,
0x0200,
0x0400,
0x0800,
0x1000,
0x2000,
0x4000,
0x8000,
0x8000,
0x0000,
0x0000,

/* Character 48 (0x30):
   width 8
   +--------+
   |        |
   |        |
   |   **   |
   |  *  *  |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |  *  *  |
   |   **   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1800,
0x2400,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x2400,
0x1800,
0x0000,
0x0000,

/* Character 49 (0x31):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |  **    |
   | * *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   | *****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x3000,
0x5000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x7c00,
0x0000,
0x0000,

/* Character 50 (0x32):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   |      * |
   |     *  |
   |   **   |
   |  *     |
   | *      |
   | ****** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x0200,
0x0400,
0x1800,
0x2000,
0x4000,
0x7e00,
0x0000,
0x0000,

/* Character 51 (0x33):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   |      * |
   |     *  |
   |    *   |
   |   ***  |
   |      * |
   |      * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x0200,
0x0400,
0x0800,
0x1c00,
0x0200,
0x0200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 52 (0x34):
   width 8
   +--------+
   |        |
   |        |
   |     *  |
   |    **  |
   |   * *  |
   |  *  *  |
   | *   *  |
   | *   *  |
   | ****** |
   |     *  |
   |     *  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0400,
0x0c00,
0x1400,
0x2400,
0x4400,
0x4400,
0x7e00,
0x0400,
0x0400,
0x0000,
0x0000,

/* Character 53 (0x35):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   | *      |
   | *      |
   | * ***  |
   | **   * |
   |      * |
   |      * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x4000,
0x4000,
0x5c00,
0x6200,
0x0200,
0x0200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 54 (0x36):
   width 8
   +--------+
   |        |
   |        |
   |   ***  |
   |  *     |
   | *      |
   | *      |
   | * ***  |
   | **   * |
   | *    * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1c00,
0x2000,
0x4000,
0x4000,
0x5c00,
0x6200,
0x4200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 55 (0x37):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   |      * |
   |     *  |
   |    *   |
   |    *   |
   |   *    |
   |   *    |
   |  *     |
   |  *     |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x0200,
0x0400,
0x0800,
0x0800,
0x1000,
0x1000,
0x2000,
0x2000,
0x0000,
0x0000,

/* Character 56 (0x38):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *    * |
   |  ****  |
   | *    * |
   | *    * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4200,
0x3c00,
0x4200,
0x4200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 57 (0x39):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *   ** |
   |  *** * |
   |      * |
   |      * |
   |     *  |
   |  ***   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4600,
0x3a00,
0x0200,
0x0200,
0x0400,
0x3800,
0x0000,
0x0000,

/* Character 58 (0x3a):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |   *    |
   |  ***   |
   |   *    |
   |        |
   |        |
   |   *    |
   |  ***   |
   |   *    |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x1000,
0x3800,
0x1000,
0x0000,
0x0000,
0x1000,
0x3800,
0x1000,
0x0000,

/* Character 59 (0x3b):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |   *    |
   |  ***   |
   |   *    |
   |        |
   |        |
   |  ***   |
   |  **    |
   | *      |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x1000,
0x3800,
0x1000,
0x0000,
0x0000,
0x3800,
0x3000,
0x4000,
0x0000,

/* Character 60 (0x3c):
   width 8
   +--------+
   |        |
   |        |
   |      * |
   |     *  |
   |    *   |
   |   *    |
   |  *     |
   |   *    |
   |    *   |
   |     *  |
   |      * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0200,
0x0400,
0x0800,
0x1000,
0x2000,
0x1000,
0x0800,
0x0400,
0x0200,
0x0000,
0x0000,

/* Character 61 (0x3d):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | ****** |
   |        |
   |        |
   | ****** |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x7e00,
0x0000,
0x0000,
0x7e00,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 62 (0x3e):
   width 8
   +--------+
   |        |
   |        |
   | *      |
   |  *     |
   |   *    |
   |    *   |
   |     *  |
   |    *   |
   |   *    |
   |  *     |
   | *      |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4000,
0x2000,
0x1000,
0x0800,
0x0400,
0x0800,
0x1000,
0x2000,
0x4000,
0x0000,
0x0000,

/* Character 63 (0x3f):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   |      * |
   |     *  |
   |    *   |
   |    *   |
   |        |
   |    *   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x0200,
0x0400,
0x0800,
0x0800,
0x0000,
0x0800,
0x0000,
0x0000,

/* Character 64 (0x40):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *  *** |
   | * *  * |
   | * * ** |
   | *  * * |
   | *      |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4e00,
0x5200,
0x5600,
0x4a00,
0x4000,
0x3c00,
0x0000,
0x0000,

/* Character 65 (0x41):
   width 8
   +--------+
   |        |
   |        |
   |   **   |
   |  *  *  |
   | *    * |
   | *    * |
   | *    * |
   | ****** |
   | *    * |
   | *    * |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1800,
0x2400,
0x4200,
0x4200,
0x4200,
0x7e00,
0x4200,
0x4200,
0x4200,
0x0000,
0x0000,

/* Character 66 (0x42):
   width 8
   +--------+
   |        |
   |        |
   | ****   |
   | *   *  |
   | *    * |
   | *   *  |
   | ****   |
   | *   *  |
   | *    * |
   | *   *  |
   | ****   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7800,
0x4400,
0x4200,
0x4400,
0x7800,
0x4400,
0x4200,
0x4400,
0x7800,
0x0000,
0x0000,

/* Character 67 (0x43):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *      |
   | *      |
   | *      |
   | *      |
   | *      |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4000,
0x4000,
0x4000,
0x4000,
0x4000,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 68 (0x44):
   width 8
   +--------+
   |        |
   |        |
   | ****   |
   | *   *  |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *   *  |
   | ****   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7800,
0x4400,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4400,
0x7800,
0x0000,
0x0000,

/* Character 69 (0x45):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   | *      |
   | *      |
   | *      |
   | ****   |
   | *      |
   | *      |
   | *      |
   | ****** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x4000,
0x4000,
0x4000,
0x7800,
0x4000,
0x4000,
0x4000,
0x7e00,
0x0000,
0x0000,

/* Character 70 (0x46):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   | *      |
   | *      |
   | *      |
   | ****   |
   | *      |
   | *      |
   | *      |
   | *      |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x4000,
0x4000,
0x4000,
0x7800,
0x4000,
0x4000,
0x4000,
0x4000,
0x0000,
0x0000,

/* Character 71 (0x47):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *      |
   | *      |
   | *      |
   | *  *** |
   | *    * |
   | *   ** |
   |  *** * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4000,
0x4000,
0x4000,
0x4e00,
0x4200,
0x4600,
0x3a00,
0x0000,
0x0000,

/* Character 72 (0x48):
   width 8
   +--------+
   |        |
   |        |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | ****** |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4200,
0x4200,
0x4200,
0x4200,
0x7e00,
0x4200,
0x4200,
0x4200,
0x4200,
0x0000,
0x0000,

/* Character 73 (0x49):
   width 8
   +--------+
   |        |
   |        |
   | *****  |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   | *****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7c00,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x7c00,
0x0000,
0x0000,

/* Character 74 (0x4a):
   width 8
   +--------+
   |        |
   |        |
   |   *****|
   |     *  |
   |     *  |
   |     *  |
   |     *  |
   |     *  |
   |     *  |
   | *   *  |
   |  ***   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1f00,
0x0400,
0x0400,
0x0400,
0x0400,
0x0400,
0x0400,
0x4400,
0x3800,
0x0000,
0x0000,

/* Character 75 (0x4b):
   width 8
   +--------+
   |        |
   |        |
   | *    * |
   | *   *  |
   | *  *   |
   | * *    |
   | **     |
   | * *    |
   | *  *   |
   | *   *  |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4200,
0x4400,
0x4800,
0x5000,
0x6000,
0x5000,
0x4800,
0x4400,
0x4200,
0x0000,
0x0000,

/* Character 76 (0x4c):
   width 8
   +--------+
   |        |
   |        |
   | *      |
   | *      |
   | *      |
   | *      |
   | *      |
   | *      |
   | *      |
   | *      |
   | ****** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4000,
0x4000,
0x4000,
0x4000,
0x4000,
0x4000,
0x4000,
0x4000,
0x7e00,
0x0000,
0x0000,

/* Character 77 (0x4d):
   width 8
   +--------+
   |        |
   |        |
   |*     * |
   |*     * |
   |**   ** |
   |* * * * |
   |*  *  * |
   |*  *  * |
   |*     * |
   |*     * |
   |*     * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8200,
0x8200,
0xc600,
0xaa00,
0x9200,
0x9200,
0x8200,
0x8200,
0x8200,
0x0000,
0x0000,

/* Character 78 (0x4e):
   width 8
   +--------+
   |        |
   |        |
   | *    * |
   | *    * |
   | **   * |
   | * *  * |
   | *  * * |
   | *   ** |
   | *    * |
   | *    * |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4200,
0x4200,
0x6200,
0x5200,
0x4a00,
0x4600,
0x4200,
0x4200,
0x4200,
0x0000,
0x0000,

/* Character 79 (0x4f):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 80 (0x50):
   width 8
   +--------+
   |        |
   |        |
   | *****  |
   | *    * |
   | *    * |
   | *    * |
   | *****  |
   | *      |
   | *      |
   | *      |
   | *      |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7c00,
0x4200,
0x4200,
0x4200,
0x7c00,
0x4000,
0x4000,
0x4000,
0x4000,
0x0000,
0x0000,

/* Character 81 (0x51):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | * *  * |
   | *  * * |
   |  ****  |
   |      * |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x5200,
0x4a00,
0x3c00,
0x0200,
0x0000,

/* Character 82 (0x52):
   width 8
   +--------+
   |        |
   |        |
   | *****  |
   | *    * |
   | *    * |
   | *    * |
   | *****  |
   | * *    |
   | *  *   |
   | *   *  |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7c00,
0x4200,
0x4200,
0x4200,
0x7c00,
0x5000,
0x4800,
0x4400,
0x4200,
0x0000,
0x0000,

/* Character 83 (0x53):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   | *    * |
   | *      |
   | *      |
   |  ****  |
   |      * |
   |      * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x4200,
0x4000,
0x4000,
0x3c00,
0x0200,
0x0200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 84 (0x54):
   width 8
   +--------+
   |        |
   |        |
   |******* |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0xfe00,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x0000,
0x0000,

/* Character 85 (0x55):
   width 8
   +--------+
   |        |
   |        |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 86 (0x56):
   width 8
   +--------+
   |        |
   |        |
   |*     * |
   |*     * |
   | *   *  |
   | *   *  |
   | *   *  |
   |  * *   |
   |  * *   |
   |  * *   |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8200,
0x8200,
0x4400,
0x4400,
0x4400,
0x2800,
0x2800,
0x2800,
0x1000,
0x0000,
0x0000,

/* Character 87 (0x57):
   width 8
   +--------+
   |        |
   |        |
   |*     * |
   |*     * |
   |*     * |
   |*     * |
   |*  *  * |
   |*  *  * |
   |*  *  * |
   |* * * * |
   | *   *  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8200,
0x8200,
0x8200,
0x8200,
0x9200,
0x9200,
0x9200,
0xaa00,
0x4400,
0x0000,
0x0000,

/* Character 88 (0x58):
   width 8
   +--------+
   |        |
   |        |
   |*     * |
   |*     * |
   | *   *  |
   |  * *   |
   |   *    |
   |  * *   |
   | *   *  |
   |*     * |
   |*     * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8200,
0x8200,
0x4400,
0x2800,
0x1000,
0x2800,
0x4400,
0x8200,
0x8200,
0x0000,
0x0000,

/* Character 89 (0x59):
   width 8
   +--------+
   |        |
   |        |
   |*     * |
   |*     * |
   | *   *  |
   |  * *   |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8200,
0x8200,
0x4400,
0x2800,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x0000,
0x0000,

/* Character 90 (0x5a):
   width 8
   +--------+
   |        |
   |        |
   | ****** |
   |      * |
   |     *  |
   |    *   |
   |   *    |
   |  *     |
   | *      |
   | *      |
   | ****** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7e00,
0x0200,
0x0400,
0x0800,
0x1000,
0x2000,
0x4000,
0x4000,
0x7e00,
0x0000,
0x0000,

/* Character 91 (0x5b):
   width 8
   +--------+
   |        |
   |        |
   |  ****  |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3c00,
0x2000,
0x2000,
0x2000,
0x2000,
0x2000,
0x2000,
0x2000,
0x3c00,
0x0000,
0x0000,

/* Character 92 (0x5c):
   width 8
   +--------+
   |        |
   |        |
   |*       |
   |*       |
   | *      |
   |  *     |
   |   *    |
   |    *   |
   |     *  |
   |      * |
   |      * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x8000,
0x8000,
0x4000,
0x2000,
0x1000,
0x0800,
0x0400,
0x0200,
0x0200,
0x0000,
0x0000,

/* Character 93 (0x5d):
   width 8
   +--------+
   |        |
   |        |
   | ****   |
   |    *   |
   |    *   |
   |    *   |
   |    *   |
   |    *   |
   |    *   |
   |    *   |
   | ****   |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7800,
0x0800,
0x0800,
0x0800,
0x0800,
0x0800,
0x0800,
0x0800,
0x7800,
0x0000,
0x0000,

/* Character 94 (0x5e):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |  * *   |
   | *   *  |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x2800,
0x4400,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 95 (0x5f):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |******* |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0xfe00,
0x0000,

/* Character 96 (0x60):
   width 8
   +--------+
   |        |
   |   *    |
   |    *   |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x1000,
0x0800,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,

/* Character 97 (0x61):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ****  |
   |      * |
   |  ***** |
   | *    * |
   | *   ** |
   |  *** * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3c00,
0x0200,
0x3e00,
0x4200,
0x4600,
0x3a00,
0x0000,
0x0000,

/* Character 98 (0x62):
   width 8
   +--------+
   |        |
   |        |
   | *      |
   | *      |
   | *      |
   | * ***  |
   | **   * |
   | *    * |
   | *    * |
   | **   * |
   | * ***  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4000,
0x4000,
0x4000,
0x5c00,
0x6200,
0x4200,
0x4200,
0x6200,
0x5c00,
0x0000,
0x0000,

/* Character 99 (0x63):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ****  |
   | *    * |
   | *      |
   | *      |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3c00,
0x4200,
0x4000,
0x4000,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 100 (0x64):
   width 8
   +--------+
   |        |
   |        |
   |      * |
   |      * |
   |      * |
   |  *** * |
   | *   ** |
   | *    * |
   | *    * |
   | *   ** |
   |  *** * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0200,
0x0200,
0x0200,
0x3a00,
0x4600,
0x4200,
0x4200,
0x4600,
0x3a00,
0x0000,
0x0000,

/* Character 101 (0x65):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ****  |
   | *    * |
   | ****** |
   | *      |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3c00,
0x4200,
0x7e00,
0x4000,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 102 (0x66):
   width 8
   +--------+
   |        |
   |        |
   |   ***  |
   |  *   * |
   |  *     |
   |  *     |
   | *****  |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1c00,
0x2200,
0x2000,
0x2000,
0x7c00,
0x2000,
0x2000,
0x2000,
0x2000,
0x0000,
0x0000,

/* Character 103 (0x67):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  *** * |
   | *   *  |
   | *   *  |
   |  ***   |
   | *      |
   |  ****  |
   | *    * |
   |  ****  |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3a00,
0x4400,
0x4400,
0x3800,
0x4000,
0x3c00,
0x4200,
0x3c00,

/* Character 104 (0x68):
   width 8
   +--------+
   |        |
   |        |
   | *      |
   | *      |
   | *      |
   | * ***  |
   | **   * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4000,
0x4000,
0x4000,
0x5c00,
0x6200,
0x4200,
0x4200,
0x4200,
0x4200,
0x0000,
0x0000,

/* Character 105 (0x69):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |   *    |
   |        |
   |  **    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   | *****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x1000,
0x0000,
0x3000,
0x1000,
0x1000,
0x1000,
0x1000,
0x7c00,
0x0000,
0x0000,

/* Character 106 (0x6a):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |     *  |
   |        |
   |    **  |
   |     *  |
   |     *  |
   |     *  |
   |     *  |
   | *   *  |
   | *   *  |
   |  ***   |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0400,
0x0000,
0x0c00,
0x0400,
0x0400,
0x0400,
0x0400,
0x4400,
0x4400,
0x3800,

/* Character 107 (0x6b):
   width 8
   +--------+
   |        |
   |        |
   | *      |
   | *      |
   | *      |
   | *   *  |
   | *  *   |
   | ***    |
   | *  *   |
   | *   *  |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x4000,
0x4000,
0x4000,
0x4400,
0x4800,
0x7000,
0x4800,
0x4400,
0x4200,
0x0000,
0x0000,

/* Character 108 (0x6c):
   width 8
   +--------+
   |        |
   |        |
   |  **    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   | *****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x3000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x7c00,
0x0000,
0x0000,

/* Character 109 (0x6d):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |*** **  |
   |*  *  * |
   |*  *  * |
   |*  *  * |
   |*  *  * |
   |*     * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0xec00,
0x9200,
0x9200,
0x9200,
0x9200,
0x8200,
0x0000,
0x0000,

/* Character 110 (0x6e):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | * ***  |
   | **   * |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x5c00,
0x6200,
0x4200,
0x4200,
0x4200,
0x4200,
0x0000,
0x0000,

/* Character 111 (0x6f):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ****  |
   | *    * |
   | *    * |
   | *    * |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3c00,
0x4200,
0x4200,
0x4200,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 112 (0x70):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | * ***  |
   | **   * |
   | *    * |
   | **   * |
   | * ***  |
   | *      |
   | *      |
   | *      |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x5c00,
0x6200,
0x4200,
0x6200,
0x5c00,
0x4000,
0x4000,
0x4000,

/* Character 113 (0x71):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  *** * |
   | *   ** |
   | *    * |
   | *   ** |
   |  *** * |
   |      * |
   |      * |
   |      * |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3a00,
0x4600,
0x4200,
0x4600,
0x3a00,
0x0200,
0x0200,
0x0200,

/* Character 114 (0x72):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | * ***  |
   |  *   * |
   |  *     |
   |  *     |
   |  *     |
   |  *     |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x5c00,
0x2200,
0x2000,
0x2000,
0x2000,
0x2000,
0x0000,
0x0000,

/* Character 115 (0x73):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |  ****  |
   | *    * |
   |  **    |
   |    **  |
   | *    * |
   |  ****  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x3c00,
0x4200,
0x3000,
0x0c00,
0x4200,
0x3c00,
0x0000,
0x0000,

/* Character 116 (0x74):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |  *     |
   |  *     |
   | *****  |
   |  *     |
   |  *     |
   |  *     |
   |  *   * |
   |   ***  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x2000,
0x2000,
0x7c00,
0x2000,
0x2000,
0x2000,
0x2200,
0x1c00,
0x0000,
0x0000,

/* Character 117 (0x75):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | *   *  |
   | *   *  |
   | *   *  |
   | *   *  |
   | *   *  |
   |  *** * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x4400,
0x4400,
0x4400,
0x4400,
0x4400,
0x3a00,
0x0000,
0x0000,

/* Character 118 (0x76):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | *   *  |
   | *   *  |
   | *   *  |
   |  * *   |
   |  * *   |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x4400,
0x4400,
0x4400,
0x2800,
0x2800,
0x1000,
0x0000,
0x0000,

/* Character 119 (0x77):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   |*     * |
   |*     * |
   |*  *  * |
   |*  *  * |
   |* * * * |
   | *   *  |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x8200,
0x8200,
0x9200,
0x9200,
0xaa00,
0x4400,
0x0000,
0x0000,

/* Character 120 (0x78):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | *    * |
   |  *  *  |
   |   **   |
   |   **   |
   |  *  *  |
   | *    * |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x4200,
0x2400,
0x1800,
0x1800,
0x2400,
0x4200,
0x0000,
0x0000,

/* Character 121 (0x79):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | *    * |
   | *    * |
   | *    * |
   | *   ** |
   |  *** * |
   |      * |
   | *    * |
   |  ****  |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x4200,
0x4200,
0x4200,
0x4600,
0x3a00,
0x0200,
0x4200,
0x3c00,

/* Character 122 (0x7a):
   width 8
   +--------+
   |        |
   |        |
   |        |
   |        |
   |        |
   | ****** |
   |     *  |
   |    *   |
   |   *    |
   |  *     |
   | ****** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x7e00,
0x0400,
0x0800,
0x1000,
0x2000,
0x7e00,
0x0000,
0x0000,

/* Character 123 (0x7b):
   width 8
   +--------+
   |        |
   |        |
   |    *** |
   |   *    |
   |   *    |
   |    *   |
   |  **    |
   |    *   |
   |   *    |
   |   *    |
   |    *** |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x0e00,
0x1000,
0x1000,
0x0800,
0x3000,
0x0800,
0x1000,
0x1000,
0x0e00,
0x0000,
0x0000,

/* Character 124 (0x7c):
   width 8
   +--------+
   |        |
   |        |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |   *    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x1000,
0x0000,
0x0000,

/* Character 125 (0x7d):
   width 8
   +--------+
   |        |
   |        |
   | ***    |
   |    *   |
   |    *   |
   |   *    |
   |    **  |
   |   *    |
   |    *   |
   |    *   |
   | ***    |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x7000,
0x0800,
0x0800,
0x1000,
0x0c00,
0x1000,
0x0800,
0x0800,
0x7000,
0x0000,
0x0000,

/* Character 126 (0x7e):
   width 8
   +--------+
   |        |
   |        |
   |  *  *  |
   | * * *  |
   | *  *   |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   |        |
   +--------+
*/
0x0000,
0x0000,
0x2400,
0x5400,
0x4800,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
};

/* Character->glyph mapping. */
static const uInt16 _console_sysfont_offset[] = {
	13, /* (0x00) */
	0,	/* (0x01) */
	0,	/* (0x02) */
	0,	/* (0x03) */
	0,	/* (0x04) */
	0,	/* (0x05) */
	0,	/* (0x06) */
	0,	/* (0x07) */
	0,	/* (0x08) */
	0,	/* (0x09) */
	0,	/* (0x0a) */
	0,	/* (0x0b) */
	0,	/* (0x0c) */
	0,	/* (0x0d) */
	0,	/* (0x0e) */
	0,	/* (0x0f) */
	0,	/* (0x10) */
	0,	/* (0x11) */
	0,	/* (0x12) */
	0,	/* (0x13) */
	0,	/* (0x14) */
	0,	/* (0x15) */
	0,	/* (0x16) */
	0,	/* (0x17) */
	0,	/* (0x18) */
	0,	/* (0x19) */
	0,	/* (0x1a) */
	0,	/* (0x1b) */
	0,	/* (0x1c) */
	0,	/* (0x1d) */
	0,	/* (0x1e) */
	0,	/* (0x1f) */
	13,	/* (0x20) */
	26,	/* (0x21) */
	39,	/* (0x22) */
	52,	/* (0x23) */
	65,	/* (0x24) */
	78,	/* (0x25) */
	91,	/* (0x26) */
	104,	/* (0x27) */
	117,	/* (0x28) */
	130,	/* (0x29) */
	143,	/* (0x2a) */
	156,	/* (0x2b) */
	169,	/* (0x2c) */
	182,	/* (0x2d) */
	195,	/* (0x2e) */
	208,	/* (0x2f) */
	221,	/* (0x30) */
	234,	/* (0x31) */
	247,	/* (0x32) */
	260,	/* (0x33) */
	273,	/* (0x34) */
	286,	/* (0x35) */
	299,	/* (0x36) */
	312,	/* (0x37) */
	325,	/* (0x38) */
	338,	/* (0x39) */
	351,	/* (0x3a) */
	364,	/* (0x3b) */
	377,	/* (0x3c) */
	390,	/* (0x3d) */
	403,	/* (0x3e) */
	416,	/* (0x3f) */
	429,	/* (0x40) */
	442,	/* (0x41) */
	455,	/* (0x42) */
	468,	/* (0x43) */
	481,	/* (0x44) */
	494,	/* (0x45) */
	507,	/* (0x46) */
	520,	/* (0x47) */
	533,	/* (0x48) */
	546,	/* (0x49) */
	559,	/* (0x4a) */
	572,	/* (0x4b) */
	585,	/* (0x4c) */
	598,	/* (0x4d) */
	611,	/* (0x4e) */
	624,	/* (0x4f) */
	637,	/* (0x50) */
	650,	/* (0x51) */
	663,	/* (0x52) */
	676,	/* (0x53) */
	689,	/* (0x54) */
	702,	/* (0x55) */
	715,	/* (0x56) */
	728,	/* (0x57) */
	741,	/* (0x58) */
	754,	/* (0x59) */
	767,	/* (0x5a) */
	780,	/* (0x5b) */
	793,	/* (0x5c) */
	806,	/* (0x5d) */
	819,	/* (0x5e) */
	832,	/* (0x5f) */
	845,	/* (0x60) */
	858,	/* (0x61) */
	871,	/* (0x62) */
	884,	/* (0x63) */
	897,	/* (0x64) */
	910,	/* (0x65) */
	923,	/* (0x66) */
	936,	/* (0x67) */
	949,	/* (0x68) */
	962,	/* (0x69) */
	975,	/* (0x6a) */
	988,	/* (0x6b) */
	1001,	/* (0x6c) */
	1014,	/* (0x6d) */
	1027,	/* (0x6e) */
	1040,	/* (0x6f) */
	1053,	/* (0x70) */
	1066,	/* (0x71) */
	1079,	/* (0x72) */
	1092,	/* (0x73) */
	1105,	/* (0x74) */
	1118,	/* (0x75) */
	1131,	/* (0x76) */
	1144,	/* (0x77) */
	1157,	/* (0x78) */
	1170,	/* (0x79) */
	1183,	/* (0x7a) */
	1196,	/* (0x7b) */
	1209,	/* (0x7c) */
	1222,	/* (0x7d) */
	1235,	/* (0x7e) */
};

static const FontDesc consoleDesc = {
	"8x13",
	8,
	13,
	11,
	0,
	127,
	_console_font_bits,
	_console_sysfont_offset,  /* encode table */
	0,  /* fixed width*/
	0,
	sizeof(_console_font_bits)/sizeof(uInt16)
};

} // End of namespace GUI

#endif
