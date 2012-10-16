#include "include.h"

int iwlist_scan(ref_t *ref)
{
	char	*data = NULL;

	system("/sbin/iwlist wlan0 scan > scan.txt");
	data = (char *)get_file("scan.txt");
	if (data == NULL)
	{
		perror("Unable to read scan.txt");
		return 1;
	}

	iwlist_parse(data, ref);
	free((void *)data);
	remove("scan.txt");

	return 0;
}

int iwlist_match(ref_t *ref, dbh_t *header, ref_t *db, int *x, int *y)
{
	int i, j, k;
	float diff = 0.0f;
	float min_diff = 1000.0f;
	float old_diff = 1000.0f;
	int ref_index = -1;
	int hits = 0;

	for(i = 0; i < header->num_refpoints; i++)
	{
		for(j = 0; j < db[i].num_cells; j++)
		{
			for(k = 0; k < ref->num_cells; k++)
			{
				if (strcmp(db[i].cell[j].address, ref->cell[k].address) == 0)
				{
					// got a matching pair
					diff += fabs(db[i].cell[j].signal - ref->cell[k].signal);
					hits++;
				}
			}
		}

		if (hits)
		{
			diff /= hits;
			min_diff = MIN(min_diff, diff);
			if (old_diff != min_diff)
			{
				old_diff = min_diff;
				ref_index = i;
			}

			printf("(%d,%d) has %d hits and %f difference\n", db[i].x, db[i].y, hits, diff);
		}
		diff = 0;
		hits = 0;
	}
	printf("Minimum difference was %f\n", min_diff);
	if (ref_index != -1)
	{
		printf("Reference point match is db[%d] (%d,%d)\n", ref_index, db[ref_index].x, db[ref_index].y);
		*x = db[ref_index].x;
		*y = db[ref_index].y;
	}
	else
	{
		*x = 0;
		*y = 0;
	}
	return 0;
}

int iwlist_match_epsilon(ref_t *ref, dbh_t *header, ref_t *db, int *x, int *y, float epsilon)
{
	int i, j, k;
	float diff = 0.0f;
	int ref_index = -1;
	int hits = 0;
	int num_points = 0;

	for(i = 0; i < header->num_refpoints; i++)
	{
		for(j = 0; j < db[i].num_cells; j++)
		{
			for(k = 0; k < ref->num_cells; k++)
			{
				if (strcmp(db[i].cell[j].address, ref->cell[k].address) == 0)
				{
					// got a matching pair
					diff += fabs(db[i].cell[j].signal - ref->cell[k].signal);
					hits++;
				}
			}
		}

		if (hits)
		{
			diff /= hits;

			if (diff < epsilon)
			{
				x[num_points] = db[i].x;
				y[num_points] = db[i].y;
				printf("%d Set includes (%d,%d) with %d hits and %f difference\n", num_points, db[i].x, db[i].y, hits, diff);
				num_points++;
			}
		}
		diff = 0;
		hits = 0;
	}
	return num_points;
}

int main(int argc, char *argv[])
{
	char	*data = NULL;
	ref_t	*ref;
	ref_t	*db;
	int	xmatch[16], ymatch[16];
	int	num_match;
	int	ret = 0;
	int	x, y;
	int	i;
	int	num_scan, period;
	dbh_t	*header;

	if (argc != 3)
	{
		printf("Usage: iwlist_match <num scans> <time period seconds>\n");
		return 0;
	}

	num_scan = atoi(argv[1]);
	period = 1000000 * atoi(argv[2]) / num_scan;

	ref = (ref_t *)malloc( num_scan * sizeof(ref_t) );
	if (ref == NULL)
	{
		perror("malloc failed");
		return 1;
	}


	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		perror("Unable to open database db.bin");
		return 2;
	}
	header = (dbh_t *)data;
	db = (ref_t *)(data + sizeof(dbh_t));

	
	for(i = 0; i < num_scan; i++)
	{
		ret += iwlist_scan(&ref[i]);
		usleep(period);
	}

	if (ret != 0)
	{
		printf("scan failed\n");
		return 1;
	}

	// average signal of scans
	for(i = 1; i < num_scan; i++)
	{
		combine_ref(&ref[0], &ref[i]);
	}
	divide_ref(&ref[0]);

	// Find closest match
	iwlist_match(&ref[0], header, db, &x, &y);
//	num_match = iwlist_match_epsilon(&ref1, header, db, xmatch, ymatch, 2.0f);
	printf("Sending (%d,%d)\n", x, y);
	return 0;
}
