/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* file: aUtil.h						   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* description: Definition of platform-independent utilities	   */
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


#ifndef _aUtil_H_
#define _aUtil_H_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * define symbol import mechanism
 */

#ifndef aUTIL_EXPORT
#define aUTIL_EXPORT aLIB_IMPORT
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Error codes for all routines.
 */

typedef enum aErr {
  
  aErrNone = 0,		/* 0 */
  aErrMemory,		/* 1 */
  aErrParam,		/* 2 */
  aErrNotFound,		/* 3 */
  aErrFileNameLength,	/* 4 */
  aErrBusy,		/* 5 */
  aErrIO,		/* 6 */
  aErrMode,		/* 7 */
  aErrWrite,		/* 8 */
  aErrRead,		/* 9 */
  aErrEOF,		/* 10 */
  aErrNotReady,		/* 11 */
  
  aErrPermission,	/* 12 */
  aErrRange,		/* 13 */
  aErrSize,		/* 14 */
  aErrOverrun,		/* 15 */
  
  aErrParse,		/* 16 */
  
  aErrConfiguration,	/* 17 */
  aErrTimeout,		/* 18 */
  
  aErrInitialization,	/* 19 */
  
  aErrVersion,		/* 20 */
  
  aErrUnimplemented,	/* 21 */
  
  aErrDuplicate,	/* 22 */
  
  aErrCancel,		/* 23 */
  
  aErrPacket,		/* 24 */
  
  aErrConnection,       /* 25 */
  
  aErrUnknown    /* must be last in list */
  
} aErr;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Win32 definitions
 */

#ifdef aWIN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define aLIBRETURN			int
#define aLIBREF				void *
#define aLIB_IMPORT                     __declspec(dllimport)

#ifdef aDEBUG
#define aDebugAlert(msg)		MessageBox((HWND)NULL, msg,           \
TEXT("Debug"), MB_OK);
#else /* aDEBUG */
#define aDebugAlert(msg)
#endif /* aDEBUG */


/* safe routines which are available on newer platforms, but not older */
#ifdef _MSC_VER
#define aStringCopySafe(d, l, s)	strcpy_s((d), (l), (s))
#define aStringCatSafe(d, l, s)	strcat_s((d), (l), (s))
#define aSNPRINTF                     sprintf_s
#define aSNSCANF(format, size, ...)   sscanf_s(format, ##__VA_ARGS__)
#else /* _MSC_VER */
#define aStringCopySafe(d, l, s)      strcpy(d, s)
#define aStringCatSafe(d, l, s)	strcat((d), (s))
#define aSNPRINTF(format, size, ...)	sprintf(format, ##__VA_ARGS__)
#define aSNSCANF(format, size, ...)   sscanf(format, ##__VA_ARGS__)
#endif /* _MSC_VER */


#define aMemCopy(dst, src, len)		memcpy(dst, src, len)
#define aBZero(buffer, len)		memset(buffer, 0x00, len)
#define aStringCopy(dst, src)		strcpy(dst, src)
#define aStringNCopy(dst, src, n)	strncpy(dst, src, n)
#define aStringCompare(s1, s2)		strcmp(s1, s2)
#define aStringLen(s)			strlen(s)
#define aStringCat(s1, s2)		strcat(s1, s2)
#define aStringFromInt(s, i)		aSNPRINTF(s, sizeof(s), "%d", i)
#define aIntFromString(pi, s)		aSNSCANF(s, sizeof(s), "%d", pi)


#define aMemHandle			void *
#define aMemPtr				void *
#define aMemSize			size_t

#define aByte				char
#define aShort				short
#define aFloat				float
#define aInt32				int
#define aUInt8                          unsigned char
#define aUInt16                         unsigned short
#define aUInt32                         unsigned int

#ifndef aLEAKCHECK
#define aMemAlloc(size)		malloc(size)
#define aMemFree(p)			free(p)
#endif /* aLEAKCHECK */

#define aszEOL				"\x0D\x0A"

#define aLITTLEENDIAN

#endif /* aWIN */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * UNIX definitions (all Unix platforms and MacOS X)
 */

#if defined(aUNIX) || defined(aMACX)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define aLIBRETURN			int
#define aLIBREF				void *
#define aLIB_IMPORT

