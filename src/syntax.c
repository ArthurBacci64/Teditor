#include "syntax.h"

// utility macros for the recurring PALETTE_COLOR scheme
#define KEYWORD_COLOR   PALETTE_COLOR(PALETTE_RED, PALETTE_OFF)
#define OPERATOR_COLOR  PALETTE_COLOR(PALETTE_GREEN, PALETTE_OFF)
#define TYPES_COLOR     PALETTE_COLOR(PALETTE_MAGENTA, PALETTE_OFF)
#define MACRO_COLOR     PALETTE_COLOR(PALETTE_MAGENTA, PALETTE_OFF)
#define PAREN_COLOR     PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF)
#define LITERAL_COLOR   PALETTE_COLOR(PALETTE_YELLOW, PALETTE_OFF)
#define STDLIB_COLOR    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF)

// U/u: unsigned L/l: long F/f: float (for fp numbers) .: trailing dot (for fp numbers)
static const char c_cpp_number_suffixes[] = "UuLlFf.";
static const struct MATCH c_cpp_number_strmatch[] = {
    STRMATCH("%E"), STRMATCH("%c"), STRMATCH("%lf"), STRMATCH("%x"), STRMATCH("%g"),
    STRMATCH("%%"), STRMATCH("%s"), STRMATCH("%u"), STRMATCH("%ld"), STRMATCH("%o"),
    STRMATCH("%lu"), STRMATCH("%G"), STRMATCH("%e"), STRMATCH("%hi"), STRMATCH("%llu"),
    STRMATCH("%hu"), STRMATCH("%lld"), STRMATCH("%X"), STRMATCH("%n"), STRMATCH("%Lf"),
    STRMATCH("%lli"), STRMATCH("%li"), STRMATCH("%i"), STRMATCH("%d"), STRMATCH("%f"),
    STRMATCH("%p"), STRMATCH("%l"), STRMATCH("\\a"), STRMATCH("\\0"), STRMATCH("\\b"),
    STRMATCH("\\f"), STRMATCH("\\n"), STRMATCH("\\r"), STRMATCH("\\t"), STRMATCH("\\v"),
    STRMATCH("\\\\"), STRMATCH("\\'"), STRMATCH("\\\""), STRMATCH("\\?"), STRMATCH("\\x"),
    STRMATCH("\\u"), STRMATCH("\\U"),
};

/*
C syntax highlighting descriptor
*/

static struct KWD c_kwd[] = {
    KEYWORD("break", KEYWORD_COLOR), KEYWORD("case", KEYWORD_COLOR), KEYWORD("default", KEYWORD_COLOR), // ansi c
    KEYWORD("continue", KEYWORD_COLOR), KEYWORD("while", KEYWORD_COLOR), KEYWORD("enum", KEYWORD_COLOR),
    KEYWORD("do", KEYWORD_COLOR), KEYWORD("else", KEYWORD_COLOR), KEYWORD("extern", KEYWORD_COLOR),
    KEYWORD("for", KEYWORD_COLOR), KEYWORD("goto", KEYWORD_COLOR), KEYWORD("if", KEYWORD_COLOR),
    KEYWORD("return", KEYWORD_COLOR), KEYWORD("sizeof", KEYWORD_COLOR), KEYWORD("struct", KEYWORD_COLOR),
    KEYWORD("switch", KEYWORD_COLOR), KEYWORD("typedef", KEYWORD_COLOR), KEYWORD("union", KEYWORD_COLOR),
    KEYWORD("asm", KEYWORD_COLOR), KEYWORD("fortran", KEYWORD_COLOR),

    KEYWORD("char", TYPES_COLOR), KEYWORD("double", TYPES_COLOR), KEYWORD("long", TYPES_COLOR), // primitives
    KEYWORD("short", TYPES_COLOR), KEYWORD("int", TYPES_COLOR), KEYWORD("void", TYPES_COLOR),
    KEYWORD("float", TYPES_COLOR), KEYWORD("auto", TYPES_COLOR), // auto is c11
    
    KEYWORD("const", TYPES_COLOR), KEYWORD("signed", TYPES_COLOR), KEYWORD("unsigned", TYPES_COLOR), //type modifiers
    KEYWORD("inline", TYPES_COLOR), KEYWORD("volatile", TYPES_COLOR), KEYWORD("static", TYPES_COLOR),
    KEYWORD("restrict", TYPES_COLOR), KEYWORD("register", TYPES_COLOR),

    KEYWORD("_Alignas", TYPES_COLOR), KEYWORD("_Alignof", TYPES_COLOR), // c11 and c99
    KEYWORD("_Atomic", TYPES_COLOR), KEYWORD("_Bool", TYPES_COLOR), KEYWORD("_Complex", TYPES_COLOR),
    KEYWORD("_Decimal128", TYPES_COLOR), KEYWORD("_Decimal32", TYPES_COLOR), KEYWORD("_Decimal64", TYPES_COLOR),
    KEYWORD("_Generic", TYPES_COLOR), KEYWORD("_Imaginary", TYPES_COLOR), KEYWORD("_Noreturn", TYPES_COLOR),
    KEYWORD("_Static_assert", TYPES_COLOR), KEYWORD("_Thread_local", TYPES_COLOR),

    KEYWORD("bool", MACRO_COLOR), KEYWORD("alignas", MACRO_COLOR), KEYWORD("alignof", MACRO_COLOR), // stdlib macros/typedefs
    KEYWORD("complex", MACRO_COLOR), KEYWORD("imaginary", MACRO_COLOR), KEYWORD("thread_local", MACRO_COLOR),
    KEYWORD("static_assert", MACRO_COLOR), KEYWORD("assert", MACRO_COLOR), KEYWORD("noreturn", MACRO_COLOR),
    KEYWORD("va_list", TYPES_COLOR), KEYWORD("uintmax_t", TYPES_COLOR), KEYWORD("intmax_t", TYPES_COLOR),
    KEYWORD("size_t", TYPES_COLOR), KEYWORD("wchar_t", TYPES_COLOR), KEYWORD("ptrdiff_t", TYPES_COLOR),
    KEYWORD("int8_t", TYPES_COLOR), KEYWORD("int16_t", TYPES_COLOR), KEYWORD("int32_t", TYPES_COLOR),
    KEYWORD("int64_t", TYPES_COLOR), KEYWORD("uint8_t", TYPES_COLOR), KEYWORD("uint16_t", TYPES_COLOR),
    KEYWORD("uint32_t", TYPES_COLOR), KEYWORD("uint64_t", TYPES_COLOR), KEYWORD("intptr_t", TYPES_COLOR),
    KEYWORD("uintptr_t", TYPES_COLOR), KEYWORD("fexcept_t", TYPES_COLOR), KEYWORD("fenv_t", TYPES_COLOR),
    KEYWORD("time_t", TYPES_COLOR), KEYWORD("clock_t", TYPES_COLOR), KEYWORD("imaxdiv_t", TYPES_COLOR),
    KEYWORD("ldiv_t", TYPES_COLOR), KEYWORD("lldiv_t", TYPES_COLOR), KEYWORD("div_t", TYPES_COLOR),
    KEYWORD("float_t", TYPES_COLOR), KEYWORD("double_t", TYPES_COLOR), KEYWORD("sig_atomic_t", TYPES_COLOR),
    KEYWORD("memory_order", TYPES_COLOR), KEYWORD("atomic_flag", TYPES_COLOR), KEYWORD("thrd_t", TYPES_COLOR),
    KEYWORD("mtx_t", TYPES_COLOR), KEYWORD("thrd_start_t", TYPES_COLOR), KEYWORD("cnd_t", TYPES_COLOR),
    KEYWORD("tss_t", TYPES_COLOR), KEYWORD("tss_dtor_t", TYPES_COLOR), KEYWORD("mbstate_t", TYPES_COLOR),
    KEYWORD("char16_t", TYPES_COLOR), KEYWORD("char32_t", TYPES_COLOR), KEYWORD("wint_t", TYPES_COLOR),
    KEYWORD("wctrans_t", TYPES_COLOR), KEYWORD("wctype_t", TYPES_COLOR),

