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
        lines = realloc(lines, (num_lines + 1) * sizeof(struct LINE));
        memmove(&lines[num_lines + 2], &lines[num_lines + 1], sizeof(struct LINE));
        new_line(num_lines, lines[num_lines - 1].length);
        num_lines++;
    }
    
    for (unsigned int i = 0; i < num_lines; i++) {
        for (unsigned int j = 0; j < lines[i].length; j++) {
            unsigned char b[4];
            fwrite(b, sizeof(unsigned char), utf8ToMultibyte(lines[i].data[j], b), fpw);
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
}

void read_lines(void) {
    if (fp == NULL) {
        num_lines = 1;
        lines = malloc(sizeof(struct LINE));

        lines[0].len = READ_BLOCKSIZE;
        lines[0].data = malloc(lines[0].len * sizeof(uchar32_t));
        lines[0].color = malloc(lines[0].len * sizeof(unsigned char));
        lines[0].length = 0;
        lines[0].data[0] = '\0';
        lines[0].ident = 0;
        
        syntaxHighlight();
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

        lines = realloc(lines, ++num_lines * sizeof(struct LINE));

        lines[i].len = READ_BLOCKSIZE;
        lines[i].data = malloc(lines[i].len * sizeof *(lines[i].data));
        lines[i].color = malloc(lines[i].len * sizeof *(lines[i].color));
        lines[i].length = 0;
        lines[i].ident = 0;

        char c;
        unsigned int j;
        char passed_spaces = 0;

        for (j = 0; (c = fgetc(fp)) != lineend && c != EOF; j++) {

            if (lines[i].length + 1 >= lines[i].len) {
                lines[i].len += READ_BLOCKSIZE;
                lines[i].data = realloc(lines[i].data, lines[i].len * sizeof(uchar32_t));
                lines[i].color = realloc(lines[i].color, lines[i].len * sizeof(unsigned char));
            }

            if (passed_spaces == 0 && c != ' ')
                passed_spaces = 1;
            else if (passed_spaces == 0)
                lines[i].ident++;

            unsigned char uc = *(unsigned char *)&c;

            utf8ReadFile(uc, j, i, fp);

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
        read_only = !(
            (st.st_mode & S_IWOTH) // all user write permission
            || (getuid() == st.st_uid && (st.st_mode & S_IWUSR)) // owner write permission
            || (getgid() == st.st_gid && (st.st_mode & S_IWGRP)) // group write permission
        );
    } else
        read_only = errno == EACCES; // if stat fails and errno is not EACCES, read_only will not be set
}
