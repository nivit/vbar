#include <vbar.h>

__ef_private int static_mod_env(__ef_unused module_s* mod, __ef_unused int id, char* dest){
	*dest = 0;
	return 0;
}

__ef_private int static_mod_free(__ef_unused module_s* mod){
	return 0;
}

int static_mod_load(module_s* mod, char* path){
	mod->data = NULL;

	mod->refresh = NULL;
	mod->getenv = static_mod_env;
	mod->free = static_mod_free;
	mod->blink = FALSE;
	mod->blinktime = 500;
	mod->blinkstatus = 0;
	strcpy(mod->longformat, "long format");
	strcpy(mod->shortformat, "short");
	mod->reftime = INT_MAX;
	strcpy(mod->i3.name, "generic");
	strcpy(mod->i3.instance, "static");	
	modules_icons_init(mod, 1);
	modules_icons_set(mod, 0, "⊶");
	
	config_s conf;
	config_init(&conf, 256);
	modules_default_config(mod, &conf);
	config_load(&conf, path);
	config_destroy(&conf);
	
	mod->tick = mod->reftime + time_ms();
	return 0;
}



