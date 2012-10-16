#include "include.h"

#ifdef _WIN32
	#include <winsock2.h>
#endif

#define NUM_ROBOTS 4

void meet(ref_t *ref, int num_ref, int *xmeet, int *ymeet, int *xrobot, int *yrobot, int num_robot);
void midpoint(int *x, int *y, int *x_coord, int *y_coord, int length);
void nearest(int *x, int *y, int *x_coord, int *y_coord, int length);
int locate(int *x, int *y);

int main(int argc, char *argv[])
{
	struct sockaddr_in	servaddr;
	int	sock;
	int	csock;	//client sock
	int	port_array;
	char	*robot[NUM_ROBOTS];
	int	bot;
	int	x_coord[NUM_ROBOTS];
	int	y_coord[NUM_ROBOTS];
	int	x_start, y_start;
	int	x, y;
	int	i, ret;
	int	flag = 0;
	int	hardcode = 0;

	char	*data;
	ref_t	*ref;
	dbh_t	*header;

	robot[0] = "red";
	robot[1] = "blue";
	robot[2] = "orange";
	robot[3] = "white";

	if (argc < 3)
	{
		printf("Usage: iwlist_listen <robot#> <port> [x y]\n");
		return 0;
	}

	printf("Port %d\n", atoi(argv[2]));
	if (argc == 5)
	{
		x_start = atoi(argv[3]);
		y_start = atoi(argv[4]);
		printf("Using hardcoded starting location (%d,%d)\n", x_start, y_start);
		hardcode = 1;
	}

	bot = atoi(argv[1]);

	if (bot > 3 || bot < 0)
	{
		printf("Robot numbers range [0,3]\n");
		return 0;
	}
	printf("%s robot\n", robot[bot]);


	// Load database
	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		printf("Unable to open db.bin");
		return 0;
	}

	header = (dbh_t *)data;
	ref = (ref_t *)(data + sizeof(dbh_t));



	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[2]));

	bind(sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	listen(sock, 3);
	printf("listening %s robot\n", robot[bot]);

	while (1)
	{
		char buffer[81] = {0};

		csock = accept(sock, NULL, NULL);
		printf("%s bot accepted connection\n", robot[bot]);

		while (1)
		{
			recv(csock, buffer, 80, 0);
			printf("<- [%s]\n", buffer);

			if (strcmp(buffer, "locate") == 0)
			{
				printf("<- locate\n");
				if (hardcode == 0)
				{
					if ( locate(&x_start, &y_start) )
					{
						printf("locate failed\n");
						continue;
					}
				}

				sprintf(buffer, "location %s (%d,%d)", robot[bot], x_start, y_start);
				printf("-> %s\n", buffer);
				send(csock, buffer, strlen(buffer) + 1, 0);
			}

			if ( 8 == sscanf(buffer, "meet (%d,%d) (%d,%d) (%d,%d) (%d,%d)",
				&x_coord[0], &y_coord[0],
				&x_coord[1], &y_coord[1],
				&x_coord[2], &y_coord[2],
				&x_coord[3], &y_coord[3]))
			{
				char cmd[80];

				printf("<- %s\n", buffer);
				sprintf(buffer, "ok");
				printf("-> %s\n", buffer);
				send(csock, buffer, strlen(buffer) + 1, 0);

				meet(ref, header->num_refpoints, &x, &y, x_coord, y_coord, NUM_ROBOTS);
				printf("Meeting at (%d,%d)\n", x, y);
				sprintf(cmd, "./iwlist_drive %d %d %d %d scan", x_start, y_start, x, y);
				printf("%s\n", cmd);
				system(cmd);
				break;
			}


			if (strcmp(buffer, "done") == 0)
			{
				sprintf(buffer, "ok");
				printf("-> %s\n", buffer);
				send(csock, buffer, strlen(buffer) + 1, 0);
				break;
			}
			usleep(1000000);
		}
		printf("connection complete\n");
	}

}

/*
	Converts an array of robot positions into nearest common meetpoint
		ie: find midpoint then closest reference point to midpoint
		input: reference point database, num refpoints, robot positions, robot position size
		returns (x,y) position of meet point
*/
void meet(ref_t *ref, int num_ref, int *x, int *y, int *xrobot, int *yrobot, int num_robot)
{
	int *xref;
	int *yref;
	int i;

	xref = (int *)malloc(num_ref * sizeof(int));
	if (xref == NULL)
	{
		perror("malloc() failed");
		return;
	}

	yref = (int *)malloc(num_ref * sizeof(int));
	if (yref == NULL)
	{
		perror("malloc() failed");
		return;
	}

	for(i = 0; i < num_ref; i++)
	{
		xref[i] = ref[i].x;
		yref[i] = ref[i].y;
	}

	midpoint(x, y, xrobot, yrobot, num_robot);
	nearest(x, y, xref, yref, num_ref);
}

/*
	Averages x, y array and rounds to nearest integer
*/
void midpoint(int *x, int *y, int *x_coord, int *y_coord, int length)
{
	float xf = 0.0f;
	float yf = 0.0f;
	int i;

	if (length == 0)
		return;

	for(i = 0; i < length; i++)
	{
		xf += x_coord[i];
		yf += y_coord[i];
	}
	xf /= length;
	yf /= length;


	// Round to nearest int
	if (xf - (int)xf >= 0.5)
	{
		*x = (int)xf + 1;
	}
	else
	{
		*x = (int)xf;
	}

	if (yf - (int)yf >= 0.5)
	{
		*y = (int)yf + 1;
	}
	else
	{
		*y = (int)yf;
	}
}


/*
	Finds nearest (x,y) point in database to given (x,y) point
*/
void nearest(int *x, int *y, int *x_coord, int *y_coord, int length)
{
	int min_distance = INT_MAX;
	int distance;
	int index = 0;
	int i;

	for(i = 0; i < length; i++)
	{
		distance = fabs((float)x_coord[i] - *x) + fabs((float)y_coord[i] - *y);
		if (distance < min_distance)
		{
			min_distance = distance;
			index = i;
		}
	}

	*x = x_coord[index];
	*y = y_coord[index];
}

/*
	Runs iwlist_match command and parses result
*/
int locate(int *x, int *y)
{
	char *data;
	char *pdata;

	system("./iwlist_match > match.txt"); // Fixme: call function directly
	data = get_file("match.txt");
	if (data == NULL)
	{
		printf("Unable to find results of iwlist_match\n");
		return 1;
	}

	pdata = strstr(data, "Sending");
	if (pdata == NULL)
	{
		printf("Unable to parse results of iwlist_match\n");
		free((void *)data);
		return 1;
	}

	sscanf(pdata, "Sending (%d,%d)", x, y);
	printf("wifi position is (%d,%d)\n", *x, *y);
	remove("match.txt");
	free((void *)data);
	return 0;
}
