#ifndef __INTP_H__
#define __INTP_H__

#include <vbar/type.h>

typedef void(*intpcall_f)(modules_s* mods, module_s* mod, size_t argc, char* argv[], size_t* argl);

void intp_register_command(char* name, intpcall_f call, void* autoarg);
char* intp_interpretate(char* line, module_s* mod);


#endif
