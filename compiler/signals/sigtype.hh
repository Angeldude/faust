/************************************************************************
 ************************************************************************
    FAUST compiler
	Copyright (C) 2003-2004 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************
 ************************************************************************/



#ifndef _SigType_
#define _SigType_

#include <vector>
#include <string>
#include <iostream>
#include "smartpointer.hh"
#include "interval.hh"
#include "instructions.hh"

/*********************************************************************
*
*						Type System for FAUST
*
*	<type> 			::= <simpletype> ||  table(<type>)
*			 		 	||  <type>|<type>  || <type>*<type>
*	<simpletype>	::= <nature><variability><computability><vectorability>||<boolean>
*	<nature>		::= TINT || TREAL
*	<variability>	::= TKONST || TBLOCK || TSAMP
*	<computability>	::= TCOMP  || TINIT  || TEXEC
*	<vectorability> ::= KVECT  || KSCAL  || KTRUESCAL
*	<boolean>       ::= KNUM   || KBOOL
*
**********************************************************************/


//--------------------------------------------------
// qualite des types simples

enum { kInt = 0, kReal = 1 };                           ///< nature : integer or floating point values
enum { kNum = 0 , kBool = 1};                           ///< boolean : when a signal stands for a boolean value ( while being of c-type int or float )
enum { kKonst = 0, kBlock = 1, kSamp = 3 };             ///< variability : how fast values change
enum { kComp = 0, kInit = 1, kExec = 3 };               ///< computability : when values are available
enum { kVect = 0, kScal = 1, kTrueScal = 3/*, kIndex = 4*/};///< vectorability: when a signal can be vectorized ( actually, only kVect and kScal matter; kTrueScal and kIndex don't denote types but are here to simplify code generation )

/*---------------------------------------------------------------------

	AbstractType :
	The root class for SimpleType, TableType and TupletType

	Type :
	A smartPointer to type

----------------------------------------------------------------------*/

using namespace std;

class AudioType;

typedef P<AudioType> Type;

/**
 * The Root class for all audio data types.
 * All audio types have a "variability" (how fast the values change) and
 * a "computability" (when the values are available). Simple types have
 * also a "nature" (integer or floating point).
 */
class AudioType
{
  protected:
	const int   		fNature;  			///< the kind of data represented
	const int   		fVariability; 		///< how fast values change
	const int   		fComputability;		///< when are values available
    const int   		fVectorability;     ///< when a signal can be vectorized
    const int   		fBoolean;           ///< when a signal stands for a boolean value

    const interval	    fInterval;          ///< Minimal and maximal values the signal can take


  public :
	AudioType(int n, int v, int c, int vec = kVect, int b = kNum, interval i=interval())
		  : fNature(n), fVariability(v), fComputability(c),
		    fVectorability(vec), fBoolean(b),
		    fInterval(i) {}									///< constructs an abstract audio type
	virtual ~AudioType() 									{} 	///< not really useful here, but make compiler happier

	int 	nature() 		const	{ return fNature; 		}	///< returns the kind of values (integre or floating point)
	int 	variability() 	const	{ return fVariability; }	///< returns how fast values change (constant, by blocks, by samples)
	int 	computability() const	{ return fComputability;}	///< returns when values are available (compilation, initialisation, execution)
	int 	vectorability() const 	{ return fVectorability;} 	///< returns when a signal can be vectorized
	int 	boolean() 		const	{ return fBoolean; } 		///< returns when a signal stands for a boolean value

	interval getInterval() 	const	{ return fInterval; }		///< returns the interval (min dn max values) of a signal


	virtual AudioType* promoteNature(int n)		= 0;			///< promote the nature of a type
	virtual AudioType* promoteVariability(int n)	= 0;			///< promote the variability of a type
	virtual AudioType* promoteComputability(int n)	= 0;			///< promote the computability of a type
    virtual AudioType* promoteVectorability(int n)	= 0;			///< promote the vectorability of a type
	virtual AudioType* promoteBoolean(int n)   	= 0;			///< promote the booleanity of a type
	virtual AudioType* promoteInterval(const interval& i) = 0;		///< promote the interval of a type

