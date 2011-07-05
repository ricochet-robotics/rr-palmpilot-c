/*
 * $Id: board.c,v 1.2 2003/02/14 06:01:37 keithp Exp $
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

static CustomPatternType   tablePattern = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static CustomPatternType    gridPattern = {
    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
};

static CustomPatternType    wallPattern = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
};

static void
FillTable (RectangleType *r)
{
    WinSetPattern ((void *) tablePattern);
    WinFillRectangle (r, 0);
}

static void
FillGrid (RectangleType *r)
{
    WinSetPattern ((void *) gridPattern);
    WinFillRectangle (r, 0);
}

static void
FillWall (RectangleType *r)
{
    WinSetPattern ((void *) wallPattern);
    WinFillRectangle (r, 0);
}

typedef struct _Image {
    VoidHand	hand;
    BitmapPtr	bitmap;
} Image;

static void
ImageInit (Image *image, Int res)
{
    if ((image->hand = DmGetResource (bitmapRsc, res)))
    {
	image->bitmap = MemHandleLock (image->hand);
    }
}

static void
ImageFini (Image *image)
{
    if (image->hand)
    {
	if (image->bitmap)
	{
	    MemHandleUnlock (image->hand);
	    image->bitmap = 0;
	}
	DmReleaseResource (image->hand);
	image->hand = 0;
    }
}

static void
ImageDraw (Image *image, Int x, Int y)
{
    WinDrawBitmap (image->bitmap, x, y);
}

static Image	robotActiveImage;
static Image	robotPassiveImage;
static Image	robotActiveTargetImage;
static Image	robotPassiveTargetImage;
static Image	targetImage;

/*
 * Here's a view of each grid square.  The tail of the semicolon
 * marks the origin of the cell
 *
 *   || GRID_OFF
 *   |---| GRID_THICK
 *    |-----------CELL_WIDTH--------------|
 * _ :;::::::::::::::::::::::::::::::::::::::_
 * | ::::::::::::::::::::::::::::::::::::::::|
 * | ::::                                ::::| CELL_PAD
 * C ::::    +----------------------+    ::::_
 * E ::::    |                      |    ::::
 * L ::::    |                      |    ::::
 * L ::::    |                      |    ::::
 * | ::::    |                      |    ::::
 * H ::::    |                      |    ::::
 * E ::::    |                      |    ::::
 * I ::::    |                      |    ::::
 * G ::::    |                      |    ::::
 * H ::::    |                      |    ::::
 * T ::::    |                      |    ::::
 * | ::::    |                      |    ::::
 * | ::::    |                      |    ::::
 * | ::::    +----------------------+    :::: _ _
 * | ######################################## | | WALL_OFF
 * _ ######################################## | _
 *   ######################################## | WALL_THICK
 *   ######################################## |
 *   ######################################## _
 */

static void
GridDraw (void)
{
    RectangleType   r;
    Int		    i;

    for (i = 1; i < BOARD_WIDTH; i++)
    {
	r.topLeft.x = i * CELL_WIDTH - GRID_OFF;
	r.topLeft.y = 0;
	r.extent.x = GRID_THICK;
	r.extent.y = BOARD_HEIGHT * CELL_HEIGHT;
	FillGrid (&r);
    }
    for (i = 1; i < BOARD_HEIGHT; i++)
    {
	r.topLeft.x = 0;
	r.topLeft.y = i * CELL_HEIGHT - GRID_OFF;
	r.extent.x = BOARD_WIDTH * CELL_WIDTH;
	r.extent.y = GRID_THICK;
	FillGrid (&r);
    }
}

static void
BoundsDraw (void)
{
    RectangleType   r;
    int		    i;

    for (i = 0; i <= BOARD_WIDTH; i += BOARD_WIDTH)
    {
	r.topLeft.x = i * CELL_WIDTH - WALL_OFF;
	r.topLeft.y = 0;
	r.extent.x = WALL_THICK;
	r.extent.y = BOARD_HEIGHT * CELL_HEIGHT;
	FillWall (&r);
    }
    
    for (i = 0; i <= BOARD_HEIGHT; i += BOARD_HEIGHT)
    {
	r.topLeft.x = 0;
	r.topLeft.y = i * CELL_HEIGHT - WALL_OFF;
	r.extent.x = BOARD_WIDTH * CELL_WIDTH;
	r.extent.y = WALL_THICK;
	FillWall (&r);
    }
}

static Boolean
CenterNumber (int x, int y, int count, char label)
{
    char	buf[6];
    char	*b;
    int		w, n;

    if (count > 9999)
	return false;
    b = buf;
    *b++ = label;
    *b++ = ' ';
    if (count >= 1000)
    {
	*b++ = '0' + count / 1000;
	count = count % 1000;
    }
    if (count >= 100)
    {
	*b++ = '0' + count / 100;
	count = count % 100;
    }
    if (count >= 10)
    {
	*b++ = '0' + count / 10;
	count = count % 10;
    }
    *b++ = '0' + count;
    n = b - buf;
    w = FntLineWidth (buf, n);

    WinDrawInvertedChars (buf, n, x - w/2, y);
    return true;
}

static void
TimerAlarm (void)
{
    SndPlaySystemSound (sndAlarm);
}

