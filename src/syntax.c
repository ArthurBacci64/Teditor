#include "syntax.h"

/*
C syntax highlighting descriptor
*/

static struct KWD c_kwd[] = {
    KEYWORD("break", 0x10), KEYWORD("case", 0x10), KEYWORD("default", 0x10), // ansi c
    KEYWORD("continue", 0x10), KEYWORD("while", 0x10), KEYWORD("enum", 0x10),
    KEYWORD("do", 0x10), KEYWORD("else", 0x10), KEYWORD("extern", 0x10),
    KEYWORD("for", 0x10), KEYWORD("goto", 0x10), KEYWORD("if", 0x10),
    KEYWORD("return", 0x10), KEYWORD("sizeof", 0x10), KEYWORD("struct", 0x10),
    KEYWORD("switch", 0x10), KEYWORD("typedef", 0x10), KEYWORD("union", 0x10),
    KEYWORD("asm" , 0x10), KEYWORD("fortran" , 0x10),

    KEYWORD("char", 0x40), KEYWORD("double", 0x40), KEYWORD("long", 0x40), // c types
    KEYWORD("short", 0x40), KEYWORD("int", 0x40), KEYWORD("void", 0x40),
    KEYWORD("float", 0x40), KEYWORD("auto", 0x40),
    
    KEYWORD("const", 0x40), KEYWORD("signed", 0x40), KEYWORD("unsigned", 0x40), //type modifiers
    KEYWORD("inline", 0x40), KEYWORD("volatile", 0x40), KEYWORD("static", 0x40),
    KEYWORD("restrict", 0x40), KEYWORD("register", 0x40),

    KEYWORD("_Alignas", 0x40), KEYWORD("_Alignof", 0x40), // c11 and c99
    KEYWORD("_Atomic", 0x40), KEYWORD("_Bool", 0x40), KEYWORD("_Complex", 0x40),
    KEYWORD("_Decimal128", 0x40), KEYWORD("_Decimal32", 0x40), KEYWORD("_Decimal64", 0x40),
    KEYWORD("_Generic", 0x40), KEYWORD("_Imaginary", 0x40), KEYWORD("_Noreturn", 0x40),
    KEYWORD("_Static_assert", 0x40), KEYWORD("_Thread_local", 0x40),

    KEYWORD("bool", 0x10), KEYWORD("alignas", 0x10), KEYWORD("alignof", 0x10), //macros
    KEYWORD("complex", 0x10), KEYWORD("imaginary", 0x10), KEYWORD("thread_local", 0x10),

    KEYWORD("defined", 0x10), KEYWORD("#define", 0x10), KEYWORD("#undef", 0x10), //preprocessor
    KEYWORD("#ifdef", 0x10), KEYWORD("#ifndef", 0x10), KEYWORD("#if", 0x10),
    KEYWORD("#elif" , 0x10), KEYWORD("#else" , 0x10), KEYWORD("#endif" , 0x10),
    KEYWORD("#line" , 0x10), KEYWORD("#error" , 0x10), KEYWORD("#warning" , 0x10),
    KEYWORD("#pragma" , 0x10), KEYWORD("_Pragma" , 0x10), KEYWORD("#include", 0x10),

    KEYWORD("false", 0x20), KEYWORD("true", 0x20), KEYWORD("NULL", 0x20), // standard constants
    KEYWORD("stdin", 0x20), KEYWORD("stdout", 0x20), KEYWORD("stderr", 0x20),

    OPERATOR("*" , 0x30), OPERATOR("," , 0x30), OPERATOR(";" , 0x30),
    OPERATOR("/" , 0x30), OPERATOR("-" , 0x30), OPERATOR("+" , 0x30),
    OPERATOR("%" , 0x30), OPERATOR("^" , 0x30), OPERATOR("&" , 0x30),
    OPERATOR("~" , 0x30), OPERATOR("|" , 0x30), OPERATOR("!" , 0x30),
    OPERATOR("<" , 0x30), OPERATOR(">" , 0x30), OPERATOR("=" , 0x30),
    OPERATOR("?" , 0x30), OPERATOR(":" , 0x30), OPERATOR("." , 0x30),
    
