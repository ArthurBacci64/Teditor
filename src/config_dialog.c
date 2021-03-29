#include "ted.h"

static void tablen(char **words, unsigned int words_len) {
    if (words_len == 1) {
        int answer_int = atoi(words[0]);
        if (answer_int > 0)
            config.tablen = answer_int;
    }
}

static void linebreak(char **words, unsigned int words_len) {
    if (words_len == 1) {
        if (strcasecmp(words[0], "LF") == 0)
            config.line_break_type = 0;
            
        else if (strcasecmp(words[0], "CRLF") == 0)
            config.line_break_type = 1;

        else if (strcasecmp(words[0], "CR") == 0)
            config.line_break_type = 2;
    }
}

static void use_spaces(char **words, unsigned int words_len) {
    if (words_len == 1) {
        if (strcasecmp(words[0], "TRUE") == 0 || strcmp(words[0], "1") == 0)
            config.use_spaces = 1;
        else if (strcasecmp(words[0], "FALSE") == 0 || strcmp(words[0], "0") == 0)
            config.use_spaces = 0;
    }
}

static void autotab(char **words, unsigned int words_len) {
    if (words_len == 1) {
        if (strcasecmp(words[0], "TRUE") == 0 || strcmp(words[0], "1") == 0)
            config.autotab = 1;
        else if (strcasecmp(words[0], "FALSE") == 0 || strcmp(words[0], "0") == 0)
            config.autotab = 0;
    }
}

static void save_as(char **words, unsigned int words_len) {
    if (words_len == 1) {
        if (needs_to_free_filename)
            free(filename);

        filename = malloc(sizeof(words[0]));
        strcpy(filename, words[0]);
        needs_to_free_filename = 1;
        read_only = 0; // todo: check if the new filename is read only
        savefile();
    }
}

static void manual(char **words, unsigned int words_len) {
    if (words_len == 0) {
        openFile(home_path(".config/ted/docs/help.txt"), 1);
        read_only = 1;
    } else if (words_len == 1) {
        char fname[1000];
        snprintf(fname, 1000, ".config/ted/docs/%s.txt", words[0]);
        openFile(home_path(fname), 1);
        read_only = 1;
    }
}

static void syntax(char **words, unsigned int words_len) {
    if (words_len == 1) {
        char *str = malloc(strlen(words[0]) + 2);
        *str = '.';
        strcpy(str + 1, words[0]);

        if (!detect_extension(str))
            config.current_syntax = &default_syntax;
            
        free(str);
    }
}

struct {
    const char *name;
    void (*function)(char **words, unsigned int words_len);
} fns[] = {
    {"tablen"    , tablen    },
    {"linebreak" , linebreak },
    {"use-spaces", use_spaces},
    {"autotab"   , autotab   },
    {"save-as"   , save_as   },
    {"manual"    , manual    },
    {"syntax"    , syntax    }
};

struct HINTS hints[] =
    { {"tablen"    , " <tablen>"                         }
    , {"linebreak" , " {LF, CR, CRLF}"                   }
    , {"use-spaces", " {0/FALSE, 1/TRUE}"                }
    , {"autotab"   , " {0/FALSE, 1/TRUE}"                }
    , {"save-as"   , " <filename>"                       }
    , {"manual"    , " <page (nothing for index)>"       }
    , {"syntax"    , " <language (blank for disabling)>" }
    , {NULL        , NULL                                }
    };

void config_dialog(void) {
    char *base_hint = "{tablen, linebreak, use-spaces, autotab, save-as, manual, syntax}";
    char *command = prompt_hints("Enter command: ", "", base_hint, hints);
    
    if (!command)
        return;

    int words_len;
    char **words = split_str(command, &words_len);

    const unsigned int fnslen = sizeof(fns) / sizeof(*fns);
    for (unsigned int i = 0; i < fnslen; i++) {
        if (strcmp(words[0], fns[i].name) == 0) {
            fns[i].function(words + 1, words_len - 1);
            break;
        }
    }

    free(command);
    for (int i = 0; i < words_len; i++)
        free(words[i]);
    free(words);
}
