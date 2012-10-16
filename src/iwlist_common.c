#include "include.h"

char *get_file(char *filename)
{
	FILE	*file;
	char	*buffer;
	int	file_size, bytes_read;

	file = fopen(filename, "rb");
	if (file == NULL)
		return 0;
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	buffer = (char *) malloc( file_size * sizeof(char) + 1 );
	bytes_read = (int)fread(buffer, sizeof(char), file_size, file);
	if (bytes_read != file_size)
		return 0;
	fclose(file);
	buffer[file_size] = '\0';
	return buffer;
}

void append_file(char* filename, char *buffer)
{
	FILE	*file;

	file = fopen(filename, "a");
	if (file == NULL)
		return;
	fwrite(buffer, sizeof(char), strlen(buffer), file);
	fclose(file);
	return;	
}

void iwlist_parse(char *data, ref_t *ref)
{
	char *cell = NULL;
	char *address = NULL;
	char *essid = NULL;
	char *level = NULL;
	char *noise = NULL;
	int i;

	cell = strstr(data, "Cell");
	for(i = 0; cell && i < MAX_CELLS; i++)
	{
		char data[80];


		ref->cell[i].num_samples = 1;
		address = strstr(cell, "Address:");
		sscanf(address + 8, "%s", data);
		strcpy(ref->cell[i].address, data);

		essid = strstr(cell, "ESSID:");
		sscanf(essid + 6, "\"%s", data);
		data[strlen(data) - 1] = '\0';
		strcpy(ref->cell[i].essid, data);

		level = strstr(cell, "Signal level=");
		sscanf(level + 13, "%s", data);
		sscanf(data, "%f", &(ref->cell[i].signal));

		noise = strstr(cell, "Noise level=");
		sscanf(noise + 12, "%s", data);
		sscanf(data, "%f", &(ref->cell[i].noise));

		#ifdef DEBUG
			printf("\nCell %d\n", i);
			printf("Address %s\n", ref->cell[i].address);
			printf("Essid %s\n", ref->cell[i].essid);
			printf("Signal %f\n", ref->cell[i].signal);
			printf("Noise %f\n", ref->cell[i].noise);
		#endif

		cell = (char *)strstr(cell + 4, "Cell");
	}
	ref->num_cells = i;
}

int seq_parse(int *x_coord, int *y_coord, int num_refpoints)
{
	char	*data = NULL;
	char	*pdata = NULL;
	int	i;
	int	ret;

	data = get_file("seq.txt");
	if (data == NULL)
	{
		perror("Failed to open seq.txt");
		return 2;
	}

	pdata = data;
	for(i = 0; i < num_refpoints; i++)
	{
		int seq;

		ret = sscanf(pdata, "%d\t(%d,%d)", &seq, &x_coord[i], &y_coord[i]);
		if (ret != 3)
		{
			printf("seq_parse() failed on sequence %d\n", i+1);
			return 4;
		}

		if (i != seq - 1)
		{
			printf("seq_parse() failed sequence mismatch %d != %d\n", i, seq - 1);
			return 4;
		}

		#ifdef DEBUG
			printf("i = %d, seq = %d, (%d,%d)\n", i, seq, x_coord[i], y_coord[i]);
		#endif
		pdata = (char *)strstr(pdata, ")");
		pdata++;
	}

	if (i != num_refpoints)
	{
		printf("Not enough sequence data in seq.txt\n");
		printf("Found %d coordinate pairs\n", i + 1);
		return 3;
	}
	return 0;
}

/*
	This function adds the signal and noise values together, but does not do the division.
	Also appends missing cells.
	Keeps accurate sample count;
	division occurs after all samples have been added.
*/
void combine_ref(ref_t *dst, ref_t *src)
{
	int s, d;

	for(s = 0; s < src->num_cells; s++)
	{
		for(d = 0; d < dst->num_cells; d++)
		{
			// match found, add signals together but dont divide
			if ( strcmp(src->cell[s].address, dst->cell[d].address) == 0 )
			{
				dst->cell[d].signal += src->cell[s].signal;
				dst->cell[d].noise += src->cell[s].noise;
				dst->cell[d].num_samples++;
				break;
			}
		}
		
		// cell found that didnt exist in dst, add it.
		if (d == dst->num_cells)
		{
			dst->cell[dst->num_cells] = src->cell[s];
			dst->num_cells++;
		}
	}
}

/*
	Performs divide after all scans have been added together resulting in average signal.
*/
void divide_ref(ref_t *ref)
{
	int i;

	for(i = 0; i < ref->num_cells; i++)
	{
		ref->cell[i].signal /= ref->cell[i].num_samples;
		ref->cell[i].noise /= ref->cell[i].num_samples;
	}
}
