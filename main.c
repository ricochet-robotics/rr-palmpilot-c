/*
 * $Id: main.c,v 1.2 2003/02/14 06:01:37 keithp Exp $
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

FormPtr mainFrm;
FormPtr	rulesFrm;
Boolean	needRedisplay;
Game	game;

static void
Save (void)
{
    PrefSetAppPreferencesV10 (RicochetFileCreator, RicochetVersionNum,
			      &game, sizeof (Game));
}

static Boolean
Solved (void)
{
    if (game.board[game.targetx][game.targety] & game.robot)
	return true;
    return false;
}

static Boolean
NextTarget (void)
{
    int	x, y;
    if (game.ntarget >= NTARGET)
	return false;
    game.target = game.targets[game.ntarget++];
    game.robot = ColorToRobot (TargetColor (game.target));
    game.nhistory = 0;
    for (y = 0; y < BOARD_HEIGHT; y++)
	for (x = 0; x < BOARD_WIDTH; x++)
	    if ((game.board[x][y] & (TARGET_MASK|COLOR_MASK)) == game.target)
	    {
		game.targetx = x;
		game.targety = y;
	    }
    DamageBoard ();
    return true;
}

static void
NewGame (void)
{
    int	color, shape, i;

    DIAG (("NewGame\n"));
    game.seed = TimGetTicks ();
    RandomBoard (game.board);
    DamageBoard ();
    i = 0;
    for (color = RED; color <= BLUE; color = COLOR_NEXT(color))
	for (shape = TARGET_TRIANGLE;
	     shape <= TARGET_CIRCLE;
	     shape = TARGET_NEXT(shape))
	{
	    game.targets[i++] = shape|color;
	}
    game.targets[i++] = TARGET_WHIRL|COLOR_ANY;
    DIAG (("Generated %d targets\n", i));
    Shuffle (game.targets, i);
    game.ntarget = 0;
    DIAG (("NextTarget\n"));
    NextTarget ();
}

static void
Restore (void)
{
    if (!PrefGetAppPreferencesV10 (RicochetFileCreator, RicochetVersionNum,
				   &game, sizeof (Game)))
    {
	NewGame ();
    }
}

static Boolean  pen_down;
static int	pen_down_x;
static int	pen_down_y;
static Object	selected_robot;

static void
StartApplication (void)
{
    mainFrm = 0;
    pen_down = false;
    selected_robot = 0;
    MemSet (&game, sizeof (Game), '\0');
    Restore ();
    BoardInit ();
    DamageBoard ();
}

static Boolean
PenLocation (int pen_x, int pen_y, int *xp, int *yp)
{
    if (pen_x < 0 || BOARD_WIDTH * CELL_WIDTH <= pen_x)
	return false;
    if (pen_y < 0 || BOARD_HEIGHT * CELL_HEIGHT <= pen_y)
	return false;
    *xp = pen_x / CELL_WIDTH;
    *yp = pen_y / CELL_HEIGHT;
    return true;
}

static void
TimerStop (void)
{
    if (game.timer_running)
    {
	game.timer_running = false;
	DamageCount ();
    }
}

static void
TimerStart (void)
{
    if (!game.timer_running)
    {
	game.timer_running = true;
	game.timer_start = TimGetTicks ();
	DamageCount ();
    }
}

static void
TimerToggle (void)
{
    if (game.timer_running)
	TimerStop ();
    else
	TimerStart ();
}

#define IsCenterC(c)	(7 <= (c) && (c) < 9)
#define IsCenter(x,y)	(IsCenterC(x) && IsCenterC(y))

static void
HandleBoardPen (int down_x, int down_y,
		int up_x, int up_y)
{
    Direction   dir;

    if (IsCenter(down_x, down_y))
    {
	if (IsCenter (up_x, up_y))
	    TimerToggle ();
    }
    else
    {
	if ((game.board[down_x][down_y] & ROBOT_MASK))
	    selected_robot = game.board[down_x][down_y] & ROBOT_MASK;
	else if (!selected_robot || 
		 !FindRobot (selected_robot, &down_x, &down_y))
	    return;
	if (up_x == down_x)
	{
	    if (up_y == down_y)
		return;
	    if (up_y < down_y)
		dir = DirUp;
	    else
		dir = DirDown;
	}
	else if (up_y == down_y)
	{
	    if (up_x < down_x)
		dir = DirLeft;
	    else
		dir = DirRight;
	}
	else
	    return;
	Do (selected_robot, dir);
    }
}

static void
RicochetRestart (void)
{
    while (Undo());
}

static void
RicochetNext (void)
{
    if (!Solved ())
	while (Undo());
    NextTarget ();
}

static void
RicochetNew (void)
{
    NewGame ();
}

static void
RicochetUndo (void)
{
    Undo ();
}

static void
Help (Int   id)
{
    FormPtr	frm;

    frm = FrmInitForm (id);
    if (frm)
    {
	FrmDoDialog (frm);
	FrmDeleteForm (frm);
    }
}


static void
RulesSetScrolling (void)
{
    FieldPtr	    field;

    field = FrmGetObjectPtr (rulesFrm, FrmGetObjectIndex (rulesFrm, rulesText));
    FrmUpdateScrollers (rulesFrm,
			FrmGetObjectIndex (rulesFrm, rulesScrollUp),
			FrmGetObjectIndex (rulesFrm, rulesScrollDown),
			FldScrollable (field, winUp),
			FldScrollable (field, winDown));
}

static void
RulesScroll (WinDirectionType dir, Boolean screen)
{
    FieldPtr	    field;
    Word	    fieldIndex;
    Word	    lines;

    fieldIndex = FrmGetObjectIndex (rulesFrm, rulesText);
    field = FrmGetObjectPtr (rulesFrm, fieldIndex);
    if (screen)
    {
	lines = FldGetVisibleLines (field) - 2;
	if (lines < 1)
	    lines = 1;
    }
    else
	lines = 1;
    FldScrollField (field, lines, dir);
    RulesSetScrolling ();
}

static Boolean
RulesFormHandleEvent(EventPtr event)
{
    Boolean handled = false;
    
    switch (event->eType) {
    case ctlRepeatEvent:
	switch (event->data.ctlRepeat.controlID) {
	case rulesScrollUp:
	    RulesScroll (winUp, false);
	    break;
	case rulesScrollDown:
	    RulesScroll (winDown, false);
	    break;
	}
	break;
    case ctlSelectEvent:
	switch (event->data.ctlSelect.controlID) {
	case rulesScrollUp:
	    RulesScroll (winUp, false);
	    break;
	case rulesScrollDown:
	    RulesScroll (winDown, false);
	    break;
	}
	break;
    case keyDownEvent:
	switch (event->data.keyDown.chr) {
	case pageDownChr:
	    RulesScroll (winDown, true);
	    handled = true;
	    break;
	case pageUpChr:
	    RulesScroll (winUp, true);
	    handled = true;
	    break;
	}
	break;
    default:
    }
    return handled;
}

void
Rules (void)
{
    FieldPtr	field;
    VoidHand	rulesH;
    
    rulesH = DmGetResource ('tSTR', ricochetRulesText);
    if (rulesH)
    {
	rulesFrm = FrmInitForm (rulesForm);
	field = FrmGetObjectPtr (rulesFrm, 
				 FrmGetObjectIndex (rulesFrm, rulesText));
	FldSetInsPtPosition (field, 0);
	FldSetTextHandle (field, (Handle) rulesH);
	RulesSetScrolling ();
	FrmSetEventHandler (rulesFrm, RulesFormHandleEvent);
	FrmDoDialog (rulesFrm);
	FldSetTextHandle (field, 0);
	DmReleaseResource (rulesH);
	FrmDeleteForm (rulesFrm);
    }
}

static void
RicochetMenuHandleEvent (EventPtr event)
{
    switch (event->data.menu.itemID) {
    case menuRicochetRestart:
	RicochetRestart ();
	break;
    case menuRicochetNext:
	RicochetNext ();
	break;
    case menuRicochetNew:
	RicochetNext ();
	break;

    case menuTimerStart:
	TimerStart ();
	break;
    case menuTimerStop:
	TimerStop ();
	break;

    case menuHelpControls:
	Help(controlForm);
	break;
    case menuHelpRules:
	Rules ();
	break;
    case menuHelpGetInfo:
	Help(infoForm);
	break;
	
    }
}

static Boolean
RicochetKeyHandleEvent (EventPtr event)
{
    Boolean handled = true;
    
    switch (event->data.keyDown.chr) {
    case 'r':
    case 'R':
	RicochetRestart();
	break;
    case 'n':
    case 'N':
	RicochetNext ();
	break;
    case 'g':
    case 'G':
	RicochetNew ();
	break;
    case 'u':
    case 'U':
	RicochetUndo ();
	break;

    case 't':
    case 'T':
	TimerStart ();
	break;
    case 's':
    case 'S':
	TimerStop ();
	break;

    case 'c':
    case 'C':
	Help (controlForm);
	break;
    case 'l':
    case 'L':
	Rules ();
	break;
    case 'i':
    case 'I':
	Help (infoForm);
	break;

    default:
	handled = false;
	break;
    }
    return handled;
}

static Boolean
MainFormHandleEvent(EventPtr event)
{
    Boolean handled = false;
    int	    x, y;
    
    switch (event->eType) {
    case frmUpdateEvent:
	if (event->data.frmUpdate.formID == mainForm)
	{
	    DIAG (("Call Redisplay\n"));
	    Redisplay ();
	    handled = true;
	}
	break;
    case frmOpenEvent:
	if (event->data.frmOpen.formID == mainForm)
	{
	    FrmDrawForm (mainFrm);
	    DamageBoard ();
	}
	break;
    case penDownEvent:
	if (PenLocation (event->screenX, event->screenY, &x, &y))
	{
	    pen_down = true;
	    pen_down_x = x;
	    pen_down_y = y;
	    handled = true;
	}
	break;
    case penUpEvent:
	if (pen_down &&
	    PenLocation (event->screenX, event->screenY, &x, &y))
	{
	    HandleBoardPen (pen_down_x, pen_down_y, x, y);
	    handled = true;
	}
	pen_down = false;
	break;
    case keyDownEvent:
	RicochetKeyHandleEvent (event);
	break;
    case menuEvent:
	RicochetMenuHandleEvent (event);
	break;
    default:
	break;
    }
    return handled;
}

static Boolean 
ApplicationHandleEvent (EventPtr event)
{
    Word formId;
    FormPtr frm;

    if (event->eType == frmLoadEvent)
    {
	// Load the form resource.
	formId = event->data.frmLoad.formID;
	frm = FrmInitForm (formId);
	if (frm)
	{
	    switch (formId) {
	    case mainForm:
		mainFrm = frm;
		FrmSetEventHandler (frm, MainFormHandleEvent);
		break;
	    }
	    FrmSetActiveForm (frm);
	}
	// Set the event handler for the form.  The handler of the currently
	// active form is called by FrmHandleEvent each time is receives an
	// event.
	return (true);
    }
    return (false);
}

static void
EventLoop (void)
{
    EventType	event;
    SDWord	timeout;
    UInt32	timer_next;
    Word	error;

    do
    {
        timeout = evtWaitForever;
	timer_next = 0;
	if (needRedisplay)
	    timeout = 0;
	else if (game.timer_running)
	{
	    UInt32  now = TimGetTicks ();
	    int	    sec = (now - game.timer_start) / sysTicksPerSecond;
	    
	    timer_next = ((sec + 1) * sysTicksPerSecond + game.timer_start);
	    timeout = timer_next - now;
	}
	EvtGetEvent (&event, timeout);
	DIAG (("Event %d\n", event.eType));
	if (timer_next && TimGetTicks () <= timer_next)
	    DamageCount ();
	if (needRedisplay && event.eType == nilEvent)
	    FrmUpdateForm (mainForm, frmRedrawUpdateCode);
	if (event.eType == nilEvent)
	    continue;
	
	/* Give the system a chance to handle the event. */
	if (! SysHandleEvent (&event))
	{
	    if (!MenuHandleEvent (0, &event, &error))
	    {
		/* Give the application a chance to handle the event. */
		if (!ApplicationHandleEvent (&event))
		{
		    /* Let the form object provide default handling of the event. */
		    FrmDispatchEvent (&event);
		}
	    }
	}
    }
    while (event.eType != appStopEvent);
    // ** SPECIAL NOTE **
    // In order for the Emulator to exit
    // cleanly when the File|Quit menu option is
    // selected, the running application
    // must exit.  The emulator will generate an 
    // ÒappStopEventÓ when Quit is selected.
}

static void
StopApplication(void)
{
    Save ();
    FrmCloseAllForms ();
    BoardFini ();
}

DWord 
PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
    if (cmd == sysAppLaunchCmdNormalLaunch)
    {
	StartApplication ();
	FrmGotoForm (mainForm);
	EventLoop ();
	StopApplication ();
    }
    return (0);
}

#include <string.h>

void
diag (char *fmt, ...)
{
    va_list args;
    char    line[1024];
    va_start (args, fmt);

    vsprintf (line, fmt, args);
    va_end (args);
    WinPaintChars (line, strlen (line), 0, 0);
    DbgMessage (line);
    SysTaskDelay (sysTicksPerSecond/4);
}