    KEYWORD("false", STDLIB_COLOR), KEYWORD("true", STDLIB_COLOR), KEYWORD("NULL", STDLIB_COLOR), // stdlib constants/variables
    KEYWORD("stdin", STDLIB_COLOR), KEYWORD("stdout", STDLIB_COLOR), KEYWORD("stderr", STDLIB_COLOR),
    KEYWORD("errno", STDLIB_COLOR), KEYWORD("_Imaginary_I", STDLIB_COLOR), KEYWORD("_Complex_I", STDLIB_COLOR),

    KEYWORD("defined", KEYWORD_COLOR), KEYWORD("define", KEYWORD_COLOR), KEYWORD("undef", KEYWORD_COLOR), // preprocessor
    KEYWORD("ifdef", KEYWORD_COLOR), KEYWORD("ifndef", KEYWORD_COLOR), KEYWORD("elif", KEYWORD_COLOR),
    KEYWORD("endif", KEYWORD_COLOR), KEYWORD("#line", KEYWORD_COLOR), KEYWORD("#error", KEYWORD_COLOR),
    KEYWORD("#warning", KEYWORD_COLOR), KEYWORD("#pragma", KEYWORD_COLOR), KEYWORD("_Pragma", KEYWORD_COLOR),
    KEYWORD("include", KEYWORD_COLOR), OPERATOR("#", KEYWORD_COLOR),

    KEYWORD("<assert.h>", LITERAL_COLOR), KEYWORD("<complex.h>", LITERAL_COLOR), KEYWORD("<ctype.h>", LITERAL_COLOR), // stdlib headers
    KEYWORD("<errno.h>", LITERAL_COLOR), KEYWORD("<fenv.h>", LITERAL_COLOR), KEYWORD("<float.h>", LITERAL_COLOR),
    KEYWORD("<inttypes.h>", LITERAL_COLOR), KEYWORD("<iso646.h>", LITERAL_COLOR), KEYWORD("<limits.h>", LITERAL_COLOR),
    KEYWORD("<locale.h>", LITERAL_COLOR), KEYWORD("<math.h>", LITERAL_COLOR), KEYWORD("<setjmp.h>", LITERAL_COLOR),
    KEYWORD("<signal.h>", LITERAL_COLOR), KEYWORD("<stdalign.h>", LITERAL_COLOR), KEYWORD("<stdarg.h>", LITERAL_COLOR),
    KEYWORD("<stdatomic.h>", LITERAL_COLOR), KEYWORD("<stdbool.h>", LITERAL_COLOR), KEYWORD("<stddef.h>", LITERAL_COLOR),
    KEYWORD("<stdint.h>", LITERAL_COLOR), KEYWORD("<stdio.h>", LITERAL_COLOR), KEYWORD("<stdlib.h>", LITERAL_COLOR),
    KEYWORD("<stdnoreturn.h>", LITERAL_COLOR), KEYWORD("<string.h>", LITERAL_COLOR), KEYWORD("<tgmath.h>", LITERAL_COLOR),
    KEYWORD("<threads.h>", LITERAL_COLOR), KEYWORD("<time.h>", LITERAL_COLOR), KEYWORD("<uchar.h>", LITERAL_COLOR),
    KEYWORD("<wchar.h>", LITERAL_COLOR), KEYWORD("<wctype.h>", LITERAL_COLOR),

    OPERATOR("*", OPERATOR_COLOR), OPERATOR(",", OPERATOR_COLOR), OPERATOR(";", OPERATOR_COLOR),
    OPERATOR("/", OPERATOR_COLOR), OPERATOR("-", OPERATOR_COLOR), OPERATOR("+", OPERATOR_COLOR),
    OPERATOR("%", OPERATOR_COLOR), OPERATOR("^", OPERATOR_COLOR), OPERATOR("&", OPERATOR_COLOR),
    OPERATOR("~", OPERATOR_COLOR), OPERATOR("|", OPERATOR_COLOR), OPERATOR("!", OPERATOR_COLOR),
    OPERATOR("<", OPERATOR_COLOR), OPERATOR(">", OPERATOR_COLOR), OPERATOR("=", OPERATOR_COLOR),
    OPERATOR("?", OPERATOR_COLOR), OPERATOR(":", OPERATOR_COLOR), OPERATOR(".", OPERATOR_COLOR),
    
    OPERATOR("(", PAREN_COLOR), OPERATOR(")", PAREN_COLOR),
    OPERATOR("{", PAREN_COLOR), OPERATOR("}", PAREN_COLOR),
    OPERATOR("[", PAREN_COLOR), OPERATOR("]", PAREN_COLOR),
};

static const char *c_exts[] = {"c", "h"};

static struct SHD c_syntax = {
    "C", 0,
    sizeof c_exts / sizeof *c_exts, c_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof c_kwd / sizeof *c_kwd, c_kwd, //Keywords
    LITERAL_COLOR, PALETTE_COLOR(PALETTE_BRIGHT_RED, PALETTE_OFF),
    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF), PALETTE_COLOR(PALETTE_OFF, PALETTE_CYAN),
    LITERAL_COLOR, LITERAL_COLOR, TYPES_COLOR,
    "\"\'", sizeof c_cpp_number_strmatch / sizeof *c_cpp_number_strmatch, c_cpp_number_strmatch,// Strings charaters
    STRMATCH("//"), {STRMATCH("/*"), STRMATCH("*/")}, // Comments
    {"{[(", "}])"},
    {STRMATCH("0x"), STRMATCH("0"), STRMATCH("")},
    c_cpp_number_suffixes,
    {"0123456789aAbBcCdDeEfF", "01234567", "01", "0123456789"}
};

/*
C++ syntax highlighting descriptor
*/

static struct KWD cpp_kwd[] = {
    KEYWORD("alignas", KEYWORD_COLOR), KEYWORD("alignof", KEYWORD_COLOR), KEYWORD("and", KEYWORD_COLOR),
    KEYWORD("and_eq", KEYWORD_COLOR), KEYWORD("asm", KEYWORD_COLOR), KEYWORD("catch", KEYWORD_COLOR),
    KEYWORD("bitand", KEYWORD_COLOR), KEYWORD("bitor", KEYWORD_COLOR), KEYWORD("compl", KEYWORD_COLOR),
    KEYWORD("break", KEYWORD_COLOR), KEYWORD("case", KEYWORD_COLOR), KEYWORD("class", KEYWORD_COLOR),
    KEYWORD("concept", KEYWORD_COLOR), KEYWORD("const_cast", KEYWORD_COLOR), KEYWORD("continue", KEYWORD_COLOR),
    KEYWORD("co_await", KEYWORD_COLOR), KEYWORD("co_return", KEYWORD_COLOR), KEYWORD("co_yield", KEYWORD_COLOR),
    KEYWORD("decltype", KEYWORD_COLOR), KEYWORD("default", KEYWORD_COLOR), KEYWORD("delete", KEYWORD_COLOR),
    KEYWORD("do", KEYWORD_COLOR), KEYWORD("else", KEYWORD_COLOR), KEYWORD("enum", KEYWORD_COLOR),
    KEYWORD("dynamic_cast", KEYWORD_COLOR), KEYWORD("goto", KEYWORD_COLOR), KEYWORD("if", KEYWORD_COLOR),
    KEYWORD("explicit", KEYWORD_COLOR), KEYWORD("export", KEYWORD_COLOR), KEYWORD("extern", KEYWORD_COLOR),
    KEYWORD("for", KEYWORD_COLOR), KEYWORD("namespace", KEYWORD_COLOR), KEYWORD("new", KEYWORD_COLOR),
    KEYWORD("noexcept", KEYWORD_COLOR), KEYWORD("not", KEYWORD_COLOR), KEYWORD("not_eq", KEYWORD_COLOR),
    KEYWORD("operator", KEYWORD_COLOR), KEYWORD("or", KEYWORD_COLOR), KEYWORD("sizeof", KEYWORD_COLOR),
    KEYWORD("or_eq", KEYWORD_COLOR), KEYWORD("reinterpret_cast", KEYWORD_COLOR), KEYWORD("requires", KEYWORD_COLOR),
    KEYWORD("return", KEYWORD_COLOR), KEYWORD("static_assert", KEYWORD_COLOR), KEYWORD("static_cast", KEYWORD_COLOR),
    KEYWORD("struct", KEYWORD_COLOR), KEYWORD("switch", KEYWORD_COLOR), KEYWORD("synchronized", KEYWORD_COLOR),
    KEYWORD("template", KEYWORD_COLOR), KEYWORD("throw", KEYWORD_COLOR), KEYWORD("try", KEYWORD_COLOR),
    KEYWORD("typedef", KEYWORD_COLOR), KEYWORD("typeid", KEYWORD_COLOR), KEYWORD("typename", KEYWORD_COLOR),
    KEYWORD("union", KEYWORD_COLOR), KEYWORD("using", KEYWORD_COLOR), KEYWORD("while", KEYWORD_COLOR),
    KEYWORD("xor_eq", KEYWORD_COLOR), KEYWORD("transaction_safe_dynamic", KEYWORD_COLOR), KEYWORD("final", KEYWORD_COLOR),
    KEYWORD("override", KEYWORD_COLOR), KEYWORD("transaction_safe", KEYWORD_COLOR), KEYWORD("xor", KEYWORD_COLOR),
    KEYWORD("this", KEYWORD_COLOR),