#ifdef aDEBUG
#ifdef aMACX
#define aDebugAlert(msg)		printf("ERROR! %s\n", msg)
#else
#define aDebugAlert(msg)		printf("ERROR! %s\n", msg)
#endif
#else
#define aDebugAlert(msg)
#endif

#define aMemCopy(dst, src, len)		memcpy(dst, src, len)
#define aBZero(buffer, len)		memset(buffer, 0x00, len)
#define aStringCopy(dst, src)		strcpy(dst, src)
#define aStringNCopy(dst, src, n)	strncpy(dst, src, n)
#define aStringCompare(s1, s2)		strcmp(s1, s2)
#define aStringLen(s)			strlen(s)
#define aStringCat(s1, s2)		strcat(s1, s2)
#define aStringFromInt(s, i)		snprintf(s, sizeof(s), "%d", (int)i)
#define aIntFromString(pi, s)		sscanf(s, "%d", pi)

/* safe routines */
#ifndef strlcpy
#ifdef __cplusplus
extern "C" {
#endif
aUTIL_EXPORT size_t strlcpy(char* dst, const char* src, size_t len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#ifndef strlcat
#ifdef __cplusplus
extern "C" {
#endif
aUTIL_EXPORT size_t strlcat(char *dst, const char *src, size_t size);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#define aStringCopySafe(d, l, s)	strlcpy((d), (s), (l))
#define aStringNCopySafe(d, l, s)	strlcpy((d), (s), (l))
#define aStringCatSafe(d, l, s)	        strlcat((d), (s), (l))
#define aSNPRINTF                       snprintf
#define aSNSCANF(format, size, ...)     sscanf(format, ##__VA_ARGS__)

#define aByte				char
#define aShort				short
#define aFloat				float
#define aInt32				int
#define aUInt8                          unsigned char
#define aUInt16                         unsigned short
#define aUInt32                         unsigned int

#ifdef aMACX
#define aMemHandle			char**
#else
#define aMemHandle			void *
#endif
#define aMemPtr				void *
#define aMemSize			size_t

#ifndef aLEAKCHECK
#define aMemAlloc(size)			malloc(size)
#define aMemFree(p)			free(p)
#endif /* aLEAKCHECK */

#define aszEOL				"\n"

#ifdef aMACX 

#ifdef __BIG_ENDIAN__
#define aBIGENDIAN
#elif __LITTLE_ENDIAN__
#define aLITTLEENDIAN
#endif

#else /* else not mac so we are likely little endian */
#define aLITTLEENDIAN
#endif

#endif /* aUNIX */





/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common word order handling based on aBIGENDIAN or aLITTLEENDIAN
 * defines created above
 */

#ifdef aBIGENDIAN    /* network word order */
#define aH2TS(s)			s
#define aT2HS(s)			s
#define aH2TUS(s)			s
#define aT2HUS(s)			s
#define aH2TA(s)			s
#define aT2HA(s)			s
#else
#ifdef aLITTLEENDIAN
#define aH2TS(s)  (aSHORT)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#define aT2HS(s)  (aSHORT)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#define aH2TUS(s) (unsigned aSHORT)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#define aT2HUS(s) (unsigned aSHORT)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#define aH2TA(s)  (tADDRESS)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#define aT2HA(s)  (tADDRESS)(((s & 0x00FF) << 8) | ((s & 0xFF00) >> 8))
#else
error!!!! Word Order Not Defined and likely no prefix header
#endif
#endif 



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Maximum number of filename bytes.
 */
#define aFILE_NAMEMAXCHARS	127

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * We define a unique boolean type to avoid conflict with other 
 * software the libraries or software may interact with.
 */
typedef int aBool;
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * These seem obvious but it can be quite useful to flush out 
   * bugs to use these instead of 1 and 0 directly.
   */
#define aTrue 1
#define aFalse 0
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * some compilers don't define NULL so we do it here
   */
#ifndef NULL
#define NULL 0L
#endif /* NULL */
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * signature in front of all compiled tea files
   */
#define aTEA_4BYTE_SIGNATURE	"aTEA"
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * math stuff
   */
#define aPI 3.1415926535897932384626433832795
#define a2PI 6.283185307179586476925286766559

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * basic types
   */
#define aSHORT	short
#define aFLOAT	float

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * word alignment for use in math optimizations, etc.
   */
#define aWORD_ALIGN_PAD(a) (((a) > 1) ? ((((a)-1)|3)+1) : (a))

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * version packing to uniformly handle major, minor, and builds
   */
  
#define aVERSION_PACK(maj, min, bld) (((maj) << 28)                  \
| ((min) << 24)                \
| ((bld) & 0xFFFFFF))
#define aVERSION_UNPACK_MAJOR(pack) ((pack) >> 28)
#define aVERSION_UNPACK_MINOR(pack) ((pack & 0xF000000) >> 24)
#define aVERSION_UNPACK_BUILD(pack) ((pack) & 0xFFFFFF)

  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * definition of a color (RGB)
   */

typedef unsigned long aColorType;



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * utility routines
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
aUTIL_EXPORT aLIBRETURN 
aUtil_ParseInt(int* pInt,
	       const char* pString,
	       aErr* pErr);

aUTIL_EXPORT aLIBRETURN
aUtil_ParseFloat(float* pFloat,
		 const char* pString,
		 aErr* pErr);

aUTIL_EXPORT aLIBRETURN
aUtil_ULongFromInetAddr(unsigned long* pAddr,
			const char* pString,
			aErr* pErr);
  
aUTIL_EXPORT aLIBRETURN
aUtil_FormatInetAddr(char* address,
		     const unsigned long ulAddr,
		     aErr* pErr);
  
aUTIL_EXPORT aLIBRETURN
aUtil_GetFileExtension(char* extension, 
		       const char* filename,
		       aErr* pErr);

aUTIL_EXPORT aLIBRETURN
aUtil_GetFileRoot(char* root, 
		  const char* filename,
		  aErr* pErr);

/* these routines resolve the endian mess on all platforms */
aUTIL_EXPORT void 
aUtil_StoreShort(char* storage, 
		 aSHORT val);

aUTIL_EXPORT void 
aUtil_StoreInt(char* storage, 
	       int val);

aUTIL_EXPORT void 
aUtil_StoreFloat(char* storage, 
		 float val);
  
aUTIL_EXPORT void 
aUtil_StoreLong(char* storage, 
		long l);

aUTIL_EXPORT aSHORT 
aUtil_RetrieveShort(const char* storage);

aUTIL_EXPORT int 
aUtil_RetrieveInt(const char* storage);

aUTIL_EXPORT float
aUtil_RetrieveFloat(const char* storage);

aUTIL_EXPORT unsigned short
aUtil_RetrieveUShort(const char* storage);

aUTIL_EXPORT const char* 
aString_StartsWith(const char* string,
		   const char* prefix);

aUTIL_EXPORT const char* 
aString_CopyToWS(char* copy, 
		 const int nMaxLen,
		 const char* source);

aUTIL_EXPORT const char* 
aString_CopyToChar(char* copy, 
		   const char* source,
		   const char c);
  
#ifdef __cplusplus 
}
#endif /* __cplusplus */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common piece for aAssert implementation
 */

#ifdef aDEBUG
#define aAssert(exp) if ((exp) == 0) aAssertion(#exp, __FILE__, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
aUTIL_EXPORT void 
aAssertion(const char* expression,
	   const char* filename,
	   const unsigned int linenum);
  
#ifdef __cplusplus 
}
#endif /* __cplusplus */
#else /* aDEBUG */
  #define aAssert(exp)
