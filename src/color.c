#include "ted.h"

void init_syntax_state(struct SHSTATE *state) {
    state->multi_line_comment = 0;
    state->backslash = 0;
    state->string = '\0';
    state->waiting_to_close = 0;
    state->at_line = 0;
}

int syntaxHighlight(void) {
    struct itimerval interval = {0}, zeroed = {0};// set preemptive timer
    interval.it_value.tv_usec = SYNTAX_TIMEOUT;
    setitimer(ITIMER_REAL, &interval, NULL);

    if (config.current_syntax == &default_syntax) {// just reset color to all lines
        for (unsigned int at = config.selected_buf.syntax_state.at_line; at < num_lines; at++) {
            if (syntax_yield) {
                syntax_yield = 0;
                config.selected_buf.syntax_state.at_line = at;
                return SYNTAX_TODO;
            }
            memset(lines[at].color, 0, (lines[at].length + 1) * sizeof(*lines[at].color));
        }
        goto END;
    }
    //todo: these are superfluous
    bool multi_line_comment = config.selected_buf.syntax_state.multi_line_comment;
    bool backslash = config.selected_buf.syntax_state.backslash;
    char string = config.selected_buf.syntax_state.string;
    unsigned int waiting_to_close = config.selected_buf.syntax_state.waiting_to_close;
    unsigned int slinecommentlen = config.current_syntax->singleline_comment.length; 
    unsigned int mlinecommentstart = config.current_syntax->multiline_comment[0].length;
    unsigned int mlinecommentend = config.current_syntax->multiline_comment[1].length;
    unsigned int hexprefixlen = config.current_syntax->number_prefix[0].length;
    unsigned int octprefixlen = config.current_syntax->number_prefix[1].length;
    unsigned int binprefixlen = config.current_syntax->number_prefix[2].length;

    for (unsigned int at = config.selected_buf.syntax_state.at_line; at < num_lines; at++) {
        if (syntax_yield) {//save state
            config.selected_buf.syntax_state.multi_line_comment = multi_line_comment;
            config.selected_buf.syntax_state.backslash = backslash;
            config.selected_buf.syntax_state.string = string;
            config.selected_buf.syntax_state.waiting_to_close = waiting_to_close;
            config.selected_buf.syntax_state.at_line = at;
            syntax_yield = 0;
            return SYNTAX_TODO;
        }

        bool comment = 0;
        memset(lines[at].color, 0, (lines[at].length + 1) * sizeof(*lines[at].color));
        for (unsigned int i = 0; i <= lines[at].length; i++) {
            if (lines[at].data[i] == '\\') {
                lines[at].color[i] = string ? config.current_syntax->string_color : 0;
                backslash = !backslash;
                for (unsigned int j = 0; j < config.current_syntax->stringmatch_len; j++)
                    if (!uchar32_cmp(&lines[at].data[i], config.current_syntax->stringmatch[j].name, config.current_syntax->stringmatch[j].length)) {
                        unsigned int len = config.current_syntax->stringmatch[j].length;
                        memset(&lines[at].color[i], config.current_syntax->stringmatch_color, len);
                        i += len - 1;
                        if (strchr(config.current_syntax->stringmatch[j].name, '\\'))
                            backslash = !backslash;
                        break;
                    }
                continue;
            }
            if (!(comment || multi_line_comment) && strchr(config.current_syntax->stringchars, lines[at].data[i]) && !backslash) {
                if (!string)
                    string = lines[at].data[i];
                else if (lines[at].data[i] == (uchar32_t)string) {
                    lines[at].color[i] = config.current_syntax->string_color;
                    string = '\0';
                    continue;
                }
            }
            backslash = 0;
            
            if (string) {
                unsigned int len = 0;
                for (unsigned int j = 0; j < config.current_syntax->stringmatch_len; j++)
                    if (!uchar32_cmp(&lines[at].data[i], config.current_syntax->stringmatch[j].name, config.current_syntax->stringmatch[j].length)) {
                        len = config.current_syntax->stringmatch[j].length;
                        break;
                    }

                if (len) {
                    memset(&lines[at].color[i], config.current_syntax->stringmatch_color, len);
                    i += len - 1;
                } else
                    lines[at].color[i] = config.current_syntax->string_color;
                continue;
            }
        
            char *datachar = malloc(lines[at].length + 1);
            for (unsigned int l = 0; l <= lines[at].length; l++)
                datachar[l] = (char)lines[at].data[l];
                
            if (slinecommentlen != 0 &&
                lines[at].length >= slinecommentlen && i <= lines[at].length - slinecommentlen &&
                memcmp(&datachar[i], config.current_syntax->singleline_comment.name, slinecommentlen) == 0)
                comment = 1;

            else if (mlinecommentstart != 0
                     && lines[at].length >= slinecommentlen && i <= lines[at].length - mlinecommentstart
                     && memcmp(&datachar[i], config.current_syntax->multiline_comment[0].name, mlinecommentstart) == 0)
                multi_line_comment = 1;

            else if (mlinecommentend != 0 && i >= mlinecommentend
                     && memcmp(&datachar[i - mlinecommentend], config.current_syntax->multiline_comment[1].name, mlinecommentend) == 0)
                multi_line_comment = 0;
                
            free(datachar);
            lines[at].color[i] = comment || multi_line_comment ? config.current_syntax->comment_color : 0;
            if (comment || multi_line_comment) continue;
            
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
                unsigned int numlen = 0, prefixlen = 0, suffixlen = 0;
                const char *numbers = config.current_syntax->number_strings[3];

                if (hexprefixlen != 0 && lines[at].length - i >= hexprefixlen
                    && !uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[0].name, hexprefixlen)
                    && ((hexprefixlen >= octprefixlen && hexprefixlen >= binprefixlen)
                        || (uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[1].name, octprefixlen)
                            && uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[2].name, binprefixlen)))) {
                    prefixlen = hexprefixlen;
                    numbers = config.current_syntax->number_strings[0];

                } else if (octprefixlen != 0 && lines[at].length - i >= octprefixlen
                           && !uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[1].name, octprefixlen)
                           && ((octprefixlen >= binprefixlen)
                               || uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[2].name, binprefixlen))) {
                    prefixlen = octprefixlen;
                    numbers = config.current_syntax->number_strings[1];

                } else if (binprefixlen != 0 && lines[at].length - i >= binprefixlen
                           && !uchar32_casecmp(&lines[at].data[i], config.current_syntax->number_prefix[2].name, binprefixlen)) {
                    prefixlen = binprefixlen;
                    numbers = config.current_syntax->number_strings[2];
                }

                numlen = prefixlen;
                while ((i + numlen) < lines[at].length && strchr(numbers, lines[at].data[i + numlen])) numlen++;

                if (numlen != 0) {
                    while ((i + numlen + suffixlen) < lines[at].length
                    && strchr(config.current_syntax->number_suffixes, lines[at].data[i + numlen + suffixlen])) suffixlen++;

                    if ((i + numlen + suffixlen) == lines[at].length
                        || strchr(config.current_syntax->word_separators, lines[at].data[i + numlen + suffixlen])) {
                        unsigned int start = 0;

                        if (numlen == prefixlen) {
                            for (unsigned int j = 0; j < prefixlen; j++)
                                if (!strchr(numbers, lines[at].data[i + j])) {
                                    start = prefixlen;
                                    break;
                                }
                        } else
                            memset(&lines[at].color[i], config.current_syntax->number_prefix_color, prefixlen);

                        for (unsigned int j = start; j < numlen; j++)
                            if (!lines[at].color[i + j])
                                lines[at].color[i + j] = config.current_syntax->number_color;

                        memset(&lines[at].color[i + numlen], config.current_syntax->number_suffix_color, suffixlen);
                        i += numlen + suffixlen;
                    }
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
END:
    setitimer(ITIMER_REAL, &zeroed, NULL);
    syntax_yield = 0;
    return SYNTAX_END;
}

void readColor(unsigned int at, unsigned int at1, unsigned char *fg, unsigned char *bg) {
    *fg = (lines[at].color[at1] & 0xF0) >> 4;
    *bg = lines[at].color[at1] & 0x0F;
}
