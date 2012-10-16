/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* file: aIO.h                                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* description: Definition of a platform-independent I/O layer.    */
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

#ifndef _aIO_H_
#define _aIO_H_

#include "aUtil.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * opaque reference to the I/O library
 */

typedef aLIBREF aIOLib;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * define symbol import mechanism
 */

#ifndef aIO_EXPORT
#define aIO_EXPORT aLIB_IMPORT
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * I/O library manipulation routines
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

aIO_EXPORT aLIBRETURN
aIO_GetLibRef(aIOLib* pIORef, 
              aErr* pErr);

aIO_EXPORT aLIBRETURN 
aIO_ReleaseLibRef(aIOLib ioRef, 
		  aErr* pErr);

aIO_EXPORT aLIBRETURN 
aIO_GetVersion(aIOLib ioRef, 
	       unsigned long* pulVersion,
	       aErr* pErr);

#ifdef __cplusplus 
}
#endif /* __cplusplus */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * opaque reference to a file
 */

typedef void* aFileRef;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Serial Input buffer size.
 */

#define aSERIAL_INBUFSIZE     1024


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Maximum number of TCP/IP connections on a server socket
 */

#define aTCP_MAXCONNECT         0


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File I/O modes.
 */

typedef enum aFileMode {
  aFileModeReadOnly,
  aFileModeWriteOnly,
  aFileModeUnknown
} aFileMode;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File storage areas.
 */

typedef enum aFileArea {
  aFileAreaUser,
  aFileAreaSystem,
  aFileAreaObject,
  aFileAreaBinary,
  aFileAreaTest,
  aFileAreaInclude,
  aFileAreaSource,
  aFileAreaAsset,
  aFileAreaPlugin,
  aFileAreaSymonym,
  aFileAreaDocumentation,
  aFileAreaNative
} aFileArea;

#define txtFileAreaUser			"aUser"
#define txtFileAreaSystem		"aSystem"
#define txtFileAreaObject		"aObject"
#define txtFileAreaBinary		"aBinary"
#define txtFileAreaTest			"aTest"
#define txtFileAreaInclude		"aInclude"
#define txtFileAreaSource		"aSource"
#define txtFileAreaAsset		"aAsset"
#define txtFileAreaPlugin		"aPlugin"
#define txtFileAreaSymonym		"aSymonym"
#define txtFileAreaDocumentation	"aDocumentation"
#define txtFileAreaNative		"native"

#define nMAXFILEAREACHARS	14


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File Routine definitions
 */

