/*
 * XXX: If you change this, fix base17() and isdigit17() in
 * the code below!
 */

#define SIZE 16

typedef enum {
    BLANK,
    TARGET,
    ROBOT
} celltype;

typedef struct {
    celltype tag;
    int robot;
} cellval;

typedef struct {
    cellval val;
    /* 0 == N, 1 == E, 2 == S, 3 == W */
    bool[4] wall;
} cell;

typedef enum {
    RED, GREEN, BLUE, YELLOW
} color;

typedef cell[*,*] map_array;
public typedef *map_array map;

public map blank_map() {
    cell[SIZE, SIZE] result = {
	{{val = cellval.BLANK,  wall = {false ...}} ...}
	...
    };
    return &result;
}

typedef struct {
    map board;
    /* XXX -1 == any */
    int me; 
} state_struct;

public typedef *state_struct state;

public state new_state(map board, int me) {
    state_struct result = {
	board = board,
	me = me
    };
    return &result;
}


int[*] color_name = { 'R', 'G', 'B', 'Y' };

int color_index(int name) {
    for (int i = 0; i < dim(color_name); i++)
	if (color_name[i] == name)
	    return i;
    abort("bad color name");
}

void read_eol() {
    while(true) {
	int ch = getchar();
	if (ch == '\n')
	    break;
	if (ch != ' ' && ch != '\t')
	    abort("garbage at end of map line");
    }
}

void read_eof() {
    while(true) {
	int ch = getchar();
	if (ch == -1)
	    break;
	if (ch != ' ' && ch != '\t' && ch != '\n')
	    abort("garbage at end of map line");
    }
}

public state read_map_instance() {

    bool read_wall() {
	switch (getchar()) {
	case '-':
	case '|':
	case '+':
	    return true;
	case ' ':
	    return false;
	}
	abort("illegal wall in map");
    }

    state result = new_state(blank_map(), -1);

    for (int y = 0; y < SIZE * 2 + 1; y++) {
	for (int x = 0; x < SIZE * 2 + 1; x++) {
	    int x2 = x // 2;
	    int y2 = y // 2;
	    if (x == 0 && y == 0) {
		int c = getchar();
		if (c == '*')
		    result->me = -1;
		else
		    result->me = color_index(c);
		continue;
	    }
	    if (x % 2 == 1 && y % 2 == 1) {
		int ch = getchar();
		switch (ch) {
		case '.':
		    (*result->board)[x2, y2].val.BLANK = <>;
		    break;
		case 'R':
		case 'G':
		case 'B':
		case 'Y':
		    (*result->board)[x2, y2].val.ROBOT = color_index(ch);
		    break;
		case '*':
		    (*result->board)[x2, y2].val.TARGET = <>;
		    break;
		default:
		    abort("illegal cell in map");
		}
		continue;
	    }
	    bool w = read_wall();
	    if (y % 2 == 0) {
		if (x % 2 == 0)
		    continue;
		if (x2 < SIZE && y2 > 0)
		    (*result->board)[x2, y2 - 1].wall[2] = w;
		if (x2 < SIZE && y2 < SIZE)
		    (*result->board)[x2, y2].wall[0] = w;
		continue;
	    }
	    if (x2 > 0 && y2 < SIZE)
		(*result->board)[x2 - 1, y2].wall[1] = w;
	    if (x2 < SIZE && y2 < SIZE)
		(*result->board)[x2, y2].wall[3] = w;
	}
	read_eol();
    }
    read_eof();
    return result;
}


public void print_map_instance(state s) {

    void print_solid_wall() {
	for (int x = 0; x < 2 * SIZE - 1; x++)
	    putchar('-');
	putchar('+');
	putchar('\n');
    }

    void print_gapped_wall(int y) {
	assert(y > 0, "gapped top wall");
	putchar('|');
	for (int x = 0; x < SIZE; x++) {
	    *cell c = &((*s->board)[x, y]);
	    if (x > 0) {
		*cell cx = &((*s->board)[x - 1, y]);
		*cell cy = &((*s->board)[x, y - 1]);
		int n = 0;
		int boolnum(bool b) {
		    return b ? 1 : 0;
		}
		n += boolnum(c->wall[0]);
		n += boolnum(c->wall[3]);
		n += boolnum(cx->wall[0]);
		n += boolnum(cy->wall[3]);
		if (n > 1)
		    putchar('+');
		else
		    putchar(' ');
	    }
	    if (c->wall[0])
		putchar('-');
	    else
		putchar(' ');
	}
	putchar('|');
	putchar('\n');
    }

    void print_cells(int y) {
	putchar('|');
	for (int x = 0; x < SIZE; x++) {
	    union switch((*s->board)[x, y].val) {
	    case BLANK: putchar('.'); break;
	    case ROBOT n: putchar(color_name[n]); break;
	    case TARGET: putchar('*'); break;
	    }
	    if ((*s->board)[x, y].wall[1])
		putchar('|');
	    else
		putchar(' ');
	}
	putchar('\n');
    }

    if (s->me == -1)
	putchar('*');
    else
	putchar(color_name[s->me]);
    print_solid_wall();
    for (int y = 0; y < SIZE; y++) {
	if (y > 0)
	    print_gapped_wall(y);
	print_cells(y);
    }
    putchar('+');
    print_solid_wall();
}

