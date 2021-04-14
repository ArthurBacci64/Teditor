#ifndef TED_HEADER
#define TED_HEADER

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#define READ_BLOCKSIZE 100
#define ctrl(x) ((x) & 0x1f)
#define cx cursor.x
#define cy cursor.y

#define CTRL_KEY_RIGHT 0x232
#define CTRL_KEY_LEFT  0x223

#define NUM_PAIRS 6

#define IN_RANGE(x, min, max)   ((x) >= (min)) && ((x) <= (max))
#define OUT_RANGE(x, min, max)  ((x) < (min)) || ((x) > (max))

#define SYNTAX_END  -1  // syntaxHighlight highlighted all the file
#define SYNTAX_TODO -2  // syntaxHighlight didn't finish yet

#define INPUT_TIMEOUT  5     //timeout for input in ncurses (in milliseconds)
#define SYNTAX_TIMEOUT 30000 //time slice in which syntaxHighlight runs (in microseconds) (30 milliseconds)

typedef uint32_t uchar32_t;

// message_and_prompt.c
struct HINTS {
    const char *word;
    const char *hint;
};

char *prompt(const char *msgtmp, char *def);
char *prompt_hints(const char *msgtmp, char *def, char *shadow, struct HINTS *hints);
void message(char *msg);

// ted.c
void setcolor(int c);

// config_dialog.c
void config_dialog(void);
bool run_command(char **words, int words_len);
void parse_command(char *command);

// open_and_save.c
void savefile(void);
void read_lines(void);
void detect_linebreak(void);
void openFile(char *fname, bool needs_to_free);
void detect_read_only(char *fname);

// show.c
void show_menu(char *message, char *shadow);
void show_lines(void);

// free.c
void free_lines(void);

// keypress.c
void expandLine(unsigned int at, int x);
void new_line(unsigned int at, int x);
void process_keypress(int c);

// cursor_in_valid_position.c
void cursor_in_valid_position(void);
void change_position(unsigned int x, unsigned int y);

// mouse.c
void processMouseEvent(MEVENT ev);

// utf8.c
void utf8ReadFile(unsigned char uc, uchar32_t *out, FILE *fp_);
int utf8ToMultibyte(uchar32_t c, unsigned char *out, bool validate);
bool validate_utf8(unsigned char *ucs);

// color.c
int syntaxHighlight(void);
void readColor(unsigned int at, unsigned int at1, unsigned char *fg, unsigned char *bg);

// utils.c
char *home_path(const char *path);
char *split_spaces(char *str, char **save);
char **split_str(const char *str, int *num_str);
void calculate_len_line_number(void);
int uchar32_cmp(const uchar32_t *s1, const char *s2, unsigned int stringlen);
int uchar32_casecmp(const uchar32_t *s1, const char *s2, unsigned int stringlen);
int uchar32_sub(const uchar32_t *hs, const char *sub, unsigned int hslen, unsigned int sublen);
struct LINE blank_line(void);

// extension.c
bool detect_extension(char *fname);

// modify.c
bool modify(void);
bool add_char(int x, int y, uchar32_t c);
bool remove_char(int x, int y);

struct KWD {
    const char *string;
    unsigned char color;
    unsigned int length;
    bool operator;
};

struct MATCH {
    const char *name;
    unsigned int length;
};

/*
Syntax Highlighting Descriptor
*/
struct SHD {
    const char *name;
    bool limited_scroll; // if set syntaxHighlight won't scroll over all the source
    unsigned int exts_len;
    const char **extensions;
    const char *word_separators;
    unsigned int kwdlen;
    struct KWD *keywords;
    unsigned char string_color;
    unsigned char stringmatch_color;
    unsigned char comment_color;
    unsigned char match_color;
    unsigned char number_color;
    unsigned char number_prefix_color;
    unsigned char number_suffix_color;
    const char *stringchars;
    unsigned int stringmatch_len;
    const struct MATCH *stringmatch;
    const struct MATCH singleline_comment;
    const struct MATCH multiline_comment[2];
    const char *match[2];
    const struct MATCH number_prefix[3]; // 0: hexadecimal 1: octal 2: binary
    const char *number_suffixes;
    const char *number_strings[4]; // 0: hexadecimal 1: octal 2: binary 3: decimal
};

// syntaxHighlight state
struct SHSTATE {
    bool multi_line_comment;
    bool backslash;
    char string;
    unsigned int waiting_to_close;
    unsigned int at_line;
};

void init_syntax_state(struct SHSTATE *state);

struct BUFFER {
    bool modified;
    bool read_only;
    bool can_write;
    struct SHSTATE syntax_state;
};

struct CFG {
    bool strict_utf8; // high/low surrogates will be replaced (for now leave it always set)
    unsigned int tablen;
    int lines;
    unsigned char line_break_type; // 0: LF  1: CRLF  2: CR
    bool use_spaces;
    bool autotab;
    bool automatch;
    bool insert_newline;
    struct SHD *current_syntax;
    unsigned int syntax_len;
    struct SHD **syntaxes;
    struct BUFFER selected_buf;
};


/*
ffffbbbb
00000000 == default with default background
00010000 == color1 with default background
00100000 == color2 with default background
00000001 == default with color1 background

There are 16 foreground colors and 16 background colors
This is good for now, in case where more colors be need, color can be changed from `unsigned char *` to `uint16_t *` (256 foregrounds + 256 backgrounds)
*/
struct LINE {
    unsigned int len;
    uchar32_t *data;
    unsigned char *color;
    unsigned int length;
    unsigned int ident;
    bool multiline_comment;
};


struct CURSOR {
    unsigned int x;
    unsigned int y;
};

struct TEXT_SCROLL {
    unsigned int x;
    unsigned int y;
};

extern struct CFG config;
extern char *filename;
extern unsigned int num_lines;
extern unsigned int len_line_number;
extern struct LINE *lines;
extern FILE *fp;
extern struct CURSOR cursor;
extern struct TEXT_SCROLL text_scroll;
extern unsigned int last_cursor_x;
extern bool colors_on;
extern bool needs_to_free_filename;
extern char *menu_message;
extern struct SHD default_syntax;
extern const uchar32_t substitute_char;
extern const char *substitute_string;
extern sig_atomic_t syntax_yield;
extern bool syntax_change;

#endif

