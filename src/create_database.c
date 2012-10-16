#include "include.h"

void write_database(dbh_t *header, ref_t *ref, char *filename)
{
	FILE *file = fopen(filename, "wb");

	if (file == NULL)
	{
		perror("Unable to write file");
		return;
	}

	if ( fwrite(header, sizeof(dbh_t), 1, file) != 1 )
	{
		perror("Unable to write data to file");
		return;
	}

	if ( fwrite(ref, sizeof(ref_t), header->num_refpoints, file) != header->num_refpoints )
	{
		perror("Unable to write data to file");
		return;
	}
	fclose(file);
}

int main(int argc, char *argv[])
{
	char	*data = NULL;
	char	file[80] = "";
	ref_t	**ref;
	int	*x_coord;
	int	*y_coord;
	int	num_refpoints;
	int	num_scans;
	int	i, j;
	dbh_t	header;
	int	xmin = INT_MAX;
	int	xmax = INT_MIN;
	int	ymin = INT_MAX;
	int	ymax = INT_MIN;

	if (argc != 3)
	{
		printf("Usage: create_database <num_refpoints> <num_scans>\n");
		return 0;
	}
	num_refpoints = atoi(argv[1]);
	num_scans = atoi(argv[2]);

	x_coord = (int *)malloc(num_refpoints * sizeof(int));
	if (x_coord == NULL)
	{
		perror("malloc failed");
		return 0;
	}

	y_coord = (int *)malloc(num_refpoints * sizeof(int));
	if (y_coord == NULL)
	{
		perror("malloc failed");
		return 0;
	}

	// Allocate a pointer for each scan
	ref = (ref_t **)malloc(num_scans * sizeof(ref_t *));
	if (ref == NULL)
	{
		perror("malloc failed");
		return 0;
	}


	if ( seq_parse(x_coord, y_coord, num_refpoints) != 0)
	{
		printf("seq_parse() failed\n");
		return 1;
	}

	// Allocate memory for three data sets (one for each scan)
	for(i = 0; i < num_scans; i++)
	{
		ref[i] = (ref_t *)calloc(num_refpoints, sizeof(ref_t));
		if (ref[i] == NULL)
		{
			perror("calloc() failed");
			return 1;
		}
	}

	// Get the data
	for(j = 0; j < num_refpoints; j++)
	{
		for(i = 0; i < num_scans; i++)
		{
			sprintf(file, "scan%d-%d", j+1, i+1);
			data = (char *)get_file(file);
			if (data == NULL)
			{
				printf("Failed to open %s\n", file);
				return 2;
			}
			iwlist_parse(data, &ref[i][j]);
			ref[i][j].x = x_coord[j];
			ref[i][j].y = y_coord[j];
			free((void *)data);
			data = NULL;
		}
	}

	// Now we want to combine the databases and average any matching access points
	// we must also combine the lists as we may have new access points in a scan that werent in others.

	for(i = 0; i < num_refpoints; i++)
	{
		for(j = 1; j < num_scans; j++)
		{
			combine_ref(&ref[0][i], &ref[j][i]);
		}
		divide_ref(&ref[0][i]);
	}

	for(i = 0; i < num_refpoints; i++)
	{
		xmin = MIN(xmin, ref[0][i].x);
		xmax = MAX(xmax, ref[0][i].x);
		ymin = MIN(ymin, ref[0][i].y);
		ymax = MAX(ymax, ref[0][i].y);
	}

	header.num_refpoints = num_refpoints;
	header.width = xmax - xmin + 1;
	header.height = ymax - ymin + 1;
	header.xoffset = -xmin;
	header.yoffset = -ymin;

	printf("Writing database\n");
	write_database(&header, ref[0], "db.bin");
	return 0;
}