    OPERATOR("(", 0x50), OPERATOR(")", 0x50),
    OPERATOR("{", 0x50), OPERATOR("}", 0x50),
    OPERATOR("[", 0x50), OPERATOR("]", 0x50),
};

static const char *c_exts[] = {"c", "h"};

static struct SHD c_syntax = {
    "C",
    sizeof c_exts / sizeof *c_exts, c_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof c_kwd / sizeof *c_kwd, c_kwd, //Keywords
    0x60, 0x50, 0x05, 0x30,
    "\"\'", // Strings charaters
    "//", {"/*", "*/"}, // Comments
    {"{[(", "}])"},
    {"0x", "0", ""}
};

/*
C++ syntax highlighting descriptor
*/

static struct KWD cpp_kwd[] = {
    KEYWORD("alignas", 0x10), KEYWORD("alignof", 0x10), KEYWORD("and", 0x10),
    KEYWORD("and_eq", 0x10), KEYWORD("asm", 0x10), KEYWORD("catch", 0x10),
    KEYWORD("bitand", 0x10), KEYWORD("bitor", 0x10), KEYWORD("compl", 0x10),
    KEYWORD("break", 0x10), KEYWORD("case", 0x10), KEYWORD("class", 0x10),
    KEYWORD("concept", 0x10), KEYWORD("const_cast", 0x10), KEYWORD("continue", 0x10),
    KEYWORD("co_await", 0x10), KEYWORD("co_return", 0x10), KEYWORD("co_yield", 0x10),
    KEYWORD("decltype", 0x10), KEYWORD("default", 0x10), KEYWORD("delete", 0x10),
    KEYWORD("do", 0x10), KEYWORD("else", 0x10), KEYWORD("enum", 0x10),
    KEYWORD("dynamic_cast", 0x10), KEYWORD("goto", 0x10), KEYWORD("if", 0x10),
    KEYWORD("explicit", 0x10), KEYWORD("export", 0x10), KEYWORD("extern", 0x10),
    KEYWORD("for", 0x10), KEYWORD("namespace", 0x10), KEYWORD("new", 0x10),
    KEYWORD("noexcept", 0x10), KEYWORD("not", 0x10), KEYWORD("not_eq", 0x10),
    KEYWORD("operator", 0x10), KEYWORD("or", 0x10), KEYWORD("sizeof", 0x10),
    KEYWORD("or_eq", 0x10), KEYWORD("reinterpret_cast", 0x10), KEYWORD("requires", 0x10),
    KEYWORD("return", 0x10), KEYWORD("static_assert", 0x10), KEYWORD("static_cast", 0x10),
    KEYWORD("struct", 0x10), KEYWORD("switch", 0x10), KEYWORD("synchronized", 0x10),
    KEYWORD("template", 0x10), KEYWORD("throw", 0x10), KEYWORD("try", 0x10),
    KEYWORD("typedef", 0x10), KEYWORD("typeid", 0x10), KEYWORD("typename", 0x10),
    KEYWORD("union", 0x10), KEYWORD("using", 0x10), KEYWORD("while", 0x10),
    KEYWORD("xor_eq", 0x10), KEYWORD("transaction_safe_dynamic", 0x10), KEYWORD("final", 0x10),
    KEYWORD("override", 0x10), KEYWORD("transaction_safe", 0x10), KEYWORD("xor", 0x10),

    KEYWORD("signed", 0x40), KEYWORD("unsigned", 0x40), KEYWORD("virtual", 0x40), // type modifiers
    KEYWORD("volatile", 0x40), KEYWORD("friend", 0x40), KEYWORD("inline", 0x40),
    KEYWORD("mutable", 0x40), KEYWORD("thread_local", 0x40), KEYWORD("atomic_cancel", 0x40),
    KEYWORD("atomic_commit", 0x40), KEYWORD("atomic_noexcept", 0x40), KEYWORD("constinit", 0x40),
    KEYWORD("const", 0x40), KEYWORD("consteval", 0x40), KEYWORD("constexpr", 0x40),
    KEYWORD("private", 0x40), KEYWORD("protected", 0x40), KEYWORD("public", 0x40),
    KEYWORD("reflexpr", 0x40), KEYWORD("register", 0x40), KEYWORD("static", 0x40),

