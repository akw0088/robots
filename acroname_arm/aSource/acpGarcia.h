/////////////////////////////////////////////////////////////////////
//                                                                 //
// file: acpGarcia.h                                               //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// description: Definition of the acpGarcia API object.            //
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

#ifndef _acpGarcia_H_
#define _acpGarcia_H_

#include "aMath.h"
#include "aStem.h"

#define aGARCIA_MAX_RADIUS 0.167447f
#define aGARCIA_NUM_OUTLINE_POINTS 38

typedef enum {
  kFrontLeft = 0,
  kFrontRight,
  kSideLeft,
  kSideRight,
  kRearLeft,
  kRearRight,
  kDownLeft,
  kDownRight
} rangerID;

typedef enum {
  kFront = 0,
  kSide,
  kRear,
  kDown  
} rangerPair;


class acpGarcia : acpStem
  {
  public:
    ///////////////////////////////////////////////////////////
    
    class queue;
    class primitive // generic primitive base class
    {
    public:
      primitive(acpGarcia* pGarcia,
		unsigned char module,
		unsigned char slot,
		unsigned short size);
      virtual ~primitive(void);
      
      void addAllowedError(const short allowedStatus);
      
    protected:
      virtual void execute(void) = 0;
      virtual void finish(const aByte condition,
			  const short status);
      virtual bool isRunning(void);
      virtual void updatePosition(const int leftEncoder,
				  const int rightEncoder) = 0;
      acpGarcia& m_garcia;
      unsigned char m_module;
      unsigned char m_slot;
      unsigned int m_codeSize;
      bool m_bIsRunning;
      short m_status;
      unsigned long m_startTime;
      int m_leftEnc;
      int m_rightEnc;
      
      acpList<acpShort> m_allowedStatus;
      
      friend class queue;
    };
    
    ///////////////////////////////////////////////////////////
    
    class null : public primitive
    {
    public:
      null(acpGarcia* pGarcia);
      void setAcceleration(const float metersPerSecondSquared);
      void leftMotor(const float metersPerSecond);
      void rightMotor(const float metersPerSecond);
      void abort(void);
    private:
      void execute(void);
      void updatePosition(const int leftEncoder,
			  const int rightEncoder);
      float m_fAcceleration;
    };
    
    ///////////////////////////////////////////////////////////
    
    class move : public primitive
    {
    public:
      move(acpGarcia* pGarcia,
	   const float meters);
    private:
      void execute(void);
      void updatePosition(const int leftEncoder,
			  const int rightEncoder);
      float m_fDistance;
    };
    
    ///////////////////////////////////////////////////////////
    
    class arc : public primitive
    {
    public:
      arc(acpGarcia* pGarcia,
	  const float metersRadius,
	  const float radians);
    private:
      void execute(void);
      void updatePosition(const int leftEncoder,
			  const int rightEncoder);
      float m_fRadius;
      float m_fRadians;
    };
    
    ///////////////////////////////////////////////////////////
    
    class align : public primitive
    {
    public:
      align(acpGarcia* pGarcia,
	    const int side,
	    const int mode,
	    const float fRange);
    private:
      void execute(void);
      void updatePosition(const int leftEncoder,
			  const int rightEncoder);
      int m_side;
      int m_mode;
      float m_fRange;
    };
    
    ///////////////////////////////////////////////////////////
    // The queue manages the list of primitives to be executed
    // and all acpStem calls.  This ensures proper interleaving
    // of primitives, callbacks, and access.
    
    class queue : public ::acpRunable
    {
    public:
      /////////////////////////////////////////
      
      class addMsg : public acpMessage 
      {
      public:
	addMsg(queue& q, primitive* p) : m_q(q), m_p(p) {}
	void process(void) { m_q.m_primitives.addToTail(m_p); }
      private:
	queue& m_q;
	primitive* m_p;
      };
      
      /////////////////////////////////////////
      
      queue(acpGarcia* pGarcia);
      ~queue(void);
      void init(aIOLib ioRef);
      int run(void);
      void handleCmdMsg(const aUInt8* pData, 
			const unsigned int len);
      void add(primitive* pPrimitive);
      bool isThreadContext(void) { return m_pThread->isThreadContext(); }
      bool isIdle(void) 
      { return (!m_primitives.length() && !m_pCurrent); }
      short getStatus(void) {return m_status; }
      
    private:
      acpGarcia& m_garcia;
      acpThread* m_pThread;
      acpList<primitive> m_primitives;
      primitive* m_pCurrent;
      aIOLib m_ioRef;
      unsigned long m_nextEncoderUpdate;
      short m_status;
    };
    
    ///////////////////////////////////////////////////////////
    
    acpGarcia(void);
    virtual ~acpGarcia(void);
    
    // prepares the API for running
    bool init(aIOLib ioRef, 
	      aSettingFileRef settings = NULL);  
    
    // establishes connection and begins API functionallity
    aErr start(void);
    
    // status
    bool isConnected(void) { return acpStem::isConnected(); }
    bool isIdle(void) { return m_queue.isIdle(); }
    
    // add a new primitive to the queue
    void queuePrimitive(primitive* pPrimitive);
    
    // converts a status code to a readable string
    short getStatus(void) { return m_queue.getStatus(); }
    void statusString(acpString& s, 
		      const short status);
    
    bool isQueueContext(void) { return m_queue.isThreadContext(); }
    
    // called from queue thread, not thread safe
    unsigned long getEncoderUpdateMS(void) const { return m_encoderUpdateMS; }
    virtual void positionUpdate(void) {}
    
    //Geometry methods, can be overidden 
    //Wheel Track is the distance between the contact points of the wheels.
    virtual float getWheelTrack() { return 0.18224f; }
    virtual float getWheelRadius() { return 0.09156f; }
    
    // Revised ranger enable, get and read
    // Static method is used for ranger readings. This allows us to
    // Take a sensor reading from a Stem application.
    float static sGetRanger(acpStem* pStem, const rangerID ranger);
    
    // Use this function when taking a reading from a acpGarcia object
    // It calls the static method inline to increase performance
    inline float getRanger(const rangerID ranger) { return sGetRanger(this, ranger); };
    
    // Static method to enable and look for a reading from the ranger
    unsigned short static sGetEnabledRangerRaw(acpStem* pStem, const unsigned char enableBit,
					const unsigned char rangerCode);    
    
    void setRangerThreshold(const rangerPair ranger, const float fThreshold);
    float getRangerThreshold(const rangerPair ranger);
    
    // speed values
    float getSpeed(void);
    
    
    void setSpeed(const float fSpeed);
    
    // User Button 
    bool getUserButton(void);
    
    // manage computed position
    void setPose(const acpPose2& p) { m_pose = p; }
    acpPose2& getPose(void) { return m_pose; }
    
    // geometric boundary points
    static acpVec2 m_outline[aGARCIA_NUM_OUTLINE_POINTS];
    
    // User LED
    static void sSetUserLED(acpStem* pStem, const bool bOn);
    inline void setUserLED(const bool bOn){ sSetUserLED(this, bOn); };
    
    // IR Transmit and Receive
    void IRTransmit(const short value);
    int IRReceive(void);
    
    // Battery voltage and level
    float getBatteryVoltage(void);
    
    
  private:
    aIOLib m_ioRef;
    aSettingFileRef m_settings;
    bool m_bSettingsPassedIn;
    queue m_queue;
    unsigned long m_encoderUpdateMS;
    acpPose2 m_pose;
    

  };

#endif // _acpGarcia_H_
