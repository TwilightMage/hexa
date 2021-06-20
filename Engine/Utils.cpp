#include "Utils.h"

#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <algorithm>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#if PLATFORM_WINDOWS
#include <dbghelp.h>
#include <psapi.h>
#endif

String Utils::get_gl_error_message(uint code)
{
    if (code == GL_INVALID_ENUM) return "Invalid enum";
    if (code == GL_INVALID_VALUE) return "Invalid value";
    if (code == GL_INVALID_OPERATION) return "Invalid operation";
    if (code == GL_INVALID_FRAMEBUFFER_OPERATION) return "Invalid framebuffer operation";
    if (code == GL_OUT_OF_MEMORY) return "Out of memory";
    if (code == GL_STACK_UNDERFLOW) return "Stack underflow";
    if (code == GL_STACK_OVERFLOW) return "Stack overflow";
    return "";
}

bool Utils::check_gl_error()
{
    List<String> errors;
    while (uint err = glGetError())
    {
        errors.Add(get_gl_error_message(err));
    }
    if (errors.length() > 0)
    {
        print_error("OpenGL", String::join(errors, ", ") + "\n" + get_callstack_string(1));
        return false;
    }

    return true;
}

void Utils::discard_gl_error()
{
    while (glGetError())
    { 
    }
}

List<String> Utils::get_callstack(uint offset)
{
    offset += 1;

    List<String> result;
    
    HANDLE  process;
    HANDLE  thread;

    STACKFRAME64        stack;
    ULONG               frame;    
    DWORD64             displacement;

    DWORD disp;
    IMAGEHLP_LINE64 *line;

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

    memset( &stack, 0, sizeof( STACKFRAME64 ) );

    process                = GetCurrentProcess();
    thread                 = GetCurrentThread();
    displacement           = 0;
#if !defined(_M_AMD64)
    stack.AddrPC.Offset    = (*ctx).Eip;
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode   = AddrModeFlat;
#endif

    CONTEXT* context = new CONTEXT();
    context->ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(context);
#ifdef _M_X64
    stack.AddrPC.Offset = context->Rip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Rsp;
    stack.AddrStack.Mode = AddrModeFlat;    
    stack.AddrFrame.Offset = context->Rbp;
    stack.AddrFrame.Mode = AddrModeFlat;
#else
    stack.AddrPC.Offset = context->Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Esp;
    stack.AddrStack.Mode = AddrModeFlat;    
    stack.AddrFrame.Offset = context->Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
#endif

    SymInitialize( process, NULL, TRUE ); //load symbols

    for(frame = 0; ; frame++)
    {
        //get next call from stack
        BOOL flag = StackWalk64
        (
#if defined(_M_AMD64)
            IMAGE_FILE_MACHINE_AMD64
#else
            IMAGE_FILE_MACHINE_I386
#endif
            ,
            process,
            thread,
            &stack,
            context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );

        if(!flag) break;        

        //get symbol name for address
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, ( ULONG64 )stack.AddrPC.Offset, &displacement, pSymbol);

        line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);       

        //try to get line
        if (SymGetLineFromAddr64(process, stack.AddrPC.Offset - 1, &disp, line))
        {
            if (frame >= offset)
            {
                String filename = line->FileName;
                result.Add(String::format("\tat %s in %s:%lu", pSymbol->Name, filename.substring(filename.index_of("Hexa")).c(), line->LineNumber));
                if (strcmp(pSymbol->Name, "main") == 0) break;
            }
        }     

        free(line);
        line = NULL;
    }

    return result;
}

String Utils::get_callstack_string(uint offset)
{
    return String::join(get_callstack(offset + 1), "\n");
}

void Utils::print_callstack(String category, uint offset)
{
    print_debug(category, get_callstack_string(offset + 1));
}
