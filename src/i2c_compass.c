#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <linux/i2c-dev.h>

int main(void)
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
                return 0;
        }

        if (ioctl(file, I2C_SLAVE, addr >> 1) < 0)
        {
                // check errno
                perror("Error setting address");
                return 0;
        }


        buf[0] = 0; // register 0
        if (write(file, buf, 1) != 1)
        {
                printf("failed\n");
                return 0;
        }

        if (read(file, buf, 1) != 1)
        {
                printf("failed\n");
                return 0;
        }
        printf("Compass software revision %d\n", (int)buf[0]);

        buf[0] = 1; //register 1
        if (write(file, buf, 1) != 1)
        {
                printf("failed\n");
                return 0;
        }
        if (read(file, buf, 1) != 1)
        {
                printf("failed\n");
                return 0;
        }
        printf("Compass bearing as byte [0-255] %d\n", (int)buf[0]);


        buf[0] = 2; // register to read from
        if (write(file, buf, 1) != 1)
        {
                printf("failed\n");
                return 0;
        }

        if (read(file, buf, 2) != 2)
        {
                printf("failed\n");
                return 0;
        }
        result[1] = buf[0];
        result[0] = buf[1];
        close(file);

        printf("compass bearing as short [0-3599] %d\n", *((int *)result));

        return 0;
}

