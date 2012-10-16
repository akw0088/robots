/////////////////////////////////////////////////////////////////////
//                                                                 //
// file: acpGarcia.cpp                                             //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// description: Implementation of the acpGarcia API object.        //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// Copyright 1994-2010. Acroname Inc.                              //
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

#include "aCmd.tea"
#include "aGarciaDefs.tea"
#include "acpGarcia.h"

#define aGARCIA_API_MAX_SETTING_LEN 32

#define aGARCIA_API_PID_TIME_CONSTANT ((float)aGARCIA_PID_PERIOD / 10000.0f)

#define aGARCIA_API_PRIMITIVE_PID 0

#define aGARCIA_API_MODULE_NULL 4
#define aGARCIA_API_SLOT_NULL 5
#define aGARCIA_API_SIZE_NULL 652

#define aGARCIA_API_MODULE_MOVE 4
#define aGARCIA_API_SLOT_MOVE 1
#define aGARCIA_API_SIZE_MOVE 888

#define aGARCIA_API_MODULE_ARC 4
#define aGARCIA_API_SLOT_ARC 2
#define aGARCIA_API_SIZE_ARC 999

#define aGARCIA_API_MODULE_ALIGN 4
#define aGARCIA_API_SLOT_ALIGN 4
#define aGARCIA_API_SIZE_ALIGN 846


/////////////////////////////////////////////////////////////////////
// defines, tables, and prototypes for local routines that handle 
// geometric conversion

// inches, points, raw value, scale factor
#define aNUMGARCIAGP2D12POINTS 4
static const float g_irfunc[aNUMGARCIAGP2D12POINTS][4] = {
  {7.0f,  3.0f, 321.0f, 185.0f},
  {11.0f, 4.0f, 220.0f, 101.0f},
  {15.0f, 4.0f, 181.0f,  39.0f},
  {18.0f, 3.0f, 176.0f,   5.0f}
};

#define	aGARCIA_RANGER_OFFSET_FRONT 4.38f
#define	aGARCIA_RANGER_OFFSET_SIDE 1.01f
#define	aGARCIA_RANGER_OFFSET_REAR 2.01f

#define aGARCIA_WHEEL_RADIUS 0.09156f

static float 
sTicksToSpeed(const int ticks);

static short 
sSpeedToTicks(const float speed);

static float 
sRangerRawToMeters(const unsigned short rawValue,
		   const int type);

static unsigned short 
sRangerMetersToRaw(const float fMeters,
		   const int type);


/////////////////////////////////////////////////////////////////////

acpGarcia::primitive::primitive(acpGarcia* pGarcia,
				unsigned char module,
				unsigned char slot,
				unsigned short size) : 
m_garcia(*pGarcia),
m_module(module),
m_slot(slot),
m_codeSize(size),
m_bIsRunning(false),
m_status(aGARCIA_ERRFLAG_NOTEXECUTED)
{
}


/////////////////////////////////////////////////////////////////////

