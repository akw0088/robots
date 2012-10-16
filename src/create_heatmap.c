#include "include.h"

int rssi_to_heat(float rssi);

void create_heatmap(dbh_t *header, ref_t *ref, int *width, int *height, int **data, char *address, int scale)
{
	int	i, j;
	int	x, y;
	int	*heat;

	const int heatmap[] = 
	{
		0x001D1B3A,
		0x0020007A,
		0x004D4988,
		0x003B0087,
		0x00680795,
		0x00A1008C,
		0x00B70680,
		0x00C91565,
		0x00D62638,
		0x00E34011,
		0x00DF4F05,
		0x00EE6400,
		0x00E07C00,
		0x00F99400,
		0x00FCAB00,
		0x00FCC701,
		0x00FCDA18,
		0x00FCEA5F,
		0x00FCF4B0,
		0x00FCFC17,
                0x00FCFCF8
	};


	heat = (int *)calloc(header->width * header->height, sizeof(int));
	if (heat == NULL)
	{
		perror("calloc() failed");
		return;
	}

	for(i = 0; i < header->num_refpoints; i++)
	{
		int x = ref[i].x + header->xoffset;
		int y = ref[i].y + header->yoffset;

		for(j = 0; j < ref[i].num_cells; j++)
		{
			if ( strcmp(ref[i].cell[j].address, address) == 0)
			{
				int index = rssi_to_heat(ref[i].cell[j].signal);
				heat[y * header->width + x] = heatmap[index];
				break;
			}
		}

		if (j == ref[i].num_cells)
		{
			heat[y * header->width + x] = heatmap[0];
		}
	}	


	*width = header->width * scale;
	*height = header->height * scale;

	*data = (int *)calloc(*width * *height, sizeof(int));
	for(y = 0; y < *height; y++)
	{
		for(x = 0; x < *width; x++)
		{
			(*data)[y * *width + x] = heat[(y / scale) * header->width + (x / scale)];
/*
			// checker test
			if ((x + (y % 2)) % 2)
				(*data)[y * *width + x] = 0x00FFFFFF;
			else
				(*data)[y * *width + x] = 0x00000000;
*/
		}
	}
}

void write_bitmap(char *filename, int width, int height, int *data)
{
	FILE *file;
	bitmap_t	bitmap = {0};

	memcpy(bitmap.header.type, "BM", 2);
	bitmap.header.offset = sizeof(header_t);
	bitmap.dib.size = sizeof(dib_t);
	bitmap.dib.width = width;
	bitmap.dib.height = height;
	bitmap.dib.planes = 1;
	bitmap.dib.bpp = 32;
	bitmap.dib.compression = 0;
	bitmap.dib.image_size = width * height * sizeof(int);
	bitmap.header.file_size = sizeof(header_t) + sizeof(dib_t) + bitmap.dib.image_size;

	file = fopen(filename, "w");
	if (file == NULL)
	{
		perror("Unable to write file");
		return;
	}

	fwrite(&bitmap, 1, sizeof(bitmap_t), file);
	fwrite((void *)data, 1, width * height * 4, file);
	fclose(file);
	
}

int main(int argc, char *argv[])
{
	char	filename[80];
	char	*data = NULL;
	int	*bitmap = NULL;
	ref_t	*ref;
	int	width, height;
	int	i;
	dbh_t	*header;
	int	scale = 32;

	if (argc < 2)
	{
		printf("Usage: create_heatmap <Access Point> [scale]\n");
		return 0;
	}

	if (argc == 3)
	{
		scale = atoi(argv[2]);
	}

	//00:1A:1E:87:04:40

	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		perror("Unable to open database db.bin");
		return 2;
	}

	header = (dbh_t *)data;
	ref = (ref_t *)(data + sizeof(dbh_t));

	create_heatmap(header, ref, &width, &height, &bitmap, argv[1], scale);

	sprintf((char *)filename, "%s.bmp", argv[1]);
	for(i = 0; i < strlen(filename); i++)
	{
		if (filename[i] == ':')
			filename[i] = '.';
	}
	printf("%s %dx%d\n", filename, width, height);
	write_bitmap(filename, width, height, bitmap);
	return 0;
}

int rssi_to_heat(float rssi)
{
	if (rssi < -102)
	{
		return 0;
	}
	else if (rssi < -99.58)
	{
		return 1;
	}
	else if (rssi < -97.17)
	{
		return 2;
	}
	else if (rssi < -94.76)
	{
		return 3;
	}
	else if (rssi < -92.35)
	{
		return 4;
	}
	else if (rssi < -89.94)
	{
		return 5;
	}
	else if (rssi < -87.52)
	{
		return 6;
	}
	else if (rssi < -85.11)
	{
		return 7;
	}
	else if (rssi < -82.70)
	{
		return 8;
	}
	else if (rssi < -80.29)
	{
		return 9;
	}
	else if (rssi < -77.88)
	{
		return 10;
	}
	else if (rssi < -75.47)
	{
		return 11;
	}
	else if (rssi < -73.05)
	{
		return 12;
	}
	else if (rssi < -70.64)
	{
		return 13;
	}
	else if (rssi < -68.23)
	{
		return 14;
	}
	else if (rssi < -65.82)
	{
		return 15;
	}
	else if (rssi < -63.41)
	{
		return 16;
	}
	if (rssi < -61.0f)
	{
		return 17;
	}
	else
	{
		return 18;
	}
}
