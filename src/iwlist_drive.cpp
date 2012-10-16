#include "include.h"
#include "graph.h"

#include "acpGarcia.h"
#include "acpLaser.h"
#include "aGarciaDefs.h"

// for compass reading
#include <sys/ioctl.h> 
#include <linux/i2c-dev.h>

// prototypes
int start_garcia(acpGarcia &garcia, aIOLib &ioRef);
void close_garcia(acpGarcia &garcia, aIOLib &ioRef);
void print_status(int status);
void get_movement(float heading, int x_pos, int y_pos, int x, int y, float *turn, float *distance);
float get_compass(void);


//globals are bad
int global_status = 0;

/*
	These classes are convoluted and stupidly required by the acpGarcia API
	They return the status of movement commands asynchronously from where
	they are called in the main function.
*/
class myMove : public acpGarcia::move
{
public:
	myMove(acpGarcia* pGarcia, const float meters, const int index) : 
		acpGarcia::move(pGarcia, meters),
		m_index(index)
	{
	}

	void finish(const aByte condition, const short status) 
	{

		global_status = status;
	}

private:
	int m_index;
};

class myArc : public acpGarcia::arc
{
public:
	myArc(acpGarcia* pGarcia, const float metersRadius, 
		const float radians, const acpString name) : 
		acpGarcia::arc(pGarcia, metersRadius, radians),
		m_name(name)
	{
	}

	void finish(const aByte condition, const short status) 
	{
		global_status = status;
	}
private:
	acpString m_name;
};

/*
	This function initializes the garcia robot api
*/
int start_garcia(acpGarcia &garcia, aIOLib &ioRef)
{
	int retries = 0;
	aErr e;

	printf("start_garcia()\n");

	if (aIO_GetLibRef(&ioRef, &e))
		throw acpException(e, "getting aIO");
 
	printf("1: Configuring via garcia.config\n"); 
	// Configure garcia via the garcia.config setting file
	if (!garcia.init(ioRef))
		throw acpException(aErrUnknown, "initializing Garcia");

	printf("2: Connect to robot via settings in garcia.config\n");
	// Initiate communication with the robot
	if ((e = garcia.start()))
		throw acpException(e, "starting garcia");
  
	printf("3: Waiting for connection\n");
	printf("waiting.");

	while (!garcia.isConnected())
	{
		// show status and wait for half-second if not connected
		printf(".");
		aIO_MSSleep(ioRef, 500, NULL);
		// bail after 10 tries if we didn't connect
		if (retries > 10)
		{
			printf("\nunable to connect\n");
			return -1;
		}
		retries++;
	}
	printf("connected\n");
	printf("speed: %f\nbattery voltage: %f\nInitial compass bearing: %f\n", garcia.getSpeed(), garcia.getBatteryVoltage(), get_compass());
	garcia.setSpeed(0.3);
	return 0;
}


/*
	This function waits for queued movement to finish and releases the garcia api
*/
void close_garcia(acpGarcia &garcia, aIOLib &ioRef)
{
	aErr e;
	while (!garcia.isIdle())
		aIO_MSSleep(ioRef, 500, NULL);

	if (aIO_ReleaseLibRef(ioRef, &e))
		throw acpException(e, "releasing aIO");
}


/*
	This function ensures that an given (x,y) point is within the database
*/
int validate_point(ref_t *ref, int num_refpoints,  int x, int y)
{
	int i;

        for(i = 0; i < num_refpoints; i++)
        {
                if (x == ref[i].x && y == ref[i].y)
                {
                        return 0;
                }
        }
	return 1;
}

