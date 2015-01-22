#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include "base_map.h"
#include "nick_map.h"

#define DEF_OUTPUT "stdout"
#define DEF_FORMAT "tsv"
#define DEF_ENZ_NAME "BspQI"
#define DEF_REC_SEQ "GCTCTTCN^"

static int verbose = 0;

static char enzyme[MAX_ENZYME_NAME_SIZE] = DEF_ENZ_NAME;
static char rec_seq[MAX_REC_SEQ_SIZE] = DEF_REC_SEQ;
static char output_file[PATH_MAX] = DEF_OUTPUT;
static int output_cmap = 0;
static int transform_to_number = 0;

static void print_usage(void)
{
	fprintf(stderr, "\n"
			"Usage: bntools nick [options] <x.fa> [...]\n"
			"\n"
			"Options:\n"
			"   <x.fa> [...]   input FASTA file(s) to generate restriction map\n"
			"   -o FILE        output file ["DEF_OUTPUT"]\n"
			"   -f {tsv|cmap}  output format ["DEF_FORMAT"]\n"
			"   -e STR         restriction enzyme name ["DEF_ENZ_NAME"]\n"
			"   -r STR         recognition sequence ["DEF_REC_SEQ"]\n"
			"   -n             transform chromosome/contig names into numbers\n"
			"   -v             show verbose messages\n"
			"\n");
}

static int check_options(int argc, char * const argv[])
{
	int c;
	while ((c = getopt(argc, argv, "o:f:e:r:nv")) != -1) {
		switch (c) {
		case 'o':
			snprintf(output_file, sizeof(output_file), "%s", optarg);
			break;
		case 'f':
			if (strcmp(optarg, "tsv") == 0) {
				output_cmap = 0;
			} else if (strcmp(optarg, "cmap") == 0) {
				output_cmap = 1;
			} else {
				fprintf(stderr, "Error: Unknown output format '%s'!\n", optarg);
				return 1;
			}
			break;
		case 'e':
			snprintf(enzyme, sizeof(enzyme), "%s", optarg);
			break;
		case 'r':
			snprintf(rec_seq, sizeof(rec_seq), "%s", optarg);
			break;
		case 'n':
			transform_to_number = 1;
			break;
		case 'v':
			++verbose;
			break;
		default:
			return 1;
		}
	}
	if (optind >= argc) {
		print_usage();
		return 1;
	}
	return 0;
}

int nick_main(int argc, char * const argv[])
{
	struct nick_map map;
	int i, ret = 0;

	if (check_options(argc, argv)) {
		return 1;
	}

	nick_map_init(&map);

	if ((ret = nick_map_set_enzyme(&map, enzyme, rec_seq)) != 0) {
		goto final;
	}

	for (i = optind; i < argc; ++i) {
		if ((ret = nick_map_load_fasta(&map, argv[i],
				transform_to_number, verbose)) != 0) {
			goto final;
		}
	}
	ret = nick_map_save(&map, output_file, output_cmap);
final:
	nick_map_free(&map);
	return ret;
}
