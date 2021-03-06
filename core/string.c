#include <vbar/string.h>

int str_len_cmp(char* a, size_t la, char* b, size_t lb){
	for(; la > 0 && lb > 0; ++a, ++b, --la, --lb ){
		if( *a != *b ) return -1;
	}
	if( la || lb ) return 1;
	return 0;
}	

char* str_skip_h(char* str) {
	while( *str && (*str == ' ' || *str == '\t') ) ++str;
	return str;
}

__ef_can_null char* str_copy_to_str_ifsize(char* dst, size_t len, char* src, char* find){
	char* to = strpbrk(src, find);
	if( to == NULL ){
		return NULL;
	}

	size_t max = to - src;
	if( max > len ){
		return NULL;
	}

	memcpy(dst, src, sizeof(char) * max);
	dst[max] = 0;

	return src + max;
}

__ef_can_null char* str_copy_to_ch(char* dst, size_t len, char* src, char ch){
	char* to = strchr(src, ch);
	if( to == NULL ){
		return NULL;
	}

	size_t max = to - src;
	if( max > len ){
		return NULL;
	}

	memcpy(dst, src, sizeof(char) * max);
	dst[max] = 0;

	return src + max;
}

char* str_nncpy_src(char* dst, size_t lend, char* src, size_t lens){
	iassert(lend > 1);
	--lend;
	while( lens-->0 && lend-->0 && *src){
		*dst++=*src++;
	}
	*dst = 0;
	return src;
}

char* str_ncpy(char* dst, size_t lend, char* src){
	iassert(lend > 1);
	--lend;
	while( lend-->0 && *src ){
		*dst++=*src++;
	}
	*dst = 0;
	return dst;
}
