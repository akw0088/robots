/////////////////////////////////////////////////////////////////////
//                                                                 //
// file: acpLaser.h 		 	                           //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// description: Implementation of Acroname Laser object.	   //
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

#ifndef _acpLaser_H_
#define _acpLaser_H_

#include "aIO.h"

#define aLASER_MAXSETTING	32
#define aLASER_SAMPLES		1081 // most samples taken
#define aLASER_MSTIMEOUT	250 // 750
#define aLASER_BAUDRATE		115200

#define aLASER_CMDLEN           16
#define aLASER_CMDLENSCAN	64

#define aLASER_PROTOCOL_KEY	"protocol"
#define aLASER_MINSTEP_KEY	"minimum_step"
#define aLASER_MAXSTEP_KEY	"maximum_step"

#define aLASER_SETTINGFILE	"laser.config"

#ifdef aWIN
#define aLASER_DEFAULT_PORTNAME	"COM4"
#else
#define aLASER_DEFAULT_PORTNAME	"tty.usbmodem1d11"
#endif

#define aLASER_SCIP1_0		1.0f
#define aLASER_SCIP2_0		2.0f

typedef enum {
  aLASER_2ENCODING = 0,
  aLASER_3ENCODING
} aLaserEncoding;

// Information structure about laser information
typedef struct aLaserInfo {
  char vendorName[100];
  char firmware[100];
  char protocolVersion[100];
  char serialNumber[100];
  char sensorModel[100];
  int measurementMinStep;
  int measurementMaxStep;
  int minDistance;
  int maxDistance;
  int measurementSteps;
  int frontStep;
  int motorSpeed;
  float detectionAngle;
} aLaserInfo;

class acpLaser
{  
public:
  
  acpLaser();
  ~acpLaser();

  // laser interaction
  bool establishLink(void);
  bool gatherInfo(void);
  bool init(aSettingFileRef& settings, 
	    aStreamRef logger = NULL);
  bool readLine(char* line,
		const int nMaxChars);
  bool writeLine(const char* line);

  // laser commands
  bool cmdVersion(void);
  bool cmdScan(void);
  bool cmdMotorOff(void);
  bool cmdMotorOn(void);
  bool cmdReset(void);  
  bool cmdSetBaudrate(const int baudrate);
  bool cmdSpecifications(void);
  bool cmdUseSCIP2(void);

  
  // Access functions to private members
  bool	isInitialized(void);
  const float getReading(const int index) const;
  const int getMeasurementSteps(void) { return (m_measurementSteps); };
  const bool getInfo(aLaserInfo* info) { 
    *info = m_info;
    return true;
  };

  
private:
  
  aIOLib		  m_ioRef;
  aSettingFileRef	  m_settings;
  aStreamRef 		  m_laserStream;
  aStreamRef		  m_log;
  
  // Character string information of laser (grabbed from the laser)
  // and the config file if there
  aLaserInfo m_info;
  
  // laser state
  int m_targetBaudrate;
  float m_fProtocol;
  int m_status;
  int m_encoding;
  int m_minimumStep;
  int m_maximumStep;
  int m_measurementSteps;
  bool m_bInited;
  
  char checksumCheck(int *sum);
  char errorCode(const char* pErr);
  void log(const char* pMsg);

  float m_vals[aLASER_SAMPLES + 1];
  acpString m_scanCommand;
  
  // Lookup table of functions. Different between SCIP protocols.
  const char** m_pCurrentProtocol;
  
};


// inline functions which must show up in the header

inline const float acpLaser::getReading(const int index) const
  { return m_vals[index]; }

#endif // _acpLaser_H_