acpGarcia::primitive::~primitive(void)
{
  if (m_bIsRunning)
    m_garcia.VM_KILL(m_module, aGARCIA_API_PRIMITIVE_PID);
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::primitive::addAllowedError(const short allowedStatus)
{
  // only allow one entry for each status
  aLISTITERATE(acpShort, m_allowedStatus, pAllowed) {
    if (allowedStatus == *pAllowed)
      return;
  }
  m_allowedStatus.add(new acpShort(allowedStatus));
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::primitive::finish(const aByte condition,
			     const short status)
{
  if (!m_garcia.isQueueContext())
    throw acpException(aErrConfiguration, "finish called from wrong thread");
  m_bIsRunning = false;
  m_status = status;
}


/////////////////////////////////////////////////////////////////////

bool 
acpGarcia::primitive::isRunning(void)
{
  return m_bIsRunning;
}


/////////////////////////////////////////////////////////////////////

acpGarcia::null::null(acpGarcia* pGarcia) :
primitive(pGarcia, 
	  aGARCIA_API_MODULE_NULL,
	  aGARCIA_API_SLOT_NULL,
	  aGARCIA_API_SIZE_NULL),
m_fAcceleration(0.5f)
{
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::null::setAcceleration(const float metersPerSecondSquared)
{
  m_fAcceleration = metersPerSecondSquared;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::null::leftMotor(const float metersPerSecond)
{
  if (m_garcia.isConnected() && m_bIsRunning) {
    m_garcia.PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_NULL_LVEL, 
		    sSpeedToTicks(metersPerSecond));
  }
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::null::rightMotor(const float metersPerSecond)
{
  if (m_garcia.isConnected() && m_bIsRunning) {
    m_garcia.PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_NULL_RVEL, 
		    sSpeedToTicks(metersPerSecond));
  }
}

/////////////////////////////////////////////////////////////////////

void 
acpGarcia::null::abort(void)
{    
  
  m_garcia.PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_STATUS, 
		  aGARCIA_ERRFLAG_ABORT);
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::null::execute(void)
{  
  // (velocity increments per step / acceleration) * (1000ms per sec)
  float accTime = ((sTicksToSpeed(1) / m_fAcceleration) * 1000.0f);
  if (accTime < 0.0f)
    accTime = -accTime;
  aUInt16 acc = (short)accTime;
  if (acc < 1)
    acc = 1;
  aUInt8 data[sizeof(aUInt16)];
  aUtil_StoreShort((char*)data, acc);
  m_garcia.VM_RUN(m_module, m_slot, aGARCIA_API_PRIMITIVE_PID, 
		  data, sizeof(aUInt16));
  m_bIsRunning = true;
}


/////////////////////////////////////////////////////////////////////

#define aNULL_EQUAL_TOLERANCE 0.01

void 
acpGarcia::null::updatePosition(const int leftEncoder,
				const int rightEncoder)
{
  // if the encoders are the same, we haven't moved
  if ((m_leftEnc == leftEncoder) && (m_rightEnc == rightEncoder))
    return;
  
  // see how far in meters each side traveled
  float dl = (float)(leftEncoder - m_leftEnc);
  dl /= aGARCIA_TICKS_PER_METER;
  float dr = (float)(rightEncoder - m_rightEnc);
  dr /= aGARCIA_TICKS_PER_METER;
  
  acpPose2 pose = m_garcia.getPose();
  
  // Garcia is constrained by its fixed wheel orientation to only move in 
  // the robot-space x dimension, the wheels each contibute to 
  // this x component via their relative distance traveled.
  acpVec2 dispRF((dl + dr) / 2.0f, 0);
  
  // To get the change in bearing in world-space we take the difference 
  // in distance of the two wheels and divide that by the distance between 
  // the contact points of the two wheels.
  float rotRF = (dr - dl) / m_garcia.getWheelTrack();
  
  if (aNEARLY_EQUAL(dl, dr, aNULL_EQUAL_TOLERANCE)) {
    
    // if we went nearly straight, the bearing didn't change.
    // Just do a simple move by averaging the right and left sides. 
    // Translate takes a 2D vector in robot space, and translates the
    // robot position in world space.
    pose.translate(dispRF);
    
  } else if (aNEARLY_EQUAL(-dl, dr, aNULL_EQUAL_TOLERANCE)) {
    
    // if we nearly pivoted in place, just do a simple rotation (pivot).
    pose.rotate(rotRF);
    
  } else {
    
    //here we effectively do a translation and rotation. See notes
    //above about robot-space motion constraints. Translate takes a vector
    //in robot space. 
    pose.translate(dispRF);
    pose.rotate(rotRF);
    
  }
  
  m_garcia.setPose(pose);
  
} // acpGarcia::null::updatePosition


/////////////////////////////////////////////////////////////////////

acpGarcia::move::move(acpGarcia* pGarcia,
		      const float meters) :
primitive(pGarcia, aGARCIA_API_MODULE_MOVE,
	  aGARCIA_API_SLOT_MOVE, aGARCIA_API_SIZE_MOVE),
m_fDistance(meters)
{
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::move::execute(void)
{
  long ticks = (long)(m_fDistance * aGARCIA_TICKS_PER_METER);
  aUInt8 data[sizeof(long)];
  aUtil_StoreLong((char*)data, ticks);
  m_garcia.VM_RUN(m_module, m_slot, aGARCIA_API_PRIMITIVE_PID,
		  data, sizeof(long));
  m_bIsRunning = true;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::move::updatePosition(const int leftEncoder,
				const int rightEncoder)
{
  // if the encoders are the same, we haven't moved
  if (!leftEncoder && !rightEncoder)
    return;
  
  // see how far in meters each side traveled
  float dl = (float)leftEncoder;
  dl /= aGARCIA_TICKS_PER_METER;
  float dr = (float)rightEncoder;
  dr /= aGARCIA_TICKS_PER_METER;
  
  acpPose2 pose = m_garcia.getPose();
  acpVec2 dp(dl + dr / 2.0f, 0);
  pose.translate(dp);
  m_garcia.setPose(pose);
}


/////////////////////////////////////////////////////////////////////

acpGarcia::arc::arc(acpGarcia* pGarcia,
		    const float metersRadius,
		    const float radians) :
primitive(pGarcia, 
	  aGARCIA_API_MODULE_ARC,
	  aGARCIA_API_SLOT_ARC,
	  aGARCIA_API_SIZE_ARC),
m_fRadius(metersRadius),
m_fRadians(radians)
{
  // for now, we use pivot and ensure a radius of 0
  m_fRadius = 0.0f;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::arc::execute(void)
{
  aUInt16 ticks = (aUInt16)(m_fRadians * aGARCIA_TICKS_PER_RADIAN);
  aUInt8 data[sizeof(ticks)];
  aUtil_StoreShort((char*)data, ticks);
  m_garcia.VM_RUN(m_module, m_slot, aGARCIA_API_PRIMITIVE_PID,
		  data, sizeof(ticks));
  m_bIsRunning = true;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::arc::updatePosition(const int leftEncoder,
			       const int rightEncoder)
{
}

/////////////////////////////////////////////////////////////////////

acpGarcia::align::align(acpGarcia* pGarcia,
			const int side,
			const int mode,
			const float fRange) :
primitive(pGarcia, 
	  aGARCIA_API_MODULE_ALIGN,
	  aGARCIA_API_SLOT_ALIGN,
	  aGARCIA_API_SIZE_ALIGN),
m_side(side),
m_mode(mode),
m_fRange(fRange)
{
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::align::execute(void)
{
  aUInt8 data[4] = {m_side, m_mode};
  short range = sRangerMetersToRaw(m_fRange, aGARCIA_GP_BIT_ENABLEREAR);
  aUtil_StoreShort((char*)&data[2], range);
  m_garcia.VM_RUN(m_module, m_slot, aGARCIA_API_PRIMITIVE_PID, data, 4);
  m_bIsRunning = true;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::align::updatePosition(const int leftEncoder,
				 const int rightEncoder)
{
}

/////////////////////////////////////////////////////////////////////

acpGarcia::queue::queue(acpGarcia* pGarcia) :
m_garcia(*pGarcia),
m_pThread(NULL),
m_pCurrent(NULL),
m_nextEncoderUpdate(0),
m_status(aGARCIA_ERRFLAG_NORMAL)
{
  m_pThread = acpOSFactory::thread("primitive queue");
}


/////////////////////////////////////////////////////////////////////

acpGarcia::queue::~queue(void)
{
  delete m_pThread;
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::queue::init(aIOLib ioRef)
{
  m_ioRef = ioRef;
  m_pThread->start(this);  
  
  // see if we should initiate encoder status updates
  if (m_garcia.getEncoderUpdateMS()) {
    unsigned long now;
    if (!aIO_GetMSTicks(m_ioRef, &now, NULL))
      m_nextEncoderUpdate = now + m_garcia.getEncoderUpdateMS();
  }
}


/////////////////////////////////////////////////////////////////////

int
acpGarcia::queue::run(void)
{  
  while (!m_pThread->isDone()) {
    bool bIdle = true;
    if (m_pThread->handleMessage())
      bIdle = false;
    
    if (m_garcia.isConnected()) {
      unsigned long now;
      
      // we want to filter out some specific packets and let the others
      // flow through to our client.  Here we peek for the ones we care
      // about only.
      acpPacket* pPacket;
      // check for vm exit reporting
      if ((pPacket = m_garcia.peekPacket(aGARCIA_MOTO_ADDR, cmdMSG))) {
	handleCmdMsg(pPacket->getData(), pPacket->getLength());
	bIdle = false;
	delete pPacket;
      } // vm exit reporting
      
      if (m_pCurrent) {
	if (m_pCurrent->isRunning()) {
	  // The primitive is running do the encoder update.
	  // see if we should initiate an encoder status update
	  if (m_garcia.getEncoderUpdateMS()) {
	    if (!aIO_GetMSTicks(m_ioRef, &now, NULL) 
		&& (now > m_nextEncoderUpdate)) {
	      int l = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
					aGARCIA_MOTO_MOTOR_LEFT);
	      int r = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
					aGARCIA_MOTO_MOTOR_RIGHT);
	      
	      // let the primitive manage changing the pose where appropriate
	      m_pCurrent->updatePosition(l, r);
	      // announce a (possible) change of position
	      m_garcia.positionUpdate();
	      m_pCurrent->m_leftEnc = l;
	      m_pCurrent->m_rightEnc = r;
	      
	      m_nextEncoderUpdate += m_garcia.getEncoderUpdateMS();
	      bIdle = false;
	    }
	  }
	} else { // else primitive not running, so it is finished
	  bIdle = false;
	  // update the position one more time since it is done
	  int l = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				    aGARCIA_MOTO_MOTOR_LEFT);
	  int r = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				    aGARCIA_MOTO_MOTOR_RIGHT);
	  
	  m_pCurrent->updatePosition(l, r);
	  // announce a (possible) change of position
	  m_garcia.positionUpdate();
	  delete m_pCurrent;
	  m_pCurrent = NULL;
	} // end if connected.
	
      } else { // else no current primitive
	
	if (m_primitives.length()) {
	  m_pCurrent = m_primitives.removeHead();
	  // find the initial encoder values and then execute the primitive
	  // passing them in
	  aIO_GetMSTicks(m_ioRef, &m_pCurrent->m_startTime, NULL);
	  
	  // It is critical we get a good initial position. Grab encoder readings
	  // until two readings are equal.
	  
	  
	  int l1 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				     aGARCIA_MOTO_MOTOR_LEFT);
	  int l2 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				     aGARCIA_MOTO_MOTOR_LEFT);
	  int r1 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				     aGARCIA_MOTO_MOTOR_RIGHT);
	  int r2 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				     aGARCIA_MOTO_MOTOR_RIGHT);
	  
	  while (l1 != l2 || r1 != r2) {
	    l1 = l2;
	    l2 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				   aGARCIA_MOTO_MOTOR_LEFT);
	    r1 = r2;
	    r2 = m_garcia.MO_ENC32(aGARCIA_MOTO_ADDR, 
				   aGARCIA_MOTO_MOTOR_RIGHT);
	  }
	  
	  m_pCurrent->m_leftEnc = l2;
	  m_pCurrent->m_rightEnc = r2;
	  
	  m_pCurrent->execute();
	  bIdle = false;
	}
      }
    }
    if (bIdle)
      m_pThread->yield(2); // avoid a hard spin of idle threads
  }
  
  // clean out any primitives we may have sitting around before we
  // exit to avoid any reliance on this thread that is now done.
  primitive* p;
  while((p = m_primitives.removeHead()))
    delete p;
  
  // deleting this will force the TEA program to be killed in the destructor
  // for the primitive.
  if (m_pCurrent) {
    delete m_pCurrent;
    m_pCurrent = NULL;
  }
  return 0;
}


/////////////////////////////////////////////////////////////////////
// handleCmdMsg
//
// Currently, we just check for vmExit and toss any other messages
// on the floor.  Hopefully no client interaction requires any messages
// as if they do, we need to peek but not pull them from the list after
// determining they are not ours.  This would minimally change the 
// acpStem interface but is possible.

void 
acpGarcia::queue::handleCmdMsg(const aUInt8* pData, 
			       const unsigned int len)
{
  if (len > 0) {
    switch ((unsigned char)pData[0]) {
      case cmdMSG:
	if (len > 1) {
	  switch (pData[1]) {
	    case msg_vmExit:
	      // [0] cmdMSG
	      // [1] msg_vmExit
	      // [2] exit condition (0 is normal VM termination)
	      // [3] process ID
	      // [4] high byte of 2-byte status
	      // [5] low byte of 2-byte status
	      if ((len > 4) && (pData[3] == aGARCIA_API_PRIMITIVE_PID)) {
		short status = aUtil_RetrieveShort((char*)&pData[4]);
		// we go get the exit condition and status and let the
		// primitive clean itself from the list
		m_pCurrent->finish(pData[2], status);
		// Set the queue status
		m_status = status;
		
		// check to see if the status was set as allowed and if so,
		// we set status to normal to avoid purging the queue of 
		// remaining primitives below.
		aLISTITERATE(acpShort, m_pCurrent->m_allowedStatus, pStatus) {
		  if (status == *pStatus)
		    status = aGARCIA_ERRFLAG_NORMAL;
		}
		
		delete m_pCurrent;
		m_pCurrent = NULL;
		
		// if status is not normal or expected, we flush the queue 
		// of all other outstanding primitives
		if (status != aGARCIA_ERRFLAG_NORMAL) {
		  primitive* pTemp;
		  while ((pTemp = m_primitives.removeHead())) {
		    pTemp->finish(0, aGARCIA_ERRFLAG_WONTEXECUTE);
		    delete pTemp;
		  }
		}
	      }
	      break;
	  } // msg type switch
	} // if len > 1
    } // switch
  } // len > 0
}


/////////////////////////////////////////////////////////////////////

void
acpGarcia::queue::add(primitive* pPrimitive)
{
  // we used to do this async but then the caller would get an
  // erroneous "isActive()" call if they made the call before
  // the thread could queue the behavior.  It is now synchronous
  // at the risk of a deadlock so we need to keep an eye on it.
  //
  // we only message if we aren't in the queue thread
  if (!m_pThread->isDone()) {
    if (isThreadContext())
      m_primitives.addToTail(pPrimitive);
    else 
      m_pThread->sendMessage(new addMsg(*this, pPrimitive), false);
  } else
    delete pPrimitive;
}


/////////////////////////////////////////////////////////////////////
//
// acpGarcia constructor
//
// You will create one object for each robot your application
// communicates with.  The settings can be passed either
// in by the caller or, if not, the API will seek a file named
// "garcia.config" in the aBinary directory for the settings.

acpGarcia::acpGarcia(void) :
m_ioRef(NULL),
m_settings(NULL),
m_bSettingsPassedIn(false),
m_queue(this),
m_encoderUpdateMS(0)
{
} // constructor


/////////////////////////////////////////////////////////////////////

acpGarcia::~acpGarcia(void) 
{
  aErr e;
  
  // We only clean up the settings if we created them ourselves.
  if (!m_bSettingsPassedIn && m_settings) {
    if (aSettingFile_Destroy(m_ioRef, m_settings, &e))
      throw acpException(e, "destroying settings");
    m_settings = NULL;
  }
  
} // destructor


/////////////////////////////////////////////////////////////////////

bool 
acpGarcia::init(aIOLib ioRef, 
		aSettingFileRef settings)
{
  aErr e;
  m_ioRef = ioRef;
  
  if (settings) {
    m_settings = settings;
    m_bSettingsPassedIn = true;
  } else {
    if (aSettingFile_Create(m_ioRef, aGARCIA_API_MAX_SETTING_LEN,
			    "garcia.config", &m_settings, &e))
      throw acpException(e, "creating settings");
  }
  
  // Get a ms delay to try to update the encoders through the 
  // encoder callback.  Zero (default) means no update.
  int encoderUpdateMS;
  if (aSettingFile_GetInt(m_ioRef, m_settings, "encoder_update", 
			  &encoderUpdateMS, 0, &e))
    throw acpException(e, "getting encoder update setting");
  m_encoderUpdateMS = encoderUpdateMS;
  
  return true;
}


/////////////////////////////////////////////////////////////////////

aErr
acpGarcia::start(void)
{
  m_queue.init(m_ioRef);
  return startLinkThread(m_settings);
}


/////////////////////////////////////////////////////////////////////

void
acpGarcia::queuePrimitive(primitive* pPrimitive)
{
  m_queue.add(pPrimitive);
}


/////////////////////////////////////////////////////////////////////

void 
acpGarcia::statusString(acpString& s, 
			const short status)
{
  switch (status) {
      
    case aGARCIA_ERRFLAG_NORMAL:
      s = "No Error";
      break;
      
    case aGARCIA_ERRFLAG_STALL:
      s = "Motor Stall";
      break;
      
    case aGARCIA_ERRFLAG_FRONTR_LEFT:
      s = "Front Left Ranger";
      break;
      
    case aGARCIA_ERRFLAG_FRONTR_RIGHT:
      s = "Front Right Ranger";
      break;
      
    case aGARCIA_ERRFLAG_REARR_LEFT:
      s = "Rear Left Ranger";
      break;
      
    case aGARCIA_ERRFLAG_REARR_RIGHT:
      s = "Rear Right Ranger";
      break;
      
    case aGARCIA_ERRFLAG_SIDER_LEFT:
      s = "Side Left Ranger";
      break;
      
    case aGARCIA_ERRFLAG_SIDER_RIGHT:
      s = "Side Right Ranger";
      break;
      
    case aGARCIA_ERRFLAG_FALL_LEFT:
      s = "Left Edge Detector";
      break;
      
    case aGARCIA_ERRFLAG_FALL_RIGHT:
      s = "Right Edge Detector";
      break;
      
    case aGARCIA_ERRFLAG_ABORT:
      s = "Aborted";
      break;
      
    case aGARCIA_ERRFLAG_NOTEXECUTED:
      s = "Not Executed";
      break;
      
    case aGARCIA_ERRFLAG_WONTEXECUTE:
      s = "Will Not Execute";
      break;
      
    case aGARCIA_ERRFLAG_BATT:
      s = "Battery Low";
      break;
      
    case aGARCIA_ERRFLAG_IRRX:
      s = "IR Byte Received";
      break;
      
    default:
      s = "Unknown Status";
      break;
  } // nStatus switch
}

/////////////////////////////////////////////////////////////////////

float
acpGarcia::sGetRanger(acpStem* pStem, const rangerID ranger)
{
  
  char enableBit;
  char rangerCode;
  
  // Enable the raw ranger for reading
  switch (ranger) {
    case kFrontLeft:
      enableBit = aGARCIA_GP_BIT_ENABLEFRONT;
      rangerCode = aGARCIA_CODE_FRONT_LEFT;
      break;
    case kFrontRight:
      enableBit = aGARCIA_GP_BIT_ENABLEFRONT;
      rangerCode = aGARCIA_CODE_FRONT_RIGHT;
      break;
    case kSideLeft:
      enableBit = aGARCIA_GP_BIT_ENABLESIDE;
      rangerCode = aGARCIA_CODE_SIDE_LEFT;
      break;
    case kSideRight:
      enableBit = aGARCIA_GP_BIT_ENABLESIDE;
      rangerCode = aGARCIA_CODE_SIDE_RIGHT;
      break;
    case kRearLeft:
      enableBit = aGARCIA_GP_BIT_ENABLEREAR;
      rangerCode = aGARCIA_CODE_REAR_LEFT;
      break;
    case kRearRight:
      enableBit = aGARCIA_GP_BIT_ENABLEREAR;
      rangerCode = aGARCIA_CODE_REAR_RIGHT;
      break;
    case kDownLeft:
      enableBit = aGARCIA_GP_BIT_ENABLEDOWN;
      rangerCode = aGARCIA_CODE_DOWN_LEFT;
      break;
    case kDownRight:
      enableBit = aGARCIA_GP_BIT_ENABLEDOWN;
      rangerCode = aGARCIA_CODE_DOWN_RIGHT;
      break;      
    default:
      return -1.0f;
      break;
  }
  
  // Look for the reply packet from the stem
  unsigned short raw = sGetEnabledRangerRaw(pStem, enableBit, rangerCode);
  if (enableBit == aGARCIA_GP_BIT_ENABLEDOWN)
    return (float) raw;
  
  return sRangerRawToMeters(raw, enableBit);
  
}


/////////////////////////////////////////////////////////////////////

void
acpGarcia::setRangerThreshold(const rangerPair ranger, const float fThreshold)
{
 
  switch (ranger) {
    case kFront:
      PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_FRONTTHR, 
	     sRangerMetersToRaw(fThreshold, aGARCIA_GP_BIT_ENABLEFRONT));
      break;
    case kSide:
      PAD_IO(aGARCIA_GP_ADDR, aGARCIA_MOTO_PADS_SIDETHR, 
	     sRangerMetersToRaw(fThreshold, aGARCIA_GP_BIT_ENABLESIDE));
      break;
    case kRear:
      PAD_IO(aGARCIA_GP_ADDR, 32 + aGARCIA_GP_CTR_REARTHR * 2, 
	     sRangerMetersToRaw(fThreshold, aGARCIA_GP_BIT_ENABLEREAR));
      break;
    default:
      break;
  }

}