#ifdef __cplusplus
extern "C" {
#endif

aIO_EXPORT aLIBRETURN 
aFile_Open(aIOLib ioRef,
           const char* pFilename,
	   const aFileMode eMode,
	   const aFileArea eArea,
	   aFileRef* pFileRef,
	   aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_Close(aIOLib ioRef,
	    aFileRef fileRef,
	    aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_Read(aIOLib ioRef,
           aFileRef fileRef,
	   char* pBuffer,
	   const unsigned long nLength,
	   unsigned long* pActuallyRead,
	   aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_Write(aIOLib ioRef,
	    aFileRef fileRef,
	    const char* pBuffer,
	    const unsigned long nLength,
  	    unsigned long* pActuallyWritten,
  	    aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_Seek(aIOLib ioRef,
	   aFileRef fileRef,
	   const long nOffset,
	   aBool bFromStart,
	   aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_GetSize(aIOLib ioRef,
	      aFileRef fileRef,
	      unsigned long* pulSize,
	      aErr* pErr);

aIO_EXPORT aLIBRETURN 
aFile_Delete(aIOLib ioRef,
	     const char *pFilename,
	     const aFileArea eArea,
	     aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Directory Routine definitions
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef aErr 
(*aDirectoryListProc)(const char* pFilename,
		      const unsigned long nSize,
		      void* ref);

aIO_EXPORT aLIBRETURN 
aDirectory_List(aIOLib ioRef,
  	        const aFileArea eArea,
		const char* pExtension,
		aDirectoryListProc listProc,
		void* vpRef,
		aErr* pErr);

#ifdef __cplusplus 
}
#endif



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * setting file routines
 */

typedef void* aSettingFileRef;

#ifdef __cplusplus
extern "C" {
#endif
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_Create(aIOLib ioRef,
		      const unsigned int nMaxSettingLen,
		      const char* pFileName,
		      aSettingFileRef* pSettingFileRef,
		      aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_GetInt(aIOLib ioRef,
		      aSettingFileRef settingFileRef,
		      const char* key, 
		      int* pInt,
		      const int nDefault,
		      aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_GetULong(aIOLib ioRef,
			aSettingFileRef settingFileRef,
			const char* key, 
			unsigned long* pULong,
			const unsigned long nDefault,
			aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_GetFloat(aIOLib ioRef,
			aSettingFileRef settingFileRef,
			const char* key, 
			float* pFloat,
			const float fDefault,
			aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_GetString(aIOLib ioRef,
			 aSettingFileRef settingFileRef,
			 const char* key, 
			 char** ppString,
			 const char* pDefault,
			 aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_GetInetAddr(aIOLib ioRef,
			   aSettingFileRef settingFileRef,
			   const char* key, 
			   unsigned long* pInetAddr,
			   const unsigned long pDefault,
			   aErr* pErr);

  aIO_EXPORT aLIBRETURN
  aSettingFile_GetBool(aIOLib ioRef,
		       aSettingFileRef settingFileRef,
		       const char* key,
		       aBool* pBool,
		       const aBool boolDefault,
		       aErr* pErr);
  
  aIO_EXPORT aLIBRETURN
  aSettingFile_SetKey(aIOLib ioRef,
		      aSettingFileRef settingFileRef,
		      const char* pKey, 
		      const char* pValue,
		      aErr* pErr);
    
  aIO_EXPORT aLIBRETURN
  aSettingFile_Destroy(aIOLib ioRef,
		       aSettingFileRef settingFileRef,
		       aErr* pErr);
  
#ifdef __cplusplus 
}
#endif



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Low level operation proc defs for I/O streams.  These need to 
 * be implimented for new stream types.
 */

typedef aErr (*aStreamGetProc)(char* pData, void* ref);
typedef aErr (*aStreamPutProc)(const char* pData, void* ref);
typedef aErr (*aStreamDeleteProc)(void* ref);

typedef void* aStreamRef;

#define aStreamLibRef(aStreamRef) (*(aIOLib*)aStreamRef)

#define aSSLINFOSIZE 256
typedef struct aSSLInfoStruct {
  char organization[aSSLINFOSIZE];
  char unit[aSSLINFOSIZE];
  char name[aSSLINFOSIZE];
  char email[aSSLINFOSIZE];
} aSSLInfoStruct;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Stream operations.
 */

#define aSTREAM_SETTING_LINKTYPE_KEY	    "linktype"
#define aSTREAM_SETTING_LINKTYPE_DEFAULT    "serial"
#define aSTREAM_SETTING_PORTNAME_KEY	    "portname"
#ifdef aMACX
#define aSTREAM_SETTING_PORTNAME_DEFAULT    "tty.usbserial"
#endif
#ifdef aWIN
#define aSTREAM_SETTING_PORTNAME_DEFAULT    "COM1"
#endif
#if defined(aUNIX) && !defined(aMACX)
#define aSTREAM_SETTING_PORTNAME_DEFAULT    "ttyS0"
#endif
#define aSTREAM_SETTING_IPADDRESS_KEY	    "ip_address"
#define aSTREAM_SETTING_IPPORT_KEY	    "ip_port"
#define aSTREAM_SETTING_IPPORT_DEFAULT   8000

#ifdef __cplusplus
extern "C" {
#endif

/* creates a stream from one of several sources based on these settings:
 *
 * linktype = serial
 *  baudrate (string) specifies bit rate (9600, 19200, etc...)
 *  portname (string) specifies port name (COM1, ttyUSB0, etc...)
 *
 * linktype = ip
 *  ip_address (ip) specifies address, defaults to the machines address
 *                  (192.168.1.3, 127.0.0.1, etc...)
 *  ip_port (number) the port (8000, 8002, etc...)
 *
 * linktype = usb
 *  usb_id (number) specifies the product serial number (0x00000012, 0x0000FA32)
 */
aIO_EXPORT aLIBRETURN
aStream_CreateFromSettings(aIOLib ioRef, 
			   aSettingFileRef,
			   aStreamRef* pStreamRef,
			   aErr* pErr);

aIO_EXPORT aLIBRETURN
aStream_Create(aIOLib ioRef, 
	       aStreamGetProc getProc,
	       aStreamPutProc putProc,
	       aStreamDeleteProc deleteProc,
	       const void* procRef,
	       aStreamRef* pStreamRef,
	       aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateFileInput(aIOLib ioRef, 
			const char *pFilename,
			const aFileArea eArea,
			aStreamRef* pStreamRef,
			aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateFileOutput(aIOLib ioRef, 
			 const char *pFilename,
			 const aFileArea eArea,
			 aStreamRef* pStreamRef,
			 aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateSerial(aIOLib ioRef, 
		     const char *pPortName, 
		     const unsigned int nBaudRate, 
		     aStreamRef* pStreamRef, 
		     aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateSocket(aIOLib ioRef, 
		     const unsigned long address, 
		     const unsigned short port, 
		     const aBool bServer,
		     aStreamRef* pStreamRef, 
		     aErr* pErr);

aIO_EXPORT aLIBRETURN
aStream_CreateMemory(aIOLib ioRef, 
		     const aMemPtr pMemory,
		     const aMemSize size,
		     aStreamRef* pStreamRef,
		     aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateUSB(aIOLib ioRef, 
		  const unsigned int serialNum, 
		  aStreamRef* pStreamRef, 
		  aErr* pErr);
  
aIO_EXPORT aLIBRETURN 
aStreamBuffer_Create(aIOLib ioRef, 
		     const aMemSize nIncSize,
		     aStreamRef* pBufferStreamRef,
		     aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStreamBuffer_Get(aIOLib ioRef, 
		  aStreamRef bufferStreamRef,
		  aMemSize* aSize,
		  char** ppData,
		  aErr* pErr);

aIO_EXPORT aLIBRETURN
aStreamBuffer_Flush(aIOLib ioRef, 
		    aStreamRef bufferStreamRef,
		    aStreamRef flushStream,
		    aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateZLibFilter(aIOLib ioRef, 
			 const aStreamRef streamToFilter,
			 const aFileMode eMode,
			 aStreamRef* pFilteredStreamRef,
			 aErr* pErr);
  
aIO_EXPORT aLIBRETURN 
aStream_CreateBase64Filter(aIOLib ioRef, 
			   const aStreamRef streamToFilter,
			   const aFileMode eMode,
			   aStreamRef* pFilteredStreamRef,
			   aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_CreateSSLFilter(aIOLib ioRef, 
			const aStreamRef streamToFilter,
			aSSLInfoStruct* pSubject,
			aSSLInfoStruct* pIssuer,
			aStreamRef* pFilteredStreamRef,
		        aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_Read(aIOLib ioRef, 
	     aStreamRef streamRef,
	     char* pBuffer,
    	     const unsigned long nLength,
    	     aErr* pErr);

aIO_EXPORT aLIBRETURN
aStream_Write(aIOLib ioRef, 
	      aStreamRef streamRef,
	      const char* pBuffer,
    	      const unsigned long nLength,
    	      aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_ReadLine(aIOLib ioRef, 
		 aStreamRef streamRef,
		 char* pBuffer,
    		 const unsigned long nMaxLength,
    		 aErr* pErr);

aIO_EXPORT aLIBRETURN
aStream_WriteLine(aIOLib ioRef, 
		  aStreamRef streamRef,
		  const char* pBuffer,
		  aErr* pErr);

aIO_EXPORT aLIBRETURN
aStream_Flush(aIOLib ioRef, 
	      aStreamRef inStreamRef,
	      aStreamRef outStreamRef,
	      aErr* pErr);

aIO_EXPORT aLIBRETURN 
aStream_Destroy(aIOLib ioRef, 
		aStreamRef streamRef,
		aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Memory Pool allocator routines
 */

typedef void* aMemPoolRef;

#ifdef __cplusplus
extern "C" {
#endif

aIO_EXPORT aLIBRETURN 
aMemPool_Create(aIOLib ioRef,
		aMemSize objectSize,
		aMemSize blockSize,
		aMemPoolRef* pPoolRef,
		aErr* pErr);

aIO_EXPORT aLIBRETURN 
aMemPool_Alloc(aIOLib ioRef,
	       aMemPoolRef poolRef,
	       void** ppObj,
	       aErr* pErr);

aIO_EXPORT aLIBRETURN
aMemPool_Free(aIOLib ioRef,
	      aMemPoolRef poolRef,
	      void* pObj,
	      aErr* pErr);

aIO_EXPORT aLIBRETURN 
aMemPool_Destroy(aIOLib ioRef,
		 aMemPoolRef poolRef,
		 aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Symbol table routines
 */

typedef void* aSymbolTableRef;

#define aMAXIDENTIFIERLEN		32

typedef aErr (*symDataDeleteProc)(void* pData, void* ref);

#ifdef __cplusplus
extern "C" {
#endif


aIO_EXPORT aLIBRETURN 
aSymbolTable_Create(aIOLib ioRef,
		    aSymbolTableRef* pSymbolTableRef,
		    aErr* pErr);

aIO_EXPORT aLIBRETURN
aSymbolTable_Insert(aIOLib ioRef,
		    aSymbolTableRef symbolTableRef,
		    const char* identifier,
		    void* pData,
  		    symDataDeleteProc deleteProc,
  		    void* deleteRef,
		    aErr* pErr);

aIO_EXPORT aLIBRETURN
aSymbolTable_Set(aIOLib ioRef,
		 aSymbolTableRef symbolTableRef,
		 const char* identifier,
		 void* pData,
		 symDataDeleteProc deleteProc,
		 void* deleteRef,
		 aErr* pErr);
  
aIO_EXPORT aLIBRETURN
aSymbolTable_Find(aIOLib ioRef,
		  aSymbolTableRef symbolTableRef,
		  const char* identifier,
		  void** ppFoundData,
		  aErr* pErr);

aIO_EXPORT aLIBRETURN
aSymbolTable_Destroy(aIOLib ioRef,
		     aSymbolTableRef symbolTableRef,
		     aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * token types
 */

typedef unsigned char tkType;

#define tkInt		(tkType)0
#define tkFloat		(tkType)1
#define tkIdentifier	(tkType)2
#define tkSpecial	(tkType)3
#define tkString	(tkType)4
#define tkPreProc	(tkType)5
#define tkNewLine	(tkType)6

#define tkErrUntermCmnt		1
#define tkErrIncludeNotFnd	2
#define tkErrCompile		3
#define tkErrDuplicateDefine	4
#define tkErrBadArgumentList	5
#define tkErrBadFloat		6
#define tkErrMissingParen	7
#define tkErrInvalidChar        8
#define tkErrFilenameLength     9
#define tkErrIllegalEscape      10
#define tkErrInvalidPreProc     11
#define tkErrXMLMissingClose	50
#define tkErrXMLAttribute	51

typedef struct aToken {
  tkType		eType;
  union {
    int			integer;
    float		floatVal;
    char		identifier[aMAXIDENTIFIERLEN];
    char*		string;
    char		special;
  } v;
} aToken;

typedef struct aTokenInfo {
  unsigned int		nLine;
  unsigned int		nColumn;
  char*			pSourceName;
  char*			pSourceLine;
  const char*           pTypeName;
} aTokenInfo;

typedef unsigned int tkError;

typedef aErr (*aTokenErrProc)(tkError error,
			      const unsigned int nLine,
			      const unsigned int nColumn,
			      const unsigned int nData,
			      const char* data[],
			      void* errProcRef);

typedef void* aTokenizerRef;

#ifdef __cplusplus
extern "C" {
#endif

aIO_EXPORT aLIBRETURN
aToken_GetInfo(aIOLib ioRef,
	       const aToken* pToken,
	       aTokenInfo* pTokenInfo,
	       aErr* pErr);

aIO_EXPORT aLIBRETURN
aTokenizer_Create(aIOLib ioRef,
		  aStreamRef tokenStream,
		  const char* streamName,
		  aFileArea eIncludeArea,
		  aTokenErrProc errProc,
		  void* errProcRef,
		  aTokenizerRef* pTokenizerRef,
		  aErr* pErr);

aIO_EXPORT aLIBRETURN
aTokenizer_Next(aIOLib ioRef,
		aTokenizerRef tokenizerRef,
		aToken** ppToken,
		aErr* pErr);

aIO_EXPORT aLIBRETURN
aTokenizer_PushBack(aIOLib ioRef,
		    aTokenizerRef tokenizerRef,
		    aToken* pToken,
		    aErr* pErr);

aIO_EXPORT aLIBRETURN
aTokenizer_Dispose(aIOLib ioRef,
		   aTokenizerRef tokenizerRef,
		   aToken* pToken,
		   aErr* pErr);

aIO_EXPORT aLIBRETURN
aTokenizer_Destroy(aIOLib ioRef,
		   aTokenizerRef tokenizerRef,
		   aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * XML data handling routines
 */

typedef void* aXMLRef;
typedef void* aXMLNodeRef;

typedef aErr (*aXMLHandleStart)(aXMLNodeRef node,
				const char* pKey,
				void* vpRef);

typedef aErr (*aXMLHandleContent)(aXMLNodeRef node,
				  const char* pKey,
				  const aToken* pToken,
				  void* vpRef);

typedef aErr (*aXMLHandleEnd)(aXMLNodeRef node,
			      aXMLNodeRef parent,
			      const char* pKey,
			      void* vpRef);

typedef aErr (*aXMLDisplayError)(const char* pMsg,
				 void* vpRef);

typedef struct aXMLCallbacks {
  aXMLHandleStart	handleStart;
  aXMLHandleContent	handleContent;
  aXMLHandleEnd		handleEnd;
  aXMLDisplayError	displayError;
  void*			ref;
} aXMLCallbacks;

#ifdef __cplusplus
extern "C" {
#endif

aIO_EXPORT aLIBRETURN
aXML_Create(aIOLib ioRef,
	    aStreamRef dataStream,
	    aTokenErrProc errProc,
	    void* errProcRef,
	    aXMLRef* pXML,
	    aErr* pErr);

aIO_EXPORT aLIBRETURN
aXML_Traverse(aIOLib ioRef,
	      aXMLRef XML,
	      const aXMLCallbacks* pCallbacks,
	      aErr* pErr);

aIO_EXPORT aLIBRETURN
aXML_Destroy(aIOLib ioRef,
	     aXMLRef XML,
	     aErr* pErr);

aIO_EXPORT aLIBRETURN
aXMLNode_FindKey(aIOLib ioRef,
		 aXMLNodeRef XMLNode,
		 const char* pKey,
		 aXMLHandleContent contentProc,
		 void* vpProcRef,
		 aErr* pErr);

/* aXMLNode_GetAttribute */
/* tries to get attributes like foo in <TAG foo="sommething"> */
/* if successful, sets ppValue to point to string */
/* if not found, returns aErrNotFound in pErr */
aIO_EXPORT aLIBRETURN
aXMLNode_GetAttribute(aIOLib ioRef,
		      aXMLNodeRef XMLNode,
		      const char* pAttribute,
		      const char** ppValue,
		      aErr* pErr);

aIO_EXPORT aLIBRETURN
aXMLNode_ReportError(aIOLib ioRef,
		     aXMLNodeRef XMLNode,
		     const char* pErrorMsg,
		     aErr* pErr);
  
#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Routines for managing an external shell program.
 */

typedef void* aShellRef;

typedef enum aShellDescriptor {
  eSTDOUT = 0,
  eSTDIN = 1,
  eSTDERR = 2
} aShellDescriptor;

#ifdef __cplusplus
extern "C" {
#endif
  
aIO_EXPORT aLIBRETURN
aShell_Create(aIOLib ioRef,
	      const char* pCommand,
	      aShellRef* pShellRef,
	      aErr* pErr);

aIO_EXPORT aLIBRETURN
aShell_GetStream(aIOLib ioRef,
		 aShellRef shellRef,
		 const aShellDescriptor descriptor,
		 aStreamRef* pStreamRef,
		 aErr* pErr);

aIO_EXPORT aLIBRETURN
aShell_Execute(aIOLib ioRef,
	       aShellRef shellRef,
	       aErr* pErr);

aIO_EXPORT aLIBRETURN
aShell_Completion(aIOLib ioRef,
	          aShellRef shellRef,
	          int* retValue,
	          aErr* pErr);

aIO_EXPORT aLIBRETURN
aShell_Destroy(aIOLib ioRef,
               aShellRef shellRef,
	       aErr* pErr);

#ifdef __cplusplus 
}
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common OS-independant I/O functions
 */

#ifdef __cplusplus
extern "C" {
#endif

aIO_EXPORT aLIBRETURN
aIO_GetMSTicks(aIOLib ioRef,
	       unsigned long* pNTicks,
	       aErr* pErr);

aIO_EXPORT aLIBRETURN
aIO_MSSleep(aIOLib ioRef,
	    const unsigned long msTime,
	    aErr* pErr);

aIO_EXPORT aLIBRETURN
aIO_GetInetAddr(aIOLib ioRef,
		unsigned long* pAddress,
		aErr* pErr);

#ifdef __cplusplus 
}
#endif

#ifdef __cplusplus


///////////////////////////////////////////////////////////////////////////////
// aArguments_Separate
//
// Pulls out arguments and settings from command line argc/argv parameters.
//
// Inputs:
//  ioRef - aIOLib reference for use in manipulating the settings.
//  settings - Can be NULL.  If provided, options included in argc/argv are
//             put into the settings overwriting any existing values.  These
//             options are allways key/value pairs.  The key is always 
//             preceeded by a '-' character and the value needs to be quoted
//             if it contains spaces.  Booleans are only "true" or "false" and
//             all option keys and values are case sensitive.  If a single 
//             character key is used, it forces a default value "true" and 
//             expects no value.
//
//             Some option examples:
//               -show_window true
//               -filename foo.txt
//               -filename "file with spaces.txt"
//               -count 3
//               -ip_address 192.168.2.4
//               -d (no parameter, implies boolean true)
//
//  pArguments - Can be NULL.  If provided, the non-option argc/argv values
//               are accumulated into this list in the order they occur.  If
//               arguments are quoted, the quotes are removed which allows
//               for spaces in filenames, etc.
//  argc - The count of argc/argv arguments.  This is not modified.
//  argv - The list of actual arguments.  This is not modified.
//
// Return Value: Returns the number of arguments found in the scan, regardless
//               of whether the arguments input parameter is NULL or not.

aIO_EXPORT unsigned int
aArguments_Separate(aIOLib ioRef,
		    aSettingFileRef settings,
		    acpList<acpString>* pArguments,
		    const int argc,
		    const char* argv[]);


///////////////////////////////////////////////////////////////////////////////

class aIO_EXPORT acpStringIO : public acpString {
public:
  acpStringIO(void) : acpString() {}
  acpStringIO(const acpString& string) :
  acpString(string) {}
  acpStringIO(const aStreamRef stream);
  acpStringIO(const char* pValue) :
  acpString(pValue) {}
  virtual ~acpStringIO(void) {}
  
  // Timeout of zero means never. Filters '\r', 0x13 (stop) and 0x11 (start)
  // Returns line contents (without line ending) upon receipt of '\n'.
  aErr readLine(aStreamRef stream,
		unsigned long nMSTimeout = 0);
  
  aErr writeToStream(aStreamRef stream) const;
  
  aStreamRef getStream(aIOLib ioRef);
  
private:
  static aErr sStreamGet(char* pData,
			 void* ref);
  size_t m_current;
};


///////////////////////////////////////////////////////////////////////////////

class aIO_EXPORT acpCounterStream
{
public:
  acpCounterStream(aIOLib ioRef,
		   aStreamRef passthrough = NULL);
  virtual ~acpCounterStream(void);
  
  operator aStreamRef(void) { return m_stream; }
  int putCount(void) { return m_nPuts; }
  int getCount(void) { return m_nGets; }
  
  virtual aErr get(char* pData);
  
private:
  static aErr getProc(char* pData, 
		      void* ref);
  static aErr putProc(const char* pData, 
		      void* ref);
  static aErr writeProc(const char* pData, 
			const unsigned long nSize, 
			void* ref);
  static aErr deleteProc(void* ref);
  
  aIOLib m_ioRef;
  aStreamRef m_stream;
  aStreamRef m_passthrough;
  int m_nGets;
  int m_nPuts;
};


///////////////////////////////////////////////////////////////////////////////

class aIO_EXPORT acpTypedData {

public:  
  acpTypedData(void);
  ~acpTypedData(void);
  
  void setData(const void* pData,
               const aMemSize len,
               const acpString& type);
  
  void getData(void** ppData, 
               size_t* pLen) const;
  
  const aStreamRef getDataStream(void);
  
  const aMemSize getLength() const { return m_len; }
  const acpString& getType() const { return m_type; }
  
private:
  acpTypedData(const acpTypedData& data);
  acpTypedData& operator=(const acpTypedData& data); 
 
  aIOLib m_ioRef;
  char* m_pData;
  aMemSize m_len;
  acpString m_type;

  friend class acpHTTPRequest;
};


///////////////////////////////////////////////////////////////////////////////

#define aHTTPOPT "OPTIONS"
#define aHTTPHEAD "HEAD"
#define aHTTPGET "GET"
#define aHTTPPOST "POST"
#define aHTTPPUT "PUT"
#define aHTTPDEL "DELETE"
#define aHTTPPFIND "PROPFIND"
#define aHTTPPPATCH "PROPPATCH"
#define aHTTPMKCOL "MKCOL"
#define aHTTPCOPY "COPY"
#define aHTTPMOVE "MOVE"
#define aHTTPLOCK "LOCK"
#define aHTTPUNLOCK "UNLOCK"
#define aHTTPMKCAL "MKCALENDAR"
#define aHTTPREPORT "REPORT"

// byte to byte Operational Latency threshold in milliseconds.
#define aBTBTHRESHOLD 200

// request update callback frequency.
#define aREQ_UPDATEMS 30

class aIO_EXPORT acpHTTPRequest {
  
public:
  
  acpHTTPRequest(void);
  acpHTTPRequest(const acpString& url,
                 const acpString& method = aHTTPGET,
                 const acpString& user = "",
                 const acpString& pass = "",
                 const bool auth = false);
  
   virtual ~acpHTTPRequest(void);
  
  void setUserAgent(const acpString& agent) {
    m_userAgent = agent;
  }
  
  void setURL(const acpString& url);
  
  void setLocation(const acpString& location) { 
    m_location = (location == "")? "/": location; 
  }
  
  void verbose(bool v) {
    m_bV = v;
  }
  
  void basicAuth(bool auth) { m_bAuth = auth; }
  
  void updateCredentials(const acpString& user, 
                         const acpString& pass);
  
  void setMethod(const acpString& method) { m_method = method; }
  void addReqHeader(const acpString& key,
                    const acpString& value);
  
  void setRequestBody(acpTypedData* data) 
  {
    m_pRequestBody = data;
  }
  
  aErr sendRequest(acpTypedData* pData, 
                   acpHash* pHeaders,
                   int* pStatusCode);
  
  //An extented class can get periodic status updates.
  virtual aErr update(unsigned long currCT) { return aErrNone; }
  
  void getSSLInfo(aSSLInfoStruct& subject,
                  aSSLInfoStruct& issuer);
  
  aErr getHeaderVal(acpString& value, const char* key, acpHash* pHeaders); 
  
private:
  int parseURL(const acpString& url, aErr* pErr);
  int base64Write(acpString& dst,
                   const acpString& unenc, 
                   aErr* pErr);
  
  int readBytes(aStreamRef steam,
                const aMemSize length, 
                char* content, 
                aErr* pErr);
  
  int readLine(aStreamRef s, 
              acpString& line, 
              unsigned long nTimeout, 
              aErr* pErr);
  
  int writeLine(aStreamRef s, 
               const acpString& line,
               aErr* pErr);
  
  int getChunked(aStreamRef stream, 
                  acpTypedData* pData,
                  aErr* pErr);
  
  int readRespHeaders(aStreamRef stream,
                      acpHash* pHeaders,
                      aErr* pErr);
  
  int assembleContent(aStreamRef stream,
                      acpTypedData* pData,
                      acpHash* pHeaders,
                      aErr* pErr);
  
  void showSSLInfo(const acpString& name,
                   const aSSLInfoStruct* pInfo);
  
  aIOLib m_ioRef;
  aStreamRef m_connection;
  aStreamRef m_sock;
  
  bool m_bResp;
  
  acpString m_userAgent;
  acpString m_url;
  acpString m_host;
  
  bool m_bSSL;
  bool m_bAuth;
  bool m_bV; //verbosity of output.
  unsigned int m_port;
  unsigned long m_address;
  acpString m_location;
  acpString m_b64Credentials;
  acpString m_method;

  acpList<acpString> m_reqHeaders;
  acpTypedData* m_pRequestBody;
  
  aSSLInfoStruct m_subject; 
  aSSLInfoStruct m_issuer;
  
};

///////////////////////////////////////////////////////////////////////////////

aIO_EXPORT aLIBRETURN
aIO_GetIP4Address(aIOLib ioRef, unsigned long &address,
                  const acpString& fName, 
                  aErr *pErr);

aIO_EXPORT aLIBRETURN
aIO_GetIP4FQDomainName(aIOLib ioRef, acpString &fName,
                    const unsigned long address, 
                    aErr *pErr);

#endif


#endif /* _aIO_H_ */
