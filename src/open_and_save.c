#include "ted.h"

void savefile(void) {
    FILE *fpw = fopen(filename, "w");

    if (fpw == NULL) {
        char buf[1000];
        snprintf(buf, 1000, "Could not open the file\nErrno: %d\nPress any key", errno);

        message(buf);
        return;
    }

    if (config.insert_newline && lines[num_lines - 1].length > 0) {
        lines = realloc(lines, ++num_lines * sizeof(*lines));
        blank_line(&lines[num_lines - 1]);
    }

    for (unsigned int i = 0; i < num_lines; i++) {
        for (unsigned int j = 0; j < lines[i].length; j++) {
            unsigned char b[4];
            int len = utf8ToMultibyte(lines[i].data[j], b, 0);
            fwrite(b, sizeof(unsigned char), len, fpw);
        }
        if (num_lines > 1) {
            if (config.line_break_type == 0)
                fputc('\n', fpw);
            else if (config.line_break_type == 1)
                fputs("\r\n", fpw);
            else if (config.line_break_type == 2)
                fputc('\r', fpw);
        }
    }
    fclose(fpw);

    config.selected_buf.modified = 0;
}

void read_lines(void) {
    if (fp == NULL) {
        num_lines = 1;
        lines = malloc(num_lines * sizeof(*lines));

        blank_line(&lines[0]);
        return;
    }

    detect_linebreak();
    char lineend = config.line_break_type == 0 ? '\n' : '\r';

    num_lines = 0;
    for (unsigned int i = 0; !feof(fp); i++) {

        if (fgetc(fp) == EOF && num_lines > 0)
            break;
        else
            fseek(fp, -1, SEEK_CUR);

        lines = realloc(lines, ++num_lines * sizeof(*lines));
        blank_line(&lines[i]);

        char c;
        unsigned int j;
        char passed_spaces = 0;

        for (j = 0; (c = fgetc(fp)) != lineend && c != EOF; j++) {

            if (lines[i].length + 1 >= lines[i].len) {
                lines[i].len += READ_BLOCKSIZE;
                lines[i].data = realloc(lines[i].data, lines[i].len * sizeof(*lines[i].data));
                lines[i].color = realloc(lines[i].color, lines[i].len * sizeof(*lines[i].color));
            }

            if (passed_spaces == 0 && c != ' ')
                passed_spaces = 1;
            else if (passed_spaces == 0)
                lines[i].ident++;

            unsigned char uc = c;
            utf8ReadFile(uc, &lines[i].data[j], fp);
            lines[i].length++;
        }
        
        lines[i].data[j] = '\0';
        
        if (config.line_break_type == 1)
            fgetc(fp);
    }
    detect_extension(filename);
    syntaxHighlight();
}

void detect_linebreak(void) {
    char c;
    while (!feof(fp)) {
        c = fgetc(fp);

        if (c == '\r') {
            if (fgetc(fp) == '\n')
                config.line_break_type = 1;
            else
                config.line_break_type = 2;

            break;
        } else if (c == '\n') {
            config.line_break_type = 0;
            break;
        }
    }
    rewind(fp);
}

void openFile(char *fname, bool needs_to_free) {
    if (needs_to_free_filename)
        free(filename);
    
    filename = fname;
    needs_to_free_filename = needs_to_free;
    
    cursor.x = 0;
    cursor.y = 0;
    last_cursor_x = 0;
    text_scroll.x = 0;
    text_scroll.y = 0;
    free_lines();
    num_lines = 0;
    lines = NULL;
    
    fp = fopen(filename, "r");
    read_lines();
    if (fp != NULL)
        fclose(fp);

    calculate_len_line_number();
    detect_read_only(fname);
}

void detect_read_only(char *fname) {
    struct stat st;
    if (stat(fname, &st) == 0) {
        config.selected_buf.can_write = (st.st_mode & S_IWOTH) || // all user write permission
            (getuid() == st.st_uid && (st.st_mode & S_IWUSR)) || // owner write permission
            (getgid() == st.st_gid && (st.st_mode & S_IWGRP));  // group write permission
    } else // if stat fails and errno is not EACCES, can_write will be true
        config.selected_buf.can_write = errno != EACCES;
    config.selected_buf.read_only = !config.selected_buf.can_write;
}