    KEYWORD("signed", TYPES_COLOR), KEYWORD("unsigned", TYPES_COLOR), KEYWORD("virtual", TYPES_COLOR), // type modifiers
    KEYWORD("volatile", TYPES_COLOR), KEYWORD("friend", TYPES_COLOR), KEYWORD("inline", TYPES_COLOR),
    KEYWORD("mutable", TYPES_COLOR), KEYWORD("thread_local", TYPES_COLOR), KEYWORD("atomic_cancel", TYPES_COLOR),
    KEYWORD("atomic_commit", TYPES_COLOR), KEYWORD("atomic_noexcept", TYPES_COLOR), KEYWORD("constinit", TYPES_COLOR),
    KEYWORD("const", TYPES_COLOR), KEYWORD("consteval", TYPES_COLOR), KEYWORD("constexpr", TYPES_COLOR),
    KEYWORD("private", TYPES_COLOR), KEYWORD("protected", TYPES_COLOR), KEYWORD("public", TYPES_COLOR),
    KEYWORD("reflexpr", TYPES_COLOR), KEYWORD("register", TYPES_COLOR), KEYWORD("static", TYPES_COLOR),

    KEYWORD("char", TYPES_COLOR), KEYWORD("char8_t", TYPES_COLOR), KEYWORD("char16_t", TYPES_COLOR), // primitives
    KEYWORD("char32_t", TYPES_COLOR), KEYWORD("int", TYPES_COLOR), KEYWORD("long", TYPES_COLOR),
    KEYWORD("short", TYPES_COLOR), KEYWORD("wchar_t", TYPES_COLOR), KEYWORD("bool", TYPES_COLOR),
    KEYWORD("void", TYPES_COLOR), KEYWORD("double", TYPES_COLOR), KEYWORD("auto", TYPES_COLOR),
    KEYWORD("float", TYPES_COLOR),
    
    KEYWORD("false", LITERAL_COLOR), KEYWORD("true", LITERAL_COLOR), KEYWORD("nullptr", LITERAL_COLOR), // literals

    KEYWORD("defined", KEYWORD_COLOR), KEYWORD("define", KEYWORD_COLOR), KEYWORD("undef", KEYWORD_COLOR), // preprocessor
    KEYWORD("ifdef", KEYWORD_COLOR), KEYWORD("ifndef", KEYWORD_COLOR), KEYWORD("elif", KEYWORD_COLOR),
    KEYWORD("endif", KEYWORD_COLOR), KEYWORD("#line", KEYWORD_COLOR), KEYWORD("#error", KEYWORD_COLOR),
    KEYWORD("#warning", KEYWORD_COLOR), KEYWORD("#pragma", KEYWORD_COLOR), KEYWORD("_Pragma", KEYWORD_COLOR),
    KEYWORD("include", KEYWORD_COLOR), OPERATOR("#", KEYWORD_COLOR),
    KEYWORD("export", KEYWORD_COLOR), KEYWORD("import", KEYWORD_COLOR), KEYWORD("module", KEYWORD_COLOR), // c++20 modules

