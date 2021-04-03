#include "ted.h"

void show_menu(char *message, char *shadow) {
    int x, y;
    getyx(stdscr, y, x);

    setcolor(COLOR_PAIR(2));
    move(config.lines, 0);
    for (unsigned int i = 0; i < (unsigned int)COLS; i++)
        addch(' ');

    move(config.lines, 0);
    if (!*message) {
        unsigned int scrolled = ((double)cy / ((double)num_lines - 1)) * 100;
        printw("I:%u %u%% %s", lines[cy].ident, scrolled, filename);

        if (config.selected_buf.git_in_repo && config.selected_buf.git_branch) {
            setcolor(COLOR_PAIR(1));
            printw(" on branch %s", config.selected_buf.git_branch);
            setcolor(COLOR_PAIR(2));
            printw(" (%s)", config.selected_buf.git_tracked ? "tracked" : "untracked");
        }

        char buf[500];
        int len = snprintf(buf, 500, "%s%s%s %s %s",
                           config.selected_buf.modified ? "!" : ".",
                           config.selected_buf.read_only ? "o" : "c",
                           config.selected_buf.can_write ? "W" : "R",
                           config.current_syntax ? config.current_syntax->name : "Off", // check if current_syntax is NULL
                           config.line_break_type == 0 ? "LF" : (config.line_break_type == 1 ? "CRLF" : "CR"));

        mvprintw(config.lines, COLS - len, "%s", buf);

    } else if (shadow != NULL) {
        printw("%s", message);
        getyx(stdscr, y, x); //save message x and y

        setcolor(COLOR_PAIR(5)); // Grey
        attron(A_BOLD);
        printw("%s", shadow);

        attroff(A_BOLD);
        setcolor(COLOR_PAIR(2));
    } else
        printw("%s", message);

    move(y, x);
}

void show_lines(void) {
    for (unsigned int i = text_scroll.y; i < text_scroll.y + config.lines; i++) {
        move(i - text_scroll.y, 0);
        if (i >= num_lines) {
            setcolor(COLOR_PAIR(1));
            for (unsigned int j = 0; j < len_line_number - 1; j++)
                addch(' ');
            addch('~');
            for (unsigned int j = 0; j < (unsigned int)COLS - len_line_number; j++)
                addch(' ');
            setcolor(COLOR_PAIR(2));
            continue;
        }
        
        setcolor(COLOR_PAIR(1));

        if (i == cy) {
            setcolor(COLOR_PAIR(3));
            attron(A_BOLD);
        }
            
        printw("%*d ", len_line_number + 1, i + 1);
        
        if (i == cy)
            attroff(A_BOLD);
    
        setcolor(COLOR_PAIR(2));
        
        unsigned int size = 0;
        for (unsigned int j = 0; size < (unsigned int)COLS - len_line_number - 1; j++) {
            if (text_scroll.x + j >= lines[i].length) {
                setcolor(COLOR_PAIR(2));
                if (j + text_scroll.x == cursor.x && i == cursor.y)
                    attron(A_REVERSE);
                    
                addch(' ');
                attroff(A_REVERSE);
                size++;
                continue;
            }
            uchar32_t el = lines[i].data[text_scroll.x + j];
            
            unsigned char fg, bg;
            readColor(i, text_scroll.x + j, &fg, &bg);

            int palette[] = {
                -1, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN,// base colors
                COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN,// bright colors
            };
            
            if (colors_on)
                init_pair(NUM_PAIRS + lines[i].color[text_scroll.x + j], palette[fg], palette[bg]);
            
            if (fg || bg)
                setcolor(COLOR_PAIR(NUM_PAIRS + lines[i].color[text_scroll.x + j]));
            else if (i == cy && text_scroll.x + j < lines[i].ident && (el == ' ' || el == '\t'))
                setcolor(COLOR_PAIR(4));
            else
                setcolor(COLOR_PAIR(2));

            if (j + text_scroll.x == cursor.x && i == cursor.y)
                attron(A_REVERSE);
            else if (fg > 6)
                attron(A_BOLD);
            
            if (el == '\t') {
                for (unsigned int k = 0; k < config.tablen; k++)
                    addch(' ');
                size += config.tablen - 1;
            } else {
                unsigned char b[4];
                int len = utf8ToMultibyte(el, b, 1);

                if (len == -1) {
                    b[0] = substitute_string[0];
                    b[1] = substitute_string[1];
                    b[2] = substitute_string[2];
                    len = 3;
                }
                printw("%.*s", len, b);
            }
            
            if (j == cursor.x + text_scroll.x && i == cursor.y)
                attroff(A_REVERSE);
            else if (fg > 6)
                attroff(A_BOLD);

            if (bg)
                setcolor(COLOR_PAIR(2));
            
            size++;
        }
    }
}
