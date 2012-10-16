#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <errno.h>

#ifndef _WIN32
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <time.h>
	#include <signal.h>
	#include <fcntl.h>
#endif


#ifdef _WIN32
	#define M_PI 3.141592653589793238462643
#endif

#define MAX_CELLS 16
#define MAX_ARC 32768
#define PORT 65535

#define MAX(x,y) (x) > (y) ? (x) : (y)
#define MIN(x,y) (x) < (y) ? (x) : (y)

typedef struct
{
	int	a;
	int	b;
	float	weight;
} arc_t;

typedef struct
{
	arc_t	arc[8];
	int	num_arcs;
} node_t;

typedef struct
{
	char address[32];
	char essid[32];
	float signal;
	float noise;
	int num_samples;
} cell_t;

typedef struct
{
	cell_t cell[MAX_CELLS];
	int num_cells;
	int x;
	int y;
} ref_t;


typedef struct
{
	int num_refpoints;
	int width;
	int height;
	int xoffset;
	int yoffset;
} dbh_t;

typedef struct
{
	double x;
	double y;
} point_t;

typedef struct
{
	point_t	point[681];
	int x;
	int y;
	int heading;
} laserscan_t;

typedef struct
{
	int x;
	int y;
	int intensity;
	int color;
} position_t;

#pragma pack(1)
typedef struct
{
	char type[2];
	int file_size;
	int reserved;
	int offset;
} header_t;
#pragma pack(8)

#pragma pack(1)
typedef struct
{
	int size;
	int width;
	int height;
	short planes;
	short bpp;
	int compression;
	int image_size;
	int xres;
	int yres;
	int clr_used;
	int clr_important;
} dib_t;
#pragma pack(8)

typedef struct
{
	header_t	header;
	dib_t		dib;
} bitmap_t;

// iwlist_common functions
#if __cplusplus
extern "C"
{
#endif
	char *get_file(char *file_name);
	void append_file(char* filename, char *buffer);
	void iwlist_parse(char *data, ref_t *ref);
	int seq_parse(int *x_coord, int *y_coord, int num_refpoints);
	void combine_ref(ref_t *dst, ref_t *src);
	void divide_ref(ref_t *ref);
#ifdef __cplusplus
}
#endif

#endif