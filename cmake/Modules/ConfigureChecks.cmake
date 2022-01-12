include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)

# HEADER FILES
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)

# FUNCTIONS
check_function_exists(free HAVE_FREE)
check_function_exists(malloc HAVE_MALLOC)