/////////////////////////////////////////////////////////////////////

float
acpGarcia::getRangerThreshold(const rangerPair ranger)
{
  
  char val[2];
  char enableBit;
  
  switch (ranger) {
    case kFront:
      enableBit = aGARCIA_GP_BIT_ENABLEFRONT;
      val[0] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_FRONTTHR);
      val[1] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_FRONTTHR + 1);
      break;
    case kSide:
      enableBit = aGARCIA_GP_BIT_ENABLESIDE;
      val[0] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_SIDETHR);
      val[1] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_SIDETHR + 1);
      break;
    case kRear:
      enableBit = aGARCIA_GP_BIT_ENABLEREAR;
      val[0] = PAD_IO(aGARCIA_GP_ADDR, 32 + aGARCIA_GP_CTR_REARTHR * 2);
      val[1] = PAD_IO(aGARCIA_GP_ADDR, 32 + aGARCIA_GP_CTR_REARTHR * 2 + 1);
      break;
    default:
      // Down looking rangers and incorrect values return a negative
      // number. This should be a subtle warning!
      return -1.0f;
  }
  
  return sRangerRawToMeters((unsigned short)aUtil_RetrieveShort(val),
			    enableBit);
  
}


/////////////////////////////////////////////////////////////////////

float 
acpGarcia::getSpeed(void)
{
  char val[2];
  val[0] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_DEFVEL);
  val[1] = PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_DEFVEL + 1);
  return sTicksToSpeed((unsigned short)aUtil_RetrieveShort(val));
}

