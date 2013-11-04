#ifndef USOLIDS_UUtils
#define USOLIDS_UUtils
////////////////////////////////////////////////////////////////////////////////
//
//  UUtils - Utility namespace providing common constants and mathematical
//      utilities.
//
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <limits>
#include <cmath>
#include <cfloat>
#include <vector>
#include <algorithm>

#include "UVector3.hh"
//#include "UTransform3D.hh"

//class UVector3;
class UTransform3D;

enum ExceptionSeverity 
  { FatalError,FatalErrorInArguments,Error,Warning,Info };

namespace UUtils {

	// Mathematical constants
	// Abs

	/*
	inline short  Abs(short d);
	inline int    Abs(int d);
	inline long   Abs(long d);
	inline float  Abs(float d);
	inline double Abs(double d);
	*/

	// Sign
	inline short  Sign(short a, short b);
	inline int    Sign(int a, int b);
	inline long   Sign(long a, long b);
	inline float  Sign(float a, float b);
	inline double Sign(double a, double b);

	/*
	// Min, Max of two scalars
	inline short  Min(short a, short b);
	inline int    Min(int a, int b);
	inline unsigned int Min(unsigned int a, unsigned int b);
	inline long   Min(long a, long b);
	inline unsigned long   Min(unsigned long a, unsigned long b);
	inline float  Min(float a, float b);
	inline double Min(double a, double b);

	inline short  Max(short a, short b);
	inline int    Max(int a, int b);
	inline unsigned int Max(unsigned int a, unsigned int b);
	inline long   Max(long a, long b);
	inline unsigned long   Max(unsigned long a, unsigned long b);
	inline float  Max(float a, float b);
	inline double Max(double a, double b);
	*/

	// Trigonometric
	static const double kPi       = 3.14159265358979323846;
	static const double kTwoPi    = 2.0 * kPi;
	static const double kRadToDeg = 180.0 / kPi;
	static const double kDegToRad = kPi / 180.0;
	static const double kSqrt2    = 1.4142135623730950488016887242097;
	static const double kInfinity = DBL_MAX;

	static const double kMeshAngleDefault=(kPi/4); // Angle for mesh `wedges' in rads
	static const int kMinMeshSections=3;  // Min wedges+1 to make
	static const int kMaxMeshSections=37; // max wedges+1 to make

	inline double Infinity();

	/*
	inline double Sin(double);
	inline double Cos(double);
	inline double Tan(double);
	*/

	inline double ASin(double);
	inline double ACos(double);
	inline double ATan(double);
	inline double ATan2(double, double);

        //Warnings and Errors Messages
        void Exception(const char* originOfException,
                       const char* exceptionCode,
                       ExceptionSeverity severity,
                       int level,
		       const char* description);


	//  inline double Sqrt(double x);

	// Comparing floating points
	inline bool AreEqualAbs(double af, double bf, double epsilon) {
		//return true if absolute difference between af and bf is less than epsilon
		return std::abs(af-bf) < epsilon;
	}
	inline bool AreEqualRel(double af, double bf, double relPrec) {
		//return true if relative difference between af and bf is less than relPrec
		return std::abs(af-bf) <= 0.5*relPrec*(std::abs(af)+std::abs(bf));
	}   

	// Locate Min, Max element number in an array
	long  LocMin(long n, const double *a);
	long  LocMax(long n, const double *a);

	/*
	// Sorting
	void Sort(int n, const double* a, int* index, bool down = true);
	template <typename Element, typename Index>
	void Sort(Index n, const Element* a, Index* index, bool down=true);
	template <typename Iterator, typename IndexIterator>
	void SortItr(Iterator first, Iterator last, IndexIterator index, bool down=true);
	*/

	// TransformLimits: Use the transformation to convert the local limits defined
	// by min/max vectors to the master frame. Returns modified limits.
	void TransformLimits(UVector3 &min, UVector3 &max, const UTransform3D &transformation);    

	double Random(double min=0.0, double max=1.0);

	// Templates:
	template<typename T>
	struct CompareDesc {

		CompareDesc(T d) : fData(d) {}

		template<typename Index>
		bool operator()(Index i1, Index i2) {
			return *(fData + i1) > *(fData + i2);
		}

		T fData;
	};

	template<typename T>
	struct CompareAsc {

		CompareAsc(T d) : fData(d) {}

		template<typename Index>
		bool operator()(Index i1, Index i2) {
			return *(fData + i1) < *(fData + i2);
		}

		T fData;
	};

	int SaveVectorToExternalFile(const std::vector<double> &vector, const std::string &filename);
	int SaveVectorToExternalFile(const std::vector<UVector3> &vector, const std::string &filename);
	int SaveVectorToExternalFile(const std::vector<int> &vector, const std::string &filename);