    KEYWORD("char", 0x40), KEYWORD("char8_t", 0x40), KEYWORD("char16_t", 0x40), // primitives
    KEYWORD("char32_t", 0x40), KEYWORD("int", 0x40), KEYWORD("long", 0x40),
    KEYWORD("short", 0x40), KEYWORD("wchar_t", 0x40), KEYWORD("bool", 0x40),
    KEYWORD("void", 0x40), KEYWORD("double", 0x40), KEYWORD("auto", 0x40),
    KEYWORD("float", 0x40),
    
    KEYWORD("false", 0x20), KEYWORD("true", 0x20), KEYWORD("nullptr", 0x20), //literals
    KEYWORD("this", 0x20), KEYWORD("std::", 0x20),

    KEYWORD("defined", 0x10), KEYWORD("#define", 0x10), KEYWORD("#undef", 0x10), //preprocessor
    KEYWORD("#ifdef", 0x10), KEYWORD("#ifndef", 0x10), KEYWORD("#if", 0x10),
    KEYWORD("#elif" , 0x10), KEYWORD("#else" , 0x10), KEYWORD("#endif" , 0x10),
    KEYWORD("#line" , 0x10), KEYWORD("#error" , 0x10), KEYWORD("#warning" , 0x10),
    KEYWORD("#pragma" , 0x10), KEYWORD("_Pragma" , 0x10), KEYWORD("#include", 0x10),
    KEYWORD("export" , 0x10), KEYWORD("import" , 0x10), KEYWORD("module", 0x10), //c++20 modules

    OPERATOR("*" , 0x30), OPERATOR("," , 0x30), OPERATOR(";" , 0x30),
    OPERATOR("/" , 0x30), OPERATOR("-" , 0x30), OPERATOR("+" , 0x30),
    OPERATOR("%" , 0x30), OPERATOR("^" , 0x30), OPERATOR("&" , 0x30),
    OPERATOR("~" , 0x30), OPERATOR("|" , 0x30), OPERATOR("!" , 0x30),
    OPERATOR("<" , 0x30), OPERATOR(">" , 0x30), OPERATOR("=" , 0x30),
    OPERATOR("?" , 0x30), OPERATOR(":" , 0x30), OPERATOR("." , 0x30),
    
    OPERATOR("(", 0x50), OPERATOR(")", 0x50),
    OPERATOR("{", 0x50), OPERATOR("}", 0x50),
    OPERATOR("[", 0x50), OPERATOR("]", 0x50),
};

static const char *cpp_exts[] = {"cpp", "hpp", "cc", "hh"};

static struct SHD cpp_syntax = {
    "C++",
    sizeof cpp_exts / sizeof *cpp_exts, cpp_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof cpp_kwd / sizeof *cpp_kwd, cpp_kwd, //Keywords
    0x60, 0x50, 0x05, 0x30,
    "\"\'", // Strings charaters
    "//", {"/*", "*/"}, // Comments
    {"{[(", "}])"},
    {"0x", "0o", "0b"}
};

/*
Python syntax highlighting descriptor
*/

static struct KWD python_kwd[] = {
    KEYWORD("await", 0x10), KEYWORD("else", 0x10), KEYWORD("import", 0x10),
    KEYWORD("pass", 0x10), KEYWORD("break", 0x10), KEYWORD("except", 0x10),
    KEYWORD("in", 0x10), KEYWORD("raise", 0x10), KEYWORD("class", 0x10),
    KEYWORD("finally", 0x10), KEYWORD("is", 0x10), KEYWORD("return", 0x10),
    KEYWORD("and", 0x10), KEYWORD("continue", 0x10), KEYWORD("for", 0x10),
    KEYWORD("lambda", 0x10), KEYWORD("try", 0x10), KEYWORD("as", 0x10),
    KEYWORD("def", 0x10), KEYWORD("from", 0x10), KEYWORD("nonlocal", 0x10),
    KEYWORD("while", 0x10), KEYWORD("assert", 0x10), KEYWORD("del", 0x10),
    KEYWORD("global", 0x10), KEYWORD("not", 0x10), KEYWORD("with", 0x10),
    KEYWORD("async", 0x10), KEYWORD("elif", 0x10), KEYWORD("if", 0x10),
    KEYWORD("or", 0x10), KEYWORD("yield", 0x10),