/////////////////////////////////////////////////////////////////////

void
acpGarcia::setSpeed(const float fSpeed)
{
  float fNewSpeed = fSpeed;
  if (fSpeed < 0.0f) fNewSpeed = 0.0f;
  short speed = sSpeedToTicks(fNewSpeed);
  PAD_IO(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_PADS_DEFVEL, speed);
}


/////////////////////////////////////////////////////////////////////

bool 
acpGarcia::getUserButton(void)
{
  int button = PAD_IO(aGARCIA_GP_ADDR, aGARCIA_GP_PADB_MIRRORBUTTON);
  return (button ? false : true);
}


/////////////////////////////////////////////////////////////////////

unsigned short 
acpGarcia::sGetEnabledRangerRaw(acpStem* pStem, const unsigned char enableBit,
			       const unsigned char rangerCode)
{
  unsigned short val = 0;
  
  // We need to check what rangers are being queried
  // side and front respond with DEV_VAL from GP module
  // but rear responds with DEV_VAL from Moto module
  char module = (enableBit == aGARCIA_GP_BIT_ENABLEREAR) ? 
  aGARCIA_GP_ADDR : aGARCIA_MOTO_ADDR;
  
  // first, we prime the reflex which enables the ranger if needed
  // and then does the appropriate A/D read request.
  pStem->RFLXE_CHK(aGARCIA_GP_ADDR, enableBit, rangerCode);
  
  // We await the response A/D cmdDEV_VAL here.
  acpPacket* p = pStem->awaitPacket(module, cmdDEV_VAL);
  if (p) {
    const aUInt8* pData = p->getData();
    if (p->getLength() == 4) {
      // A2D is filled up high.  We want raw to be 0-1023.
	val = aUtil_RetrieveShort((char*)&pData[2]);
	val >>=6;
    } else if (p->getLength() == 3) {
      val = (unsigned char)pData[2];
    }
    delete p;
  }
  
  return val;
}


