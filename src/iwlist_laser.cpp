#include "include.h"

#include "acpGarcia.h"
#include "acpLaser.h"
#include "aGarciaDefs.h"

int main(int argc, char *argv[])
{
	aSettingFileRef	settings;
	aIOLib		ioref;
	aErr		e;
	aStreamRef	logfile = NULL;
	FILE		*file;

	if (argc == 2)
	{
		file = fopen(argv[1], "wb");
	}
	else
	{
		file = fopen("laser.txt", "wb");
	}

	if (file == NULL)
	{
		perror("Unable to write file");
		return 1;
	}
  
	try
	{
		if(aIO_GetLibRef(&ioref, &e))
			throw acpException(e, "creating aIO reference");

		if(aStream_CreateFileOutput(ioref, 
			"aLaserDemoLog.txt", 
			aFileAreaBinary, 
			&logfile, 
			&e))
			throw acpException(e, "creating log file");
    
		if(aSettingFile_Create(ioref,
			32,
			"laser.config",
			&settings,
			&e))
			throw acpException(e, "creating settings file");
    
		acpLaser laser;

		// Send along a log file reference so we can see what is happening
		// alternatively, we could not care!
		// laser.init(settings);
		if (laser.init(settings, logfile))
		{
			aLaserInfo info;
			laser.getInfo(&info);

			// Show us what we have
			printf("Protocol (SCIP): %s \n",info.protocolVersion);
			printf("Serial Number: %s \n",info.serialNumber);
			printf("Model: %s \n",info.sensorModel);
			printf("Vendor: %s \n",info.vendorName);
			printf("Firmware: %s \n",info.firmware);  
			printf("Angular Step Resolution: %d \n",info.measurementSteps);      
			printf("Min Measurement Step: %d \n",info.measurementMinStep);  
			printf("Max Measurement Step: %d \n",info.measurementMaxStep);  
			printf("Min Distance: %d \n",info.minDistance);  
			printf("Max Distance Step: %d \n",info.maxDistance);  
      
			// Do a data sweep of the laser
			if ( laser.cmdScan() )
			{
				printf("Reviewing laser data\n");
				for (int i = 0; i < laser.getMeasurementSteps(); i++)
				{
					fprintf(file, "%lf ", laser.getReading(i));
				}
			}
		}
		else
		{
			printf("Laser not initialized.\n");
			if (logfile)
				printf("See log file aLaserDemoLog.txt in aBinary\n");
		}

		fclose(file);
    
		// Destroy our log file reference
		if(aStream_Destroy(ioref, logfile, &e))
			throw acpException(e, "destroying log file");
		// Clean up and get out of here!
		if(aIO_ReleaseLibRef(ioref, &e))
			throw acpException(e, "destroying aIO library");
	}
	catch (acpException& exception)
	{
		printf("Exception: %s\n", exception.msg());
	}
	catch (...)
	{
		printf("bailing with uncaught exception\n");
	}

	return 0;
}