    virtual AudioType* castNature(int n) = 0;
	virtual AudioType* castInterval(const interval & i) = 0;

	virtual ostream& print(ostream& dst) const		= 0;			///< print nicely a type

    /** promote with the features of rhs */
    inline AudioType * promote(AudioType * rhs)
    {
        return promoteNature(rhs->nature())
            ->promoteVariability(rhs->variability())
            ->promoteComputability(rhs->computability())
            ->promoteVectorability(rhs->vectorability())
            ->promoteBoolean(rhs->boolean())
            ->promoteInterval(rhs->getInterval());
    }

    virtual int dimension() const
    {
        return 0;
    }

    virtual vector<int> dimensions() const
    {
        return vector<int>();
    }


    virtual AudioType * getScalarBaseType(void) = 0;
};

//printing
inline ostream& operator << (ostream& s, const AudioType& n) { return n.print(s); }


/**
 * Return the nature of a vector of types.
 */
inline int mergenature(const vector<Type>& v)
{
	int r = 0;
	for (unsigned int i = 0; i < v.size(); i++) r |= v[i]->nature();
	return r;
}



/**
 * Return the variability of a vector of types.
 */
inline int mergevariability(const vector<Type>& v)
{
	int r = 0;
	for (unsigned int i = 0; i < v.size(); i++) r |= v[i]->variability();
	return r;
}



/**
 * Return the computability of a vector of types.
 */
inline int mergecomputability(const vector<Type>& v)
{
	int r = 0;
	for (unsigned int i = 0; i < v.size(); i++) r |= v[i]->computability();
	return r;
}



/**
 * Return the vectorability of a vector of types.
 */
inline int mergevectorability(const vector<Type>& v)
{
	int r = 0;
	for (unsigned int i = 0; i < v.size(); i++) r |= v[i]->vectorability();
	return r;
}



/**
 * Return the booleanity of a vector of types.
 */
inline int mergeboolean(const vector<Type>& v)
{
	int r = 0;
	for (unsigned int i = 0; i < v.size(); i++) r |= v[i]->boolean();
	return r;
}



/**
 * Return the interval of a vector of types.
 */
inline interval mergeinterval(const vector<Type>& v)
{
	if (v.size()==0) {
		return interval();
	} else {
		double lo=0, hi=0;
		for (unsigned int i = 0; i < v.size(); i++) {
			interval r = v[i]->getInterval();
			if (!r.valid) return r;
			if (i==0) {
				lo = r.lo;
				hi = r.hi;
			} else {
				lo = min(lo,r.lo);
				hi = max(hi,r.hi);
			}
		}
		return interval(lo, hi);
	}
}




/**
 * The type of a simple numeric audio signal.
 * Beside a computability and a variability, SimpleTypes have
 * a "nature" indicating if they represent an integer or floating
 * point audio signals.
 */
class SimpleType : public AudioType
{
  public :

	SimpleType(int n, int v, int c, int vec, int b, const interval& i) : AudioType(n,v,c,vec,b,i) {
		//cerr << "new simple type " << i << " -> " << *this << endl;
	}			///< constructs a SimpleType from a nature a variability and a computability

	virtual ostream& print(ostream& dst) const;						///< print a SimpleType

	virtual AudioType* promoteNature(int n)				    { return new SimpleType(n|fNature, fVariability, fComputability, fVectorability, fBoolean, fInterval); }		///< promote the nature of a type
	virtual AudioType* promoteVariability(int v)			{ return new SimpleType(fNature, v|fVariability, fComputability, fVectorability, fBoolean, fInterval); }		///< promote the variability of a type
	virtual AudioType* promoteComputability(int c)			{ return new SimpleType(fNature, fVariability, c|fComputability, fVectorability, fBoolean, fInterval); }		///< promote the computability of a type
	virtual AudioType* promoteVectorability(int vec)		{ return new SimpleType(fNature, fVariability, fComputability, vec|fVectorability, fBoolean, fInterval); }	///< promote the vectorability of a type
	virtual AudioType* promoteBoolean(int b)        		{ return new SimpleType(fNature, fVariability, fComputability, fVectorability, b|fBoolean, fInterval); }		///< promote the booleanity of a type
	virtual AudioType* promoteInterval(const interval& i)	{ return new SimpleType(fNature, fVariability, fComputability, fVectorability, fBoolean, reunion(fInterval, i)); }