#endif /* aDEBUG */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Common piece for leak check implementation
 */

#ifdef aLEAKCHECK

#define aMemAlloc(s) aLeakCheckAlloc(s, __FILE__, __LINE__)
#define aMemFree(p) aLeakCheckFree(p, __FILE__, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif  

  aUTIL_EXPORT aMemPtr 
  aLeakCheckAlloc(aMemSize size,
		  const char* filename,
		  const unsigned int linenum);
  aUTIL_EXPORT void 
  aLeakCheckFree(aMemPtr p,
		 const char* filename,
		 const unsigned int linenum);
  aUTIL_EXPORT int 
  aLeakCheckCleanup(void);

#ifdef __cplusplus
}
#endif

#else /* aLEAKCHECK */

#define aLeakCheckCleanup() 0

#endif /* aLEAKCHECK */


#ifdef __cplusplus
///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT acpString 
{
public:
  acpString(void);
  acpString(const acpString& string);
  acpString(const char* pValue);
  acpString(const int nValue);
  virtual ~acpString(void);
  
  operator const char*(void) const; 
  
  const acpString& operator=(const acpString& RHS);
  const acpString& operator=(const char* pRHS);
  const acpString& operator=(char* pRHS);
  
  const char* operator+=(const acpString& RHS);
  const char* operator+=(const char* pRHS);
  const char* operator+=(const char cRHS);
  const char* operator+=(const unsigned char cRHS);
  const char* operator+=(const short sRHS);
  const char* operator+=(const unsigned short usRHS);
  const char* operator+=(const int iRHS);
  const char* operator+=(const unsigned int uiRHS);
  const char* operator+=(const long lRHS);
  const char* operator+=(const unsigned long ulRHS);
  const char* operator+=(const float fRHS);

  const bool operator==(const acpString& rhs) const;
  const bool operator==(char* rhs) const;
  const bool operator==(const char* rhs) const;

  const bool operator!=(const acpString& rhs) const;
  const bool operator!=(char* rhs) const;
  const bool operator!=(const char* rhs) const;
  
  const size_t length(void) const;
  const char* trim(void);
  const char* trimTo(const char c,
		     const bool bFromEnd = false);
  const char* keepTo(const char c,
		     const bool bFromEnd = false);
  const char* truncate(const char* pSuffix);
  const char* substring(const size_t offset,
			const size_t length);
  void lowercase(void);
  const char* capitalize(void);
  const bool startsWith(const char* prefix) const;
  const bool endsWith(const char* suffix) const;
  const char* format(const char* fmt, ...);
  void copyToBuffer(char* buffer, 
		    const size_t max) const;
  unsigned int countChar(const char c) const;
  const char* replace(const char* target, 
		      const char* replacement,
		      const bool bOnlyFirst = false);

protected:
  char* m_pStorage;
  size_t m_nStorageCapacity;
  size_t m_length;
  
private:
  void assume(acpString& s);
};



