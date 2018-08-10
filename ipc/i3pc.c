#include <vbar/ipc.h>
#include <vbar/string.h>
#include <vbar/delay.h>
#include <sys/time.h>
#include <sys/select.h>

//signstop sigcont
#define json_begin_block() puts("{")
#define json_end() putchar('\n')
#define json_end_block() putchar('}')

__ef_private void json_next(bool_t next){
	if( next ){
		puts(",");
	}
	else{
		putchar('\n');
	}
}

__ef_private void json_write_str(char* name, char* value, bool_t next){
	if( !*value ) return;
	putchar('"');
	fputs(name, stdout);
	fputs("\": \"", stdout);
	fputs(value, stdout);
	putchar('"');
	json_next(next);
}

__ef_private void json_write_i3color(char* name, int value, bool_t next){
	if( value < 0 ) return;
	printf("\"%s\": \"#%X\"", name, value);
	json_next(next);
}

__ef_private void json_write_i3align(char* name, i3align_e value, bool_t next){
	__ef_private char* align[] = {
		"center",
		"right",
		"left"
	};

	if( value < I3_ALIGN_CENTER || value > I3_ALIGN_LEFT ) return;
	
	printf("\"%s\": \"%s\"", name, align[value]);
	json_next(next);
}

__ef_private void json_write_unsigned(char* name, int value, bool_t next){
	if( value < 0 ) return;
	printf("\"%s\": %d", name, value);
	json_next(next);
}

__ef_private void json_write_bool(char* name, int value, bool_t next){
	if( value < 0 ) return;
	printf("\"%s\": %s", name, value ? "true" : "false" );
	json_next(next);
}

void i3bar_init(bool_t clickevents){
	fputs("{ \"version\": 1", stdout);
	if( clickevents ){
		fputs(", \"click_events\": true", stdout);
	}
	puts("}");
	puts("[");
}

void i3bar_begin_elements(){
	puts("[");
}

void i3bar_end_elements(){
	puts("],");
}

void i3bar_write_element(i3element_s* el, bool_t next){
	json_begin_block();
	json_write_str("full_text", el->full_text, TRUE );
	json_write_str("short_text", el->short_text, TRUE);
	json_write_i3color("color", el->color, TRUE);
	json_write_i3color("background", el->background, TRUE);
	json_write_i3color("border", el->border, TRUE);
	json_write_unsigned("min_width", el->min_width, TRUE);
	json_write_i3align("align", el->align, TRUE);
	json_write_str("name", el->name, TRUE);
	json_write_str("instance", el->instance, TRUE);
	json_write_bool("urgent", el->urgent, TRUE);
	json_write_bool("separator", el->seaparator, TRUE);
	json_write_unsigned("separator_block_width", el->separator_block_width, TRUE);
	json_write_bool("markup", el->markup, FALSE);
	json_end_block();
	json_next(next);
}

void i3bar_event_reset(i3event_s* ev){
	ev->wip = 1;
	ev->name[0] = 0;
	ev->instance[0] = 0;
	ev->x = -1;
	ev->y = -1;
	ev->button = -1;
	ev->relative_x = -1;
	ev->relative_y = -1;
	ev->width = -1;
	ev->height = -1;
}

__ef_private void i3bar_event_parser(i3event_s* ev, char* name, size_t lenName, char* value, size_t lenValue){	
	static char* elname[] = {
		"name",
		"instance",
		"x",
		"y",
		"button",
		"relative_x",
		"relative_y",
		"width",
		"height",
		NULL
	};

	static int eltype[] = {
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1
	};

	void* elptr[] = {
		ev->name,
		ev->instance,
		&ev->x,
		&ev->y,
		&ev->button,
		&ev->relative_x,
		&ev->relative_y,
		&ev->width,
		&ev->height
	};

	size_t i;
	for( i = 0; elname[i]; ++i){
		if( 0 == str_len_cmp(elname[i], strlen(elname[i]), name, lenName) ){
			if( eltype[i] == 0 ){
				str_nncpy_src(elptr[i], I3BAR_TEXT_MAX, value, lenValue);
			}
			else if( eltype[i] == 1){
				*((int*)elptr[i]) = strtol(value, NULL, 10);
			}
			return;
		}
	}
}

__ef_private int i3bar_event_lexer(i3event_s* ev, char* line){
	char* parse = strchr(line, '}');
	if( parse ) return 1;

	char* name = strchr(line, '"');
	if( NULL == name++ ) return 0;
	
	parse = strchr(name, '"');
	if( NULL == parse ) return 0;
	size_t lenName = parse - name;

	parse = strchr(parse, ':');
	parse = str_skip_h(parse);
	char* entok;
	if( *parse == '"'){
		++parse;
		entok = "\"";
	}
	else{
		entok = " \t\n,";
	}
	char* value = parse;
	parse = strpbrk(parse, entok);
	if( NULL == parse ) return 0;
	size_t lenValue = value - parse;

	i3bar_event_parser(ev, name, lenName, value, lenValue);

	return 0;
}

__ef_private int i3bar_scan_line(i3event_s* ev){
	char inp[1024];
	int ret;
	while( fgets(inp, 1024, stdin) && !(ret = i3bar_event_lexer(ev, inp)) );
	return ret;
}

int i3bar_wait(i3event_s* ev, long timeend){
	if( !ev->wip ){
		i3bar_event_reset(ev);
	}

	struct timeval time = { 
		.tv_sec = 0,
		.tv_usec = 0
	};

	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(STDIN_FILENO, &rset);

	while(1){
		time.tv_usec = (timeend - time_ms()) * 1000;
		dbg_info("wait %ld", time.tv_usec);
		int ret = 0;
		if( time.tv_usec > 0 ) {
			ret = select(STDIN_FILENO + 1, &rset, NULL, NULL, &time);
			if( ret == -1 ){
				dbg_error("select");
				dbg_errno();
				return -1;
			}
		}

		if( ret == 0 ){
			return I3BAR_TIMEOUT;
		}

		ret = i3bar_scan_line(ev);
		if( ret ){
			ev->wip = 0;
			ret = I3BAR_EVENT;
			if( (long)time_ms() >= timeend ) ret |= I3BAR_TIMEOUT;
			return ret;
		}
	}
	
	return -1;
}

