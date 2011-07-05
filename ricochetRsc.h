//
// $Id: ricochetRsc.h,v 1.2 2003/02/14 06:01:37 keithp Exp $
//
// Copyright © 2003 Keith Packard
//
// Permission to use, copy, modify, distribute, and sell this software and its
// documentation for any purpose is hereby granted without fee, provided that
// the above copyright notice appear in all copies and that both that
// copyright notice and this permission notice appear in supporting
// documentation, and that the name of Keith Packard not be used in
// advertising or publicity pertaining to distribution of the software without
// specific, written prior permission.  Keith Packard makes no
// representations about the suitability of this software for any purpose.  It
// is provided "as is" without express or implied warranty.
//
// KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
//

#define mainForm		    1010

#define bitmapRobotActive	    2001
#define bitmapRobotPassive	    2002
#define bitmapTarget		    2003
#define bitmapRobotActiveTarget	    2004
#define bitmapRobotPassiveTarget    2005

#define menuBar			    3000
#define menuRicochet		    3100
#define menuRicochetRestart	    menuRicochet + 0
#define menuRicochetNext	    menuRicochet + 1
#define menuRicochetNew		    menuRicochet + 2
#define menuRicochetUndo	    menuRicochet + 4

#define menuTimer		    3200
#define menuTimerStart		    menuTimer + 0
#define menuTimerStop		    menuTimer + 1

#define menuHelp		    3300
#define menuHelpControls	    menuHelp + 0
#define menuHelpRules		    menuHelp + 1
#define menuHelpGetInfo		    menuHelp + 3

#define infoForm		    1300
#define infoTitle		    1301
#define infoText		    1302
#define infoOk			    1303
#define infoFormHelp		    1310

#define controlForm		    1400
#define controlTitle		    1401
#define controlText		    1402
#define controlOk		    1403

#define rulesForm		    1500
#define rulesTitle		    1501
#define rulesText		    1502
#define rulesOk			    1503
#define rulesScrollUp		    1504
#define rulesScrollDown		    1505

#define ricochetRulesText	    1600