// inline functions (must be in header file)
inline acpString::operator const char*(void) const 
{ return m_pStorage; }
inline const size_t acpString::length(void) const
{ return m_length; }
inline const bool acpString::operator==(const acpString& rhs) const
{ return !aStringCompare(m_pStorage, rhs.m_pStorage); }
inline const bool acpString::operator==(char* rhs) const
{ return !aStringCompare(m_pStorage, rhs); }
inline const bool acpString::operator==(const char* rhs) const
{ return !aStringCompare(m_pStorage, rhs); }
inline const bool acpString::operator!=(const acpString& rhs) const
{ return (aStringCompare(m_pStorage, rhs.m_pStorage) != 0); }
inline const bool acpString::operator!=(char* rhs) const
{ return (aStringCompare(m_pStorage, rhs) != 0); }
inline const bool acpString::operator!=(const char* rhs) const
{ return (aStringCompare(m_pStorage, rhs) != 0); }



///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT acpException
{
public:
  acpException(void) throw();
  acpException(const acpException& e) throw();
  acpException(const aErr err,
	       const char* text) throw();
  ~acpException(void) throw();
  
  const aErr error(void) const throw(); 
  const char* msg(void) const throw(); 
  
private:
  aErr			m_err;
  char*			m_pMsg;
};

inline const aErr acpException::error(void) const throw() 
{ return m_err; }
inline const char* acpException::msg(void) const throw() 
{ return m_pMsg; }


///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT _listnode {
public:
  _listnode(void* pObject) : 
    m_pObject(pObject),
    m_pNext(NULL),
    m_pPrev(NULL) 
    {}
  virtual ~_listnode(void) 
    {}
  
  void* m_pObject;
  
  _listnode* m_pNext;
  _listnode* m_pPrev;
  
  friend class _listbase;
  friend class _listiter;
};

class aUTIL_EXPORT _listbase {
public:
  _listbase() :
  m_nLength(0),
  m_pHead(0L),
  m_pTail(0L) {}
  virtual		~_listbase() {}
  
  const int		length() const 
  {return m_nLength;}
  
protected:
  void		toFront(void* pObject);
  void		toBack(void* pObject);
  
  void*		head() const
  {return m_pHead ? m_pHead->m_pObject : 0L;}
  void*		removeHead();
  
  void*		tail() const
  {return m_pTail ? m_pTail->m_pObject : 0L;}
  void*		removeTail();
  void*		operator[](const int i) const;
  void*		remove(void* pObject);
  
  int			m_nLength;
  
