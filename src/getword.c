#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#if __has_include(<types.h>)
	#include <types.h>
#else
	#include <stdint.h>
	#define u64 uint64_t
	#define i32 int
#endif

#define MAX_LINE 256
#define MAX_MATCHES 50000

void _to_lower(char* str){
	
	if(str[0] < 97) str[0] += 32; 
}

char* get_word(){

	const char* wordfile = "/etc/dictionaries-common/words";
	FILE* f = fopen(wordfile, "r");
	if(!f){
		perror("fopen");
		return NULL;
	}

	// Collect 5 letter words from dictionary (skip words with a ' )
	u64 offsets[MAX_MATCHES];
	int cnt = 0;
	char* buf = malloc(MAX_LINE);

	while(!false){
		u64 pos = ftell(f);

		if(!fgets(buf, sizeof(buf), f)) break;

		buf[strcspn(buf, "\n")] = 0;
		if(strlen(buf) == 5 && strchr(buf, '\'') == NULL){
			offsets[cnt++] = pos;
		}
	}

	// Choose random 5 letter word from matches
		srand(time(NULL));
		i32 r = rand() % cnt;
		fseek(f, offsets[r], SEEK_SET);
		fgets(buf, sizeof(buf), f);
		buf[strcspn(buf, "\n")] = 0;

		_to_lower(buf);
		fclose(f);
		return buf;
}





