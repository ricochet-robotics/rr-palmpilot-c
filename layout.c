/*
 * $Id: layout.c,v 1.1 2003/02/14 06:02:37 keithp Exp $
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

#include "ricochet.h"

#define Number(a)   (sizeof (a) / sizeof (a[0]))

static ObjectLoc    red_tri1[] = {
    { 0, 2, TARGET_SQUARE|YELLOW | WALL_RIGHT | WALL_BELOW },
    { 1, 5, TARGET_CIRCLE|GREEN | WALL_LEFT | WALL_BELOW },
    { 4, 7, WALL_LEFT },
    { 5, 3, TARGET_OCTAGON|BLUE | WALL_LEFT | WALL_ABOVE },
    { 6, 6, TARGET_TRIANGLE|RED | WALL_RIGHT | WALL_ABOVE },
    { 7, 2, WALL_ABOVE },
};

static ObjectLoc    red_tri2[] = {
    { 0, 2, TARGET_TRIANGLE|RED | WALL_ABOVE | WALL_RIGHT },
    { 2, 5, TARGET_OCTAGON|BLUE | WALL_ABOVE | WALL_LEFT },
    { 4, 7, WALL_LEFT },
    { 5, 2, TARGET_CIRCLE|GREEN | WALL_LEFT | WALL_BELOW },
    { 6, 0, TARGET_SQUARE|YELLOW | WALL_BELOW | WALL_RIGHT },
    { 7, 2, WALL_ABOVE },
};

static ObjectLoc    yellow_tri1[] = {
    { 0, 2, TARGET_WHIRL|COLOR_ANY | WALL_LEFT | WALL_ABOVE },
    { 1, 6, TARGET_CIRCLE|BLUE | WALL_ABOVE | WALL_RIGHT },
    { 2, 3, TARGET_SQUARE|GREEN | WALL_BELOW | WALL_RIGHT },
    { 4, 7, WALL_LEFT },
    { 5, 2, TARGET_OCTAGON|RED | WALL_LEFT | WALL_ABOVE },
    { 6, 4, TARGET_TRIANGLE|YELLOW | WALL_LEFT | WALL_BELOW },
    { 7, 1, WALL_ABOVE },
};

static ObjectLoc    yellow_tri2[] = {
    { 1, 3, TARGET_TRIANGLE | YELLOW | WALL_BELOW | WALL_RIGHT },
    { 2, 1, TARGET_CIRCLE | BLUE | WALL_LEFT | WALL_BELOW },
    { 3, 7, WALL_LEFT },
    { 4, 0, TARGET_WHIRL|COLOR_ANY | WALL_ABOVE | WALL_LEFT },
    { 5, 6, TARGET_OCTAGON | RED | WALL_ABOVE | WALL_LEFT },
    { 6, 4, TARGET_SQUARE | GREEN | WALL_ABOVE | WALL_RIGHT },
    { 7, 3, WALL_ABOVE },
};

static ObjectLoc    green_tri1[] = {
    { 1, 2, TARGET_SQUARE|BLUE | WALL_LEFT | WALL_BELOW },
    { 1, 6, TARGET_OCTAGON|YELLOW | WALL_ABOVE | WALL_LEFT },
    { 3, 7, WALL_LEFT },
    { 4, 1, TARGET_CIRCLE|RED | WALL_ABOVE| WALL_RIGHT },
    { 6, 5, TARGET_TRIANGLE|GREEN | WALL_BELOW | WALL_RIGHT },
    { 7, 2, WALL_ABOVE },
};
    
static ObjectLoc    green_tri2[] = {
    { 1, 4, TARGET_OCTAGON|YELLOW | WALL_LEFT | WALL_ABOVE },
    { 3, 6, TARGET_CIRCLE|RED | WALL_BELOW | WALL_RIGHT },
    { 4, 1, TARGET_SQUARE|BLUE | WALL_ABOVE | WALL_RIGHT },
    { 6, 5, TARGET_TRIANGLE|GREEN | WALL_LEFT | WALL_BELOW },
    { 6, 7, WALL_LEFT },
    { 7, 2, WALL_ABOVE },
};

static ObjectLoc    blue_tri1[] = {
    { 1, 3, TARGET_CIRCLE|YELLOW | WALL_LEFT | WALL_ABOVE },
    { 2, 6, TARGET_TRIANGLE|BLUE | WALL_LEFT | WALL_BELOW },
    { 4, 7, WALL_LEFT },
    { 5, 1, TARGET_OCTAGON|GREEN | WALL_LEFT | WALL_ABOVE },
    { 6, 5, TARGET_SQUARE|RED    | WALL_ABOVE | WALL_RIGHT },
    { 7, 4, WALL_ABOVE },
};

static ObjectLoc    blue_tri2[] = {
    { 1, 3, TARGET_CIRCLE|YELLOW | WALL_RIGHT | WALL_BELOW },
    { 2, 6, TARGET_OCTAGON|GREEN | WALL_ABOVE | WALL_LEFT },
    { 4, 7, WALL_LEFT },
    { 5, 1, TARGET_TRIANGLE|BLUE | WALL_LEFT | WALL_BELOW },
    { 6, 5, TARGET_SQUARE|RED | WALL_ABOVE | WALL_RIGHT },
    { 7, 4, WALL_ABOVE },
};

ObjectLocs  Locations[4][2] = {
    { { red_tri1, Number (red_tri1) }, { red_tri2, Number (red_tri2) } },
    { { yellow_tri1, Number (yellow_tri1) }, { yellow_tri2, Number (yellow_tri2) } },
    { { green_tri1, Number (green_tri1) }, { green_tri2, Number (green_tri2) } },
    { { blue_tri1, Number (blue_tri1) }, { blue_tri2, Number (blue_tri2) } },
};

ObjectLoc
RotateLoc (ObjectLoc	a, int rot)
{
    while (rot--)
    {
	ObjectLoc   t;

	t.x = a.y;
	t.y = 7 - a.x;
	t.object = a.object & ~WALL_MASK;
	if (a.object & WALL_LEFT)
	    t.object |= WALL_BELOW;
	if (a.object & WALL_RIGHT)
	    t.object |= WALL_ABOVE;
	if (a.object & WALL_ABOVE)
	    t.object |= WALL_LEFT;
	if (a.object & WALL_BELOW)
	    t.object |= WALL_RIGHT;
	a = t;
    }
    return a;
}

static void
PlaceObject (Board	    b,
	     ObjectLoc	    l,
	     int	    rot,
	     int	    dx,
	     int	    dy)
{
    l = RotateLoc (l, rot);
    l.x += dx;
    l.y += dy;
    b[(int) l.x][(int) l.y] |= l.object;
    if (l.object & WALL_LEFT && l.x)
	b[(int) l.x - 1][(int) l.y] |= WALL_RIGHT;
    if (l.object & WALL_RIGHT && l.x < BOARD_WIDTH - 1)
	b[(int) l.x+1][(int) l.y] |= WALL_LEFT;
    if (l.object & WALL_ABOVE && l.y)
	b[(int) l.x][(int) l.y-1] |= WALL_BELOW;
    if (l.object & WALL_BELOW && l.y < BOARD_HEIGHT - 1)
	b[(int) l.x][(int) l.y+1] |= WALL_ABOVE;
}

static void
PlaceObjects (Board	    b,
	      ObjectLocs    *o,
	      int	    rot,
	      int	    dx,
	      int	    dy)
{
    int		i;

    for (i = 0; i < o->nobj; i++)
	PlaceObject (b, o->obj[i], rot, dx, dy);
}

typedef struct {
    int	    rot, dx, dy;
} BoardAlign;

static BoardAlign  aligns[4] = {
    { 2, 0, 0 },
    { 1, 8, 0 },
    { 3, 0, 8 },
    { 0, 8, 8 },
};

void
RandomBoard (Board	b)
{
    short   board_order[4];
    Object  robot_order[4];
    Object  robot;
    short   i, j;

    for (j = 0; j < BOARD_HEIGHT; j++)
	for (i = 0; i < BOARD_WIDTH; i++)
	    b[i][j] = 0;
    for (i = 0; i < 4; i++)
	board_order[i] = i;
    Shuffle (board_order, 4);
    for (i = 0; i < 4; i++)
    {
	ObjectLoc   l;
	
	j = Random(2);
	PlaceObjects (b, &Locations[board_order[i]][j],
		      aligns[i].rot,
		      aligns[i].dx,
		      aligns[i].dy);
	/*
	 * Center walls
	 */
	l.object = WALL_BELOW|WALL_RIGHT;
	l.x = 0;
	l.y = 0;
	PlaceObject (b, l, 
		     aligns[i].rot,
		     aligns[i].dx,
		     aligns[i].dy);
    }

    robot = ROBOT_RED;
    for (i = 0; i < 4; i++)
    {
	robot_order[i] = robot;
	robot = ROBOT_NEXT(robot);
    }
    Shuffle ((short *) robot_order, 4);
    for (i = 0; i < 4; i++)
    {
	ObjectLoc   l;

	l.object = robot_order[i];
	l.x = Random (8);
	l.y = Random (8);
	PlaceObject (b, l, aligns[i].rot,
		     aligns[i].dx,
		     aligns[i].dy);
    }

#if BOARD_WIDTH != BOARD_HEIGHT
    %%%% assume square board
#endif
    for (i = 0; i < BOARD_WIDTH; i++)
    {
	b[i][0] |= WALL_ABOVE;
	b[i][BOARD_HEIGHT-1] |= WALL_BELOW;
	b[0][i] |= WALL_LEFT;
	b[BOARD_WIDTH-1][i] |= WALL_RIGHT;
    }
}
