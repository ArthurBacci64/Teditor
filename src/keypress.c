#include "ted.h"

void expandLine(unsigned int at, int x) {
    while (lines[at].len <= lines[at].length + x) {
        lines[at].len += READ_BLOCKSIZE;
        lines[at].data = realloc(lines[cy].data, lines[cy].len * sizeof(uchar32_t));
        lines[at].color = realloc(lines[cy].color, lines[cy].len * sizeof(unsigned char));
    }
}

void new_line(unsigned int at, int x) {
    lines[at].len = READ_BLOCKSIZE;
    lines[at].data = malloc(lines[at].len * sizeof(uchar32_t));
    lines[at].color = malloc(lines[at].len * sizeof(unsigned char));
    lines[at].length = 0;

    expandLine(at, lines[at - 1].length - x + 1);
    memcpy(lines[at].data, &lines[at - 1].data[x], (lines[at - 1].length - x) * sizeof(uchar32_t));

    lines[at].length += lines[at - 1].length - x;
    lines[at].data[lines[at].length] = '\0';

    lines[at - 1].length = x;
    lines[at - 1].data[lines[at - 1].length] = '\0';

    calculate_len_line_number();
}

void process_keypress(int c) {
    if (c != ERR)
        message("");

    switch (c) {
    case KEY_UP:
    case ctrl('p'):
        cursor.y -= cursor.y > 0;
        cursor.x = last_cursor_x;

        cursor_in_valid_position();
        syntaxHighlight();
        break;
    case KEY_DOWN:
    case ctrl('n'):
        cursor.y++;
        cursor.x = last_cursor_x;

        cursor_in_valid_position();
        syntaxHighlight();
        break;
    case KEY_LEFT:
    case ctrl('b'):
        cursor.x -= (cursor.x > 0);
        cursor_in_valid_position();
        last_cursor_x = cx;
        break;
    case KEY_RIGHT:
    case ctrl('f'):
        cursor.x++;
        cursor_in_valid_position();
        last_cursor_x = cx;
        break;
    case KEY_HOME:
    case ctrl('a'):
        cursor.x = 0;
        last_cursor_x = cx;
        cursor_in_valid_position();
        break;
    case KEY_END:
    case ctrl('e'):
        cursor.x = lines[cursor.y].length;
        last_cursor_x = cx;
        cursor_in_valid_position();
        break;
    case ctrl('s'):
        if (!config.selected_buf.read_only)
            savefile();
        break;
    case '\t':
        if (config.use_spaces == 1) {
            for (unsigned int i = 0; i < config.tablen; i++)
                process_keypress(' ');
            return;
        } // else, it will pass though and be added to the buffer
        break;
    case ctrl('g'):
        config_dialog();
        break;
    case ctrl('q'):
        parse_command(config.selected_buf.read_only ? "read-only 0" : "read-only 1");
        break;
    case KEY_PPAGE:
    {
        unsigned int ccy = cy;
        for (unsigned int i = 0; i < (unsigned int)(ccy % config.lines + config.lines); i++)
            process_keypress(KEY_UP);
        break;
    } case KEY_NPAGE:
    {
        unsigned int ccy = cy;
        for (unsigned int i = 0; i < (unsigned int)(config.lines - (ccy % config.lines) - 1 + config.lines); i++)
            process_keypress(KEY_DOWN);
        break;
    } case KEY_MOUSE:
    {
        MEVENT event;
        if (getmouse(&event) == OK)
            processMouseEvent(event);

        break;
    } case 0x209:
    {
        if (modify()) {
            if (num_lines > 1) {
                free(lines[cy].data);
                free(lines[cy].color);
                memmove(&lines[cy], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));
                lines = realloc(lines, --num_lines * sizeof(struct LINE));
            } else {
                lines[cy].data[0] = '\0';
                lines[cy].length = 0;
            }
            cursor_in_valid_position();
            calculate_len_line_number();
            syntaxHighlight();
        }
        break;
    } case ctrl('w'):
    {
        bool passed_spaces = 0;
        while (cx > 0 && (!strchr(config.current_syntax->word_separators, lines[cy].data[cx - 1]) || !passed_spaces)) {
            if (!remove_char(cx - 1, cy))
                break;
            process_keypress(KEY_LEFT);
            if (cx > 0 && !strchr(config.current_syntax->word_separators, lines[cy].data[cx - 1]))
                passed_spaces = 1;
        }
        syntaxHighlight();
        break;
    } case ctrl('o'):
    {
        char *d = prompt("open: ", filename);
        if (d)
            openFile(d, 1);
        break;
    } case CTRL_KEY_LEFT:
    {
        char passed_spaces = 0;
        while (cx > 0) {
            process_keypress(KEY_LEFT);
            if (!strchr(config.current_syntax->word_separators, lines[cy].data[cx]))
                passed_spaces = 1;
            if (strchr(config.current_syntax->word_separators, lines[cy].data[cx]) && passed_spaces) {
                process_keypress(KEY_RIGHT);
                break;
            }
            }
        break;
    }
    case CTRL_KEY_RIGHT:
    {
        char passed_spaces = 0;
        while (lines[cy].data[cx] != '\0' && !(strchr(config.current_syntax->word_separators, lines[cy].data[cx]) && passed_spaces)) {
            if (!strchr(config.current_syntax->word_separators, lines[cy].data[cx]))
                passed_spaces = 1;
            process_keypress(KEY_RIGHT);
        }
        break;
    } case KEY_BACKSPACE: case KEY_DC: case 127:
    {
        if (modify()) {
            lines[cy].ident -= cx <= lines[cy].ident && cx > 0;

            if (cx >= 1) {
                if (remove_char(cx - 1, cy))
                    process_keypress(KEY_LEFT);
            } else if (cy > 0) {
                struct LINE del_line = lines[cy];
                memmove(&lines[cy], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));
                lines = realloc(lines, --num_lines * sizeof(struct LINE));

                cursor.y -= cursor.y > 0;
                cursor.x = lines[cy].length;
                cursor_in_valid_position();

                process_keypress(KEY_RIGHT);
                expandLine(cy, del_line.length);

                memmove(&lines[cy].data[lines[cy].length], del_line.data, del_line.length * sizeof(uchar32_t));
                lines[cy].length += del_line.length;

                lines[cy].data[lines[cy].length] = '\0';

                free(del_line.data);
                free(del_line.color);

                calculate_len_line_number();
            }

            lines[cy].ident = 0;
            for (unsigned int i = 0; lines[cy].data[i] != '\0'; i++) {
                if (lines[cy].data[i] != ' ') break;
                lines[cy].ident++;
            }
            syntaxHighlight();
        }
        break;
    } case '\n': case KEY_ENTER: case '\r':
    {
        if (modify()) {
            lines = realloc(lines, (num_lines + 1) * sizeof(struct LINE));
            memmove(&lines[cy + 2], &lines[cy + 1], (num_lines - cy - 1) * sizeof(struct LINE));
            num_lines++;

            unsigned int lcx = cx;
            last_cursor_x = 0;
            cursor.y++;
            cursor.x = 0;
            cursor_in_valid_position();
            new_line(cy, lcx);

            if (config.autotab == 1) {
                const unsigned int ident = lines[cy - 1].ident;
                lines[cy].ident = ident;
                lines[cy].len += ident + 1;
                lines[cy].data = realloc(lines[cy].data, lines[cy].len * sizeof(uchar32_t));
                lines[cy].color = realloc(lines[cy].color, lines[cy].len * sizeof(unsigned char));
                memmove(&lines[cy].data[ident], lines[cy].data, (lines[cy].length + 1) * sizeof(uchar32_t));

                for (unsigned int i = 0; i < ident; i++)
                    lines[cy].data[i] = ' ';
                lines[cy].length += ident;

                for (unsigned int i = 0; i < ident; i++)
                    process_keypress(KEY_RIGHT);
            } else
                lines[cy].ident = 0;

            syntaxHighlight();
        }
        break;
    }
    }

    if (isprint(c) || c == '\t' || (c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7)) {

        if (c == ' ' && cx <= lines[cy].ident)
            lines[cy].ident++;

        if (config.automatch && cx == lines[cy].length) {
            char *match = strchr(config.current_syntax->match[0], c);
            if (match != NULL)
                add_char(cx, cy, config.current_syntax->match[1][match - config.current_syntax->match[0]]);
        }

        uchar32_t ec = c;

        if ((c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7))
            ec += getch() << 8;
        if ((c >= 0xE0 && c <= 0xEF) || (c >= 0xF0 && c <= 0xF7))
            ec += getch() << 16;
        if (c >= 0xF0 && c <= 0xF7)
            ec += getch() << 24;

        if (add_char(cx, cy, ec))
            process_keypress(KEY_RIGHT);

        syntaxHighlight();
    }
}
