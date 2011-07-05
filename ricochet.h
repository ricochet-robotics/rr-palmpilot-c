/*
 * $Id: ricochet.h,v 1.2 2003/02/14 06:01:37 keithp Exp $
 *
 * Copyright © 2003 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "ricochetRsc.h"
#include <System/SysEvtMgr.h>
#include <PalmOS.h>
#include <PalmCompatibility.h>
#include <stdio.h>

/* Increment this every time the Game structure changes */
#define RicochetVersionNum	1
#define RicochetFileCreator	'Rico'

#define BOARD_WIDTH	16
#define BOARD_HEIGHT	16
#define CELL_WIDTH	10
#define CELL_HEIGHT	10
#define CELL_PAD	2
#define GRID_THICK	1
#define GRID_OFF	(GRID_THICK/2)
#define WALL_THICK	(CELL_PAD * 2 - GRID_THICK)
#define WALL_OFF	(CELL_PAD/2 + GRID_OFF)

#define RED		0x0001
#define YELLOW		0x0002
#define GREEN		0x0004
#define BLUE		0x0008
#define COLOR_ANY	0x000f
#define COLOR_NEXT(c)	((c) << 1)
#define COLOR_MASK	0x000f

#define TARGET_TRIANGLE	0x0010
#define TARGET_SQUARE	0x0020
#define TARGET_OCTAGON	0x0030
#define TARGET_CIRCLE	0x0040
#define TARGET_WHIRL	0x0050
#define TARGET_NEXT(t)	((t) + 0x0010)
#define TARGET_MASK	0x00f0

#define WALL_LEFT	0x0100
#define WALL_RIGHT	0x0200
#define WALL_ABOVE	0x0400
#define WALL_BELOW	0x0800
#define WALL_MASK	0x0f00

#define ROBOT_RED    	0x1000
#define ROBOT_YELLOW	0x2000
#define ROBOT_GREEN    	0x4000
#define ROBOT_BLUE    	0x8000
#define ROBOT_ANY	0xf000
#define ROBOT_NEXT(r)	((r) << 1)
#define ROBOT_MASK	0xf000

#define TargetColor(target) ((target) & COLOR_MASK)
#define ColorToRobot(color) (((color) & COLOR_MASK) << 12)

typedef unsigned short	Object;

typedef struct {
    char    	x, y;
    Object	object;
} ObjectLoc;

extern void diag(char *fmt, ...);

#if DEBUG
#define DIAG(s) diag s
#else
#define DIAG(s)
#endif

typedef struct {
    ObjectLoc	*obj;
    int		nobj;
} ObjectLocs;

typedef enum {
    DirRight, DirUp, DirLeft, DirDown
} Direction;

typedef Object	Board[BOARD_WIDTH][BOARD_HEIGHT];

#define MAX_MOVES   256
#define NTARGET	    17

typedef struct {
    Board	board;
    Object	target;
    Object	robot;
    int		targetx, targety;
    int		seed;
    
    /*
     * A shuffle of all of the targets
     */
    Object	targets[NTARGET];
    int		ntarget;
    
    /*
     * Timer
     */
    UInt32	timer_start;
    Boolean	timer_running;

    /*
     * History of moves made on this target
     */
    ObjectLoc	history[MAX_MOVES];
    int		nhistory;
} Game;

#define TIMER_LENGTH	60

extern Boolean	needRedisplay;
extern FormPtr	mainFrm;
extern Game	game;

/* board.c */
void
DamageCount (void);

void
DamageSquare (int x, int y);

void
DamageBoard (void);

void
Redisplay (void);

Boolean
BoardInit (void);

void
BoardFini (void);

/* layout.c */
void
RandomBoard (Board	b);
    
/* main.c */
DWord 
PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags);

/* move.c */
Boolean
FindRobot (Object o, int *xp, int *yp);

Boolean
Do (Object o, Direction dir);

Boolean
Undo (void);

/* random.c */
int
Random (int max);

void
SRandom (int seed);

/* shuffle.c */
void
Shuffle (short *values, int nvalue);
