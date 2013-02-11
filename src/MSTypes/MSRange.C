///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <MSTypes/MSRange.H>

ostream& operator<<(ostream& aStream_,const MSRange& aRange_)
{ return aStream_<<"min: "<<aRange_.min()<<"\tmax: "<<aRange_.max()<<"\tlength: "<<aRange_.length(); }

