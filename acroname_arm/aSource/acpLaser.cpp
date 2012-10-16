/////////////////////////////////////////////////////////////////////
//                                                                 //
// file: acpLaser.cpp                                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// description: Implementation of Acroname Laser object.           //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// Copyright 1994-2008. Acroname Inc.                              //
//                                                                 //
// This software is the property of Acroname Inc.  Any             //
// distribution, sale, transmission, or re-use of this code is     //
// strictly forbidden except with permission from Acroname Inc.    //
//                                                                 //
// To the full extent allowed by law, Acroname Inc. also excludes  //
// for itself and its suppliers any liability, wheither based in   //
// contract or tort (including negligence), for direct,            //
// incidental, consequential, indirect, special, or punitive       //
// damages of any kind, or for loss of revenue or profits, loss of //
// business, loss of information or data, or other financial loss  //
// arising out of or in connection with this software, even if     //
// Acroname Inc. has been advised of the possibility of such       //
// damages.                                                        //
//                                                                 //
// Acroname Inc.                                                   //
// www.acroname.com                                                //
// 720-564-0373                                                    //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include "acpLaser.h"

#define aLASER_DEG2RAD ((float)aPI / 180.0f)

/////////////////////////////////////////////////////////////////////
// SCIP commands. They are slightly different depending on which 
// communication protocol is used. 
#define aSCIP1 0
#define aSCIP2 1
#define aLASER_CMD_AVAILABLE 6

#define aLASER_CMD_VERSION	    0
#define aLASER_CMD_BAUDRATE	    1
#define aLASER_CMD_ON		    2
#define aLASER_CMD_OFF		    3
#define aLASER_CMD_SPECIFICATIONS   4
#define aLASER_CMD_RESET	    5
#define aLASER_CMD_USESCIP2	    6

static const char* sProtocols[2][aLASER_CMD_AVAILABLE] = {
{ "V\n", 
"S%d\n",
"",
"",
"",
""
}, 
{ 
"VV\n",    // version information
"SS%d\n",  // Change baudrate
"BM\n", // turn laser and motor on
"QT\n", // turn laser and motor off
"PP\n",  // get laser detail specifications
"RS\n" // reset laser
}
};

/////////////////////////////////////////////////////////////////////
// Laser object constructor
acpLaser::acpLaser() : 
m_laserStream(NULL),
m_log(NULL),
m_bInited(false)
{
  aErr e;
  
  // Create an aIO library reference
  if (aIO_GetLibRef(&m_ioRef, &e))
    throw acpException(e, "creating aIO reference");
}


/////////////////////////////////////////////////////////////////////
// Laser object destructor
acpLaser::~acpLaser() 
{
  aErr e;
  
  if (m_laserStream && aStream_Destroy(m_ioRef, m_laserStream, &e))
    throw acpException(e, "cleaning up link stream");
  
  if (aIO_ReleaseLibRef(m_ioRef, &e))
    throw acpException(e, "releasing aIO reference");
  
}


/////////////////////////////////////////////////////////////////////
//
// Tries to establish communication at the specified baudrate.  The
// baudrate passed in overrides the settings file baudrate.  The rest
// of the connection settings then come from the settings file.
// If it connects, it collects and validates the version information.