static void
CenterDraw (void)
{
    RectangleType   r;
    int		    text_x;

    r.topLeft.x = 7 * CELL_WIDTH - WALL_OFF;
    r.topLeft.y = 7 * CELL_HEIGHT - WALL_OFF;
    r.extent.x = 2 * CELL_WIDTH + WALL_THICK;
    r.extent.y = 2 * CELL_HEIGHT + WALL_THICK;
    FillWall (&r);
    text_x = r.topLeft.x + r.extent.x / 2;
    CenterNumber (text_x, r.topLeft.y, game.nhistory, 'm');
    if (game.timer_running)
    {
	int secs = TIMER_LENGTH - (TimGetTicks () - game.timer_start) / sysTicksPerSecond;
	int text_y = r.topLeft.y + r.extent.y - FntLineHeight ();
	
	if (secs <= 0)
	{
	    TimerAlarm ();
	    game.timer_running = false;
	}
	else
	    CenterNumber (text_x, text_y, secs, 't');
    }
}

static void
PieceDraw (Image *i, Int col, Int row)
{
    Int	x = col * CELL_WIDTH + CELL_PAD;
    Int y = row * CELL_HEIGHT + CELL_PAD;
    ImageDraw (i, x, y);
}

static void
BlankDraw (Int col, Int row)
{
    RectangleType   r;
    r.topLeft.x = col * CELL_WIDTH + CELL_PAD;
    r.topLeft.y = row * CELL_HEIGHT + CELL_PAD;
    r.extent.x = CELL_WIDTH - CELL_PAD - GRID_THICK;
    r.extent.y = CELL_HEIGHT - CELL_PAD - GRID_THICK;
    FillTable (&r);
}

static void
WallDraw (Int col, Int row, Boolean horiz)
{
    RectangleType   r;

    if (horiz)
    {
	r.topLeft.x = col * CELL_WIDTH - GRID_OFF;
	r.topLeft.y = row * CELL_HEIGHT - WALL_OFF;
	r.extent.x = CELL_WIDTH + GRID_THICK;
	r.extent.y = WALL_THICK;
    }
    else
    {
	r.topLeft.x = col * CELL_WIDTH - WALL_OFF;
	r.topLeft.y = row * CELL_HEIGHT - GRID_OFF;
	r.extent.x = WALL_THICK;
	r.extent.y = CELL_HEIGHT + GRID_THICK;
    }
    FillWall (&r);
}

void
DrawSquare (int x, int y)
{
    Image   *i = 0;
    
    if ((game.board[x][y] & (TARGET_MASK|COLOR_MASK)) == game.target)
	i = &targetImage;
    if (game.board[x][y] & ROBOT_MASK)
    {
	if ((game.board[x][y] & ROBOT_MASK) & game.robot)
	{
	    if (i)
		i = &robotActiveTargetImage;
	    else
		i = &robotActiveImage;
	}
	else
	{
	    if (i)
		i = &robotPassiveTargetImage;
	    else
		i = &robotPassiveImage;
	}
    }
    if (i)
	PieceDraw (i, x, y);
}

static char dirty[BOARD_WIDTH][BOARD_HEIGHT];
static char count_dirty;
static char all_dirty;

void
DamageCount (void)
{
    count_dirty = true;
    needRedisplay = true;
}

void
DamageSquare (int x, int y)
{
    dirty[x][y] = true;
    needRedisplay = true;
}

void
DamageBoard (void)
{
    all_dirty = true;
    needRedisplay = true;
}

void
DisplaySquare (int x, int y)
{
    BlankDraw (x, y);
    DrawSquare (x, y);
}

#define IsCenterC(c)	(7 <= (c) && (c) <= 9)
#define IsCenter(x,y)	(IsCenterC(x) && IsCenterC(y))

void
RedisplayBoard (void)
{
    RectangleType	r;
    int			x, y;
    
    DIAG (("In RedisplayBoard\n"));
    FrmEraseForm (mainFrm);
    FrmGetFormBounds (mainFrm, &r);
    FillTable (&r);
    GridDraw ();
    BoundsDraw ();
    CenterDraw ();
    for (y = 0; y < BOARD_HEIGHT; y++)
	for (x = 0; x < BOARD_WIDTH; x++)
	{
	    if (!IsCenter(x,y))
	    {
		if (x && game.board[x][y] & WALL_LEFT)
		    WallDraw (x, y, false);
		if (y && game.board[x][y] & WALL_ABOVE)
		    WallDraw (x, y, true);
	    }
	    DrawSquare (x, y);
	}
}

void
Redisplay (void)
{
    if (all_dirty)
	RedisplayBoard ();
    else
    {
	int x, y;

	for (y = 0; y < BOARD_HEIGHT; y++)
	    for (x = 0; x < BOARD_WIDTH; x++)
		if (dirty[x][y])
		    DisplaySquare (x, y);
	if (count_dirty)
	    CenterDraw ();
    }
    all_dirty = false;
    count_dirty = false;
    MemSet (dirty, sizeof (dirty), '\0');
    needRedisplay = false;
}

Boolean
BoardInit (void)
{
    MemSet (dirty, sizeof (dirty), '\0');
    count_dirty = false;
    all_dirty = false;
    needRedisplay = false;
    ImageInit (&robotActiveImage, bitmapRobotActive);
    ImageInit (&robotPassiveImage, bitmapRobotPassive);
    ImageInit (&robotActiveTargetImage, bitmapRobotActiveTarget);
    ImageInit (&robotPassiveTargetImage, bitmapRobotPassiveTarget);
    ImageInit (&targetImage, bitmapTarget);
    return true;
}

void
BoardFini (void)
{
    ImageFini (&robotActiveImage);
    ImageFini (&robotPassiveImage);
    ImageFini (&robotActiveTargetImage);
    ImageFini (&robotPassiveTargetImage);
    ImageFini (&targetImage);
}