  _listnode*		m_pHead;
  _listnode*		m_pTail;
  
private:
#ifdef aDEBUG
  bool		hasIntegrity() const;
#endif /* aDEBUG */
  
  friend class _listiter;
};

class aUTIL_EXPORT _listiter {
public:
  _listiter(const _listbase* l) :
  m_pList(l),
  m_pCurrent(l->m_pHead) {}
  virtual ~_listiter(void) {}
  inline void reset(void) {m_pCurrent = m_pList->m_pHead;}
protected:
  void* next(void);
private:
  const _listbase* m_pList;
  _listnode* m_pCurrent;
};

template<class T>
class acpList : public _listbase {
public:
  acpList<T>(void) {}
  virtual ~acpList<T>(void) {
    while (m_pHead) {
      _listnode* n = m_pHead;
      m_pHead = m_pHead->m_pNext;
      T* p = (T*)n->m_pObject;
      delete p;
      delete n;
    }
  }  
  inline void add(const T* x) { toBack((void*)x); }
  inline void addToHead(const T* x) { toFront((void*)x); }
  inline void addToTail(const T* x) { toBack((void*)x); }
  inline T* head(void) const { return (T*)_listbase::head(); }
  inline T* removeHead(void) { return (T*)_listbase::removeHead(); }
  inline T* tail(void) const { return (T*)_listbase::tail(); }
  inline T* removeTail(void) { return (T*)_listbase::removeTail(); }
  inline T* operator[](const int i) const 
    { return (T*)_listbase::operator[](i); }
  inline T* remove(const T* x) { return (T*)_listbase::remove((void*)x); }
  inline const bool isEmpty(void) const { return m_nLength == 0; }
};

template<class T>
class acpListIterator : public _listiter {
public:
  acpListIterator(const acpList<T>& list) : 
  _listiter(&list) {}
  virtual ~acpListIterator(void) {}
  inline T* next(void) {return (T*)_listiter::next();}
};

#define aLISTITERATE(t, l, v)                                       \
                                                                    \
t* v;                                                               \
acpListIterator<t> _alist(l);                                       \
while ((v = _alist.next()))


///////////////////////////////////////////////////////////////////////////////

class acpThread;
class aUTIL_EXPORT acpMessage 
{
public:
  acpMessage(acpThread* pThread = 0L) :
  m_pcThread(pThread),
  m_synchronize(0L) {}
  virtual ~acpMessage();
  
  virtual void process() = 0;

  acpThread* m_pcThread;
  void* m_synchronize;
};


///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT acpRunable {
public:
  acpRunable() {}
  virtual ~acpRunable() {}
  
  virtual int run() = 0;
};


///////////////////////////////////////////////////////////////////////////////
// 5 second wait time for syncronous thread message calls

#define aMAXTHREADSYNCWAITMS	5000

class acpRunable;
class aUTIL_EXPORT acpThread {
  
public:
  acpThread(const char* name);
  virtual ~acpThread(void);

  virtual void start(acpRunable* pRunable) = 0;
  virtual void sendMessage(acpMessage* pMessage,
			   const bool bAsync) = 0;  
  virtual acpMessage* getMessage() = 0;
  virtual bool yield(const unsigned long msec) = 0;
  virtual bool isDone(void) 
    { return m_bDone; }
  virtual bool isThreadContext() = 0;  
  virtual void sync(acpMessage* pMessage) = 0;
  virtual bool handleMessage(void);

protected:
  bool m_bRunning;
  bool m_bDone;
  unsigned long	m_nThreadID;
  unsigned long m_nCallerThreadID;
  acpRunable* m_pRunable;  
  acpList<acpMessage> m_messages;
};


///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT acpMutex {
public:
  acpMutex(const char* name) {}
  virtual		~acpMutex() {}
  
  virtual void        lock() = 0;
  virtual void        unlock() = 0;
};

///////////////////////////////////////////////////////////////////////////////

#define aKEY_END_OF_TEXT     0xFF03
#define aKEY_BACKSPACE	     0xFF08
#define aKEY_TAB             0xFF09
#define aKEY_RETURN          0xFF0D
#define aKEY_ESCAPE          0xFF1B
#define aKEY_HOME	     0xFF50
#define aKEY_LEFT_ARROW	     0xFF51
#define aKEY_UP_ARROW	     0xFF52
#define aKEY_RIGHT_ARROW     0xFF53
#define aKEY_DOWN_ARROW	     0xFF54
#define aKEY_PAGE_UP         0xFF55
#define aKEY_PAGE_DOWN       0xFF56
#define aKEY_END             0xFF57
#define aKEY_BEGIN           0xFF58
#define aKEY_SHIFT_L         0xFFE1
#define aKEY_SHIFT_R         0xFFE2