	std::string ToString(int number);
	std::string ToString(double number);

	// Equations
	//  bool         RootsQuadratic(const double coef[3], double xmin, double xmax);
	//  bool         RootsCubic(const double coef[4],double &x1, double &x2, double &x3);   

	int FileSize(const std::string &filePath);

	int StrPos(const std::string &haystack, const std::string &needle);

	inline double GetRadiusInRing(double rmin, double rmax);

	template <class T>
	inline T sqr(const T& x)
	{
		return x*x;
	}

  inline bool StrEnds (std::string const &fullString, std::string const &ending)
  {
    if (fullString.length() >= ending.length()) {
      return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
      return false;
    }
  }
}

inline double UUtils::GetRadiusInRing(double rmin, double rmax)
{
  // Generate radius in annular ring according to uniform area
  //
  if (rmin<=0.)   { return rmax*std::sqrt(Random()); }
  if (rmin!=rmax) { return std::sqrt(Random()
                           * (sqr(rmax)-sqr(rmin))+sqr(rmin)); }
  return rmin;
}

//____________________________________________________________________________
inline double UUtils::Infinity() { 
	// returns an infinity as defined by the IEEE standard
	return std::numeric_limits<double>::infinity();
}
//---- Abs ---------------------------------------------------------------------


/*
inline short UUtils::Abs(short d)
{ return (d >= 0) ? d : -d; }

inline int UUtils::Abs(int d)
{ return (d >= 0) ? d : -d; }

inline long UUtils::Abs(long d)
{ return (d >= 0) ? d : -d; }


inline double UUtils::Abs(double d)
{
return (d >= 0) ? d : -d;
return fabs(d); //return (d >= 0) ? d : -d; 
}

inline float UUtils::Abs(float d)
{
return fabs(d); // return (d >= 0) ? d : -d; 
}
*/

//---- Sign --------------------------------------------------------------------
inline short UUtils::Sign(short a, short b)
{ return (b >= 0) ? std::abs(a) : -std::abs(a); }

inline int UUtils::Sign(int a, int b)
{ return (b >= 0) ? std::abs(a) : -std::abs(a); }

inline long UUtils::Sign(long a, long b)
{ return (b >= 0) ? std::abs(a) : -std::abs(a); }

inline float UUtils::Sign(float a, float b)
{ return (b >= 0) ? std::abs(a) : -std::abs(a); }

inline double UUtils::Sign(double a, double b)
{ return (b >= 0) ? std::abs(a) : -std::abs(a); }

/*
//---- Min ---------------------------------------------------------------------
inline short std::min(short a, short b)
{ return a <= b ? a : b; }

inline int std::min(int a, int b)
{ return a <= b ? a : b; }

inline unsigned int std::min(unsigned int a, unsigned int b)
{ return a <= b ? a : b; }

inline long std::min(long a, long b)
{ return a <= b ? a : b; }

inline unsigned long std::min(unsigned long a, unsigned long b)
{ return a <= b ? a : b; }

inline float std::min(float a, float b)
{ return a <= b ? a : b; }

inline double std::min(double a, double b)
{ return a <= b ? a : b; }

//---- Max ---------------------------------------------------------------------
inline short std::max(short a, short b)
{ return a >= b ? a : b; }

inline int std::max(int a, int b)
{ return a >= b ? a : b; }

inline unsigned int std::max(unsigned int a, unsigned int b)
{ return a >= b ? a : b; }

inline long std::max(long a, long b)
{ return a >= b ? a : b; }

inline unsigned long std::max(unsigned long a, unsigned long b)
{ return a >= b ? a : b; }

inline float std::max(float a, float b)
{ return a >= b ? a : b; }

inline double std::max(double a, double b)
{ return a >= b ? a : b; }
*/

/*
//---- Trigonometric------------------------------------------------------------
inline double std::sin(double x)
{ return sin(x); }

inline double std::cos(double x)
{ return cos(x); }

inline double UUtils::Tan(double x)
{ return tan(x); }

inline double UUtils::ATan(double x)
{ return atan(x); }

*/

inline double UUtils::ASin(double x)
{ if (x < -1.) return -kPi/2;
if (x >  1.) return  kPi/2;
return std::asin(x);
}

inline double UUtils::ACos(double x)
{ if (x < -1.) return kPi;
if (x >  1.) return 0;
return std::acos(x);
}


inline double UUtils::ATan2(double y, double x)
{ if (x != 0) return  std::atan2(y, x);
if (y == 0) return  0;
if (y >  0) return  kPi/2;
else        return -kPi/2;
}

/*
inline double std::sqrt(double x)
{ return sqrt(x); }
*/

#endif