/////////////////////////////////////////////////////////////////////
// geometric outline in meters

acpVec2 acpGarcia::m_outline[38] = {
acpVec2(0.0533f, 0.0953f),
acpVec2(0.0889f, 0.0635f),
acpVec2(0.1067f, 0.0229f),
acpVec2(0.1067f, -0.0229f),
acpVec2(0.0889f, -0.0635f),
acpVec2(0.0533f, -0.0953f),
acpVec2(0.0533f, -0.0787f),
acpVec2(0.0032f, -0.0787f),
acpVec2(0.0032f, -0.0813f),
acpVec2(0.0483f, -0.0813f),

acpVec2(0.0508f, -0.0838f),
acpVec2(0.0508f, -0.0940f),
acpVec2(0.0483f, -0.0965f),
acpVec2(-0.0483f, -0.0965f),
acpVec2(-0.0508f, -0.0940f),
acpVec2(-0.0508f, -0.0838f),
acpVec2(-0.0483f, -0.0813f),
acpVec2(-0.0032f, -0.0813f),
acpVec2(-0.0032f, -0.0787f),
acpVec2(-0.0533f, -0.0787f),

acpVec2(-0.0533f, -0.0635f),
acpVec2(-0.1651f, -0.0279f),
acpVec2(-0.1651f, 0.0279f),
acpVec2(-0.0533f, 0.0635f),
acpVec2(-0.0533f, 0.0787f),
acpVec2(-0.0032f, 0.0787f),
acpVec2(-0.0032f, 0.0813f),
acpVec2(-0.0483f, 0.0813f),
acpVec2(-0.0508f, 0.0838f),
acpVec2(-0.0508f, 0.0940f),

acpVec2(-0.0483f, 0.0965f),
acpVec2(0.0483f, 0.0965f),
acpVec2(0.0508f, 0.0940f),
acpVec2(0.0508f, 0.0838f),
acpVec2(0.0483f, 0.0813f),
acpVec2(0.0032f, 0.0813f),
acpVec2(0.0032f, 0.0787f),
acpVec2(0.0533f, 0.0787f)
};