int main(const int argc, const char *argv[])
{
	acpGarcia	garcia;
	aIOLib		ioRef;
	Graph		graph;
	char		*data;
	node_t		*node;
	ref_t		*ref;
	dbh_t		*header;
	int		**point_to_seq;

	int		*path;
	int		path_length;
	float		heading = 0.0f;
	float		turn;
	float		distance;
	int		i;
	int		laser_scan = 0;
	int		status = 0;
	int		collision = 0;
	int		num_collisions = 0;


	if (argc < 5)
	{
		printf("Usage: iwlist_drive x_start y_start x_end y_end [scan]\n");
		return 0;
	}

	if (argc == 6)
	{
		laser_scan = 1;
	}

	int x_start = atoi(argv[1]);
	int y_start = atoi(argv[2]);
	int x_end = atoi(argv[3]);
	int y_end = atoi(argv[4]);
	int x_pos = x_start;
	int y_pos = y_start;
	int start;
	int end;


	// Load database
	data = (char *)get_file("db.bin");
	if (data == NULL)
	{
		printf("Unable to open db.bin\n");
		return 0;
	}

	header = (dbh_t *)data;
	ref = (ref_t *)(data + sizeof(dbh_t));

	// Validate input
	if ( validate_point(ref, header->num_refpoints, x_start, y_start) )
	{
		printf("Error: Invalid start point\n");
		return 0;
	}
	if ( validate_point(ref, header->num_refpoints, x_end, y_end) )
	{
		printf("Error: Invalid end point\n");
		return 0;
	}

	printf("Loaded database %d points %dx%d offset %dx%d\n", header->num_refpoints, header->width, header->height, header->xoffset, header->yoffset);


	// Load Graph
	node = (node_t *)get_file("graph.bin");
	if (node == NULL)
	{
		printf("Unable to open graph.bin");
		return 0;
	}
	graph.load(node, header->num_refpoints);

	// Create array that converts (x,y) points to sequence numbers used as graph nodes
	point_to_seq = (int **)malloc(header->width * sizeof(int *));
	if (point_to_seq == NULL)
	{
		perror("malloc failed");
		return 0;
	}

	for(i = 0; i < header->width; i++)
	{
		point_to_seq[i] = (int *)malloc(header->height * sizeof(int));
		if (point_to_seq[i] == NULL)
		{
			perror("malloc failed");
			return 0;
		}
	}


	for(int i = 0; i < header->num_refpoints; i++)
	{
		point_to_seq[ref[i].x + header->xoffset][ref[i].y + header->yoffset] = i;
	}

	// Get start and end sequence numbers and find shortest path
	start = point_to_seq[x_start + header->xoffset][y_start + header->yoffset];
	end = point_to_seq[x_end + header->xoffset][y_end + header->yoffset];
	path = graph.dijkstra_path(start, end, &path_length);
	printf("Attemping to move from sequence %d to %d (%d,%d)->(%d,%d)\n", start, end, x_start, y_start, x_end, y_end);

	try
	{
		float straight_path = 0.0f;


		printf("Creating garcia object\n");
		if (start_garcia(garcia, ioRef) != 0)
		{
			printf("start_garica() failed\n");
			return 0;
		}


		// path follow code should be taken into seperate function
		while (1)
		{
			// Loop through all discrete movements in the path
			for(i = 0; i < path_length; i++)
			{
				acpString name;
				acpGarcia::primitive *pPrimitive;
	
	
				get_movement(heading, x_pos, y_pos, ref[path[i]].x, ref[path[i]].y, &turn, &distance);
				printf("moving from (%d,%d)->(%d,%d) turn %f distance %f\n", x_pos, y_pos, ref[path[i]].x, ref[path[i]].y, turn, distance);
	
				if (turn)
				{
	
					/*
						Stopping every point when going straight looks bad so this code fixes that
						scan mode will still stop to take laser scans
						this mode makes it hard to know where we stopped due to failure
					*/
					if (straight_path > 0.0)
					{
						pPrimitive = new myMove(&garcia, straight_path * 0.94, i);
						garcia.queuePrimitive(pPrimitive);
						while (!garcia.isIdle())
							aIO_MSSleep(ioRef, 500, NULL);
						status = global_status;
	
						printf("move %d completed with status %d\n", i, status);
						if (status != aGARCIA_ERRFLAG_NORMAL)
						{
							print_status(status);
							collision = 1;
							break;
						}

						x_pos = ref[path[i]].x;
						y_pos = ref[path[i]].y;
					}
	
					pPrimitive = new myArc(&garcia, 0.0f, turn * 1.04 * (aPI / 180.0f), name.format("turn %d", turn));
					heading += turn;
					garcia.queuePrimitive(pPrimitive);
					while (!garcia.isIdle())
						aIO_MSSleep(ioRef, 500, NULL);
					status = global_status;
	
					printf("move %d completed with status %d\n", i, status);
					if (status != aGARCIA_ERRFLAG_NORMAL)
					{
						print_status(status);
						collision = 1;
						break;
					}

					printf("heading %f compass %f\n", heading, get_compass());
				}
	
				if (laser_scan)
				{
					char cmd[80] = {0};
	
					pPrimitive = new myMove(&garcia, distance * 0.94, i);
					garcia.queuePrimitive(pPrimitive);
					while (!garcia.isIdle())
						aIO_MSSleep(ioRef, 500, NULL);
					status = global_status;
	
					printf("move %d completed with status %d\n", i, status);
					if (status != aGARCIA_ERRFLAG_NORMAL)
					{
						print_status(status);
						collision = 1;
						break;
					}

					x_pos = ref[path[i]].x;
					y_pos = ref[path[i]].y;
					printf("writing laserscan%d-%d-%d.txt\n", x_pos, y_pos, (int)heading);
					sprintf(cmd, "./iwlist_laser laserscan%d-%d-%d.txt", x_pos, y_pos, heading);
					system(cmd);
				}
				else
				{
					straight_path += distance;
					x_pos = ref[path[i]].x;
					y_pos = ref[path[i]].y;
				}
			}

			if (collision)
			{
				acpGarcia::primitive *pPrimitive;

				num_collisions++;
				printf("***Collision occurred*** (%d total)\n", num_collisions);

				// try to backup. FIXME: should really update x_pos and y_pos after backing up
				pPrimitive = new myMove(&garcia, -0.94 / 2.0, -1);
				garcia.queuePrimitive(pPrimitive);
				while (!garcia.isIdle())
					aIO_MSSleep(ioRef, 500, NULL);
				status = global_status;


				// we dont really know how far along a straight path we moved, so give up
				if (laser_scan == 0)
				{
					printf("giving up :(\n");
					close_garcia(garcia, ioRef);
					break;
				}
					

				if (num_collisions > 10)
				{
					printf("Too many collisions\n");
					printf("At this point odometer data is incorrect or we are stuck\n");
					close_garcia(garcia, ioRef);
					return 0;
				}

				// add 10 to arc we failed to cross
				graph.modify_weight(point_to_seq[x_pos + header->xoffset][y_pos + header->yoffset], path[i], 10.0f);
	
				// make new path
				start = point_to_seq[x_pos + header->xoffset][y_pos + header->yoffset];
				end = point_to_seq[x_end + header->xoffset][y_end + header->yoffset];
				path = graph.dijkstra_path(start, end, &path_length);

				printf("Starting new path in 5 seconds\n");
				usleep(5000000);
				collision = 0;
				continue;
			}
	
	
			if (straight_path > 0)
			{
				acpGarcia::primitive *pPrimitive;
				pPrimitive = new myMove(&garcia, straight_path * 0.94, i);
				garcia.queuePrimitive(pPrimitive);
			}
	
			// Wait for everything to finish and exit
			printf("Exiting\n");
			close_garcia(garcia, ioRef);
			break;
		}
	}
	catch (const acpException& e)
	{
		printf("Exception caught [%d]: %s\n", e.error(), e.msg());
	}

	return 0;
}