    KEYWORD("<concepts>", LITERAL_COLOR), KEYWORD("<coroutine>", LITERAL_COLOR), KEYWORD("<cstdlib>", LITERAL_COLOR), // stdlib headers
    KEYWORD("<csignal>", LITERAL_COLOR), KEYWORD("<csetjmp>", LITERAL_COLOR), KEYWORD("<cstdarg>", LITERAL_COLOR),
    KEYWORD("<typeinfo>", LITERAL_COLOR), KEYWORD("<typeindex>", LITERAL_COLOR), KEYWORD("<type_traits>", LITERAL_COLOR),
    KEYWORD("<bitset>", LITERAL_COLOR), KEYWORD("<functional>", LITERAL_COLOR), KEYWORD("<utility>", LITERAL_COLOR),
    KEYWORD("<ctime>", LITERAL_COLOR), KEYWORD("<chrono>", LITERAL_COLOR), KEYWORD("<cstddef>", LITERAL_COLOR),
    KEYWORD("<initializer_list>", LITERAL_COLOR), KEYWORD("<tuple>", LITERAL_COLOR), KEYWORD("<any>", LITERAL_COLOR),
    KEYWORD("<optional>", LITERAL_COLOR), KEYWORD("<variant>", LITERAL_COLOR), KEYWORD("<compare>", LITERAL_COLOR),
    KEYWORD("<version>", LITERAL_COLOR), KEYWORD("<source_location>", LITERAL_COLOR), KEYWORD("<new>", LITERAL_COLOR),
    KEYWORD("<memory>", LITERAL_COLOR), KEYWORD("<scoped_allocator>", LITERAL_COLOR), KEYWORD("<memory_resource>", LITERAL_COLOR),
    KEYWORD("<climits>", LITERAL_COLOR), KEYWORD("<cfloat>", LITERAL_COLOR), KEYWORD("<cstdint>", LITERAL_COLOR),
    KEYWORD("<cinttypes>", LITERAL_COLOR), KEYWORD("<limits>", LITERAL_COLOR), KEYWORD("<exception>", LITERAL_COLOR),
    KEYWORD("<stdexcept>", LITERAL_COLOR), KEYWORD("<cassert>", LITERAL_COLOR), KEYWORD("<system_error>", LITERAL_COLOR),
    KEYWORD("<cerrno>", LITERAL_COLOR), KEYWORD("<cctype>", LITERAL_COLOR), KEYWORD("<cwctype>", LITERAL_COLOR),
    KEYWORD("<cstring>", LITERAL_COLOR), KEYWORD("<cwchar>", LITERAL_COLOR), KEYWORD("<cuchar>", LITERAL_COLOR),
    KEYWORD("<string>", LITERAL_COLOR), KEYWORD("<string_view>", LITERAL_COLOR), KEYWORD("<charconv>", LITERAL_COLOR),
    KEYWORD("<format>", LITERAL_COLOR), KEYWORD("<array>", LITERAL_COLOR), KEYWORD("<vector>", LITERAL_COLOR),
    KEYWORD("<deque>", LITERAL_COLOR), KEYWORD("<list>", LITERAL_COLOR), KEYWORD("<forward_list>", LITERAL_COLOR),
    KEYWORD("<set>", LITERAL_COLOR), KEYWORD("<map>", LITERAL_COLOR), KEYWORD("<unordered_set>", LITERAL_COLOR),
    KEYWORD("<unordered_map>", LITERAL_COLOR), KEYWORD("<stack>", LITERAL_COLOR), KEYWORD("<queue>", LITERAL_COLOR),
    KEYWORD("<span>", LITERAL_COLOR), KEYWORD("<iterator>", LITERAL_COLOR), KEYWORD("<ranges>", LITERAL_COLOR),
    KEYWORD("<algorithm>", LITERAL_COLOR), KEYWORD("<execution>", LITERAL_COLOR), KEYWORD("<cmath>", LITERAL_COLOR),
    KEYWORD("<complex>", LITERAL_COLOR), KEYWORD("<valarray>", LITERAL_COLOR), KEYWORD("<random>", LITERAL_COLOR),
    KEYWORD("<numeric>", LITERAL_COLOR), KEYWORD("<ratio>", LITERAL_COLOR), KEYWORD("<cfenv>", LITERAL_COLOR),
    KEYWORD("<bit>", LITERAL_COLOR), KEYWORD("<numbers>", LITERAL_COLOR), KEYWORD("<locale>", LITERAL_COLOR),
    KEYWORD("<clocale>", LITERAL_COLOR), KEYWORD("<codecvt>", LITERAL_COLOR), KEYWORD("<iosfwd>", LITERAL_COLOR),
    KEYWORD("<ios>", LITERAL_COLOR), KEYWORD("<istream>", LITERAL_COLOR), KEYWORD("<ostream>", LITERAL_COLOR),
    KEYWORD("<iostream>", LITERAL_COLOR), KEYWORD("<fstream>", LITERAL_COLOR), KEYWORD("<sstream>", LITERAL_COLOR),
    KEYWORD("<syncstream>", LITERAL_COLOR), KEYWORD("<strstream>", LITERAL_COLOR), KEYWORD("<iomanip>", LITERAL_COLOR),
    KEYWORD("<streambuf>", LITERAL_COLOR), KEYWORD("<cstdio>", LITERAL_COLOR), KEYWORD("<filesystem>", LITERAL_COLOR),
    KEYWORD("<regex>", LITERAL_COLOR), KEYWORD("<atomic>", LITERAL_COLOR), KEYWORD("<thread>", LITERAL_COLOR),
    KEYWORD("<stop_token>", LITERAL_COLOR), KEYWORD("<mutex>", LITERAL_COLOR), KEYWORD("<shared_mutex>", LITERAL_COLOR),
    KEYWORD("<future>", LITERAL_COLOR), KEYWORD("<condition_variable>", LITERAL_COLOR), KEYWORD("<semaphore>", LITERAL_COLOR),
    KEYWORD("<latch>", LITERAL_COLOR), KEYWORD("<barrier>", LITERAL_COLOR), KEYWORD("<cstdlib>", LITERAL_COLOR),
    KEYWORD("<stdlib.h>", LITERAL_COLOR), KEYWORD("<assert.h>", LITERAL_COLOR), KEYWORD("<cassert>", LITERAL_COLOR),
    KEYWORD("<ctype.h>", LITERAL_COLOR), KEYWORD("<cctype>", LITERAL_COLOR), KEYWORD("<errno.h>", LITERAL_COLOR),
    KEYWORD("<cerrno>", LITERAL_COLOR), KEYWORD("<fenv.h>", LITERAL_COLOR), KEYWORD("<cfenv>", LITERAL_COLOR),
    KEYWORD("<float.h>", LITERAL_COLOR), KEYWORD("<cfloat>", LITERAL_COLOR), KEYWORD("<inttypes.h>", LITERAL_COLOR),
    KEYWORD("<cinttypes>", LITERAL_COLOR), KEYWORD("<limits.h>", LITERAL_COLOR), KEYWORD("<climits>", LITERAL_COLOR),
    KEYWORD("<locale.h>", LITERAL_COLOR), KEYWORD("<clocale>", LITERAL_COLOR), KEYWORD("<math.h>", LITERAL_COLOR),
    KEYWORD("<cmath>", LITERAL_COLOR), KEYWORD("<setjmp.h>", LITERAL_COLOR), KEYWORD("<csetjmp>", LITERAL_COLOR),
    KEYWORD("<signal.h>", LITERAL_COLOR), KEYWORD("<csignal>", LITERAL_COLOR), KEYWORD("<stdarg.h>", LITERAL_COLOR),
    KEYWORD("<cstdarg>", LITERAL_COLOR), KEYWORD("<stddef.h>", LITERAL_COLOR), KEYWORD("<cstddef>", LITERAL_COLOR),
    KEYWORD("<stdint.h>", LITERAL_COLOR), KEYWORD("<cstdint>", LITERAL_COLOR), KEYWORD("<stdio.h>", LITERAL_COLOR),
    KEYWORD("<cstdio>", LITERAL_COLOR), KEYWORD("<stdlib.h>", LITERAL_COLOR), KEYWORD("<cstdlib>", LITERAL_COLOR),
    KEYWORD("<string.h>", LITERAL_COLOR), KEYWORD("<cstring>", LITERAL_COLOR), KEYWORD("<time.h>", LITERAL_COLOR),
    KEYWORD("<ctime>", LITERAL_COLOR), KEYWORD("<uchar.h>", LITERAL_COLOR), KEYWORD("<cuchar>", LITERAL_COLOR),
    KEYWORD("<wchar.h>", LITERAL_COLOR), KEYWORD("<cwchar>", LITERAL_COLOR), KEYWORD("<wctype.h>", LITERAL_COLOR),
    KEYWORD("<cwctype>", LITERAL_COLOR), KEYWORD("<stdatomic.h>", LITERAL_COLOR), KEYWORD("<cstdatomic>", LITERAL_COLOR),
    KEYWORD("std", STDLIB_COLOR), // stdlib namespace

    OPERATOR("*", OPERATOR_COLOR), OPERATOR(",", OPERATOR_COLOR), OPERATOR(";", OPERATOR_COLOR),
    OPERATOR("/", OPERATOR_COLOR), OPERATOR("-", OPERATOR_COLOR), OPERATOR("+", OPERATOR_COLOR),
    OPERATOR("%", OPERATOR_COLOR), OPERATOR("^", OPERATOR_COLOR), OPERATOR("&", OPERATOR_COLOR),
    OPERATOR("~", OPERATOR_COLOR), OPERATOR("|", OPERATOR_COLOR), OPERATOR("!", OPERATOR_COLOR),
    OPERATOR("<", OPERATOR_COLOR), OPERATOR(">", OPERATOR_COLOR), OPERATOR("=", OPERATOR_COLOR),
    OPERATOR("?", OPERATOR_COLOR), OPERATOR(":", OPERATOR_COLOR), OPERATOR(".", OPERATOR_COLOR),
    
    OPERATOR("(", PAREN_COLOR), OPERATOR(")", PAREN_COLOR),
    OPERATOR("{", PAREN_COLOR), OPERATOR("}", PAREN_COLOR),
    OPERATOR("[", PAREN_COLOR), OPERATOR("]", PAREN_COLOR),
};

static const char *cpp_exts[] = {"cpp", "hpp", "cc", "hh"};

static struct SHD cpp_syntax = {
    "C++", 0,
    sizeof cpp_exts / sizeof *cpp_exts, cpp_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof cpp_kwd / sizeof *cpp_kwd, cpp_kwd, //Keywords
    LITERAL_COLOR, PALETTE_COLOR(PALETTE_BRIGHT_RED, PALETTE_OFF),
    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF), PALETTE_COLOR(PALETTE_OFF, PALETTE_CYAN),
    LITERAL_COLOR, LITERAL_COLOR, TYPES_COLOR,
    "\"\'", sizeof c_cpp_number_strmatch / sizeof *c_cpp_number_strmatch, c_cpp_number_strmatch,// Strings charaters
    STRMATCH("//"), {STRMATCH("/*"), STRMATCH("*/")}, // Comments
    {"{[(", "}])"},
    {STRMATCH("0x"), STRMATCH("0"), STRMATCH("0b")},
    c_cpp_number_suffixes,
    {"0123456789aAbBcCdDeEfF'", "01234567'", "01'", "0123456789'"} // ' is a digit divisor
};

/*
Python syntax highlighting descriptor
*/