    KEYWORD("abs", 0x20), KEYWORD("all", 0x20), KEYWORD("any", 0x20), // builtins
    KEYWORD("ascii", 0x20), KEYWORD("bin", 0x20), KEYWORD("isinstance", 0x20),
    KEYWORD("breakpoint", 0x20), KEYWORD("bytearray", 0x20), KEYWORD("bytes", 0x20),
    KEYWORD("callable", 0x20), KEYWORD("chr", 0x20), KEYWORD("classmethod", 0x20),
    KEYWORD("compile", 0x20), KEYWORD("complex", 0x20), KEYWORD("copyright", 0x20),
    KEYWORD("credits", 0x20), KEYWORD("delattr", 0x20), KEYWORD("dict", 0x20),
    KEYWORD("dir", 0x20), KEYWORD("divmod", 0x20), KEYWORD("enumerate", 0x20),
    KEYWORD("eval", 0x20), KEYWORD("exec", 0x20), KEYWORD("exit", 0x20),
    KEYWORD("filter", 0x20), KEYWORD("format", 0x20), KEYWORD("frozenset", 0x20),
    KEYWORD("getattr", 0x20), KEYWORD("globals", 0x20), KEYWORD("issubclass", 0x20),
    KEYWORD("hasattr", 0x20), KEYWORD("hash", 0x20), KEYWORD("help", 0x20),
    KEYWORD("hex", 0x20), KEYWORD("id", 0x20), KEYWORD("input", 0x20),
    KEYWORD("iter", 0x20), KEYWORD("len", 0x20), KEYWORD("license", 0x20),
    KEYWORD("list", 0x20), KEYWORD("locals", 0x20), KEYWORD("map", 0x20),
    KEYWORD("max", 0x20), KEYWORD("memoryview", 0x20), KEYWORD("min", 0x20),
    KEYWORD("next", 0x20), KEYWORD("object", 0x20), KEYWORD("oct", 0x20),
    KEYWORD("open", 0x20), KEYWORD("ord", 0x20), KEYWORD("pow", 0x20),
    KEYWORD("print", 0x20), KEYWORD("property", 0x20), KEYWORD("quit", 0x20),
    KEYWORD("range", 0x20), KEYWORD("repr", 0x20), KEYWORD("reversed", 0x20),
    KEYWORD("round", 0x20), KEYWORD("set", 0x20), KEYWORD("setattr", 0x20),
    KEYWORD("slice", 0x20), KEYWORD("sorted", 0x20), KEYWORD("staticmethod", 0x20),
    KEYWORD("str", 0x20), KEYWORD("sum", 0x20), KEYWORD("super", 0x20),
    KEYWORD("tuple", 0x20), KEYWORD("type", 0x20), KEYWORD("vars", 0x20),
    KEYWORD("zip", 0x20),

