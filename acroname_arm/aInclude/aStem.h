/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* file: aStem.h                                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* description: Definition of a platform-independent BrainStem	   */
/*		communication layer.    			   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Copyright 1994-2010. Acroname Inc.                              */
/*                                                                 */
/* This software is the property of Acroname Inc.  Any             */
/* distribution, sale, transmission, or re-use of this code is     */
/* strictly forbidden except with permission from Acroname Inc.    */
/*                                                                 */
/* To the full extent allowed by law, Acroname Inc. also excludes  */
/* for itself and its suppliers any liability, wheither based in   */
/* contract or tort (including negligence), for direct,            */
/* incidental, consequential, indirect, special, or punitive       */
/* damages of any kind, or for loss of revenue or profits, loss of */
/* business, loss of information or data, or other financial loss  */
/* arising out of or in connection with this software, even if     */
/* Acroname Inc. has been advised of the possibility of such       */
/* damages.                                                        */
/*                                                                 */
/* Acroname Inc.                                                   */
/* www.acroname.com                                                */
/* 720-564-0373                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _aStem_H_
#define _aStem_H_

#include "aIO.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * opaque reference to the I/O library
 */

typedef aLIBREF aStemLib;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * define symbol import mechanism
 */

#ifndef aSTEM_EXPORT
#define aSTEM_EXPORT aLIB_IMPORT
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * stem library constants
 */

#define aSTEM_MAGICADDR			173
#define aSTEM_MAXSETTINGLEN		32
#define aSTEM_SETTINGSFILE		"stem.config"
#define aSTEM_TEADELAY_DEFAULT		5
#define	aSTEM_KEY_TEADELAY		"tea-delay"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * aStem library manipulation routines
 */