    virtual AudioType* castNature(int n)                    { return new SimpleType(n, fVariability, fComputability, fVectorability, fBoolean, fInterval); }
    virtual AudioType* castInterval(const interval& i)      { return new SimpleType(fNature, fVariability, fComputability, fVectorability, fBoolean, i); }

    AudioType * getScalarBaseType(void)                     { return this; }
};

inline Type intCast (Type t)	                        { return t->castNature(kInt); }
inline Type floatCast (Type t)	                        { return t->castNature(kReal); }
inline Type sampCast (Type t)	                        { return t->promoteVariability(kSamp); }
inline Type castInterval (Type t, const interval& i)    { return t->castInterval(i); }

/**
 * The type of a table of audio data.
 * Beside a computability and a variability, TableTypes have
 * a "content" indicating the type of the data stored in the table.
 */
class TableType : public AudioType
{
  protected :
	const Type fContent;											///< type of that data stored in the table

  public :
	TableType(const Type& t) :
		  AudioType(t->nature(), t->variability(), t->computability(), t->vectorability(), t->boolean()),
		  fContent(t) {}		///< construct a TableType with a content of a type t

	TableType(const Type& t, int n, int v, int c, int vec, int b, const interval& i = interval()) :
		  AudioType(t->nature()|n, t->variability()|v, t->computability()|c, t->vectorability()|vec, t->boolean()|b, i),
		  fContent(t) {}		///< construct a TableType with a content of a type t, promoting nature, variability, computability, vectorability and booleanity

	Type content() const				{ return fContent; 	}		///< return the type of data store in the table
	virtual ostream& print(ostream& dst) const;						///< print a TableType

	virtual AudioType* promoteNature(int n)				{ return new TableType(fContent, n|fNature, fVariability, fComputability, fVectorability, fBoolean, fInterval); }	///< promote the nature of a type
	virtual AudioType* promoteVariability(int v)			{ return new TableType(fContent, fNature, v|fVariability, fComputability, fVectorability, fBoolean, fInterval); }	///< promote the variability of a type
	virtual AudioType* promoteComputability(int c)			{ return new TableType(fContent, fNature, fVariability, c|fComputability, fVectorability, fBoolean, fInterval); }	///< promote the computability of a type
	virtual AudioType* promoteVectorability(int vec)		{ return new TableType(fContent, fNature, fVariability, fComputability, vec|fVectorability, fBoolean, fInterval);}///< promote the vectorability of a type
	virtual AudioType* promoteBoolean(int b)        		{ return new TableType(fContent, fNature, fVariability, fComputability, fVectorability, b|fBoolean, fInterval); }	///< promote the booleanity of a type
	virtual AudioType* promoteInterval(const interval& i)	{ return new TableType(fContent, fNature, fVariability, fComputability, fVectorability, fBoolean, reunion(i, fInterval)); }			///< promote the interval of a type

    virtual AudioType* castNature(int n)                    { return new TableType(fContent, n, fVariability, fComputability, fVectorability, fBoolean, fInterval); }
    virtual AudioType* castInterval(const interval& i)      { return new TableType(fContent, fNature, fVariability, fComputability, fVectorability, fBoolean, i); }

    AudioType * getScalarBaseType(void)                     { throw std::logic_error("internal error: vector of tables"); }
};



/**
 * The type of a tuplet of data.
 * Beside a computability and a variability, TupletTypes have
 * a set of components.
 */
class TupletType : public AudioType
{
  protected:
	vector<Type> fComponents;

  public:
	TupletType(const vector<Type>& vt) :
		  AudioType(mergenature(vt),mergevariability(vt),mergecomputability(vt),mergevectorability(vt),mergeboolean(vt), mergeinterval(vt)),
		  fComponents(vt) {}

