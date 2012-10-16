#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <math.h>
#include "include.h"

int graph_text(dbh_t *header, ref_t *ref, float distance)
{
	FILE	*file;
	int	i, j;
	float	root2 = sqrt(2 * distance * distance);


	file = fopen("graph.txt", "w");
	if (file == NULL)
	{
		perror("Unable to open graph.txt for writing");
		return 0;
	}

	for(i = 0; i < header->num_refpoints; i++)
	{
		for(j = 0; j < header->num_refpoints; j++)
		{
			int a = ref[i].x - ref[j].x;
			int b = ref[i].y - ref[j].y;

			if (i == j)
				continue;

			if ( a * a + b * b <= 2 )
			{
				float weight = (a == 0 || b == 0) ? distance : root2;

				fprintf(file, "%d %d %f\n", i+1, j+1, weight);
			}
		}
	}
	fclose(file);

	return 0;
}

int main(int argc, char *argv[])
{
	FILE	*file;
	dbh_t	*header;
	ref_t	*ref;
	arc_t	arc[MAX_ARC];
	int	num_arcs;
	node_t	*node;
	char	*data;
	char	*pdata;
	int	i, j;
	int	ret = 2;
	float	f = 1.0f;


	// ensure floating point loaded by visual studio
	f = f - 1.0;

	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		printf("Unable to open db.bin\n");
		return 0;
	}
	header = (dbh_t *)data;
	ref = (ref_t *)(data + sizeof(dbh_t));

	printf("Use \"create_graph --default [distance]\" to automatically generated graph.txt\n");
	if (argc == 2)
	{
		printf("Generating default graph.txt\n");
		graph_text(header, ref, 1.0f);
		return 0;
	}

	if (argc == 3)
	{
		float distance = atof(argv[3]);

		printf("Generating default graph.txt with distance set to %f\n", distance);
		graph_text(header, ref, distance);
		return 0;
	}


	printf("Generating graph using graph.txt\n");

	node = (node_t *)calloc(header->num_refpoints, sizeof(node_t));
	if (node == NULL)
	{
		perror("calloc() failed");
		return 0;
	}

	data = (char *)get_file("graph.txt");
	if (data == NULL)
	{
		printf("Unable to open graph.txt\n");
		return 1;
	}

	pdata = data;
	for(i = 0; ret && i < MAX_ARC; i++)
	{
		ret = sscanf(pdata, "%d %d %f", &arc[i].a, &arc[i].b, &arc[i].weight);

		// convert sequence to index -- sequence numbers are not necessarily one less than index
		arc[i].a = arc[i].a - 1;
		arc[i].b = arc[i].b - 1;

		pdata = (char *)strstr(pdata, "\n");
		if (pdata == NULL)
			break;
		pdata++;
	}
	free((void *)data);
	num_arcs = i;

	printf("Found %d arcs\n", num_arcs);

	if (MAX_ARC == i)
	{
		printf("Warning: MAX_ARC hit, most likely missing arc data.\n");
	}


	for(i = 0; i < header->num_refpoints; i++)
	{
		for (j = 0; j < num_arcs; j++)
		{
			if (arc[j].a == i)
			{
				node[i].arc[node[i].num_arcs] = arc[j];
				node[i].num_arcs++;

				if (node[i].num_arcs == 9)
				{
					printf("Node has too many arcs!\n");
					return 0;
				}
			}
		}
	}


	file = fopen("graph.bin", "wb");
	if (file == NULL)
	{
		perror("Unable to write file");
		return 1;
	}

	fwrite(node, sizeof(node_t), header->num_refpoints, file);
	fclose(file);
	return 0;
}
