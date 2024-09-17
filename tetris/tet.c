#include "tet.h"

char shapes[] = ".... D... ..D. .Vl. .... BL.. .FH. ;D;. "
                ".... CJH. BJI. .Si. BJJH .CH. BI.. BKH. "
                ".... .... .... .... .... .... .... ,.,. "
                ".... .... .... .... .... .... .... .... "

                ".... .FH. .D.. .Vl. ..D. ..D. .D.. ,D;. "
                ".... .E.. .E.. .Si. ..E. .FI. .CL. .GH. "
                ".... .A.. .CH. .... ..E. .A.. ..A. ,A;. "
                ".... .... .... .... ..A. .... .... .... "

                ".... .... .... .Vl. .... .... .... ,.,. "
                ".... BJL. FJH. .Si. .... BL.. .FH. BNH. "
                ".... ..A. A... .... BJJH .CH. BI.. ;A;. "
                ".... .... .... .... .... .... .... .... "

                ".... .D.. BL.. .Vl. .D.. .D.. D... ;D,. "
                ".... .E.. .E.. .Si. .E.. FI.. CL.. BM.. "
                ".... BI.. .A.. .... .E.. A... .A.. ;A,. "
                ".... .... .... .... .A.. .... .... .... ";

struct shadow shadows[4][8] = {
    // pre-computed shadow positions for each piece
    {{0, 0, 0},
     {0, 3, 1},
     {0, 3, 1},
     {1, 2, 0},
     {0, 4, 1},
     {0, 3, 1},
     {0, 3, 1},
     {0, 3, 1}},
    {{0, 0, 0},
     {1, 2, 0},
     {1, 2, 2},
     {1, 2, 0},
     {2, 1, 0},
     {1, 2, 1},
     {1, 2, 1},
     {1, 2, 1}},
    {{0, 0, 0},
     {0, 3, 1},
     {0, 3, 1},
     {1, 2, 0},
     {0, 4, 2},
     {0, 3, 2},
     {0, 3, 2},
     {0, 3, 1}},
    {{0, 0, 0},
     {0, 2, 2},
     {0, 2, 0},
     {1, 2, 0},
     {1, 1, 0},
     {0, 2, 1},
     {0, 2, 1},
     {0, 2, 1}},
};

int center[] = {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1};

int colors[] = {
    0x000000, // unused
    0x1983c4, // J
    0xfa8333, // L
    0xffca39, // square
    0x1be7ff, // line
    0xff5a5f, // Z
    0x89c926, // S
    0x88488f, // T
    0xffffff, // shine color
    0x6f7866, // garbage colors
    0x9fa896, 0xffca39, 0xff5a5f,
};

int kicks[] = {
    // clockwise                            counterclockwise
    0, 0, -1, 0, -1, 1, 0, -2, -1, -2, 0, 0, 1, 0, 1, 1, 0, -2, 1, -2,  // rot 0
    0, 0, -1, 0, -1, -1, 0, 2, -1, 2, 0, 0, -1, 0, -1, -1, 0, 2, -1, 2, // rot 1
    0, 0, 1, 0, 1, 1, 0, -2, 1, -2, 0, 0, -1, 0, -1, 1, 0, -2, -1, -2,  // rot 2
    0, 0, 1, 0, 1, -1, 0, 2, 1, 2, 0, 0, 1, 0, 1, -1, 0, 2, 1, 2,       // rot 3
    // line-clockwise                       line-counterclockwise
    0, 0, 2, 0, -1, 0, 2, -1, -1, 2, 0, 0, 1, 0, -2, 0, 1, 2, -2, -1, // rot 0
    0, 0, -2, 0, 1, 0, -2, 1, 1, -2, 0, 0, 1, 0, -2, 0, 1, 2, -2, -1, // rot 1
    0, 0, -1, 0, 2, 0, -1, -2, 2, 1, 0, 0, -2, 0, 1, 0, -2, 1, 1, -2, // rot 2
    0, 0, 2, 0, -1, 0, 2, -1, -1, 2, 0, 0, -1, 0, 2, 0, -1, -2, 2, 1, // rot 3
};

float combo_bonus[] = {1.f,  1.5f, 2.f,  3.f,  4.f,  5.f,  6.f,  8.f,  10.f,
                       12.f, 15.f, 20.f, 25.f, 30.f, 40.f, 50.f, 75.f, 100.f};

int rewards[] = {0, 100, 250, 500, 1000}; // points for clearing 0,1,2,3,4 lines
int speeds[] = {100, 80, 70, 60, 52, 46, 40, 35, 30, 26, 22,
                18,  15, 12, 10, 8,  6,  5,  4,  3,  2};
char countdown_msg[][20] = {"        Go!", "       - 1 -", "       - 2 -",
                            "       - 3 -"};
struct player play[NPLAY];
struct player *p;

int MAX_COMBO = ((sizeof combo_bonus / sizeof *combo_bonus) - 1);
int MAX_SPEED = ((sizeof speeds / sizeof *speeds) - 1);

struct particle parts[NPARTS];
struct particle flows[NFLOWS];
enum state state;

int win_x = 1000; // window size
int win_y = 750;
int bs, bs2, bs4;   // individual block size, in half, in quarters
int tick;           // counts up one per frame
int nplay = 1;      // number of players
int assign_me;      // who is getting an input device assigned?
int menu_pos;       // current position in menu
int text_x, text_y; // position of text drawing
int line_height;    // text line height
int garbage_race;
int npart;
int seed;

SDL_GLContext ctx;
SDL_Event event;
SDL_Window *win;
SDL_Renderer *renderer;
