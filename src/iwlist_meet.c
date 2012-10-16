#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
	#include <winsock.h>
#else
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

	#define closesocket close
	#define SOCKET_ERROR -1
#endif


#define NUM_ROBOTS 4

int main(void)
{
#ifdef WIN32
	WSADATA	WSAData;
#endif
	struct sockaddr_in	servaddr[NUM_ROBOTS];
	int sock[NUM_ROBOTS];
	char *ip_array[NUM_ROBOTS];
	int port_array[NUM_ROBOTS];
	char *robot[NUM_ROBOTS];
	int	x_coord[NUM_ROBOTS];
	int	y_coord[NUM_ROBOTS];
	int i, ret;

	robot[0] = "red";
	robot[1] = "blue";
	robot[2] = "orange";
	robot[3] = "white";

	ip_array[0] = "129.120.3.14";
	ip_array[1] = "129.120.3.12";
	ip_array[2] = "129.120.3.15";
	ip_array[3] = "129.120.3.13";

	port_array[0] = 65535;
	port_array[1] = 65534;
	port_array[2] = 65533;
	port_array[3] = 65532;

#ifdef WIN32
	WSAStartup(MAKEWORD(2,0), &WSAData);
#endif

	for(i = 0; i < NUM_ROBOTS; i++)
	{
		sock[i] = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

		memset(&servaddr[i], 0, sizeof(struct sockaddr_in));
		servaddr[i].sin_family = AF_INET;
		servaddr[i].sin_addr.s_addr = inet_addr(ip_array[i]);
		servaddr[i].sin_port = htons(port_array[i]);

		// 3 way handshake
		printf("Attempting to connect to %s bot\n", robot[i]);
		ret = connect(sock[i], (struct sockaddr *)&servaddr[i], sizeof(struct sockaddr_in));

		if (ret == SOCKET_ERROR)
		{
#ifdef WIN32
			ret = WSAGetLastError();

			switch(ret)
			{
			case WSAETIMEDOUT:
				printf("Fatal Error: Connecting to robot %s timed out.\n", robot[i]);
				break;
			case WSAECONNREFUSED:
				printf("Fatal Error: robot %s refused connection.\n(Server program is not running)\n", robot[i]);
				break;
			case WSAEHOSTUNREACH:
				printf("Fatal Error: router sent ICMP packet (destination unreachable)\n");
				break;
			default:
				printf("Fatal Error: %d\n", ret);
				break;
			}
#else
			ret = errno;

                        switch(ret)
                        {
			case ENETUNREACH:
				printf("Fatal Error: The network is unreachable from this host at this time.\n(Bad IP address)\n");
				break;
                        case ETIMEDOUT:
                                printf("Fatal Error: Connecting to robot %s timed out.\n", robot[i]);
                                break;
                        case ECONNREFUSED:
                                printf("Fatal Error: robot %s refused connection.\n(Server program is not running)\n", robot[i]);
                                break;
                        case EHOSTUNREACH:
                                printf("Fatal Error: router sent ICMP packet (destination unreachable)\n");
                                break;
                        default:
                                printf("Fatal Error: %d\n", ret);
                                break;
                        }
#endif
			return 0;
		}
		printf("TCP handshake completed with robot %s\n", robot[i]);
	}

	printf("Collecting location information\n");
	for(i = 0; i < NUM_ROBOTS; i++)
	{
		char *cmd = "locate";
		char buffer[81] = {0};
		char color[16];
		int x, y;

		printf("%s -> locate\n", robot[i]);
		send( sock[i], cmd, 7, 0);
		recv( sock[i], buffer, 80, 0);

		if (3 == sscanf(buffer, "location %s (%d,%d)", color, &x, &y ) )
		{
			if ( strcmp(color, robot[i]) == 0 )
			{
				printf("%s <- %s\n", robot[i], buffer);
				x_coord[i] = x;
				y_coord[i] = y;
			}
			else
			{
				printf("Fatal Error: [%s bot] Recieved wrong robot color.\n", robot[i]);
				return 0;
			}
		}
		else
		{
			printf("Fatal Error: [%s bot] Garbage response\n", robot[i]);
			return 0;
		}
	}


	printf("Robots reported following locations\n");
	for(i = 0; i < NUM_ROBOTS; i++)
	{
		printf("%s (%d, %d)\n", robot[i], x_coord[i], y_coord[i]);
	}

	printf("Sending meet command\n");
	for(i = 0; i < NUM_ROBOTS; i++)
	{
		char buffer[81] = {0};

		sprintf(buffer, "meet (%d,%d) (%d,%d) (%d,%d) (%d,%d)",
			x_coord[0], y_coord[0],
			x_coord[1], y_coord[1],
			x_coord[2], y_coord[2],
			x_coord[3], y_coord[3]  );

		printf("%s -> %s\n", robot[i], buffer);
		send( sock[i], buffer, strlen(buffer) + 1, 0);
		recv( sock[i], buffer, 80, 0);

		if ( strcmp(buffer, "ok") != 0 )
		{
			printf("Fatal Error: [%s bot] Garbage response\n", robot[i]);
			return 0;
		}

		printf("%s <- %s\n", robot[i], buffer);
		closesocket(sock[i]);
	}

	printf("Finished Successfully!\n");
	return 0;
}
