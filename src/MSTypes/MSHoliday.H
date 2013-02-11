#ifndef MSHolidayHEADER
#define MSHolidayHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSTypes/MSString.H>
#include <MSTypes/MSDate.H>

#if (__GNUC__ < 3)
class ostream;
#else
#include <iostream>
#include <fstream>
using namespace std;
#endif


typedef MSString MSResourceCode;

class MSTypesExport MSHoliday : public MSDate 
{
public:
  MSHoliday(void);
  MSHoliday(const MSHoliday&);  
  MSHoliday(const MSDate&,const MSResourceCode&,const MSString& description_="");

  const MSResourceCode& resourceCode(void) const;
  const MSString& description(void) const;  

  MSHoliday& operator=(const MSHoliday&);
  
  friend MSTypesExport ostream& operator<<(ostream&,const MSHoliday&);

private:
   MSResourceCode _resourceCode;
   MSString       _description;
};

// MSHoliday inlines
inline const MSResourceCode& MSHoliday::resourceCode(void) const
{ return _resourceCode; }

inline const MSString& MSHoliday::description(void) const
{ return _description; }

inline MSDate const& key(MSHoliday const& holiday_)
{ return (MSDate const&)holiday_; }

#endif