/*
	All possible movement error messages
*/
void print_status(int status)
{
	switch(status)
	{
	case aGARCIA_ERRFLAG_NORMAL:
		printf("Terminated without error\n");
		break;
	case aGARCIA_ERRFLAG_STALL:
		printf("Detected stall\n");
		break;
	case aGARCIA_ERRFLAG_FRONTR_LEFT:
		printf("front left ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_FRONTR_RIGHT:
		printf("front right ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_REARR_LEFT:
		printf("rear left ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_REARR_RIGHT:
		printf("rear right ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_SIDER_LEFT:
		printf("left ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_SIDER_RIGHT:
		printf("right ranger exceeded threshold\n");
		break;
	case aGARCIA_ERRFLAG_FALL_LEFT:
		printf("left fall detector triggered\n");
		break;
	case aGARCIA_ERRFLAG_FALL_RIGHT:
		printf("right fall dectector triggered\n");
		break;
	case aGARCIA_ERRFLAG_ABORT:
		printf("User aborted\n");
		break;
	case aGARCIA_ERRFLAG_NOTEXECUTED:
		printf("Primitive awaiting execution\n");
		break;
	case aGARCIA_ERRFLAG_WONTEXECUTE:
		printf("Primitive dropped due to previous failure\n");
		break;
	case aGARCIA_ERRFLAG_BATT:
		printf("***Low battery***\n");
		break;
	case aGARCIA_ERRFLAG_IRRX:
		printf("IR command received during execution\n");
		break;
	}
}


/*
	This function converts movement from sequence point A to B into a turn angle and movement distance
*/
void get_movement(float heading, int x_pos, int y_pos, int x, int y, float *turn, float *distance)
{
	int dist = 0;
	int angle = 0;
	*turn = 0.0f;

	// going right
	if (x - x_pos > 0)
	{
		dist++;
		angle = 1;
	}
	else if ( x == x_pos )
	{
	}
	else
	{
		dist++;
		angle = 2;
	}

	// going up
	if (y - y_pos > 0)
	{
		dist++;
		if (angle == 1)
			*turn = 45.0f;
		else if (angle == 0)
			*turn = 90.0f;
		else
			*turn = 135.0f;
	}
	else if (y == y_pos )
	{
		if (angle == 2)
			*turn = 180.0f;
	}
	else
	{
		dist++;
		if (angle == 1)
			*turn = 315.0f;
		else if (angle == 0)
			*turn = 270.0f;
		else
			*turn = 225.0f;
	}

	// FIXME: This hardcodes distance, distance should be equal to weight stored in graph
	if (dist == 2)
	{
		// going diagonal
		*distance = sqrt(2);
	}
	else
	{
		//going straight
		*distance = 1.0f;
	}

	*turn -= heading;

	while (*turn < 0.0f)
	{
		*turn += 360.0f;
	}


// Commented out because the robot was not turning with negative turn angle
/*
	if (*turn > 180.0f)
	{
		*turn -= 360.0f;
	}
*/
	return;
}

float get_compass(void)
{
        int file;
        char device[] = "/dev/i2c-0";
        int addr = 0xC0;
        char buf[4];
        char result[4] = {0};


        file = open(device, O_RDWR);
        if (file < 0)
        {
                //couldnt open
                perror("Failed to open i2c device");
                return -1;
        }

        if (ioctl(file, I2C_SLAVE, addr >> 1) < 0)
        {
                // check errno
                perror("Error setting address");
                return -1;
        }

        buf[0] = 2; // register to read from
        if (write(file, buf, 1) != 1)
        {
                printf("failed\n");
                return -1;
        }

        if (read(file, buf, 2) != 2)
        {
                printf("failed\n");
                return -1;
        }
        result[1] = buf[0];
        result[0] = buf[1];
        close(file);

        return *((int *)result) / 10.0f;
}
