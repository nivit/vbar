#include <vbar.h>
#include <time.h>

typedef enum{ DT_DY, DT_DM, DT_DD, DT_TH, DT_TM, DT_TS, DT_COUNT } datetime_e;

typedef struct datetime{
	unsigned long toblink;
	datetime_e dt[DT_COUNT];
}datetime_s;

__ef_private int datetime_mod_refresh(module_s* mod){
	datetime_s* dt = mod->data;
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	dt->dt[DT_DY] = tm->tm_year + 1900;
	dt->dt[DT_DM] = tm->tm_mon + 1;
	dt->dt[DT_DD] = tm->tm_mday;
	dt->dt[DT_TH] = tm->tm_hour;
	dt->dt[DT_TM] = tm->tm_min;
	dt->dt[DT_TS] = tm->tm_sec;

	module_set_urgent(mod, (time_t)dt->toblink == t );	
	return 0;
}

__ef_private int datetime_mod_env(module_s* mod, int id, char* dest){
	datetime_s* dt = mod->data;
	if( (unsigned)id >= DT_COUNT ){
		dbg_error("index to large");
		return -1;
	}
	sprintf(dest, modules_format_get(mod, id, "d"), dt->dt[id]);	
	return 0;
}

__ef_private int datetime_mod_free(module_s* mod){
	free(mod->data);
	return 0;
}

int datetime_mod_load(module_s* mod, char* path){
	datetime_s* dt = ef_mem_new(datetime_s);
	dt->toblink = 0;
	
	mod->data = dt;
	mod->refresh = datetime_mod_refresh;
	mod->getenv = datetime_mod_env;
	mod->free = datetime_mod_free;

	strcpy(mod->att.longunformat, "date $2/$01/$0");
	strcpy(mod->att.shortunformat, "$2/$1/$0");
	strcpy(mod->att.name, "generic");
	strcpy(mod->att.instance, "datetime");
	modules_icons_init(mod, 1);
	modules_icons_set(mod, 0, "⌚");
	modules_format_init(mod, DT_COUNT);
	for( size_t i = 0; i < DT_COUNT; ++i){
		modules_format_set(mod, i, "2");
	}
	modules_format_set(mod, DT_DY, "4");

	datetime_mod_refresh(mod);

	config_s conf;
	config_init(&conf, 256);
	modules_default_config(mod, &conf);
	config_add(&conf, "blink.on", CNF_LU, &dt->toblink, 0, 0, NULL);
	config_load(&conf, path);
	config_destroy(&conf);

	return 0;
}


