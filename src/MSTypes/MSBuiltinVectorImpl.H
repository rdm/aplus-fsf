#ifndef MSBuiltinVectorImplHEADER
#define MSBuiltinVectorImplHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSTypes/MSVectorImpl.H>

#if (__GNUC__ < 3)
class ostream;
class istrstream;
class ostrstream;
class MSTypesExport MSBuiltinVectorImplOps
{    
public:
  MSBuiltinVectorImplOps();
  virtual ~MSBuiltinVectorImplOps();

  virtual void   setToNumber (void *, unsigned int, double) const =0;
  virtual double getAsNumber (const void *, unsigned int) const =0;

  virtual void readFromStream (void *, unsigned int, istrstream &) const =0;
  virtual void writeToStream  (const void *, unsigned int, ostrstream &) const =0;

  virtual void whitespace (istrstream &) const =0;
  virtual unsigned int stringLen (const char *) const =0;
};
#else
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
class MSTypesExport MSBuiltinVectorImplOps
{    
public:
  MSBuiltinVectorImplOps();
  virtual ~MSBuiltinVectorImplOps();

  virtual void   setToNumber (void *, unsigned int, double) const =0;
  virtual double getAsNumber (const void *, unsigned int) const =0;

  virtual void readFromStream (void *, unsigned int, istringstream &) const =0;
  virtual void writeToStream  (const void *, unsigned int, ostringstream &) const =0;

  virtual void whitespace (istringstream &) const =0;
  virtual unsigned int stringLen (const char *) const =0;
};
#endif



class MSTypesExport MSBuiltinVectorImpl : public MSVectorImpl
{
protected:
  MSBuiltinVectorImplOps *_pBuiltInOps;

public:
  MSBuiltinVectorImpl (MSVectorImplOps *, MSBuiltinVectorImplOps *, unsigned int =0);
  MSBuiltinVectorImpl (MSVectorImplOps *, MSBuiltinVectorImplOps *, unsigned int, void *);
  MSBuiltinVectorImpl (const MSBuiltinVectorImpl &);
  MSBuiltinVectorImpl (MSVectorImplOps *, MSBuiltinVectorImplOps *, void *, unsigned int);
  
  virtual ~MSBuiltinVectorImpl();

  virtual MSVectorImpl *create (unsigned int =0, unsigned int =0) const;
  virtual MSVectorImpl *clone  () const;

  virtual MSString asMSF();

  virtual MSError::ErrorStatus setFromString (const char *, const char =' ');
  virtual MSError::ErrorStatus setFromMSF    (const char *);

  virtual MSIndexVector gradeUp  () const;
  virtual MSIndexVector gradeDown() const;
  double median() const;
  double variance(double, MSEstimateType =MSUnbiased) const;

  MSBuiltinVectorImpl *movingAverage (unsigned int);
};

#endif  // MSBuiltinVectorImplHEADER
