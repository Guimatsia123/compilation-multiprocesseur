#pragma once
#include <SDL.h>

#define GL3_PROTOTYPES 1

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#define BWIDTH 10  // board width, height
#define BHEIGHT 25
#define VHEIGHT 20 // visible height
#define BAG_SZ 7   // bag size
#define GARB_LVLS 4 // levels of queued garbage
#define NPLAY 4
#define NPARTS 200
#define NFLOWS 20
#define CTDN_TICKS 96
#define SHOW_FPS 0

// collision test results
enum { NONE = 0, WALL, NORMAL };

// Bits in each letter indicate which sides connect when drawing
// A 1000001    - up
// B 1000010    - right
// D 1000100    - down
// H 1001000    - left
//   1010000    - connect left corner where up or down connects - for square piece
//   1100000    - ...     right ...

extern char shapes[];
struct shadow { int x, w, y; };
extern struct shadow shadows[4][8];

// helps center shapes in preview box
extern int center[];
extern int colors[];
extern int kicks[];
extern float combo_bonus[];
extern int rewards[];
extern int speeds[];
extern char countdown_msg[][20];

extern int MAX_COMBO;
extern int MAX_SPEED;

struct piece { int x, y, rot, color; };
struct spot { int color, part; };
struct row {
        struct spot col[BWIDTH];
        int fullness;
        int special;
        int offset;
};

struct player {
        struct row row[BHEIGHT];        // the board, excluding the falling piece
        int left, right, down;          // true when holding a direction
        int move_cooldown;              // cooldown before hold-to-repeat movement
        struct piece it;                // current falling piece - "it"
        struct piece beam;              // hard drop beam
        struct piece held;              // shape in the hold box
        int beam_tick;                  // tick that beam was created
        int hold_uses;                  // have we swapped with the hold already?
        int bag[BAG_SZ];                // "bag" of upcoming pieces
        int bag_idx;                    // last position used up in bag
        int next[5];                    // next pieces in preview (take from bag)
        int grounded;                   // is piece on the ground?
        int grounded_moves;             // how many moves have we made on the ground?
        int last_dx_tick;               // tick of most recent left/right movement
        int lines, score, best;         // scoring
        int combo;                      // clears in-a-row
        int reward, reward_x, reward_y; // for hovering points indicator
        int garbage[GARB_LVLS + 1];     // queued garbage, e.g. received from opponents
        int garbage_tick;               // keeps track of when to age garbage
        int garbage_remaining;          // how many lines of garbage remain to clear to win
        int garbage_bits;               // fractions of garbage attached to each particle
        int top_garb;                   // highest position of garbage stack drawn
        int level;                      // difficultly level (lines/10)
        int countdown_time;             // ready-set-go countdown
        int idle_time;                  // how long the player has been idle in ticks
        int shiny_lines;
        int shine_time;                 // delay in ticks before clearing line(s)
        int dead_time;                  // delay in ticks after game over
        int board_x, board_y, board_w;  // positions and sizes of things
        int preview_x, preview_y;       // position of preview
        int box_w;                      // width of hold box / preview box
        int ticks;                      // counts up while game is going
        int seed1, seed2;               // make garbage and bags fair
        float shake_x, shake_y;         // amount the board is offset by shaking
        int flash;                      // flashing from receiving garbage
        char *tspin;
        int device;                     // SDL's input device id
        char dev_name[80];              // input device "name"
};
extern struct player play[NPLAY];
extern struct player *p;                      // one per player

struct particle { float x, y, r, vx, vy; int opponent, bits; };
extern struct particle parts[NPARTS];
extern struct particle flows[NFLOWS];

enum state { MAIN_MENU = 0, NUMBER_MENU, ASSIGN, PLAY, GAMEOVER};
extern enum state state;

extern int win_x;         // window size
extern int win_y;
extern int bs, bs2, bs4;         // individual block size, in half, in quarters
extern int tick;                 // counts up one per frame
extern int nplay;            // number of players
extern int assign_me;            // who is getting an input device assigned?
extern int menu_pos;             // current position in menu
extern int text_x, text_y;       // position of text drawing
extern int line_height;          // text line height
extern int garbage_race;
extern int npart;
extern int seed;

extern SDL_GLContext ctx;
extern SDL_Event event;
extern SDL_Window *win;
extern SDL_Renderer *renderer;

void do_events();
void setup();
void update_player();
void move(int dx, int dy, int gravity);
void reset_fall();
void bake();
void new_game();
int is_solid_part(int shape, int rot, int i, int j);
int is_tspin_part(int shape, int rot, int i, int j);
int collide(int x, int y, int rot);
void update_particles();
