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

void setcolor(int c) {
    if (colors_on)
        attrset(c);
}

unsigned int last_cursor_x = 0;

struct CFG config = {
    1, 4, 0, 0, 1, 1, 1, 1,
    &default_syntax, 0, NULL,
    {0, 0, 1, {0}},
};

sig_atomic_t syntax_yield = 0; // flag set by the SIGALRM handler
bool syntax_change = 0; // used to reset syntax highlighting state

void sighandler(int x) {
    signal(SIGALRM, sighandler);
    syntax_yield = 1;// set the flag for yielding from syntaxHighlight
}

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
            filename = malloc(1000 * sizeof *filename);
            *filename = '\0';

            if (getcwd(filename, 1000) != NULL) strncat(filename, "/", 1000);
            else strncat(filename, "./", 1000); //try relative path
            strncat(filename, argv[1], 1000);
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
    timeout(INPUT_TIMEOUT);

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
    int last_LINES = LINES;
    int last_COLS = COLS;

    fp = fopen(filename, "r");
    read_lines();
    if (fp) fclose(fp);
    detect_read_only(filename);

    signal(SIGALRM, sighandler);
    init_syntax_state(&config.selected_buf.syntax_state);
    bool syntax_todo = syntaxHighlight() == SYNTAX_TODO;

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

        int c = getch();
        if (c == ctrl('c')) {
            if (config.selected_buf.modified) {
                char *prt = prompt_hints("Unsaved changes: ", "", "'exit' to exit", NULL);
                if (prt && !strcmp("exit", prt)) {
                    free(prt);
                    break;
                }
                free(prt);
            } else
                break;
        }
        process_keypress(c);

        if (syntax_todo || syntax_change) {// call syntaxHighlight after processing char
            if (syntax_change) { // reset syntax state before calling syntaxHighlight
                syntax_change = 0;
                syntax_yield = 0;
                init_syntax_state(&config.selected_buf.syntax_state);
            }
            syntax_todo = syntaxHighlight() == SYNTAX_TODO;
        }
    }

    // TODO: add free_everything function
    free_lines();
    if (needs_to_free_filename == 1)
        free(filename);

    endwin();
    return 0;
}

