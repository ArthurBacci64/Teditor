#include "ted.h"

bool detect_extension(char *fname) {
    char *ext = strrchr(fname, '.');
    if (!ext)
        goto END;
    
    ext++; // Removes the .
    
    const int fextlen = strlen(ext);
    
    for (unsigned int i = 0; i < config.syntax_len; i++) {
        struct SHD *syntax = config.syntaxes[i];
        for (unsigned int j = 0; j < syntax->exts_len; j++) {
            int sextlen = strlen(syntax->extensions[j]);
            
            // fextlen == sextlen can be removed, but it gives more speed
            if (fextlen == sextlen && !strncmp(ext, syntax->extensions[j], fextlen)) {
                config.current_syntax = syntax;
                return true; // Found
            }
        }
    }
    
END:
    config.current_syntax = &default_syntax;
    return false; // Not found
}