	TupletType(const vector<Type>& vt, int n, int v, int c) :
		  AudioType(n|mergenature(vt), v|mergevariability(vt), c|mergecomputability(vt),mergevectorability(vt),mergeboolean(vt), interval()),
		  fComponents(vt) {}

  	TupletType(const vector<Type>& vt, int n, int v, int c, int vec) :
		  AudioType(n|mergenature(vt), v|mergevariability(vt), c|mergecomputability(vt), vec|mergevectorability(vt), mergeboolean(vt), interval()),
		  fComponents(vt) {}

	TupletType(const vector<Type>& vt, int n, int v, int c, int vec, int b, const interval& i) :
		  AudioType(n|mergenature(vt), v|mergevariability(vt), c|mergecomputability(vt), vec|mergevectorability(vt), b|mergeboolean(vt), i),
		  fComponents(vt) {}

	int arity()	const						{ return fComponents.size(); }
	Type operator[](unsigned int i) const	{ return fComponents[i]; }
	virtual ostream& print(ostream& dst) const;

	virtual AudioType* promoteNature(int n)				{ return new TupletType(fComponents, n|fNature, fVariability, fComputability, fVectorability, fBoolean, fInterval); }	///< promote the nature of a type
	virtual AudioType* promoteVariability(int v)			{ return new TupletType(fComponents, fNature, v|fVariability, fComputability, fVectorability, fBoolean, fInterval); }	///< promote the variability of a type
	virtual AudioType* promoteComputability(int c)			{ return new TupletType(fComponents, fNature, fVariability, c|fComputability, fVectorability, fBoolean, fInterval); }	///< promote the computability of a type
	virtual AudioType* promoteVectorability(int vec)		{ return new TupletType(fComponents, fNature, fVariability, fComputability, vec|fVectorability, fBoolean, fInterval);}	///< promote the vectorability of a type
	virtual AudioType* promoteBoolean(int b)        		{ return new TupletType(fComponents, fNature, fVariability, fComputability, fVectorability, b|fBoolean, fInterval);  }	///< promote the booleanity of a type
	virtual AudioType* promoteInterval(const interval& i)	{ return new TupletType(fComponents, fNature, fVariability, fComputability, fVectorability, fBoolean, reunion(i, fInterval));  }	///< promote the interval of a type

    virtual AudioType* castNature(int n)                    { return new TupletType(fComponents, n, fVariability, fComputability, fVectorability, fBoolean, fInterval); }
    virtual AudioType* castInterval(const interval& i)      { return new TupletType(fComponents, fNature, fVariability, fComputability, fVectorability, fBoolean, i);  }

    AudioType * getScalarBaseType(void)                     { throw std::logic_error("internal error: vector of tuplets"); }
};


/**
 * The type of a vector of types.
 */
class FaustVectorType : public AudioType
{
  protected:
    const int fSize;
    AudioType * fType;

private:
    FaustVectorType(int size, AudioType * type, int n, int v, int c, int vec, int b, const interval& i) :
          AudioType(n, v, c, vec, b, i), fSize(size), fType(type)
    {}

public:
    FaustVectorType(int size, AudioType * type) :
          AudioType(*type), fSize(size), fType(type)
    {}

    virtual ostream& print(ostream& dst) const;

    virtual AudioType* promoteNature(int n)             { return new FaustVectorType(fSize, fType, nature() | n, variability(), computability(), vectorability(), boolean(), getInterval()); }
    virtual AudioType* promoteVariability(int v)        { return new FaustVectorType(fSize, fType, nature(), variability() | v, computability(), vectorability(), boolean(), getInterval()); }
    virtual AudioType* promoteComputability(int c)      { return new FaustVectorType(fSize, fType, nature(), variability(), computability() | c, vectorability(), boolean(), getInterval()); }
    virtual AudioType* promoteVectorability(int vec)    { return new FaustVectorType(fSize, fType, nature(), variability(), computability(), vectorability() | vec, boolean(), getInterval()); }
    virtual AudioType* promoteBoolean(int b)            { return new FaustVectorType(fSize, fType, nature(), variability(), computability(), vectorability(), boolean() | b, getInterval()); }
    virtual AudioType* promoteInterval(interval const & i)  { return new FaustVectorType(fSize, fType, nature(), variability(), computability(), vectorability(), boolean(), reunion(i, getInterval())); }