#ifdef __cplusplus
extern "C" {
#endif

aSTEM_EXPORT aLIBRETURN 
aStem_GetLibRef(aStemLib* pStemRef,
		aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_GetNamedLibRef(aStemLib* pStemRef,
		     const char* pName,
		     aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_ReleaseLibRef(aStemLib stemRef,
		    aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_GetVersion(aStemLib stemRef,
		 unsigned long *pVersion,
		 aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Maximum number of data bytes in BrainStem packets
 */

#define aSTEMMAXPACKETBYTES  	8


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Opaque packet reference
 */

typedef void* aPacketRef;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * typedef for packet filter
 */

typedef aBool (*aPacketFilter)(const unsigned char module,
			       const unsigned char dataLength,
			       const char* data,
			       void* ref);
typedef aErr (*aHeartbeatCallback)(const aBool bOn,
			           void* ref);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * link types
 */

typedef enum {
 kStemModuleStream,
 kStemRelayStream
} aStemStreamType;


#ifdef __cplusplus
extern "C" {
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Stem Routine definitions
 */

aSTEM_EXPORT aLIBRETURN 
aStem_SetStream(aStemLib stemRef, 
		aStreamRef streamRef,
		aStemStreamType streamType,
		aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_SetHBCallback(aStemLib stemRef,
		    aHeartbeatCallback cbProc,
		    void* cbRef,
		    aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_DebugLine(aStemLib stemRef,
		const char *line,
		aErr* pErr);

aSTEM_EXPORT aLIBRETURN
aStem_GetPacket(aStemLib stemRef,
		aPacketFilter filterProc,
		void* filterRef,
		unsigned long nMSTimeout,
		aPacketRef* pPacketRef,
		aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_SendPacket(aStemLib stemRef,
		 const aPacketRef packetRef,
		 aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_CreateRelayStream(aStemLib stemRef,
			const unsigned char nAddress,
			aStreamRef* pStreamRef,
			aErr* pErr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Packet Routine definitions
 */

aSTEM_EXPORT aLIBRETURN 
aPacket_Create(aStemLib stemRef,
	       const aUInt8 module,
	       const aUInt8 length,
	       const aUInt8* data,
	       aPacketRef* pPacketRef,
	       aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aPacket_Format(aStemLib stemRef,
	       const aPacketRef packetRef,
	       char* pBuffer,
	       const aUInt16 nMaxLength,
	       aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aPacket_GetData(aStemLib stemRef,
		const aPacketRef packetRef,
		aUInt8* pModule,
		aUInt8* pLength,
		aUInt8* data,
		aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aPacket_Destroy(aStemLib stemRef,
		const aPacketRef packetRef,
		aErr* pErr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TEA Program File I/O definitions
 */

aSTEM_EXPORT aLIBRETURN 
aStem_CreateTEAFileInput(aStemLib stemRef,
			 const unsigned char module,
			 const int nFileNumber,
			 aStreamRef* pStreamRef,
			 aErr* pErr);

aSTEM_EXPORT aLIBRETURN 
aStem_CreateTEAFileOutput(aStemLib stemRef,
			  const unsigned char module,
			  const int nFileNumber,
			  aStreamRef* pStreamRef,
			  aErr* pErr);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

///////////////////////////////////////////////////////////////////////////////
#define	kHasAddress 	0x01
#define kHasLength	0x02


#ifdef aOLDSCHOOL
#undef aSTEM_EXPORT
#define aSTEM_EXPORT
#endif

class aSTEM_EXPORT acpPacket : public acpMessage 
{
public:
  acpPacket(aStemLib stemRef,
	    aPacketRef packetRef);
  acpPacket(void) : 
  m_packetRef(NULL),
  m_flags(0) {}
  virtual ~acpPacket(void);

  bool isFull(void) { return (m_flags == (kHasAddress | kHasLength))
			     && (m_length == m_data[1]); }
  bool isEmpty(void) { return !(m_flags & kHasAddress); }  
  void reset(void) { m_flags = 0; }  
  virtual void process(void) {} 
  aPacketRef getRef(void) { return m_packetRef; }
  aUInt8 getAddress(void);
  aUInt8 getLength(void);
  const aUInt8* getData(void);
  
  void addByte(const aUInt8 byte);
  aErr writeToStream(aStreamRef stream);
  
private:
  aStemLib m_stemRef;
  aPacketRef m_packetRef;
  aUInt8 m_flags;
  aUInt8 m_length;
  aUInt8 m_data[aSTEMMAXPACKETBYTES + 2];
};


///////////////////////////////////////////////////////////////////////////////

#define aGP2_DIRCOMM_RX		4
#define aGP2_DIRCOMM_TX		5

#ifdef aOLDSCHOOL
#undef aSTEM_EXPORT
#define aSTEM_EXPORT
#endif

class aSTEM_EXPORT acpStem : acpRunable 
{
 public:
  acpStem(void);
  virtual ~acpStem(void);
  
  // establishes the ioRef and stemRef as well
  virtual aErr startLinkThread(aSettingFileRef settings);
  
  virtual void heartbeat(const bool bOn) {}
  
  // The default is to return connection status
  // immediately but sending in a non-default nMSWait
  // will block for up to that long trying to connect.
  bool isConnected(unsigned long nMSWait = 0);

  // stuffs a line into StemDebug (if present)
  void log(const acpString& msg);
  
  int run(void);

  // Loads into module/slot specified. Reports an error in loadErr including
  // aErrPermission if the module's EEPROM seems to be locked.
  void loadSlot(aStreamRef fileStream,
		const aByte module,
		const aByte slot);
  aErr m_loadErr;
  
  // get an analog value normalized from 0.0 to 1.0
  float A2D_RD(const aUInt8 module,
	       const aUInt8 channel);
  
  // get a counter value
  aInt32 CTR_SET(const aUInt8 module, 
		 const aUInt8 counter);
  
  // send and retrieve a debug packet, true if reply
  bool DEBUG(const aUInt8 module,
	     aUInt8* pData,
	     const aUInt8 dataLen);

  // get a digital channel configuration
  aUInt8 DIG_CFG(const aUInt8 module,
		 const aUInt8 channel);

  // set a digital channel configuration
  void DIG_CFG(const aUInt8 module,
	       const aUInt8 channel,
	       const aUInt8 configuration);

  // get a digital channel value
  aInt32 DIG_IO(const aUInt8 module,
		const aUInt8 channel);
  
  // set a digital channel value
  void DIG_IO(const aUInt8 module,
	      const aUInt8 channel,
	      const aUInt8 value);
  
  // use a module to read data on the I2C bus
  void IIC_RD(const aUInt8 module,
	      const aUInt8 addr,
	      const aUInt8 dataLen,
	      aUInt8* pValues);

  // send data on the I2C bus
  void IIC_XMIT(const aUInt8 addr,
		const aUInt8* pData,
		const aUInt8 dataLen);

  // read a GP202 value
  aInt32 IR02_RD(const aUInt8 module,
		 const aUInt8 index);

  // configure an IR capable pin
  void IRP_CFG(const aUInt8 module,
	       const aUInt8 pin,
	       const aUInt8 config);
  
  // transmit a 2-byte value over IR
  void IRP_XMIT(const aUInt8 module,
		const aUInt8 pin,
		const aUInt16 param);
  
  // low-level eeprom read (not for general use)
  void MEM_RD(const aUInt8 module,
	      const aUInt16 addr,
	      const aUInt8 dataLen,
	      aUInt8* pValues);
  
  // low-level eeprom write (not for general use)
  void MEM_WR(const aUInt8 module,
	      const aUInt16 addr,
	      const aUInt8* pData,
	      const aUInt8 dataLen);
  
  // get a motion channel parameter as two bytes
  aUInt16 MO_CFG(const aUInt8 module,
		 const aUInt8 channel,
		 const aUInt8 param);

  // set a motion channel parameter as two bytes
  void MO_CFG(const aUInt8 module,
	      const aUInt8 channel,
	      const aUInt8 param,
	      const aUInt8 value,
	      const aUInt8 flags);

  // set a motion channel parameter as a fixed-point value
  void MO_CFG(const aUInt8 module,
	      const aUInt8 channel,
	      const aUInt8 param,
	      const aShort fixedValue);
  
  // set a motion channel parameter as a float
  void MO_CFG(const aUInt8 module,
	      const aUInt8 channel,
	      const aUInt8 param,
	      float& value);

  // set the encoder value for a motion channel
  void MO_ENC32(const aUInt8 module,
		const aUInt8 channel,
		const aInt32 value);

  // get the encoder value for a motion channel
  aInt32 MO_ENC32(const aUInt8 module,
		  const aUInt8 channel);

  // peek at the motion channel's closed-loop values
  void MO_PEEK(const aUInt8 module,
	       const aUInt8 channel);

  // save a motion channels RAM settings to EEPROM
  void MO_SAV(const aUInt8 module,
	      const aUInt8 channel);

  // set the setpoint for a motion channel
  void MO_SET(const aUInt8 module,
	      const aUInt8 channel,
	      const aUInt16 setting);

  // get the setpoint for a motion channel
  aUInt16 MO_SET(const aUInt8 module,
		 const aUInt8 channel);

  // write a 2-byte value to the scratch pad
  void PAD_IO(const aUInt8 module,
	      const aUInt8 offset,
	      const aUInt16 value);
  
  // read a scratch pad byte
  aUInt8 PAD_IO(const aUInt8 module,
		const aUInt8 offset);
  
  // retrieves the pin state timer value
  aInt32 PTIME_RD(const aUInt8 module,
		  const aUInt8 channel,
		  const bool bGetReply = true);
  
  // propagates a reflex with a byte value
  void RAW_INPUT(const aUInt8 module,
		 const aUInt8 msg,
		 const aUInt8 input);
  
  // propagates a reflex with an 2-byte value
  void RAW_INPUT(const aUInt8 module,
		 const aUInt8 msg,
		 const aUInt16 input); 

  // check if a reflex enabler is activated
  void RFLXE_CHK(const aUInt8 module,
		 const aUInt8 reflexID,
		 const aUInt8 input);
  
  // get the absolute postion of a servo
  aUInt8 SRV_ABS(const aUInt8 module,
		 const aUInt8 channel);
  
  // set the absolute position of the specified servo
  void SRV_ABS(const aUInt8 module,
	       const aUInt8 channel,
	       const aUInt8 position);

  // get the servo configuration bits for the specified servo
  aUInt8 SRV_CFG(const aUInt8 module,
		 const aUInt8 channel);

  // set the servo configuration bits for the specified servo
  void SRV_CFG(const aUInt8 module,
	       const aUInt8 channel,
	       const aUInt8 configuration);

  // get a servo offset and range from RAM
  void SRV_LMT(const aUInt8 module,
	       const aUInt8 channel,
	       aUInt8* pOffset,
	       aUInt8* pRange);

  // set a servo offset and range in RAM
  void SRV_LMT(const aUInt8 module,
	       const aUInt8 channel,
	       const aUInt8 offset,
	       const aUInt8 range);

  // save all the servo RAM settings for the module
  void SRV_SAV(const aUInt8 module);

  // stop a servo's motion
  void SRV_STOP(const aUInt8 module,
		const aUInt8 channel);

  // set the timer
  void TMR_SET(const aUInt8 module,
	       const aUInt8 tid,
	       const aUInt16 time);

  // get a system value, returning -1 if not available
  aInt32 VAL_GET(const aUInt8 module,
		 const aUInt8 paramID);

  // kill a VM process
  void VM_KILL(const aUInt8 module,
	       const aUInt8 pid);

  // run the slot's program with any available pid and any expected parameters
  void VM_RUN(const aUInt8 module,
	      const aUInt8 slot,
	      const aUInt8* pData,
	      const aUInt8 dataLen);

  // run the slot's program with a specific pid and any expected parameters
  void VM_RUN(const aUInt8 module,
	      const aUInt8 slot,
	      const aUInt8 pid,
	      const aUInt8* pData,
	      const aUInt8 dataLen);
  
  void formatPacket(acpString& line,
		    const aPacketRef packet);
  aPacketRef createPacket(const aUInt8 module,
			  const aUInt8 length,
			  const aUInt8* data);
  void sendPacket(aPacketRef packetRef);
  void destroyPacket(aPacketRef packetRef);
  
  // returns next packet or NULL
  acpPacket* getPacket(void);

  acpPacket* peekPacket(const aUInt8 module, 
			const aUInt8 cmd);
  acpPacket* awaitPacket(const aUInt8 module, 
			 const aUInt8 cmd);
  
  // convenience functions
  // on entry, value is converted to nearest actual, prescale and freq get set
  static void convertToMoto1Frequency(float& fValue, aByte& pre, aByte& freq);
  static int convertFromMoto1Frequency(const short param);
  void sleep(const unsigned long msec);
  
  // Checks to see if an EEPROM lock (hardware) is in place.
  bool eepromIsLocked(const aUInt8 module);

protected:
  int			  m_yieldTime;
  int			  m_connectTimeout;
  
private:
  aIOLib		  m_ioRef;
  aStemLib		  m_stemRef;
  aSettingFileRef	  m_settings;
  aStreamRef              m_link;
  acpThread*		  m_pThread;
  bool                    m_bThreadUp;
  acpMutex*		  m_pLinkPacketMutex;
  acpList<acpPacket>	  m_linkPackets;
  unsigned long		  m_lastHB;
  
  static aErr		  cbHeartbeat(const aBool bOn, void* ref);

  unsigned long m_nReplyTimeout;
  friend class acpSendPacketMessage;
  friend class acpLoadSlotMessage;
};

#endif // __cplusplus


#endif /* _aStem_H_ */