    KEYWORD("False", 0x40), KEYWORD("None", 0x40), KEYWORD("True", 0x40), // builtin types
    KEYWORD("bool", 0x40), KEYWORD("int", 0x40), KEYWORD("float", 0x40),
    KEYWORD("ArithmeticError", 0x40), KEYWORD("AssertionError", 0x40), KEYWORD("AttributeError", 0x40),
    KEYWORD("BaseException", 0x40), KEYWORD("BlockingIOError", 0x40), KEYWORD("BrokenPipeError", 0x40),
    KEYWORD("BufferError", 0x40), KEYWORD("BytesWarning", 0x40), KEYWORD("ChildProcessError", 0x40),
    KEYWORD("ConnectionAbortedError", 0x40), KEYWORD("ConnectionError", 0x40), KEYWORD("ConnectionRefusedError", 0x40),
    KEYWORD("ConnectionResetError", 0x40), KEYWORD("DeprecationWarning", 0x40), KEYWORD("EOFError", 0x40),
    KEYWORD("Ellipsis", 0x40), KEYWORD("EnvironmentError", 0x40), KEYWORD("Exception", 0x40),
    KEYWORD("False", 0x40), KEYWORD("FileExistsError", 0x40), KEYWORD("FileNotFoundError", 0x40),
    KEYWORD("FloatingPointError", 0x40), KEYWORD("FutureWarning", 0x40), KEYWORD("GeneratorExit", 0x40),
    KEYWORD("IOError", 0x40), KEYWORD("ImportError", 0x40), KEYWORD("ImportWarning", 0x40),
    KEYWORD("IndentationError", 0x40), KEYWORD("IndexError", 0x40), KEYWORD("InterruptedError", 0x40),
    KEYWORD("IsADirectoryError", 0x40), KEYWORD("KeyError", 0x40), KEYWORD("KeyboardInterrupt", 0x40),
    KEYWORD("LookupError", 0x40), KEYWORD("MemoryError", 0x40), KEYWORD("ModuleNotFoundError", 0x40),
    KEYWORD("NameError", 0x40), KEYWORD("None", 0x40), KEYWORD("NotADirectoryError", 0x40),
    KEYWORD("NotImplemented", 0x40), KEYWORD("NotImplementedError", 0x40), KEYWORD("OSError", 0x40),
    KEYWORD("OverflowError", 0x40), KEYWORD("PendingDeprecationWarning", 0x40), KEYWORD("PermissionError", 0x40),
    KEYWORD("ProcessLookupError", 0x40), KEYWORD("RecursionError", 0x40), KEYWORD("ReferenceError", 0x40),
    KEYWORD("ResourceWarning", 0x40), KEYWORD("RuntimeError", 0x40), KEYWORD("RuntimeWarning", 0x40),
    KEYWORD("StopAsyncIteration", 0x40), KEYWORD("StopIteration", 0x40), KEYWORD("SyntaxError", 0x40),
    KEYWORD("SyntaxWarning", 0x40), KEYWORD("SystemError", 0x40), KEYWORD("SystemExit", 0x40),
    KEYWORD("TabError", 0x40), KEYWORD("TimeoutError", 0x40), KEYWORD("True", 0x40),
    KEYWORD("TypeError", 0x40), KEYWORD("UnboundLocalError", 0x40), KEYWORD("UnicodeDecodeError", 0x40),
    KEYWORD("UnicodeEncodeError", 0x40), KEYWORD("UnicodeError", 0x40), KEYWORD("UnicodeTranslateError", 0x40),
    KEYWORD("UnicodeWarning", 0x40), KEYWORD("UserWarning", 0x40), KEYWORD("ValueError", 0x40),
    KEYWORD("Warning", 0x40), KEYWORD("ZeroDivisionError", 0x40),

    OPERATOR("*" , 0x30), OPERATOR("," , 0x30), OPERATOR(";" , 0x30),
    OPERATOR("/" , 0x30), OPERATOR("-" , 0x30), OPERATOR("+" , 0x30),
    OPERATOR("%" , 0x30), OPERATOR("^" , 0x30), OPERATOR("&" , 0x30),
    OPERATOR("~" , 0x30), OPERATOR("|" , 0x30), OPERATOR("!" , 0x30),
    OPERATOR("<" , 0x30), OPERATOR(">" , 0x30), OPERATOR("=" , 0x30),
    OPERATOR(":" , 0x30), OPERATOR("\\" , 0x30), OPERATOR("." , 0x30),

    OPERATOR("(", 0x50), OPERATOR(")", 0x50),
    OPERATOR("{", 0x50), OPERATOR("}", 0x50),
    OPERATOR("[", 0x50), OPERATOR("]", 0x50)
};

static const char *python_exts[] = {"py", "py3", "pyx", "pyw", "pyd", "pyde"};

static struct SHD python_syntax = {
    "Python",
    sizeof python_exts / sizeof *python_exts, python_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/", // Characters that separates words
    sizeof python_kwd / sizeof *python_kwd, python_kwd, //Keywords
    0x40, 0x50, 0x05, 0x30,
    "\"\'`", // Strings charaters
    "#", {"", ""}, // Comments
    {"{[(", "}])"},
    {"0x", "0o", "0b"}
};

/*
Shell syntax highlighting descriptor
*/

static struct KWD sh_kwd[] = {
    KEYWORD("if", 0x10), KEYWORD("then", 0x10), KEYWORD("else", 0x10),
    KEYWORD("fi", 0x10), KEYWORD("while", 0x10), KEYWORD("for", 0x10),
    KEYWORD("do", 0x10), KEYWORD("done", 0x10), KEYWORD("break", 0x10),
    KEYWORD("continue", 0x10), KEYWORD("return", 0x10), KEYWORD("in", 0x10),
    KEYWORD("case", 0x10), KEYWORD("esac", 0x10), KEYWORD("until", 0x10),
    KEYWORD("function", 0x10), KEYWORD("elif", 0x10),

