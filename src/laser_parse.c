#include "include.h"


//There will always be 681 data points that do 239.77 degreees, that makes theta increment = (239.77 / 681) * (pi / 180)
int read_laserdrop(char *filename, laserscan_t *scan)
{
	char	*data, *pdata;
	double	theta =  150.0 * (M_PI / 180.0);
	int	i;
	int j = 0;

	data = (char *)get_file(filename);
	pdata = data;
	for(i = 0;; i++, j++)
	{
		double radius;

		sscanf(pdata, "%lf", &radius);
		radius /= 5.0;
		scan->point[i].x = radius * cos(theta);
		scan->point[i].y = radius * sin(theta);
		theta += (239.77 / 681.0) * (M_PI / 180.0);

		pdata = strstr(pdata, " ");
		if (pdata == NULL)
		{
			break;
		}
		pdata++;
	}
	free((void *)data);
	return 0;
}

int write_laserscan(char *filename, laserscan_t *scan, int num_scan)
{
	FILE		*file;

	file = fopen(filename, "wb");
	if (file == NULL)
	{
		perror("Unable to write laserscan\n");
		return 1;
	}

	fwrite(&num_scan, sizeof(int), 1, file);
	fwrite(scan, sizeof(laserscan_t), num_scan, file);
	fclose(file);

	return 0;
}

int main(int argc, char *argv[])
{
	int i;
	laserscan_t *scan;

	if (argc == 1)
	{
		printf("Usage: laser_parse [file1] [file2] ...\n");
		return 0;
	}

	argc--;
	printf("Combining %d files\n", argc);
	scan = (laserscan_t *)malloc( argc * sizeof(laserscan_t) );
	if (scan == NULL)
	{
		perror("malloc failed");
	}

	for(i = 0; i < argc; i++)
	{
		int x, y, heading;

		read_laserdrop(argv[i+1], &scan[i]);
		sscanf(argv[i+1], "laserscan%d-%d-%d.txt", &x, &y, &heading);
		scan[i].x = x;
		scan[i].y = y;
		scan[i].heading = heading;
	}

	printf("writing laser.bin\n");
	write_laserscan("laser.bin", scan, argc);
	return 0;	
}