static struct KWD python_kwd[] = {
    KEYWORD("await", KEYWORD_COLOR), KEYWORD("else", KEYWORD_COLOR), KEYWORD("import", KEYWORD_COLOR),
    KEYWORD("pass", KEYWORD_COLOR), KEYWORD("break", KEYWORD_COLOR), KEYWORD("except", KEYWORD_COLOR),
    KEYWORD("in", KEYWORD_COLOR), KEYWORD("raise", KEYWORD_COLOR), KEYWORD("class", KEYWORD_COLOR),
    KEYWORD("finally", KEYWORD_COLOR), KEYWORD("is", KEYWORD_COLOR), KEYWORD("return", KEYWORD_COLOR),
    KEYWORD("and", KEYWORD_COLOR), KEYWORD("continue", KEYWORD_COLOR), KEYWORD("for", KEYWORD_COLOR),
    KEYWORD("lambda", KEYWORD_COLOR), KEYWORD("try", KEYWORD_COLOR), KEYWORD("as", KEYWORD_COLOR),
    KEYWORD("def", KEYWORD_COLOR), KEYWORD("from", KEYWORD_COLOR), KEYWORD("nonlocal", KEYWORD_COLOR),
    KEYWORD("while", KEYWORD_COLOR), KEYWORD("assert", KEYWORD_COLOR), KEYWORD("del", KEYWORD_COLOR),
    KEYWORD("global", KEYWORD_COLOR), KEYWORD("not", KEYWORD_COLOR), KEYWORD("with", KEYWORD_COLOR),
    KEYWORD("async", KEYWORD_COLOR), KEYWORD("elif", KEYWORD_COLOR), KEYWORD("if", KEYWORD_COLOR),
    KEYWORD("or", KEYWORD_COLOR), KEYWORD("yield", KEYWORD_COLOR),

    KEYWORD("abs", STDLIB_COLOR), KEYWORD("all", STDLIB_COLOR), KEYWORD("any", STDLIB_COLOR), // builtins
    KEYWORD("ascii", STDLIB_COLOR), KEYWORD("bin", STDLIB_COLOR), KEYWORD("isinstance", STDLIB_COLOR),
    KEYWORD("breakpoint", STDLIB_COLOR), KEYWORD("bytearray", STDLIB_COLOR), KEYWORD("bytes", STDLIB_COLOR),
    KEYWORD("callable", STDLIB_COLOR), KEYWORD("chr", STDLIB_COLOR), KEYWORD("classmethod", STDLIB_COLOR),
    KEYWORD("compile", STDLIB_COLOR), KEYWORD("complex", STDLIB_COLOR), KEYWORD("copyright", STDLIB_COLOR),
    KEYWORD("credits", STDLIB_COLOR), KEYWORD("delattr", STDLIB_COLOR), KEYWORD("dict", STDLIB_COLOR),
    KEYWORD("dir", STDLIB_COLOR), KEYWORD("divmod", STDLIB_COLOR), KEYWORD("enumerate", STDLIB_COLOR),
    KEYWORD("eval", STDLIB_COLOR), KEYWORD("exec", STDLIB_COLOR), KEYWORD("exit", STDLIB_COLOR),
    KEYWORD("filter", STDLIB_COLOR), KEYWORD("format", STDLIB_COLOR), KEYWORD("frozenset", STDLIB_COLOR),
    KEYWORD("getattr", STDLIB_COLOR), KEYWORD("globals", STDLIB_COLOR), KEYWORD("issubclass", STDLIB_COLOR),
    KEYWORD("hasattr", STDLIB_COLOR), KEYWORD("hash", STDLIB_COLOR), KEYWORD("help", STDLIB_COLOR),
    KEYWORD("hex", STDLIB_COLOR), KEYWORD("id", STDLIB_COLOR), KEYWORD("input", STDLIB_COLOR),
    KEYWORD("iter", STDLIB_COLOR), KEYWORD("len", STDLIB_COLOR), KEYWORD("license", STDLIB_COLOR),
    KEYWORD("list", STDLIB_COLOR), KEYWORD("locals", STDLIB_COLOR), KEYWORD("map", STDLIB_COLOR),
    KEYWORD("max", STDLIB_COLOR), KEYWORD("memoryview", STDLIB_COLOR), KEYWORD("min", STDLIB_COLOR),
    KEYWORD("next", STDLIB_COLOR), KEYWORD("object", STDLIB_COLOR), KEYWORD("oct", STDLIB_COLOR),
    KEYWORD("open", STDLIB_COLOR), KEYWORD("ord", STDLIB_COLOR), KEYWORD("pow", STDLIB_COLOR),
    KEYWORD("print", STDLIB_COLOR), KEYWORD("property", STDLIB_COLOR), KEYWORD("quit", STDLIB_COLOR),
    KEYWORD("range", STDLIB_COLOR), KEYWORD("repr", STDLIB_COLOR), KEYWORD("reversed", STDLIB_COLOR),
    KEYWORD("round", STDLIB_COLOR), KEYWORD("set", STDLIB_COLOR), KEYWORD("setattr", STDLIB_COLOR),
    KEYWORD("slice", STDLIB_COLOR), KEYWORD("sorted", STDLIB_COLOR), KEYWORD("staticmethod", STDLIB_COLOR),
    KEYWORD("str", STDLIB_COLOR), KEYWORD("sum", STDLIB_COLOR), KEYWORD("super", STDLIB_COLOR),
    KEYWORD("tuple", STDLIB_COLOR), KEYWORD("type", STDLIB_COLOR), KEYWORD("vars", STDLIB_COLOR),
    KEYWORD("zip", STDLIB_COLOR),

    KEYWORD("False", TYPES_COLOR), KEYWORD("None", TYPES_COLOR), KEYWORD("True", TYPES_COLOR), // builtin types
    KEYWORD("bool", TYPES_COLOR), KEYWORD("int", TYPES_COLOR), KEYWORD("float", TYPES_COLOR),
    KEYWORD("ArithmeticError", TYPES_COLOR), KEYWORD("AssertionError", TYPES_COLOR), KEYWORD("AttributeError", TYPES_COLOR),
    KEYWORD("BaseException", TYPES_COLOR), KEYWORD("BlockingIOError", TYPES_COLOR), KEYWORD("BrokenPipeError", TYPES_COLOR),
    KEYWORD("BufferError", TYPES_COLOR), KEYWORD("BytesWarning", TYPES_COLOR), KEYWORD("ChildProcessError", TYPES_COLOR),
    KEYWORD("ConnectionAbortedError", TYPES_COLOR), KEYWORD("ConnectionError", TYPES_COLOR), KEYWORD("ConnectionRefusedError", TYPES_COLOR),
    KEYWORD("ConnectionResetError", TYPES_COLOR), KEYWORD("DeprecationWarning", TYPES_COLOR), KEYWORD("EOFError", TYPES_COLOR),
    KEYWORD("Ellipsis", TYPES_COLOR), KEYWORD("EnvironmentError", TYPES_COLOR), KEYWORD("Exception", TYPES_COLOR),
    KEYWORD("False", TYPES_COLOR), KEYWORD("FileExistsError", TYPES_COLOR), KEYWORD("FileNotFoundError", TYPES_COLOR),
    KEYWORD("FloatingPointError", TYPES_COLOR), KEYWORD("FutureWarning", TYPES_COLOR), KEYWORD("GeneratorExit", TYPES_COLOR),
    KEYWORD("IOError", TYPES_COLOR), KEYWORD("ImportError", TYPES_COLOR), KEYWORD("ImportWarning", TYPES_COLOR),
    KEYWORD("IndentationError", TYPES_COLOR), KEYWORD("IndexError", TYPES_COLOR), KEYWORD("InterruptedError", TYPES_COLOR),
    KEYWORD("IsADirectoryError", TYPES_COLOR), KEYWORD("KeyError", TYPES_COLOR), KEYWORD("KeyboardInterrupt", TYPES_COLOR),
    KEYWORD("LookupError", TYPES_COLOR), KEYWORD("MemoryError", TYPES_COLOR), KEYWORD("ModuleNotFoundError", TYPES_COLOR),
    KEYWORD("NameError", TYPES_COLOR), KEYWORD("None", TYPES_COLOR), KEYWORD("NotADirectoryError", TYPES_COLOR),
    KEYWORD("NotImplemented", TYPES_COLOR), KEYWORD("NotImplementedError", TYPES_COLOR), KEYWORD("OSError", TYPES_COLOR),
    KEYWORD("OverflowError", TYPES_COLOR), KEYWORD("PendingDeprecationWarning", TYPES_COLOR), KEYWORD("PermissionError", TYPES_COLOR),
    KEYWORD("ProcessLookupError", TYPES_COLOR), KEYWORD("RecursionError", TYPES_COLOR), KEYWORD("ReferenceError", TYPES_COLOR),
    KEYWORD("ResourceWarning", TYPES_COLOR), KEYWORD("RuntimeError", TYPES_COLOR), KEYWORD("RuntimeWarning", TYPES_COLOR),
    KEYWORD("StopAsyncIteration", TYPES_COLOR), KEYWORD("StopIteration", TYPES_COLOR), KEYWORD("SyntaxError", TYPES_COLOR),
    KEYWORD("SyntaxWarning", TYPES_COLOR), KEYWORD("SystemError", TYPES_COLOR), KEYWORD("SystemExit", TYPES_COLOR),
    KEYWORD("TabError", TYPES_COLOR), KEYWORD("TimeoutError", TYPES_COLOR), KEYWORD("True", TYPES_COLOR),
    KEYWORD("TypeError", TYPES_COLOR), KEYWORD("UnboundLocalError", TYPES_COLOR), KEYWORD("UnicodeDecodeError", TYPES_COLOR),
    KEYWORD("UnicodeEncodeError", TYPES_COLOR), KEYWORD("UnicodeError", TYPES_COLOR), KEYWORD("UnicodeTranslateError", TYPES_COLOR),
    KEYWORD("UnicodeWarning", TYPES_COLOR), KEYWORD("UserWarning", TYPES_COLOR), KEYWORD("ValueError", TYPES_COLOR),
    KEYWORD("Warning", TYPES_COLOR), KEYWORD("ZeroDivisionError", TYPES_COLOR),

