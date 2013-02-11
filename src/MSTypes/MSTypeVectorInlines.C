#ifndef MSTypeVectorINLINES
#define MSTypeVectorINLINES

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


template <class Type>
INLINELINKAGE MSString MSTypeVector<Type>::name()
{
  Type aType;
//  return MSString("MSTypeVector<") + aType.className() + ">";
    return MSString("MSTypeVector<") + ::className(aType) + ">";
}


template <class Type>
INLINELINKAGE MSString className (const Type & t)
{
  return t.className();
}

#endif  // MSTypeVectorINLINES