public state read_runs_instance() {

    bool isdigit17(int ch) {
	return
	    ch >= '1' && ch <= '9' ||
	    ch >= 'a' && ch <= 'g';
    }

    int base17(int ch) {
	if (ch >= '0' && ch <= '9')
	    return ch - '0';
	if (ch >= 'a' && ch <= 'g')
	    return ch - 'a' + 10;
	abort("bogus");
    }

    state result = new_state(blank_map(), -2);

    void read_horizontal_run(int i) {
	int c = 0;
	int ch = getchar();
	while(true) {
	    if (isdigit17(ch)) {
		int dc = base17(ch);
		assert(dc > 0, "0-length run");
		c += dc;
	    } else {
		switch(ch) {
		case ' ':
		case '\t':
		    read_eol();
		    /* fall through */
		case '\n':
		    assert(c == SIZE, "run length wrong");
		    return;
		case '|':
		    (*result->board)[c - 1, i].wall[1] = true;
		    (*result->board)[c, i].wall[3] = true;
		    break;
		case 'R':
		case 'G':
		case 'B':
		case 'Y':
		    (*result->board)[c, i].val.ROBOT = color_index(ch);
		    c++;
		    break;
		case '*':
		    (*result->board)[c, i].val.TARGET = <>;
		    c++;
		    break;
		}
	    }
	    ch = getchar();
	}
    }

    void read_vertical_run(int i) {
	int c = 0;
	int ch = getchar();
	while(true) {
	    if (isdigit17(ch)) {
		int dc = base17(ch);
		assert(dc > 0, "0-length run");
		c += dc;
	    } else {
		switch(ch) {
		case ' ':
		case '\t':
		    read_eol();
		    /* fall through */
		case '\n':
		    assert(c == SIZE, "run length wrong");
		    return;
		case '|':
		    (*result->board)[i, c - 1].wall[2] = true;
		    (*result->board)[i, c].wall[0] = true;
		    break;
		case 'R':
		case 'G':
		case 'B':
		case 'Y':
		    assert((*result->board)[i, c].val.ROBOT ==
			   color_index(ch),
			   "misplaced drone");
		    c++;
		    break;
		case '*':
		    assert((*result->board)[i, c].val.TARGET == <>,
			   "misplaced target");
		    c++;
		    break;
		}
	    }
	    ch = getchar();
	}
    }

    for (int i = 0; i < SIZE; i++) {
	(*result->board)[i, 0].wall[0] = true;
	(*result->board)[SIZE - 1, i].wall[1] = true;
	(*result->board)[i, SIZE - 1].wall[2] = true;
	(*result->board)[0, i].wall[3] = true;
    }

    int ch = getchar();
    if (ch == '*')
	result->me = -1;
    else
	result->me = color_index(ch);
    read_eol();
    read_eol();
    for (int i = 0; i < SIZE; i++)
	read_horizontal_run(i);
    read_eol();
    for(int i = 0; i < SIZE; i++)
	read_vertical_run(i);
    read_eof();
    return result;
}

public void print_runs_instance(state s) {

    void printbase17(int d) {
	assert(d >= 0 && d <= 17, "bad base 17 print digit");
	if (d >= 0 && d <= 9)
	    putchar('0' + d);
	else
	    putchar('a' + d - 10);
    }

    typedef struct {
	cellval val;
	bool wall;
    } runcell;

    void print_run(runcell[*] c) {
	int l = 0;
	for (int i = 0; i < dim(c); i++) {
	    if (c[i].val == cellval.BLANK && !c[i].wall) {
		l++;
		continue;
	    }
	    union switch (c[i].val) {
	    case BLANK:
		printbase17(l + 1);
		break;
	    case ROBOT n:
		printbase17(l);
		putchar(color_name[n]);
		break;
	    case TARGET:
		printbase17(l);
		putchar('*'); break;
	    }
	    if (i < dim(c) - 1 && c[i].wall)
		putchar('|');
	    l = 0;
	}
	if (l > 0)
	    printbase17(l);
	putchar('\n');
    }

    if (s->me == -1)
	putchar('*');
    else
	putchar(color_name[s->me]);
    putchar('\n');
    putchar('\n');
    for (int y = 0; y < SIZE; y++) {
	runcell[SIZE] c =
	    { [x] = { val = (*s->board)[x, y].val,
		      wall = (*s->board)[x, y].wall[1] } };
	print_run(c);
    }
    putchar('\n');
    for (int x = 0; x < SIZE; x++) {
	runcell[SIZE] c =
	    { [y] = { val = (*s->board)[x, y].val,
		      wall = (*s->board)[x, y].wall[2] } };
	print_run(c);
    }
}