typedef unsigned int aKeyType;

class aUTIL_EXPORT acpCommandLine {

public:
  typedef enum {
    standard,
    white,
    yellow,
    green,
    red,
    blue,
    raw
  } eColor;
  
  acpCommandLine(const bool bRaw = false);
  virtual ~acpCommandLine(void);
  
  void setPrompt(const char* prompt);
  bool nextLine(acpString& line);
  void displayLine(const acpString& line, const eColor lineColor);
  unsigned long getCharCount(void) const { return m_nOutputChars; }
  
protected:
  
  ///////////////////////////////////////////////////////////////////////
  // These three routines are all that is needed to establish a command
  // line interface with history, cursor management, etc.  These must be
  // all implemented for any platform.  The nextRawChar call is only used
  // in the input line below so it can be made dummy if input is overloaded
  // and handled otherwise.
  
  // returns the next single character (8-byte) or -1 if none available
  virtual int nextRawChar(void) = 0;
  // outputs a single byte
  virtual void outputRawChar(const char c) = 0;
  // outputs multiple bytes
  virtual void outputRawChars(const char* s, const unsigned int len) = 0;
  
  ///////////////////////////////////////////////////////////////////////
  // The following methods are used to establish our default consistent 
  // behavior for commmand lines.  These are handled differently for
  // different platforms and clients so they can be overloaded 
  // appropriately.  The defaults here resolve to standard ANSI escape
  // sequences for standard terminals.  These all work well when the 
  // client connection is through a socket or other stream that is accessed
  // from a terminal (potentially remotely).  These are typically overloaded
  // for STDIO calls directly in a shell (like an application running in a 
  // DOS shell.)
  
  // moves cursor from where it is in either direction (zero based)
  virtual void moveCursor(const int p);
  // plays the bell tone
  virtual void bell(void);
  // scrolls up one line and begins with the cursor at 0
  virtual void newline(void);
  // removes anything on the line leaving the cursor at 0
  virtual void eraseline(void);
  // retrieves the next character or 0 if none without blocking
  virtual aKeyType input(void);
  // changes output color
  virtual void color(const eColor color);

  // true when in raw mode
  bool m_bRaw;

private:
  acpMutex* m_pMutex;
  void lockCursor();
  void unlockCursor();

  void ensureStorage();
  void setLine(const acpString& string);
  char* m_line;
  size_t m_lineLen;
  size_t m_lineStorage;
  acpString m_prompt;
  eColor m_color;
  bool m_bComplete;
  bool m_bEditing;
  int m_position;

  unsigned long m_nOutputChars;
  void outputCountedChar(const char c) {
    m_nOutputChars++;
    outputRawChar(c);
  }
  void outputCountedChars(const char* s, const unsigned int len) {
    m_nOutputChars += len;
    outputRawChars(s, len);
  }

  int m_historySize;
  int m_historyCur;
  acpList<acpString> m_history;
};


///////////////////////////////////////////////////////////////////////////////

// returns true to keep traversing
typedef bool (*acpHashProc)(const char* key, 
			    const char* value, 
			    void* ref);
class acpHashNode;
class aUTIL_EXPORT acpHash {
public:
  acpHash();
  ~acpHash();  
  void add(const char* key, const char* value);
  void set(const char* key, const char* value);
  const char* find(const char* key);
  unsigned int traverse(acpHashProc, void* vpRef);
  unsigned int count() { return traverse(NULL, NULL); }
private:
  acpHashNode* lookup(const char* key);
  acpHashNode* m_pTree;
};


///////////////////////////////////////////////////////////////////////////////

class aUTIL_EXPORT acpOSFactory {
private:
  acpOSFactory() {}
  virtual ~acpOSFactory() {}  
public:
  static acpThread* thread(const char* name);
  static acpMutex* mutex(const char* name);
  static acpCommandLine* commandLine(const bool bRaw = false);
};

#endif // __cplusplus
#endif /* _aUtil_H_ */