/////////////////////////////////////////////////////////////////////
#if 0
void 
acpGarcia::setUserLED(const bool bOn)
{
  aByte on = bOn ? 1 : 0;
  RAW_INPUT(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_RFLX_LED, on);
}
#endif

/////////////////////////////////////////////////////////////////////

void 
acpGarcia::IRTransmit(const short value)
{
  IRP_XMIT(aGARCIA_GP_ADDR, aGARCIA_GP_DIRCOMM_TX, value);
}


/////////////////////////////////////////////////////////////////////

int 
acpGarcia::IRReceive(void)
{
  return CTR_SET(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_CTR_IRRX);
}

/////////////////////////////////////////////////////////////////////

float 
acpGarcia::getBatteryVoltage(void)
{
  return A2D_RD(aGARCIA_GP_ADDR, aGARCIA_GP_ABATTERY) * 2.0f;
}

/////////////////////////////////////////////////////////////////////
// local, static geometry conversion routines
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

float 
sTicksToSpeed(const int ticks)
{
  float v = (float)ticks / (aGARCIA_API_PID_TIME_CONSTANT 
			    * (float)aGARCIA_TICKS_PER_METER);
  
  // I think this is wrong because ticks is big, speed is small
  if (v > aGARCIA_MAX_VELOCITY) 
    v = aGARCIA_MAX_VELOCITY;
  if (v < -aGARCIA_MAX_VELOCITY) 
    v = -aGARCIA_MAX_VELOCITY;
  return v;
}


