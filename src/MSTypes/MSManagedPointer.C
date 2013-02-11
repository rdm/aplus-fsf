#ifndef MSManagedPointerIMPLEMENTATION
#define MSManagedPointerIMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <iostream.h>

template<class Type>
ostream& operator<<(ostream& aStream_,const MSManagedPointer<Type>& aPointer_)
{ 
  aStream_<<(void *)aPointer_._node->_pObject<<"  ";
  return aStream_<<"Ref Count: "<<aPointer_._node->_refCount<<flush;
}

#endif