    OPERATOR("*", OPERATOR_COLOR), OPERATOR(",", OPERATOR_COLOR), OPERATOR(";", OPERATOR_COLOR),
    OPERATOR("/", OPERATOR_COLOR), OPERATOR("-", OPERATOR_COLOR), OPERATOR("+", OPERATOR_COLOR),
    OPERATOR("%", OPERATOR_COLOR), OPERATOR("^", OPERATOR_COLOR), OPERATOR("&", OPERATOR_COLOR),
    OPERATOR("~", OPERATOR_COLOR), OPERATOR("|", OPERATOR_COLOR), OPERATOR("!", OPERATOR_COLOR),
    OPERATOR("<", OPERATOR_COLOR), OPERATOR(">", OPERATOR_COLOR), OPERATOR("=", OPERATOR_COLOR),
    OPERATOR(":", OPERATOR_COLOR), OPERATOR(".", OPERATOR_COLOR), OPERATOR("\\", OPERATOR_COLOR),

    OPERATOR("(", PAREN_COLOR), OPERATOR(")", PAREN_COLOR),
    OPERATOR("{", PAREN_COLOR), OPERATOR("}", PAREN_COLOR),
    OPERATOR("[", PAREN_COLOR), OPERATOR("]", PAREN_COLOR)
};

static const char *python_exts[] = {"py", "py3", "pyx", "pyw", "pyd", "pyde"};
static const struct MATCH python_strmatch[] = {
    STRMATCH("{}"), STRMATCH("{!s}"), STRMATCH("{!r}"), STRMATCH("{!a}"), STRMATCH("{:%}"),
    STRMATCH("{:b}"), STRMATCH("{:c}"), STRMATCH("{:d}"), STRMATCH("{:e}"), STRMATCH("{:X}"),
    STRMATCH("{:E}"), STRMATCH("{:f}"), STRMATCH("{:F}"), STRMATCH("{:g}"), STRMATCH("{:x}"),
    STRMATCH("{:G}"), STRMATCH("{:o}"), STRMATCH("{:s}"), STRMATCH("%d"), STRMATCH("%f"),
    STRMATCH("%E"), STRMATCH("%c"), STRMATCH("%lf"), STRMATCH("%x"), STRMATCH("%g"),
    STRMATCH("%%"), STRMATCH("%s"), STRMATCH("%u"), STRMATCH("%ld"), STRMATCH("%o"),
    STRMATCH("%lu"), STRMATCH("%G"), STRMATCH("%e"), STRMATCH("%hi"), STRMATCH("%llu"),
    STRMATCH("%hu"), STRMATCH("%lld"), STRMATCH("%X"), STRMATCH("%n"), STRMATCH("%Lf"),
    STRMATCH("%lli"), STRMATCH("%li"), STRMATCH("%i"), STRMATCH("%p"), STRMATCH("%l"),
    STRMATCH("\\b"), STRMATCH("\\n"), STRMATCH("\\r"), STRMATCH("\\t"), STRMATCH("\\f"),
    STRMATCH("\\\\"), STRMATCH("\\'"), STRMATCH("\\\""), STRMATCH("\\x"), STRMATCH("\\u"),
    STRMATCH("\\U"), STRMATCH("\\0"),
};

static struct SHD python_syntax = {
    "Python", 0,
    sizeof python_exts / sizeof *python_exts, python_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/", // Characters that separates words
    sizeof python_kwd / sizeof *python_kwd, python_kwd, //Keywords
    LITERAL_COLOR, PALETTE_COLOR(PALETTE_BRIGHT_RED, PALETTE_OFF),
    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF), PALETTE_COLOR(PALETTE_OFF, PALETTE_CYAN),
    LITERAL_COLOR, LITERAL_COLOR, TYPES_COLOR,
    "\"\'`", sizeof python_strmatch / sizeof *python_strmatch, python_strmatch,// Strings charaters
    STRMATCH("#"), {STRMATCH(""), STRMATCH("")}, // Comments
    {"{[(", "}])"},
    {STRMATCH("0x"), STRMATCH("0o"), STRMATCH("0b")},
    "jJ",
    {"0123456789aAbBcCdDeEfF_", "01234567_", "01_", "0123456789_"} // _ is a digit divisor
};

/*
Shell syntax highlighting descriptor
*/

static struct KWD sh_kwd[] = {
    KEYWORD("if", KEYWORD_COLOR), KEYWORD("then", KEYWORD_COLOR), KEYWORD("else", KEYWORD_COLOR),
    KEYWORD("fi", KEYWORD_COLOR), KEYWORD("while", KEYWORD_COLOR), KEYWORD("for", KEYWORD_COLOR),
    KEYWORD("do", KEYWORD_COLOR), KEYWORD("done", KEYWORD_COLOR), KEYWORD("break", KEYWORD_COLOR),
    KEYWORD("continue", KEYWORD_COLOR), KEYWORD("return", KEYWORD_COLOR), KEYWORD("in", KEYWORD_COLOR),
    KEYWORD("case", KEYWORD_COLOR), KEYWORD("esac", KEYWORD_COLOR), KEYWORD("until", KEYWORD_COLOR),
    KEYWORD("function", KEYWORD_COLOR), KEYWORD("elif", KEYWORD_COLOR),

    KEYWORD("$?", TYPES_COLOR), KEYWORD("~", TYPES_COLOR), KEYWORD(".", TYPES_COLOR), // special