bool 
acpLaser::establishLink(void) 
{
  aErr e;
  
  // clean out any previous link
  if (m_laserStream) {
    if (aStream_Destroy(m_ioRef, m_laserStream, &e))
      throw acpException(e, "cleaning up link stream");
    m_laserStream = NULL;
  }  
  
  // create a stream based on the settings
  // if the stream cannot be created, then
  // report as to why
  if (aStream_CreateFromSettings(m_ioRef, m_settings, 
				 &m_laserStream, &e)) 
  {
    if (m_log) {
      
      // Tell us what kind of link type we are using
      char* pLinktype;
      aSettingFile_GetString(m_ioRef, 
			     m_settings, 
			     "linktype", 
			     &pLinktype, 
			     "serial", 
			     NULL);
      
      acpString pLink;
      
      if (!aStringCompare(pLinktype, "serial")) {
	
	char* pPortname;
	
	aSettingFile_GetString(m_ioRef, 
			       m_settings, 
			       "portname", 
			       &pPortname, 
			       aLASER_DEFAULT_PORTNAME, 
			       NULL);

	int pBaudrate;
	aSettingFile_GetInt(m_ioRef, 
			       m_settings, 
			       "baudrate", 
			       &pBaudrate, 
			       9600, 
			       NULL);

	pLink = pPortname;
	pLink += " at ";
	pLink += pBaudrate;
	pLink += " baudrate";
	
      } else if ((!aStringCompare(pLinktype, "ip"))) 
      {
	
	char* buf;
	aSettingFile_GetString(m_ioRef, 
			       m_settings, 
			       "ip_address", 
			       &buf, 
			       "127.0.0.1", 
			       NULL);	
	
	
	

	
	pLink = buf;
	pLink += ":";
	
	char* pIPPort;
	aSettingFile_GetString(m_ioRef, 
			       m_settings, 
			       "ip_port", 
			       &pIPPort, 
			       "8000", 
			       NULL);
	pLink += pIPPort;
      }
      
      
      acpString errMsg;
      switch (e) {
	case aErrNotFound:
	  errMsg += " (not found)";
	  break;
	case aErrIO:
	case aErrConnection:
	  errMsg += " (in-use or not found)";
	  break;
	case aErrBusy:
	  errMsg += " (busy)";
	  break;
	case aErrPermission:
	  errMsg += " (permission denied)";
	  break;
	default:
	  // we don't add any more text for other link failures
	  break;
      } // switch
      
      
      
      acpString info("Laser link failed. Attempted ");
      info += pLinktype;
      info += " connection at ";
      info += pLink;
      info += errMsg;
      
      log(info);
      
    }
    
    return false;
  }
  
  return true;
  
} // establishComm


/////////////////////////////////////////////////////////////////////
// This function collects information from the laser (when it can)
// and fills in a data structure with this information. 
// Proper usage would first establish a link with the laser

bool
acpLaser::gatherInfo(void)
{
  aErr e;
  
  // Get the communication protocal format, may want to use something
  // else
  if (aSettingFile_GetFloat(m_ioRef, 
			    m_settings, 
			    aLASER_PROTOCOL_KEY, 
			    &m_fProtocol, 
			    aLASER_SCIP2_0, 
			    &e))
    throw acpException(e, "getting communication protocol from settings");
  
  
  // Change the SCIP protocol 2.0
  // unless the user does not want us to based on the settings file
  if (!(m_fProtocol == aLASER_SCIP1_0))
    cmdUseSCIP2();
  
  // Define laser command specific to which model and protocol to use
  // Set up the 2-byte scan command and relevenant encoding approaches
  if (m_fProtocol == aLASER_SCIP2_0)
    m_pCurrentProtocol = sProtocols[aSCIP2];
  else if (m_fProtocol == aLASER_SCIP1_0)
    m_pCurrentProtocol = sProtocols[aSCIP1];
  else {
    acpString info("Laser communication protocol SCIP ");
    info += m_fProtocol;
    info += " not defined";
    log(info);
    
    return false;
  }  
  
  // Learn about the laser we are using
  if(!cmdVersion())
    return false;
  
  // Get the laser specifications
  if(!cmdSpecifications())
    return false;
  
  // Determine our detection angle range
  // This is the only value not reported by the dang sensor
  acpString model(m_info.sensorModel);
  
  if (!strncmp(model, "UTM-30LX", 8))
    m_info.detectionAngle = 270.25f * aLASER_DEG2RAD;
  else if (!strncmp( model,"URG-04LX", 8))
    m_info.detectionAngle = 239.77f * aLASER_DEG2RAD;
  else if (!strncmp( model,"UBG-05LX-F01", 12))
    m_info.detectionAngle = 239.77f * aLASER_DEG2RAD;
  else if (!strncmp( model,"UHG-08LX", 8))
    m_info.detectionAngle = 270.35f * aLASER_DEG2RAD;
  else
    m_info.detectionAngle = 270.0f * aLASER_DEG2RAD;
  
  // Check the setting file so see if we should change our 
  // max sample index value
  // otherwise, the range is the max value of the particular ranger
  if (aSettingFile_GetInt(m_ioRef, 
			  m_settings, 
			  aLASER_MAXSTEP_KEY, 
			  &m_maximumStep, 
			  m_info.measurementMaxStep, 
			  &e))
    throw acpException(e, "getting max step from settings");  
  
  // Check the setting file so see if we should change 
  // our min sample index value
  // otherwise, the range is the min value of the particular ranger
  if (aSettingFile_GetInt(m_ioRef, 
			  m_settings, 
			  aLASER_MINSTEP_KEY, 
			  &m_minimumStep, 
			  m_info.measurementMinStep, 
			  &e))
    throw acpException(e, "getting min step from settings");
  
  // Check the max value range
  if (m_maximumStep > m_info.measurementMaxStep)
    m_maximumStep = m_info.measurementMaxStep;
  if (m_maximumStep < m_minimumStep) {
    m_maximumStep = m_info.measurementMaxStep;
    m_minimumStep = m_info.measurementMinStep;
  }
  
  // check the min value range
  if (m_minimumStep < m_info.measurementMinStep)
    m_minimumStep = m_info.measurementMinStep;
  
  // Determine our scan window
  m_measurementSteps = m_maximumStep - m_minimumStep;
  
  // Now that we know about our laser model, let's decide which
  // scanning encoding method we should us
  // For the moment only use 2 character encoding
  // if the model does not support a protocol (SCIP 1.0), 
  // or the desired SCIP protocol is not supported, we 
  // would have already bailed in this routine.
  if (m_fProtocol == aLASER_SCIP2_0) {
    aSNPRINTF(m_scanCommand, aLASER_CMDLEN, "GS%04d%04d%02d\n", 
	      m_info.measurementMinStep, 
	      m_info.measurementMaxStep, 1);
  } else if (m_fProtocol == aLASER_SCIP1_0) {
    aSNPRINTF(m_scanCommand, aLASER_CMDLEN, "G%03d%03d%02d\n", 
	      m_info.measurementMinStep, m_info.measurementMaxStep, 1);
  }
  
  return true;
}