/////////////////////////////////////////////////////////////////////

short 
sSpeedToTicks(const float speed)
{
  short v = (short)(speed * aGARCIA_API_PID_TIME_CONSTANT 
		    * (float)aGARCIA_TICKS_PER_METER);
  if (v > aGARCIA_MAX_VELOCITY) 
    v = aGARCIA_MAX_VELOCITY;
  if (v < -aGARCIA_MAX_VELOCITY) 
    v = -aGARCIA_MAX_VELOCITY;
  return v;
}


/////////////////////////////////////////////////////////////////////

float 
sRangerRawToMeters(const unsigned short rawValue,
		   const int type)
{
  int v = rawValue;
  float r = 0.0f;
  int i;
  
  /* interpolate 10-bit raw voltage to inches */
  for (i = 0; i < aNUMGARCIAGP2D12POINTS; i++) {
    if (v > g_irfunc[i][2]) {
      r = g_irfunc[i][0] 
      - g_irfunc[i][1] * (v - g_irfunc[i][2]) / g_irfunc[i][3];
      break;
    }
  }
  
  /* add centering offset in inches if result is valid */
  if (r > 0.0f) {
    switch (type) {
      case aGARCIA_GP_BIT_ENABLEFRONT:
	r += aGARCIA_RANGER_OFFSET_FRONT; 
	break;
      case aGARCIA_GP_BIT_ENABLESIDE:
	r += aGARCIA_RANGER_OFFSET_SIDE; 
	break;
      case aGARCIA_GP_BIT_ENABLEREAR:
	r += aGARCIA_RANGER_OFFSET_REAR; 
	break;
    } // switch
  }
  
  // currently interpolation is done in inches so we convert to meters
  r /= 39.37008f;
  
  return r;
  
} // sRawRangerToMeters