    KEYWORD("alias", STDLIB_COLOR), KEYWORD("bg", STDLIB_COLOR), KEYWORD("bind", STDLIB_COLOR), //builtins
    KEYWORD("builtin", STDLIB_COLOR), KEYWORD("cd", STDLIB_COLOR), KEYWORD("command", STDLIB_COLOR),
    KEYWORD("continue", STDLIB_COLOR), KEYWORD("declare", STDLIB_COLOR), KEYWORD("dirs", STDLIB_COLOR),
    KEYWORD("disown", STDLIB_COLOR), KEYWORD("echo", STDLIB_COLOR), KEYWORD("enable", STDLIB_COLOR),
    KEYWORD("eval", STDLIB_COLOR), KEYWORD("exec", STDLIB_COLOR), KEYWORD("exit", STDLIB_COLOR),
    KEYWORD("export", STDLIB_COLOR), KEYWORD("false", STDLIB_COLOR), KEYWORD("fc", STDLIB_COLOR),
    KEYWORD("fg", STDLIB_COLOR), KEYWORD("getopts", STDLIB_COLOR), KEYWORD("hash", STDLIB_COLOR),
    KEYWORD("help", STDLIB_COLOR), KEYWORD("history", STDLIB_COLOR), KEYWORD("jobs", STDLIB_COLOR),
    KEYWORD("kill", STDLIB_COLOR), KEYWORD("let", STDLIB_COLOR), KEYWORD("local", STDLIB_COLOR),
    KEYWORD("logout", STDLIB_COLOR), KEYWORD("popd", STDLIB_COLOR), KEYWORD("pushd", STDLIB_COLOR),
    KEYWORD("printf", STDLIB_COLOR), KEYWORD("pwd", STDLIB_COLOR), KEYWORD("read", STDLIB_COLOR),
    KEYWORD("readarray", STDLIB_COLOR), KEYWORD("readonly", STDLIB_COLOR), KEYWORD("set", STDLIB_COLOR),
    KEYWORD("shift", STDLIB_COLOR), KEYWORD("source", STDLIB_COLOR), KEYWORD("suspend", STDLIB_COLOR),
    KEYWORD("test", STDLIB_COLOR), KEYWORD("times", STDLIB_COLOR), KEYWORD("time", STDLIB_COLOR),
    KEYWORD("trap", STDLIB_COLOR), KEYWORD("true", STDLIB_COLOR), KEYWORD("type", STDLIB_COLOR),
    KEYWORD("ulimit", STDLIB_COLOR), KEYWORD("umask", STDLIB_COLOR), KEYWORD("unalias", STDLIB_COLOR),
    KEYWORD("unset", STDLIB_COLOR), KEYWORD("wait", STDLIB_COLOR), KEYWORD("shopt", STDLIB_COLOR),

    OPERATOR("$", OPERATOR_COLOR), OPERATOR(";", OPERATOR_COLOR), OPERATOR("=", OPERATOR_COLOR),
    OPERATOR("<<-", OPERATOR_COLOR), OPERATOR("|", OPERATOR_COLOR), OPERATOR("&", OPERATOR_COLOR),
    OPERATOR("<", OPERATOR_COLOR), OPERATOR(">", OPERATOR_COLOR), OPERATOR("2>", OPERATOR_COLOR),
    OPERATOR("!", OPERATOR_COLOR), OPERATOR("+", OPERATOR_COLOR), OPERATOR("@", OPERATOR_COLOR),
    OPERATOR("*", OPERATOR_COLOR), OPERATOR("-", OPERATOR_COLOR), OPERATOR("?", OPERATOR_COLOR),

    OPERATOR("(", PAREN_COLOR), OPERATOR(")", PAREN_COLOR),
    OPERATOR("{", PAREN_COLOR), OPERATOR("}", PAREN_COLOR),
    OPERATOR("[", PAREN_COLOR), OPERATOR("]", PAREN_COLOR),
};

static const char *sh_exts[] = {"sh", "zsh"};
static const struct MATCH sh_strmatch[] = {
    STRMATCH("\\n"), STRMATCH("\\r"), STRMATCH("\\t"), STRMATCH("\\\\"), STRMATCH("\\'"),
    STRMATCH("\\\""), STRMATCH("\\x"), STRMATCH("\\u"), STRMATCH("\\0"),
};

static struct SHD sh_syntax = {
    "Shell", 0,
    sizeof sh_exts / sizeof *sh_exts, sh_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof sh_kwd / sizeof *sh_kwd, sh_kwd, //Keywords
    LITERAL_COLOR, PALETTE_COLOR(PALETTE_BRIGHT_RED, PALETTE_OFF),
    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF), PALETTE_COLOR(PALETTE_OFF, PALETTE_CYAN),
    LITERAL_COLOR, 0, 0,
    "\"\'`", sizeof sh_strmatch / sizeof *sh_strmatch, sh_strmatch, // Strings charaters
    STRMATCH("#"), {STRMATCH(""), STRMATCH("")}, // Comments
    {"{[(", "}])"},
    {STRMATCH(""), STRMATCH(""), STRMATCH("")},
    "",
    {"0123456789aAbBcCdDeEfF", "01234567", "01", "0123456789"}
};


/*
Rust syntax highlighting descriptor
*/

static struct KWD rust_kwd[] = {
    KEYWORD("break", KEYWORD_COLOR), KEYWORD("as", KEYWORD_COLOR), KEYWORD("const", KEYWORD_COLOR),
    KEYWORD("continue", KEYWORD_COLOR), KEYWORD("while", KEYWORD_COLOR), KEYWORD("enum", KEYWORD_COLOR),
    KEYWORD("crate", KEYWORD_COLOR), KEYWORD("else", KEYWORD_COLOR), KEYWORD("extern", KEYWORD_COLOR),
    KEYWORD("for", KEYWORD_COLOR), KEYWORD("fn", KEYWORD_COLOR), KEYWORD("if", KEYWORD_COLOR),
    KEYWORD("return", KEYWORD_COLOR), KEYWORD("match", KEYWORD_COLOR), KEYWORD("struct", KEYWORD_COLOR),
    KEYWORD("impl", KEYWORD_COLOR), KEYWORD("in", KEYWORD_COLOR), KEYWORD("union", KEYWORD_COLOR),
    KEYWORD("loop", KEYWORD_COLOR), KEYWORD("let", KEYWORD_COLOR), KEYWORD("mod", KEYWORD_COLOR),
    KEYWORD("move", KEYWORD_COLOR), KEYWORD("mut", KEYWORD_COLOR), KEYWORD("pub", KEYWORD_COLOR),
    KEYWORD("ref", KEYWORD_COLOR), KEYWORD("static", KEYWORD_COLOR), KEYWORD("super", KEYWORD_COLOR),
    KEYWORD("trait", KEYWORD_COLOR), KEYWORD("type", KEYWORD_COLOR), KEYWORD("unsafe", KEYWORD_COLOR),
    KEYWORD("use", KEYWORD_COLOR), KEYWORD("where", KEYWORD_COLOR), KEYWORD("async", KEYWORD_COLOR),
    KEYWORD("await", KEYWORD_COLOR), KEYWORD("dyn", KEYWORD_COLOR), KEYWORD("try", KEYWORD_COLOR),

    KEYWORD("char", TYPES_COLOR), KEYWORD("str", TYPES_COLOR), KEYWORD("u8", TYPES_COLOR), // primitives
    KEYWORD("u16", TYPES_COLOR), KEYWORD("u32", TYPES_COLOR), KEYWORD("u64", TYPES_COLOR),
    KEYWORD("u128", TYPES_COLOR), KEYWORD("i8", TYPES_COLOR), KEYWORD("i16", TYPES_COLOR),
    KEYWORD("i32", TYPES_COLOR), KEYWORD("i64", TYPES_COLOR), KEYWORD("i128", TYPES_COLOR),
    KEYWORD("usize", TYPES_COLOR), KEYWORD("isize", TYPES_COLOR), KEYWORD("f32", TYPES_COLOR),
    KEYWORD("f64", TYPES_COLOR), KEYWORD("bool", TYPES_COLOR),

    KEYWORD("Self", MACRO_COLOR), KEYWORD("self", MACRO_COLOR), KEYWORD("_", MACRO_COLOR),// special identifiers

    KEYWORD("'_", LITERAL_COLOR), KEYWORD("'static", LITERAL_COLOR),// special lifetimes

    KEYWORD("false", LITERAL_COLOR), KEYWORD("true", LITERAL_COLOR), // literals

    KEYWORD("std", STDLIB_COLOR), KEYWORD("core", STDLIB_COLOR), KEYWORD("alloc", STDLIB_COLOR),// special crates

