#pragma once
#include "BasicTypes.h"
#include "String.h"
//#include <glad/glad.h>

class EXPORT Utils
{
public:
    static String get_gl_error_message(uint code);
    static bool check_gl_error();
    static void discard_gl_error();
    static List<String> get_callstack(uint offset = 0);
    static String get_callstack_string(uint offset = 0);
    static void print_callstack(String category, uint offset = 0);
};

//#define GL_ASSERT(fail_ret)\
//if (!Utils::check_gl_error(glGetError())) return fail_ret;\