/////////////////////////////////////////////////////////////////////
// Initialization function to:
// 1.) get the settings file and log stream
// 2.) establish a link with the laser
// 3.) gather information from the laser
// 4.) zero out memory buffers
// 5.) turn the laser on
bool 
acpLaser::init(aSettingFileRef &settings, 
	       aStreamRef logger /*= NULL*/) 
{
  
  // Nab the log file stream reference, if passed in
  if (logger)
    m_log = logger;
  
  // Grab the settings file
  m_settings = settings;
  
  log("Initializing the laser");
  
  // establish a link communication with the laser
  if (!establishLink())
    return false;
  
  // Gather information from laser
  if (!gatherInfo())
    return false;
  
  // one-time initialization for laser reading buffer
  aBZero(m_vals, sizeof(m_vals));
  
  // Set our initialized flag to true
  m_bInited = true;
  
  // Turn on the laser if all is good
  cmdMotorOn();
  
  return true;
  
} // laserInit


/////////////////////////////////////////////////////////////////////
// Abstraction function to read data from the link stream. 
// All communication from the laser will utilize this function. 

bool 
acpLaser::readLine(char* line,
		   const int nMaxChars)
{
  aErr e;
  unsigned long now, done;
  
  // initialize
  line[0] = 0;
  
  if (aIO_GetMSTicks(m_ioRef, &now, &e))
    throw acpException(e, "getting time");
  
  done = now + aLASER_MSTIMEOUT;
  
  while (now < done) {
    aStream_ReadLine(m_ioRef, m_laserStream, line, nMaxChars, &e);
    if (e == aErrNone)
      return true;
    else if (e == aErrNotReady)
      aIO_GetMSTicks(m_ioRef, &now, &e);
    else if (e == aErrConnection)
      aIO_GetMSTicks(m_ioRef, &now, &e);      
    else 
      throw acpException(e, "reading in readLine");
  }
  
  return false;
  
} // readLine


/////////////////////////////////////////////////////////////////////
// Abstraction function to write data out the link stream.
// All communication to the laser will implement this function.

bool
acpLaser::writeLine(const char* line)
{
  aErr e;
  
  // write the data to the link
  if(aStream_WriteLine(m_ioRef, m_laserStream, line, &e))
    throw acpException (e, "writing to laser");
  
  return true;
  
}


/////////////////////////////////////////////////////////////////////
// This function communicates with the laser to learn as much information
// as possible from it. 
// The version details such as serial number, firmware version, vendor, 
// product name and communication protocol are discovered. 