    KEYWORD("Box", TYPES_COLOR), KEYWORD("Rc", TYPES_COLOR), KEYWORD("Arc", TYPES_COLOR),// prelude traits/enums/structs
    KEYWORD("Pin", TYPES_COLOR), KEYWORD("UnsafeCell", TYPES_COLOR), KEYWORD("Cell", TYPES_COLOR),
    KEYWORD("RefCell", TYPES_COLOR), KEYWORD("PhantomData", TYPES_COLOR), KEYWORD("Deref", TYPES_COLOR),
    KEYWORD("DerefMut", TYPES_COLOR), KEYWORD("Drop", TYPES_COLOR), KEYWORD("Copy", TYPES_COLOR),
    KEYWORD("Clone", TYPES_COLOR), KEYWORD("Send", TYPES_COLOR), KEYWORD("Sync", TYPES_COLOR),
    KEYWORD("Unpin", TYPES_COLOR), KEYWORD("UnwindSafe", TYPES_COLOR), KEYWORD("RefUnwindSafe", TYPES_COLOR),
    KEYWORD("Sized", TYPES_COLOR), KEYWORD("Debug", TYPES_COLOR), KEYWORD("Default", TYPES_COLOR),
    KEYWORD("Vec", TYPES_COLOR), KEYWORD("String", TYPES_COLOR), KEYWORD("ToString", TYPES_COLOR),
    KEYWORD("Hash", TYPES_COLOR), KEYWORD("ToOwned", TYPES_COLOR), KEYWORD("AsMut", TYPES_COLOR),
    KEYWORD("AsRef", TYPES_COLOR), KEYWORD("From", TYPES_COLOR), KEYWORD("Into", TYPES_COLOR),
    KEYWORD("Fn", TYPES_COLOR), KEYWORD("FnMut", TYPES_COLOR), KEYWORD("FnOnce", TYPES_COLOR),
    KEYWORD("Eq", TYPES_COLOR), KEYWORD("PartialEq", TYPES_COLOR), KEYWORD("Ord", TYPES_COLOR),
    KEYWORD("PartialOrd", TYPES_COLOR), KEYWORD("DoubleEndedIterator", TYPES_COLOR), KEYWORD("ExactSizeIterator", TYPES_COLOR),
    KEYWORD("Extend", TYPES_COLOR), KEYWORD("IntoIterator", TYPES_COLOR), KEYWORD("Iterator", TYPES_COLOR),
    KEYWORD("Option", TYPES_COLOR), KEYWORD("None", TYPES_COLOR), KEYWORD("Some", TYPES_COLOR),
    KEYWORD("Result", TYPES_COLOR), KEYWORD("Err", TYPES_COLOR), KEYWORD("Ok", TYPES_COLOR),

    KEYWORD("eprint!", STDLIB_COLOR), KEYWORD("eprintln!", STDLIB_COLOR), KEYWORD("print!", STDLIB_COLOR),// stdlib functions/macros
    KEYWORD("println!", STDLIB_COLOR), KEYWORD("panic!", STDLIB_COLOR), KEYWORD("trace_macros!", STDLIB_COLOR),
    KEYWORD("macro_rules!", STDLIB_COLOR), KEYWORD("vec!", STDLIB_COLOR), KEYWORD("llvm_asm!", STDLIB_COLOR),
    KEYWORD("asm!", STDLIB_COLOR), KEYWORD("assert_eq!", STDLIB_COLOR), KEYWORD("assert_ne!", STDLIB_COLOR),
    KEYWORD("assert!", STDLIB_COLOR), KEYWORD("include_bytes!", STDLIB_COLOR), KEYWORD("include_str!", STDLIB_COLOR),
    KEYWORD("include!", STDLIB_COLOR), KEYWORD("cfg!", STDLIB_COLOR), KEYWORD("cfg_if!", STDLIB_COLOR),
    KEYWORD("compile_error!", STDLIB_COLOR), KEYWORD("column!", STDLIB_COLOR), KEYWORD("concat_idents!", STDLIB_COLOR),
    KEYWORD("concat!", STDLIB_COLOR), KEYWORD("dbg!", STDLIB_COLOR), KEYWORD("drop", STDLIB_COLOR),
    KEYWORD("env!", STDLIB_COLOR), KEYWORD("file!", STDLIB_COLOR), KEYWORD("format_args!", STDLIB_COLOR),
    KEYWORD("format!", STDLIB_COLOR), KEYWORD("global_asm!", STDLIB_COLOR), KEYWORD("todo!", STDLIB_COLOR),
    KEYWORD("unreachable!", STDLIB_COLOR), KEYWORD("writeln!", STDLIB_COLOR), KEYWORD("write!", STDLIB_COLOR),
    KEYWORD("thread_local!", STDLIB_COLOR), KEYWORD("option_env!", STDLIB_COLOR), KEYWORD("module_path!", STDLIB_COLOR),
    KEYWORD("matches!", STDLIB_COLOR), KEYWORD("line!", STDLIB_COLOR), KEYWORD("log_syntax!", STDLIB_COLOR),

    OPERATOR("*", OPERATOR_COLOR), OPERATOR(",", OPERATOR_COLOR), OPERATOR(";", OPERATOR_COLOR),
    OPERATOR("/", OPERATOR_COLOR), OPERATOR("-", OPERATOR_COLOR), OPERATOR("+", OPERATOR_COLOR),
    OPERATOR("%", OPERATOR_COLOR), OPERATOR("^", OPERATOR_COLOR), OPERATOR("&", OPERATOR_COLOR),
    OPERATOR("~", OPERATOR_COLOR), OPERATOR("|", OPERATOR_COLOR), OPERATOR("!", OPERATOR_COLOR),
    OPERATOR("<", OPERATOR_COLOR), OPERATOR(">", OPERATOR_COLOR), OPERATOR("=", OPERATOR_COLOR),
    OPERATOR("?", OPERATOR_COLOR), OPERATOR(":", OPERATOR_COLOR), OPERATOR(".", OPERATOR_COLOR),
    OPERATOR("@", OPERATOR_COLOR), OPERATOR("#", OPERATOR_COLOR), OPERATOR("$", OPERATOR_COLOR),
    
    OPERATOR("(", PAREN_COLOR), OPERATOR(")", PAREN_COLOR),
    OPERATOR("{", PAREN_COLOR), OPERATOR("}", PAREN_COLOR),
    OPERATOR("[", PAREN_COLOR), OPERATOR("]", PAREN_COLOR),
};

static const char *rust_exts[] = {"rs"};
static const struct MATCH rust_strmatch[] = {
    STRMATCH("{}"), STRMATCH("{:?}"), STRMATCH("{:#?}"), STRMATCH("{:.*}"), STRMATCH("{:+}"),
    STRMATCH("\\n"), STRMATCH("\\r"), STRMATCH("\\t"), STRMATCH("\\\\"), STRMATCH("\\'"),
    STRMATCH("\\\""), STRMATCH("\\x"), STRMATCH("\\u"), STRMATCH("\\0"),
};

static struct SHD rust_syntax = {
    "Rust", 0,
    sizeof rust_exts / sizeof *rust_exts, rust_exts,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?", // Characters that separates words
    sizeof rust_kwd / sizeof *rust_kwd, rust_kwd, //Keywords
    LITERAL_COLOR, PALETTE_COLOR(PALETTE_BRIGHT_RED, PALETTE_OFF),
    PALETTE_COLOR(PALETTE_CYAN, PALETTE_OFF), PALETTE_COLOR(PALETTE_OFF, PALETTE_CYAN),
    LITERAL_COLOR, LITERAL_COLOR, TYPES_COLOR,
    "\"", sizeof rust_strmatch / sizeof *rust_strmatch, rust_strmatch,// Strings charaters
    STRMATCH("//"), {STRMATCH("/*"), STRMATCH("*/")}, // Comments
    {"{[(", "}])"},
    {STRMATCH("0x"), STRMATCH("0o"), STRMATCH("0b")},
    "uif816324sze",
    {"0123456789aAbBcCdDeEfF_", "01234567_", "01_", "0123456789_"} // _ is a digit divisor
};

/*
Default syntax
*/

struct SHD default_syntax = {
    "Default", 1,
    0, NULL,
    " \t~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?",
    0, NULL,
    0, 0, 0, 0, 0, 0, 0,
    "", 0, NULL,
    STRMATCH(""), {STRMATCH(""), STRMATCH("")},
    {"", ""},
    {STRMATCH(""), STRMATCH(""), STRMATCH("")},
    "",
    {"", "", "", ""}
};

/*
Global syntaxes
*/

struct SHD *syntaxes[] = {&c_syntax, &cpp_syntax, &python_syntax, &sh_syntax, &rust_syntax};

void register_syntax(void) {
    config.syntaxes = syntaxes;
    config.syntax_len = sizeof syntaxes / sizeof *syntaxes;
}
