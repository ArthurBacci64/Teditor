#include "ted.h"
#include "syntax.h"

struct LINE *lines = NULL;
unsigned int num_lines;
unsigned int len_line_number; // The length of the number of the last line

FILE *fp = NULL;
struct CURSOR cursor = {0, 0};
struct TEXT_SCROLL text_scroll = {0, 0};

char *filename;
char *menu_message = "";

bool colors_on = 0;
bool needs_to_free_filename;
bool read_only = 0;

void setcolor(int c) {
    if (colors_on)
        attrset(c);
}

unsigned int last_cursor_x = 0;

struct CFG config = {
    4, 0, 0, 1, 1, 1, 1,
    &default_syntax, 0, NULL
};

int main(int argc, char **argv) {
    if (argc < 2) {
        struct stat st = {0};

        char *config = home_path(".config/");
        if (stat(config, &st) == -1)
            mkdir(config, 0777);

        char *config_ted = home_path(".config/ted/");
        if (stat(config_ted, &st) == -1)
            mkdir(config_ted, 0777);
            
        filename = home_path(".config/ted/buffer");
        needs_to_free_filename = 1;
        free(config);
        free(config_ted);
    } else {
        if (*argv[1] == '/') 
            filename = argv[1];
        else {
            char *fname = malloc(1000 * sizeof *filename);
            getcwd(fname, 1000);
            filename = malloc(1000 * sizeof *filename);
            snprintf(filename, 1000, "%s/%s", fname, argv[1]);
            free(fname);
        }
        needs_to_free_filename = *argv[1] != '/';
    }
    
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(1);
    curs_set(0);

    register_syntax();

    calculate_len_line_number();

    colors_on = has_colors() && start_color() == OK;

    if (colors_on) {
        use_default_colors();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, -1, -1);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, -1, COLOR_BLACK);
        init_pair(5, COLOR_BLACK, -1);
    }

    config.lines = LINES - 1;

    fp = fopen(filename, "r");
    read_lines();
    if (fp) fclose(fp);
    detect_read_only(filename);
    
    int last_LINES = LINES;
    int last_COLS = COLS;
    
    int c;
    while (1) {
        if (last_LINES != LINES || last_COLS != COLS) {
            last_LINES = LINES;
            last_COLS = COLS;
            config.lines = LINES - 1;
            cursor_in_valid_position();
        }
        
        show_lines();
        show_menu(menu_message, NULL);
        refresh();

        c = getch();

        if (c == ctrl('c'))
            break;

        process_keypress(c);
        syntaxHighlight();
    }

    free_lines();

    if (needs_to_free_filename == 1)
        free(filename);

    endwin();
    return 0;
}