bool 
acpLaser::cmdVersion(void)
{
  log("Requesting version information from laser");
  
  // Write the command to learn about the laser
  writeLine(m_pCurrentProtocol[aLASER_CMD_VERSION]);
  
  // look for repsonse from laser
  char reply[100];
  
  // place holder for information
  acpString info;
  
  if (!readLine(reply, 100))
    return false;
  if (reply[0] != 'V')
    return false;
  
  if (!readLine(reply, 100))
    return false;
  if (errorCode(reply)) // status ok?
    return false;
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "VEND:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  info.copyToBuffer(m_info.vendorName, 100);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "PROD:", 5))
    return false;
  if (m_fProtocol == aLASER_SCIP1_0) {
    info = &reply[20];
    info.trimTo('(',true);
    info.copyToBuffer(m_info.sensorModel, 17);
  }
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "FIRM:", 5))
    return false;
  info = &reply[5];
  info.trimTo('(',true);
  info.copyToBuffer(m_info.firmware, 17);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "PROT:", 5))
    return false;
  if (m_fProtocol == aLASER_SCIP2_0) {
    info = &reply[10];
    info.trimTo(';',true);
  }
  else {
    info = &reply[17];
    info.trimTo(')', true);
  }  
  info.copyToBuffer(m_info.protocolVersion, 100);
  
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "SERI:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  info.copyToBuffer(m_info.serialNumber, 100);
  
  // toss the remaining lines because they vary between laser versions
  while (readLine(reply, 100)) {
    // toss until we get an empty line
    if (!reply[0])
      break;
  }
  
  return true;
}


/////////////////////////////////////////////////////////////////////
// Take scan from the laser

bool 
acpLaser::cmdScan(void) 
{
  char reply[100];
  
  // Check to see if the laser is initialized first
  if (!m_bInited) {
    return false;
  }
  
  log("Single laser scan");
  
  // Send the scan command
  writeLine(m_scanCommand);
  
  // We need to grab the echo from laser
  if(!readLine(reply, 100))
    return false;
  
  if (!aStringCompare(reply,m_scanCommand))
    return false;
  
  // We need to grab the error code
  if(!readLine(reply, 100))
    return false;
  
  if(errorCode(reply))
    return false;
  
  // Grab the time stamp
  if (m_fProtocol != aLASER_SCIP1_0) {
    if(!readLine(reply, 100))
      return false;
  }
  
  
  // We need to grab a line of data
  // All the next lines should be the laser scanning data
  int dataIndex = 0;
  int value = 0;
  
  do {
    if(!readLine(reply, 100)) {
      break;
    }
    
    // copy the line data
    char* pValues = reply;
    
    // chew through it, fast as possible right now
    // look for Line feed to end data decoding
    // Depending on protocol, we either need to account for the 
    // check sum byte, or just move on!
    do {
      
      // Decode the high byte
      value = (*pValues - 0x30) << 6;
      pValues++; 
      
      // check for line feed, break if we see it
      if (*pValues == '\0')
	break;      
      
      // Decode the low byte
      value |= *pValues - 0x30;
      pValues++; 
      
      if (dataIndex < m_info.measurementSteps)
	m_vals[dataIndex++] = (float) value / 1000.0f;
      
    } while (!(*pValues == '\0'));
    
    // back up the data index (will overwrite next go round
    if (m_fProtocol != aLASER_SCIP1_0)
      dataIndex--;
    
  } while (*reply);
  
  return true;
}

/////////////////////////////////////////////////////////////////////
// This function will switch off the laser, disabling the ability
// to take measurment readings. This command will only work with 
// SCIP 2.0 mode.

bool 
acpLaser::cmdMotorOff(void)
{
  acpString reply;
  
  if (m_fProtocol == aLASER_SCIP1_0)
    return false;
  
  log ("Turning motor off");
  
  // Send the stop command
  writeLine((const char *)m_pCurrentProtocol[aLASER_CMD_OFF]);
  
  // Throw out the echo response 
  if (!readLine(reply, 10))
    return false;
  
  // Throw out the error code 
  if (!readLine(reply, 10))
    return false;
  
  return true;
}

/////////////////////////////////////////////////////////////////////
// This function will illuminate the sensor's laser enabling measurement.
// When a laser operating in SCIP 2.0 mode, the laser will default
// to an off state by default. The green LED on the laser should be
// glowing continuously when switched on.

