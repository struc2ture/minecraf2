#include <e.h>

void on_run(Editor_State *s)
{
    const char *src_path = "src/main.c";
    const char *o_path = "bin/minecraf2.dylib";
    
    const char *cc = "clang";
    const char *cflags = "-I/opt/homebrew/include -DGL_SILENCE_DEPRECATION";
    const char *lflags = "-L/opt/homebrew/lib -lglfw -framework OpenGL";
    
    char *compile_command = strf("%s -g -dynamiclib %s %s %s -o %s", cc, cflags, lflags, src_path, o_path);
    
    printf("\nCompilation:\n%s\n\n", compile_command);
    int status = system(compile_command);
    printf("\nStatus: %d\n\n", status);
    
    free(compile_command);
}