    KEYWORD("$?", 0x40), KEYWORD("~", 0x40), KEYWORD(".", 0x40), // special

    KEYWORD("alias", 0x20), KEYWORD("bg", 0x20), KEYWORD("bind", 0x20), //builtins
    KEYWORD("builtin", 0x20), KEYWORD("cd", 0x20), KEYWORD("command", 0x20),
    KEYWORD("continue", 0x20), KEYWORD("declare", 0x20), KEYWORD("dirs", 0x20),
    KEYWORD("disown", 0x20), KEYWORD("echo", 0x20), KEYWORD("enable", 0x20),
    KEYWORD("eval", 0x20), KEYWORD("exec", 0x20), KEYWORD("exit", 0x20),
    KEYWORD("export", 0x20), KEYWORD("false", 0x20), KEYWORD("fc", 0x20),
    KEYWORD("fg", 0x20), KEYWORD("getopts", 0x20), KEYWORD("hash", 0x20),
    KEYWORD("help", 0x20), KEYWORD("history", 0x20), KEYWORD("jobs", 0x20),
    KEYWORD("kill", 0x20), KEYWORD("let", 0x20), KEYWORD("local", 0x20),
    KEYWORD("logout", 0x20), KEYWORD("popd", 0x20), KEYWORD("pushd", 0x20),
    KEYWORD("printf", 0x20), KEYWORD("pwd", 0x20), KEYWORD("read", 0x20),
    KEYWORD("readarray", 0x20), KEYWORD("readonly", 0x20), KEYWORD("set", 0x20),
    KEYWORD("shift", 0x20), KEYWORD("source", 0x20), KEYWORD("suspend", 0x20),
    KEYWORD("test", 0x20), KEYWORD("times", 0x20), KEYWORD("time", 0x20),
    KEYWORD("trap", 0x20), KEYWORD("true", 0x20), KEYWORD("type", 0x20),
    KEYWORD("ulimit", 0x20), KEYWORD("umask", 0x20), KEYWORD("unalias", 0x20),
    KEYWORD("unset", 0x20), KEYWORD("wait", 0x20), KEYWORD("shopt", 0x20),

    OPERATOR("$", 0x30), OPERATOR(";", 0x30), OPERATOR("=", 0x30),
    OPERATOR("<<-", 0x30), OPERATOR("|", 0x30), OPERATOR("&", 0x30),
    OPERATOR("<", 0x30), OPERATOR(">", 0x30), OPERATOR("2>", 0x30),
    OPERATOR("!", 0x30), OPERATOR("+", 0x30), OPERATOR("@", 0x30),
    OPERATOR("*", 0x30), OPERATOR("-", 0x30), OPERATOR("?", 0x30),

    OPERATOR("(", 0x50), OPERATOR(")", 0x50),
    OPERATOR("{", 0x50), OPERATOR("}", 0x50),
    OPERATOR("[", 0x50), OPERATOR("]", 0x50),
};

static const char *sh_exts[] = {"sh", "zsh"};

static struct SHD sh_syntax = {
    "Shell",
    sizeof sh_exts / sizeof *sh_exts, sh_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof sh_kwd / sizeof *sh_kwd, sh_kwd, //Keywords
    0x40, 0x50, 0x05, 0x30,
    "\"\'`", // Strings charaters
    "#", {"", ""}, // Comments
    {"{[(", "}])"},
    {"", "", ""}
};

/*
Default syntax
*/

struct SHD default_syntax = {
    "Default",
    0, NULL,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?",
    0, NULL,
    0, 0, 0, 0,
    "",
    "", {"", ""},
    {"", ""},
    {"", "", ""}
};

/*
Global syntaxes
*/

struct SHD *syntaxes[] = {&c_syntax, &cpp_syntax, &python_syntax, &sh_syntax};

void register_syntax(void) {
    config.syntaxes = syntaxes;
    config.syntax_len = sizeof syntaxes / sizeof *syntaxes;
}
