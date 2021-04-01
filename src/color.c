#include "ted.h"

void syntaxHighlight(void) {
    if (config.current_syntax == &default_syntax) {// just reset color to all visible lines
        for (unsigned int at = text_scroll.y; (at < text_scroll.y + config.lines) && (at != num_lines); ++at)
            memset(lines[at].color, 0, (lines[at].length + 1) * sizeof(*lines[at].color));
        return;
    }

    bool multi_line_comment = 0;
    bool backslash = 0;
    char string = '\0';
    unsigned int waiting_to_close = 0;

    unsigned int slinecommentlen   = strlen(config.current_syntax->singleline_comment);
    unsigned int mlinecommentstart = strlen(config.current_syntax->multiline_comment[0]);
    unsigned int mlinecommentend   = strlen(config.current_syntax->multiline_comment[1]);

    unsigned int hexprefixlen = strlen(config.current_syntax->number_prefix[0]);
    unsigned int octprefixlen = strlen(config.current_syntax->number_prefix[1]);
    unsigned int binprefixlen = strlen(config.current_syntax->number_prefix[2]);

    unsigned int syntax_start = strlen(config.current_syntax->stringchars) || mlinecommentstart || mlinecommentend ? 0 : text_scroll.y;
    unsigned int syntax_end = text_scroll.y + config.lines;

    for (unsigned int at = syntax_start; (at < syntax_end) && (at != num_lines); ++at) {
        bool comment = 0;
        memset(lines[at].color, 0, (lines[at].length + 1) * sizeof(*lines[at].color));
        for (unsigned int i = 0; i <= lines[at].length; i++) {
            if (lines[at].data[i] == '\\') {
                lines[at].color[i] = string ? config.current_syntax->syntax_string_color : 0;
                backslash = !backslash;
                continue;
            }
            if (!(comment || multi_line_comment) && strchr(config.current_syntax->stringchars, lines[at].data[i]) && !backslash) {
                if (!string)
                    string = lines[at].data[i];
                else if (lines[at].data[i] == (uchar32_t)string) {
                    lines[at].color[i] = config.current_syntax->syntax_string_color;
                    string = '\0';
                    continue;
                }
            }
            
            backslash = 0;
            
            if (string) {
                lines[at].color[i] = config.current_syntax->syntax_string_color;
                continue;
            }
        
            char *datachar = malloc(lines[at].length + 1);
            for (unsigned int l = 0; l <= lines[at].length; l++)
                datachar[l] = (char)lines[at].data[l];
                
            if (slinecommentlen != 0
                && lines[at].length >= slinecommentlen && i <= lines[at].length - slinecommentlen
                && memcmp(&datachar[i], config.current_syntax->singleline_comment, slinecommentlen) == 0)
                comment = 1;

            else if (mlinecommentstart != 0
                && lines[at].length >= slinecommentlen && i <= lines[at].length - mlinecommentstart
                && memcmp(&datachar[i], config.current_syntax->multiline_comment[0], mlinecommentstart) == 0)
                multi_line_comment = 1;

            else if (mlinecommentend != 0 && i >= mlinecommentend
                && memcmp(&datachar[i - mlinecommentend], config.current_syntax->multiline_comment[1], mlinecommentend) == 0)
                multi_line_comment = 0;
                
            free(datachar);
            lines[at].color[i] = comment || multi_line_comment ? config.current_syntax->syntax_comment_color : 0x0;
            if (comment || multi_line_comment) continue;
            
            // if lines[at].data[i] is a null byte, strchr will return
            if (lines[at].data[i] && 
                (strchr(config.current_syntax->match[0], lines[at].data[i]) || strchr(config.current_syntax->match[1], lines[at].data[i]))
            ) {
                bool opening = strchr(config.current_syntax->match[0], lines[at].data[i]);
                
                if (waiting_to_close && !opening) {
                    if (waiting_to_close == 1)
                        lines[at].color[i] = config.current_syntax->match_color;
                    waiting_to_close--;
                    continue;
                } else if (waiting_to_close && opening)
                    waiting_to_close++;
                
                if (at == cursor.y && i + 1 == cursor.x) {
                    lines[at].color[i] = config.current_syntax->match_color;
                    if (opening) {
                        waiting_to_close = 1;
                    } else {
                        unsigned int lay = 1;
                        for (int _at = at; _at >= 0; _at--) {
                            for (int _i = (int)at == _at ? i : lines[_at].length - 1; _i >= 0; _i--) {
                                if (strchr(config.current_syntax->match[0], lines[_at].data[_i])) {
                                    lay--;
                                    if (lay == 1) {
                                        lines[_at].color[_i] = config.current_syntax->match_color;
                                        _at = -1;
                                        break;
                                    }
                                } else if (strchr(config.current_syntax->match[1], lines[_at].data[_i])) {
                                    lay++;
                                }
                            }
                        }
                    }
                }
            }

            if (i == 0 || strchr(config.current_syntax->word_separators, lines[at].data[i - 1])) {
                unsigned int numlen = 0, prefixlen = 0;
                char *numbers = "0123456789";

                if (hexprefixlen != 0 && lines[at].length - i >= hexprefixlen
                    && !uchar32_cmp(&lines[at].data[i], config.current_syntax->number_prefix[0], hexprefixlen)) {
                    prefixlen = hexprefixlen;
                    numbers = "0123456789aAbBcCdDeEfF";
                } else if (octprefixlen != 0 && lines[at].length - i >= octprefixlen
                    && !uchar32_cmp(&lines[at].data[i], config.current_syntax->number_prefix[1], octprefixlen)) {
                    prefixlen = octprefixlen;
                    numbers = "01234567";
                } else if (binprefixlen != 0 && lines[at].length - i >= binprefixlen
                    && !uchar32_cmp(&lines[at].data[i], config.current_syntax->number_prefix[2], binprefixlen)) {
                    prefixlen = binprefixlen;
                    numbers = "01";
                }

                numlen += prefixlen;
                while ((i + numlen) < lines[at].length && strchr(numbers, lines[at].data[i + numlen])) numlen++;

                if ((i + numlen) == lines[at].length || strchr(config.current_syntax->word_separators, lines[at].data[i + numlen])) {
                    if (numlen - prefixlen > 0)
                        for (unsigned int j = 0; j < prefixlen; j++)
                            lines[at].color[i + j] = config.current_syntax->number_prefix_color;

                    for (unsigned int j = numlen - prefixlen > 0 ? prefixlen : 0; j < numlen; j++)
                        if (!lines[at].color[i + j])
                            lines[at].color[i + j] = config.current_syntax->number_color;
                    i += numlen;
                }
            }
            
            for (unsigned int k = 0; k < config.current_syntax->kwdlen; k++) {
                unsigned int stringlen = config.current_syntax->keywords[k].length;
                if (lines[at].length - i < stringlen) continue;

                if (!config.current_syntax->keywords[k].operator) {
                    if ((i != 0 && !strchr(config.current_syntax->word_separators, lines[at].data[i - 1]))
                        || !strchr(config.current_syntax->word_separators, lines[at].data[i + stringlen]))
                        continue;
                }
                if (uchar32_cmp(&lines[at].data[i], config.current_syntax->keywords[k].string, stringlen))
                    continue;

                for (unsigned int j = 0; j < stringlen; j++)
                    if (!lines[at].color[i + j])
                        lines[at].color[i + j] = config.current_syntax->keywords[k].color;
                i += stringlen - 1;
                break;
            }
        }
    }
}

void readColor(unsigned int at, unsigned int at1, unsigned char *fg, unsigned char *bg) {
    *fg = (lines[at].color[at1] & 0xF0) >> 4;
    *bg = lines[at].color[at1] & 0x0F;
}
