#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define NUM_ROBOTS 4

void *thread_function(void *param);

typedef struct
{
	int sock;
	char color[16];
} thread_param;

int main(void)
{
	struct sockaddr_in	servaddr[NUM_ROBOTS];
	int sock[NUM_ROBOTS];
	char *ip_array[NUM_ROBOTS];
	int port_array[NUM_ROBOTS];
	char *robot[NUM_ROBOTS];
	int	x_coord[NUM_ROBOTS];
	int	y_coord[NUM_ROBOTS];
	int i, ret;
	pthread_t	tid[NUM_ROBOTS];
	thread_param	param[NUM_ROBOTS];

	robot[0] = "red";
	robot[1] = "blue";
	robot[2] = "orange";
	robot[3] = "white";

	ip_array[0] = "127.0.0.1";
	ip_array[1] = "127.0.0.1";
	ip_array[2] = "127.0.0.1";
	ip_array[3] = "127.0.0.1";

	port_array[0] = 65535;
	port_array[1] = 65534;
	port_array[2] = 65533;
	port_array[3] = 65532;

	for(i = 0; i < NUM_ROBOTS; i++)
	{
		sock[i] = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

		param[i].sock = sock[i];
		strcpy(param[i].color, robot[i]);

		memset(&servaddr[i], 0, sizeof(struct sockaddr_in));
		servaddr[i].sin_family = AF_INET;
		servaddr[i].sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr[i].sin_port = htons(port_array[i]);

		bind(sock[i], (struct sockaddr *)&servaddr[i], sizeof(struct sockaddr_in));
		listen(sock[i], 3);
		printf("listening and spawning thread for %s robot\n", robot[i]);
		pthread_create(&tid[i], NULL, thread_function, (void *)&param[i]);
	}

	for(i = 0; i < NUM_ROBOTS; i++)
	{
		printf("Waiting for %s robot to finish\n", robot[i]);
		pthread_join(tid[i], NULL);
	}
	printf("Closing sockets\n");

	for(i = 0; i < NUM_ROBOTS; i++)
	{
		close(sock[i]);
	}

	printf("Finished\n");
	return 0;
}

void *thread_function(void *param)
{
	thread_param *info = (thread_param *)param;
	char buffer[81] = {0};
	int sock;

	usleep(1000000);
	printf("%s thread started with socket %d\n", info->color, info->sock);
	sock = accept(info->sock, NULL, NULL);
	printf("%s bot accepted connection\n", info->color);

	while (1)
	{
		recv(sock, buffer, 80, 0);
		printf("<- [%s]\n", buffer);

		if (strcmp(buffer, "locate") == 0)
		{
			printf("<- locate\n");
			sprintf(buffer, "location %s (%d,%d)", info->color, 1, 2);
			printf("-> %s\n", buffer);
			send(sock, buffer, strlen(buffer) + 1, 0);
		}
	
		if (strcmp(buffer, "meet (1,2) (1,2) (1,2) (1,2)") == 0)
		{
			printf("<- meet (1,2) (1,2) (1,2) (1,2)\n");
			sprintf(buffer, "ok");
			printf("-> %s\n", buffer);
			send(sock, buffer, strlen(buffer) + 1, 0);
			return NULL;
		}
		usleep(1000000);
	}

}