/////////////////////////////////////////////////////////////////////

unsigned short 
sRangerMetersToRaw(const float fMeters,
		   const int type)
{
  unsigned short raw = 0;
  
  // currently interpolation is done in inches so we convert from meters
  float fInches = fMeters * 39.37008f;
  
  switch (type) {
    case aGARCIA_GP_BIT_ENABLEFRONT:
      fInches -= aGARCIA_RANGER_OFFSET_FRONT;
      break;
    case aGARCIA_GP_BIT_ENABLESIDE:
      fInches -= aGARCIA_RANGER_OFFSET_SIDE;
      break;
    case aGARCIA_GP_BIT_ENABLEREAR:
      fInches -= aGARCIA_RANGER_OFFSET_REAR;
      break;
  }
  
  int i;
  for (i = 0; i < aNUMGARCIAGP2D12POINTS; i++) {
    if (fInches < g_irfunc[i][0]) {
      raw = (unsigned short)((((fInches - g_irfunc[i][0]) * g_irfunc[i][3]) 
			      / -g_irfunc[i][1]) + g_irfunc[i][2]);
      break;
    }
  }
  
  return raw;
}

/////////////////////////////////////////////////////////////////////

void
acpGarcia::sSetUserLED(acpStem* pStem, 
		       const bool bOn)
{
  aUInt8 on = bOn ? 1 : 0;
  pStem->RAW_INPUT(aGARCIA_MOTO_ADDR, aGARCIA_MOTO_RFLX_LED, on);
}