    virtual AudioType* castNature(int n)                { return new FaustVectorType(fSize, fType, n, variability(), computability(), vectorability(), boolean(), getInterval()); }
    virtual AudioType* castInterval(interval const & i) { return new FaustVectorType(fSize, fType, nature(), variability(), computability(), vectorability(), boolean(), i); }

    AudioType * dereferenceType(void)   { return fType; }
    int size(void) const                { return fSize; }

    AudioType * getScalarBaseType(void)
    {
        return fType->getScalarBaseType();
    }

    int dimension() const
    {
        return dimensions().size();
    }

    std::vector<int> dimensions() const
    {
        std::vector<int> ret;
        ret.push_back(fSize);
        FaustVectorType * vt = dynamic_cast<FaustVectorType*>(fType);
        if (vt) {
            std::vector<int> base = vt->dimensions();
            ret.insert(ret.end(), base.begin(), base.end());
        }
        return ret;
    }

    bool isParentOf(AudioType * type) {
        if (type == fType)
            return true;
        FaustVectorType * vt = dynamic_cast<FaustVectorType*>(fType);
        if (vt)
            return vt->isParentOf(type);
        else
            return false;
    }


};

//-------------------------------------------------
//-------------------------------------------------
// 				operations sur les types
//-------------------------------------------------
//-------------------------------------------------



//--------------------------------------------------
// liste de types predefinis

extern Type TINT;
extern Type TREAL;

extern Type TKONST;
extern Type TBLOCK;
extern Type TSAMP;

extern Type TCOMP;
extern Type TINIT;
extern Type TEXEC;

extern Type TINPUT;
extern Type TGUI;
extern Type INT_TGUI;
extern Type TREC;


//--------------------------------------------------
// creation de types

Type table	(const Type& t);
Type operator| 	(const Type& t1, const Type& t2);
Type operator* 	(const Type& t1, const Type& t2);


//--------------------------------------------------
// comparaison de types

bool operator==(const Type& t1, const Type& t2);
bool operator<=(const Type& t1, const Type& t2);

inline bool operator!=(const Type& t1, const Type& t2) { return !(t1==t2); }
inline bool operator< (const Type& t1, const Type& t2) { return t1<=t2 && t1!=t2; }
inline bool operator> (const Type& t1, const Type& t2) { return t2<t1; 	}
inline bool operator>=(const Type& t1, const Type& t2) { return t2<=t1; }


//--------------------------------------------------
// predicats-conversion de types

SimpleType* 	    isSimpleType (AudioType* t);
TableType* 		    isTableType  (AudioType* t);
TupletType* 	    isTupletType (AudioType* t);
FaustVectorType *   isVectorType (AudioType* t);


//--------------------------------------------------
// impressions de types

ostream& operator<<(ostream& dst, const SimpleType& t);
ostream& operator<<(ostream& dst, const Type& t);
ostream& operator<<(ostream& dst, const TableType& t);
ostream& operator<<(ostream& dst, const TupletType& t);


//--------------------------------------------------
// verification de type

Type checkInt(Type t); 				///< verifie que t est entier
Type checkKonst(Type t);			///< verifie que t est constant
Type checkInit(Type t);				///< verifie que t est connu a l'initialisation

Type checkIntParam(Type t);			///< verifie que t est connu a l'initialisation, constant et entier

Type checkWRTbl(Type tbl, Type wr);	///< verifie que wr est compatible avec le contenu de tbl

int checkDelayInterval(Type t);		///< Check if the interval of t is appropriate for a delay


//--------------------------------------------------
// conversion de type

string cType (Type t);
Typed::VarType ctType (Type t);

#endif
