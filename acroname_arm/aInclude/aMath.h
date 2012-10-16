/////////////////////////////////////////////////////////////////////
//                                                                 //
// file: aMath.h		 	  	                   //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
// description: Implementation of general purpose math routines.   //
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

#ifndef _aMath_H_
#define _aMath_H_

#include <math.h>

#include "aIO.h"

/////////////////////////////////////////////////////////////////////
// define symbol import mechanism

#ifndef aMATH_EXPORT
#define aMATH_EXPORT aLIB_IMPORT
#endif

#define aSQRT(a) sqrt(a)
#define aACOS(a) acos(a)
#define aRECIP(a) ((float)(1.0f/(a)))

// returns nearly equal based on a tolerance
#define aNEAR 0.00001f
#define aNEARLY_EQUAL(v1, v2, tolerance)                            \
  ((((v1) - (tolerance)) < (v2)) && ((v1) + (tolerance) > (v2)))


/////////////////////////////////////////////////////////////////////
// type classes
/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpByte {
public:
  acpByte(void) : m_val(0) {}
  acpByte(const acpByte& b) : m_val(b.m_val) {}
  acpByte(const int i) : m_val((aByte)i) {}
  acpByte(aStreamRef stream);
  acpByte(const aByte c) : m_val(c) {}
    
  aErr writeToStream(aStreamRef stream) const;
    
  operator aByte(void) const { return m_val; }

private:
  aByte m_val;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpShort {
public:
  acpShort(aStreamRef stream);
  acpShort(const aShort nVal) : m_val(nVal) {}
    
  void writeToStream(aStreamRef stream) const; 
    
  operator aShort() const { return m_val; }

private:
  aShort m_val;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpInt32 {
public:
  acpInt32(const aInt32 nVal) : m_val(nVal) {}
  acpInt32(aStreamRef stream);
    
  void writeToStream(aStreamRef stream) const; 
    
  operator aInt32() const { return m_val; }

private:
  aInt32 m_val;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpFloat
{
  public:
    acpFloat(const float f) : m_val(f) {}
    acpFloat(const aStreamRef stream);
    
    void writeToStream(aStreamRef stream) const;
    
    operator float() const
    { return m_val; }

  private:
    aFloat m_val;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpVec2 {

public:
  acpVec2(void) : m_x(0), m_y(0) {}
  acpVec2(const acpVec2& v) : m_x(v.m_x), m_y(v.m_y) {}
  acpVec2(const aFloat fX, 
	  const aFloat fY) : m_x(fX), m_y(fY) {}

  const bool operator==(const acpVec2& rhs);
  const acpVec2& operator+=(const acpVec2& rhs);

  aFloat length(void) const;
  aFloat dir(void) const;
  
  aFloat x(void) const { return m_x; }
  aFloat y(void) const { return m_y; }

private:
  aFloat m_x;
  aFloat m_y;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpPose2
{
public:
  acpPose2(void) : m_angle(0) {}

  operator acpVec2(void) const { return m_position; }
  operator aFloat(void) const { return m_angle; }
  const bool operator==(const acpPose2& rhs);
  void translate(const acpVec2& v);
  void rotate(const aFloat angle);  

  const acpVec2 operator*(const acpVec2& rhs) const;
  
private:
  acpVec2 m_position;
  aFloat m_angle;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpVec3 {
public:
  acpVec3(const bool init) : m_x(0), m_y(0), m_z(0) {}
  acpVec3(void);
  acpVec3(const aStreamRef stream);
  acpVec3(const aFloat x,
	  const aFloat y,
	  const aFloat z);
  acpVec3(const acpVec3 &v);
  acpVec3(const acpVec3* pV);
  acpVec3(const aFloat* pV);
  ~acpVec3(void) {}

  const acpVec3&	operator=(const acpVec3& rhs);
  const bool		operator==(const acpVec3& rhs) const;
  const bool		operator!=(const acpVec3& rhs) const;
  const acpVec3		operator+(const acpVec3& rhs) const;
  const acpVec3		operator+(void) const;
  const acpVec3&	operator+=(const acpVec3& rhs);
  const acpVec3		operator-(const acpVec3& rhs) const;
  const acpVec3		operator-(void) const;
  const acpVec3&	operator-=(const acpVec3& rhs);
  const acpVec3&	operator*=(const aFloat& scalar);
  const acpVec3&	operator/=(const aFloat& scalar);
  const acpVec3		operator*(const aFloat& scalar) const;
  const acpVec3		operator*(const double& scalar) const;
  const acpVec3		operator/(const aFloat& scalar) const;
  const acpVec3		cross(const acpVec3& rhs) const;
  const acpVec3		operator^(const acpVec3& rhs) const;
  const aFloat		dot(const acpVec3 &rhs) const;
  const aFloat		operator%(const acpVec3 &rhs) const;
  const aFloat		length(void) const;
  const acpVec3		unit(const acpVec3& v) const;
  void			normalize(void);
  const aFloat		operator!(void) const;
  const acpVec3		operator|(const aFloat& length) const;
  const acpVec3		operator|=(const aFloat& length);
  float 	  	angle(const acpVec3& vector) const;
  acpVec3 	  	reflection(const acpVec3& normal) const;
  void			zero(void);
  bool			nearly(const acpVec3& v,
			       const aFloat fTolerance = aNEAR) const;
  
  aErr			listToStream(const aStreamRef stream) const;
  void			writeToStream(const aStreamRef stream) const;
  void			format(acpString& v) const;

  aFloat m_x;
  aFloat m_y;
  aFloat m_z;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpMatrix3 {  
public:
  acpMatrix3(const bool init) {}
  acpMatrix3(void);
  acpMatrix3(const aStreamRef stream);
  acpMatrix3(const acpVec3& X,
	     const acpVec3& Y,
	     const acpVec3& Z);
  acpMatrix3(const double angle,
	     const acpVec3& axis);
  ~acpMatrix3(void) {}

  const acpVec3	    operator*(const acpVec3& rhs) const;
  const acpMatrix3  operator*(const acpMatrix3& rhs) const;
  const acpMatrix3& operator=(const acpMatrix3& rhs);
  
  const aFloat determinant(void) const;
  const acpMatrix3& invert(void);

  void produce(aFloat* pResult, const acpMatrix3& rhs) const;
  
  void writeToStream(const aStreamRef stream) const;

  bool nearly(const acpMatrix3& t,
	      const aFloat fTolerance = aNEAR) const;
  
  aFloat m_f[9];
};

inline const acpMatrix3& acpMatrix3::operator=(const acpMatrix3& rhs)
{ aMemCopy(m_f, rhs.m_f, 9 * sizeof(aFloat)); return *this; }

/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpMatrix4 {  
public:
  acpMatrix4(void);
  acpMatrix4(const acpMatrix3& M);
  acpMatrix4(const acpVec3& V);
  acpMatrix4(const double angle,
	     const acpVec3& axis);
  ~acpMatrix4(void) {}
  
  inline const acpMatrix4& operator=(const acpMatrix4& rhs)
    { aMemCopy(m_f, rhs.m_f, 16 * sizeof(aFloat)); 
      return *this; }
  const acpMatrix4 operator*(const acpMatrix4& rhs);
  void transpose(void);
  void transform3f(const float* pOriginal,
		   float* pTransformed);
  void rotate3f(const float* pOriginal,
		float* pTransformed);
  
  aFloat m_f[16];
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpTransform {
public:
  acpTransform(const bool bInit = true);
  acpTransform(const aStreamRef stream) : 
    m_scale(stream),
    m_rotation(stream),
    m_translation(stream) {}
  acpTransform(const acpVec3& scale,
	       const acpMatrix3& rotation,
	       const acpVec3& translation);
  virtual ~acpTransform(void) {}
    
  virtual void writeToStream(const aStreamRef stream) const;
    
  virtual aErr listToStream(const aStreamRef stream) const;

  const acpTransform& operator=(const acpTransform& rhs);
  const acpTransform operator*(const acpTransform& rhs) const;
  const acpTransform& operator*=(const acpTransform& rhs);
  const acpVec3 operator*(const acpVec3& rhs) const;
  void produce(const acpTransform& lhs,
	       const acpTransform& rhs);
  void scale(const acpVec3& s);
  void translate(const acpVec3& t);
  void translate3f(const float* pOriginal,
		   float* pTransformed,
		   const unsigned int nPoints = 1) const;
//  void rotate(const acpMatrix3& r);
  void rotate(const acpVec3& axis,
	      const double angle);
  void rotate3f(const float* pOriginal,
		float* pTransformed,
		const unsigned int nPoints = 1) const;
  void transform3f(const float* pOriginal,
		   float* pTransformed,
		   const unsigned int nPoints = 1) const;
  const acpVec3& getTranslation() const { return m_translation; }
  void setTranslation(const acpVec3& t) { m_translation = t; }
  const acpMatrix3& getRotation() const { return m_rotation; }
  bool isIdentity(void) const;
  bool nearly(const acpTransform& t,
	      const aFloat fTolerance = aNEAR) const;

protected:
  acpVec3 m_scale;
  acpMatrix3 m_rotation;
  acpVec3 m_translation;
};


/////////////////////////////////////////////////////////////////////

class aMATH_EXPORT acpQuaternion {
public:
  acpQuaternion(void) : m_x(1), m_y(0), m_z(0), m_w(0) {}
  acpQuaternion(const aFloat x,
		const aFloat y,
		const aFloat z,
		const aFloat angle);
  acpQuaternion(const acpVec3& axis,
		const aFloat angle);
  ~acpQuaternion(void) {}
  
  const acpQuaternion&	operator=(const acpQuaternion& rhs);
  const acpQuaternion	operator*(const acpQuaternion& rhs) const;
  const acpQuaternion&	operator*=(const acpQuaternion& rhs);
  const acpQuaternion&	operator/=(const double scalar);
  
  void set4x4Matrix(aFloat* pMatrix) const;
  
  double length(void) const;
  void normalize(void);
  
  aFloat m_x;
  aFloat m_y;
  aFloat m_z;
  aFloat m_w;
};

class aMATH_EXPORT acpMatrix {
public:

  // Multiply m1 (m x n) by m2 (n x o) and store in result (m x o)
  // assumes that all three matrices are word aligned using
  // aWORD_ALIGN_PAD which leaves extra bytes at the end of rows
  // up to the word boundary.
  static void multiply(float* pResult, 
                       const float* pM1,
		       const float* pM2,
		       const unsigned int m,
		       const unsigned int n,
		       const unsigned int o);
  static void multiply(double* pResult, 
                       const double* pM1,
		       const double* pM2,
		       const unsigned int m,
		       const unsigned int n,
		       const unsigned int o);

  static void multiplyT1(float* pResult, 
			 const float* pM1,
			 const float* pM2,
			 const unsigned int m,
			 const unsigned int n,
			 const unsigned int o);
  static void multiplyT1(double* pResult, 
			 const double* pM1,
			 const double* pM2,
			 const unsigned int m,
			 const unsigned int n,
			 const unsigned int o);

  static void multiplyT2(float* pResult, 
			 const float* pM1,
			 const float* pM2,
			 const unsigned int m,
			 const unsigned int n,
			 const unsigned int o);
  static void multiplyT2(double* pResult, 
			 const double* pM1,
			 const double* pM2,
			 const unsigned int m,
			 const unsigned int n,
			 const unsigned int o);
};

#endif // _aMath_H_
