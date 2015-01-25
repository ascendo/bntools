#ifndef __NICK_MAP_H__
#define __NICK_MAP_H__

#include <stdint.h>
#include <string.h>
#include "array.h"

#define MAX_ENZYME_NAME_SIZE 16
#define MAX_REC_SEQ_SIZE 32
#define MAX_CHROM_NAME_SIZE 64

enum nick_flag {
	NICK_LEFT_END     = 1,  /* left end of fragment */
	NICK_RIGHT_END    = 2,  /* right end of fragment */
	NICK_PLUS_STRAND  = 4,  /* nick on plus strand */
	NICK_MINUS_STRAND = 8,  /* nick on minus strand */
};

enum file_format {  /* of restriction map */
	FORMAT_UNKNOWN = 0,
	FORMAT_TXT,   /* simple text, with one molecule/contig in each line */
	FORMAT_TSV,   /* tab-separated values, with VCF-like comment header  */
	FORMAT_BNX,   /* .bnx file for molecules, by BioNano inc. */
	FORMAT_CMAP,  /* .cmap file for consensus map, by BioNano inc. */
};

struct nick {
	int pos;
	unsigned int flag;
};

struct fragment {  /* molecule, contig or chromosome */
	char *name;
	int size;  /* in bp */
	array(struct nick) nicks;  /* start (at 0) + labels + end (at `size`) */
};

struct nick_map {
	array(struct fragment) fragments;

	char enzyme[MAX_ENZYME_NAME_SIZE];
	char rec_seq[MAX_REC_SEQ_SIZE];
};

/* basic functions */

void nick_map_init(struct nick_map *map);
void nick_map_free(struct nick_map *map);
void nick_map_set_enzyme(struct nick_map *map, const char *enzyme, const char *rec_seq);

struct fragment *nick_map_add_fragment(struct nick_map *map, const char *name);
int nick_map_add_site(struct fragment *f, int pos, unsigned int flag);

/* IO functions */

int parse_format_text(const char *s);

int nick_map_load(struct nick_map *map, const char *filename);
int nick_map_save(const struct nick_map *map, const char *filename, int format);

#endif /* __NICK_MAP_H__ */
