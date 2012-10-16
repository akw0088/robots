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

void merge_cells(ref_t *dst, ref_t *a, ref_t *b)
{
	int i, j, k = 0;

	for(i = 0; i < a->num_cells; i++)
	{
		for(j = 0; j < b->num_cells; j++)
		{
			// only add cells that exist in both sides to interpolation
			if (strcmp(a->cell[i].address, b->cell[j].address) == 0)
			{
				dst->cell[k] = a->cell[i];
				dst->cell[k].signal = (a->cell[i].signal + b->cell[j].signal) / 2.0f;
				dst->cell[k].noise = (a->cell[i].noise + b->cell[j].noise) / 2.0f;
				k++;
			}
		}
	}
	dst->num_cells = k;
}

int main(void)
{
	ref_t	*ref;
	ref_t	*db;
	char	*data;
	int	i, j, k;
	int	size = -1;
	dbh_t *header;

	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		printf("Unable to open db.bin\n");
		return 0;
	}

	header = (dbh_t *)data;
	db = (ref_t *)(data + sizeof(dbh_t));

	ref = (ref_t *)malloc(4 * header->num_refpoints * sizeof(ref_t));
	if (ref == NULL)
	{
		perror("malloc() failed");
		return 0;
	}

	// copy existing points and adjust indicies (makes odd spots empty)
	memcpy(ref, db, sizeof(ref_t) * header->num_refpoints);
	for(i = 0; i < header->num_refpoints; i++)
	{
		ref[i].x *= 2;
		ref[i].y *= 2;
	}

	size = header->num_refpoints;

	//interpolate x coordinate
	for(i = 0; i < header->num_refpoints; i++)
	{
		for(j = 0; j < header->num_refpoints; j++)
		{
			if (i == j)
				continue;

			// X Coordinate
			if (db[j].x - db[i].x == 1 && db[j].y - db[i].y == 0) // j is to the right of i
			{
				ref[size] = db[i];
				ref[size].x *= 2;
				ref[size].y *= 2;
				ref[size].x++;

				merge_cells(&ref[size], &db[i], &db[j]);
				size++;
			}

			//Y Coordinate
			if (db[j].y - db[i].y == 1 && db[j].x - db[i].x == 0)	// j is above i
			{
				ref[size] = db[i];
				ref[size].x *= 2;
				ref[size].y *= 2;
				ref[size].y++;

				merge_cells(&ref[size], &db[i], &db[j]);
				size++;
			}

			// j is above i && j is right of i
			if ((db[j].y - db[i].y == 1) && (db[j].x - db[i].x == 1))
			{
				int flag1 = 0;
				int flag2 = 0;

				// dont round of corners
				for(k = 0; k < header->num_refpoints; k++)
				{
					if (db[j].x - db[k].x == 0 && db[j].y - db[k].y == 1)
					{
						//some one is below j, this is a good point
						flag1 = 1;
						break;
					}
				}

				if (flag1 == 0)
					continue;

				// dont round of corners
				for(k = 0; k < header->num_refpoints; k++)
				{
					if (db[i].x - db[k].x == 0 && db[i].y - db[k].y == -1)
					{
						//some one is above i, this is a good point
						flag2 = 1;
						break;
					}
				}

				if (flag2 == 0)
					continue;

				ref[size] = db[i];
				ref[size].x *= 2;
				ref[size].y *= 2;
				ref[size].x++;
				ref[size].y++;

	
				merge_cells(&ref[size], &db[i], &db[j]);
				size++;
			}

		}
	}

	header->num_refpoints = size;
	header->width *= 2;
	header->height *= 2;
	header->xoffset *= 2;
	header->yoffset *= 2;


	printf("Writing database size = %d\n", size);
	write_database(header, ref, "db2.bin");
	return 0;
}