bool 
acpLaser::cmdMotorOn(void)
{
  acpString reply;
  
  if (m_fProtocol == aLASER_SCIP1_0)
    return false;
  
  log("Turning motor on");
  
  // Turn the motor on 
  writeLine((const char*)m_pCurrentProtocol[aLASER_CMD_ON]);
  
  // toss the remaining lines because they vary between laser versions
  while (readLine(reply, 100)) {
    
    // toss until we get an empty line
    if (reply.endsWith(NULL))
      break;
  }
  
#if 0
  int result = 0;
  
  acpString status(reply);
  status.substring(0, 2);
  aIntFromString(&result, status);
  
  if (result == 1) {
    log("Unable to turn motor on due to laser malfunction");
  }
  
#endif
  
  return true;
}

/////////////////////////////////////////////////////////////////////
// This function will reset the laser to all the default settings
// when the laser was turned on. 
// This turns the laser off, sets motor speed and bit rate
// back to default as well as reset sensor's internal timer.
// This will NOT return communication protocol to SCIP 1.0

bool 
acpLaser::cmdReset(void)
{
  acpString reply;
  
  log ("Reset laser");
  
  // Turn the motor on 
  writeLine((const char*)m_pCurrentProtocol[aLASER_CMD_RESET]);
  
  // Throw out the echo response 
  if (!readLine(reply, 10))
    return false;
  
  // Check the error status
  if (errorCode(reply))
    return false;
  
  // If there is an error, let us know why
  // 0 is successful
  if(!errorCode(reply))
    return false;
  
  // toss the remaining lines because they vary between laser versions
  while (readLine(reply, 100)) {
    
    // toss until we get an empty line
    if (reply.endsWith(NULL))
      break;
  }
  
  return true;
}


/////////////////////////////////////////////////////////////////////
// Changes laser baudrate
// When a sensor is connected via USB, this command will not have 
// any effect on the communication speed. It will only take effect when
// the laser connection is changed to RS232. 

bool
acpLaser::cmdSetBaudrate(const int baudrate) 
{
  acpString buffer;
  acpString reply;
  int result = 0;
  
  // Build the appropriate packet
  buffer.format(m_pCurrentProtocol[aLASER_CMD_BAUDRATE], baudrate);
  
  log("Adjusting baudrate");
  
  // Send the stop command
  writeLine(buffer);
  
  // Throw out the response 
  if (!readLine(reply, 100))
    return false;
  
  if (reply != 'S')
    return false;
  
  // Get the error code
  if (!readLine(reply, 100))
    return false;
  
  if (errorCode(reply)) // Error code test
    return false;
  
  // Do something with the response code, if we want
  acpString status(reply);
  status.substring(0, 2);
  aIntFromString(&result, status);
  
  // Error code results from documentation
  switch (result) {
    case 0: // 00 - command received without error
    case 1: // 01 - bit rate has non numeric value
    case 2: // 02 - invalid bit rate
    case 3: // 03 - sensor running at defined bit rate
    case 4: // 04 - not compatible with sensor model
      break;
    default:
      return false;
      break;
  }
  
  if (!readLine(reply, 100)) // Terminating line feed
    return false;
  
  return true;  
}

/////////////////////////////////////////////////////////////////////
// This command supplies the sensor specifications. 

