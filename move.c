/*
 * $Id: move.c,v 1.1 2003/02/14 06:02:37 keithp Exp $
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

Boolean
FindRobot (Object o, int *xp, int *yp)
{
    int	x, y;

    for (y = 0; y < BOARD_HEIGHT; y++)
	for (x = 0; x < BOARD_WIDTH; x++)
	    if (game.board[x][y] & o)
	    {
		*xp = x;
		*yp = y;
		return true;
	    }
    return false;
}

Boolean
Do (Object robot, Direction dir)
{
    int		x, y;
    int		dx, dy;
    Object	wall;
    ObjectLoc	*h;
    
    if (!FindRobot (robot, &x, &y))
    {
	DIAG(("Lost object %x\n", robot));
	return false;
    }
    switch (dir) {
    case DirRight:  dx =  1; dy =  0; wall = WALL_RIGHT; break;
    case DirUp:	    dx =  0; dy = -1; wall = WALL_ABOVE; break;
    case DirLeft:   dx = -1; dy =  0; wall = WALL_LEFT; break;
    case DirDown:   dx =  0; dy =  1; wall = WALL_BELOW; break;
    default:
	DIAG(("Bad dir %d\n", dir));
	return false;
    }
    if ((game.board[x][y] & wall) || (game.board[x+dx][y+dy] & ROBOT_MASK))
    {
	DIAG (("Move blocked %x %x\n",
	       game.board[x][y], game.board[x+dx][y+dy]));
	return true;
    }
    if (game.nhistory == MAX_MOVES)
    {
	DIAG (("history full %d\n", game.nhistory))
	return false;
    }
    h = &game.history[game.nhistory++];
    DamageCount ();
    h->x = x;
    h->y = y;
    h->object = robot;
    game.board[x][y] &= ~robot;
    DamageSquare (x, y);
    DIAG (("Move %x from %d,%d\n", robot, x, y));
    do {
	x += dx;
	y += dy;
    } while ((game.board[x][y] & wall) == 0 && 
	     (game.board[x+dx][y+dy] & ROBOT_MASK) == 0);
    DIAG (("Move %x   to %d,%d\n", robot, x, y));
    game.board[x][y] |= robot;
    DamageSquare (x, y);
    return true;
}

Boolean
Undo (void)
{
    ObjectLoc	*h;
    int		x, y;

    if (game.nhistory == 0)
    {
	DIAG (("History empty\n"));
	return false;
    }
    h = &game.history[--game.nhistory];
    DamageCount ();
    if (!FindRobot (h->object, &x, &y))
    {
	DIAG (("Lost %x\n", h->object));
	return false;
    }
    game.board[x][y] &= ~h->object;
    DamageSquare (x, y);
    game.board[(int) h->x][(int) h->y] |= h->object;
    DamageSquare (h->x, h->y);
    return true;
}