bool 
acpLaser::cmdSpecifications(void)
{
  if (m_fProtocol == aLASER_SCIP1_0) {
    
    // Not all models can operate with SCIP 1.0
    if (!strncmp(m_info.sensorModel ,"URG-04LX", 8)) {
      
      // If we plan to use SCIP 1.0, then we 
      // have to be using the URG-04LX
      m_info.measurementMinStep = 44;
      m_info.measurementMaxStep = 725;
      m_info.measurementSteps = 1024;
      m_info.minDistance = 20;
      m_info.maxDistance = 5600;
      m_info.frontStep = 384;
      m_info.motorSpeed = 600;
      
      return true;
      
    } else {
      acpString info("Laser model ");
      
      info += m_info.sensorModel;
      info += " not useable with SCIP 1.0";
      log(info);
      
      return false;
    }
  }
  
  log("Requesting specifications from laser");
  
  // Write the command to learn about the laser
  writeLine(m_pCurrentProtocol[aLASER_CMD_SPECIFICATIONS]);
  
  // look for repsonse from laser
  char reply[100];
  
  // place holder for information
  acpString info;
  
  if (!readLine(reply, 100))
    return false;
  if (reply[0] != 'P')
    return false;
  
  // Read and check the error code
  if (!readLine(reply, 100))
    return false;
  
  if (errorCode(reply))
    return false;
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "MODL:", 5))
    return false;
  info = &reply[5];
  info.trimTo('(',true);
  info.copyToBuffer(m_info.sensorModel, 100);
  
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "DMIN:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.minDistance,info);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "DMAX:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.maxDistance,info);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "ARES:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.measurementSteps,info);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "AMIN:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.measurementMinStep,info);  
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "AMAX:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.measurementMaxStep,info);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "AFRT:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.frontStep,info);
  
  if (!readLine(reply, 100))
    return false;
  if (strncmp(reply, "SCAN:", 5))
    return false;
  info = &reply[5];
  info.trimTo(';',true);
  aIntFromString(&m_info.motorSpeed,info);
  
  // toss the remaining lines because they vary between laser versions
  while (readLine(reply, 100)) {
    // toss until we get an empty line
    if (!reply[0])
      break;
  }
  
  return true;
}

/////////////////////////////////////////////////////////////////////


bool 
acpLaser::cmdUseSCIP2(void)
{
  
  acpString reply;
  
  log ("Setting SCIP 2.0 mode");
  
  // Send the stop command
  writeLine("SCIP2.0");
  
  // Throw out the echo response 
  if (!readLine(reply, 10))
    return false;
  
  // Check the error status
  if (!readLine(reply, 10))
    return false;
  
  // If there is an error, let us know why
  // 0 is successful
  // if the error is 21 (aka '0E'), then we are likely already using 2.0
  char errorVal = errorCode(reply);
  if(!((errorVal == 0) || (errorVal == 21)))
    return false;
  
  // toss the remaining lines because they vary between laser versions
  while (readLine(reply, 100)) {
    
    
    // toss until we get an empty line
    if (reply.endsWith(NULL))
      break;
    
  }
  
  // Set the communication protocol to SCIP 2.0 
  m_fProtocol = aLASER_SCIP2_0;
  
  return true; 
}

/////////////////////////////////////////////////////////////////////

char 
acpLaser::checksumCheck(int *sum) {
  
  int checkSum = *sum;
  
  // perform the checksum
  checkSum = (checkSum & 0x3F) + 0x30;
  
  // Set the checksum to zero
  *sum = 0;
  
  return (char) checkSum;
  
}

/////////////////////////////////////////////////////////////////////

char 
acpLaser::errorCode(const char* pErr)
{
  int error = 0;
  
  // decode value
  error = *pErr - 0x30;
  
  // next index value
  *pErr++;
  
  // SCIP 1.0 protocol stops here
  if (*pErr == '\0')
    return (char) error;
  
  // Decode next bit
  error |= *pErr - 0x30;
  
  if (m_log) {
    switch (error) {
      case 17:
	log("ERROR: Unable to create transmission data"
	    " or reply command internally.");
	break;
      case 18:
	log("ERROR: Buffer shortage or command repeated"
	    " that is already processed");
	break;
      case 19:
	log("ERROR: Command with insufficient parameters 1");
	break;    	
      case 20:
	log("ERROR: Undefined Command 1");
	break;   
      case 21:
	log("ERROR: Undefined command 2");
	break;
      case 22:
	log("ERROR: Command with insufficient parameters 2");
	break;
      case 23:
	log("ERROR: String Character in command exceeds 16 letters");
	break;
      case 24:
	log("ERROR: String Character has invalid letters");
	break;	
      case 25:
	log("ERROR: Sensor is now in firmware update mode");
	break;	
      default:
	break;
    }
  }
  
  
  return (char) error;
  
}

/////////////////////////////////////////////////////////////////////
// log stream function

void 
acpLaser::log(const char* pMsg)
{
  if (m_log) {
    aErr e;
    if (aStream_WriteLine(m_ioRef, m_log, pMsg, &e))
      throw acpException(e, "writing to laser log file");
  }
}


/////////////////////////////////////////////////////////////////////
// Private member access functions
/////////////////////////////////////////////////////////////////////

bool 
acpLaser::isInitialized() { return m_bInited; }

