#ifndef __BufferUtilities_H
#define __BufferUtilities_H

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved.
// See .../src/LICENSE for terms of distribution.
//
//
///////////////////////////////////////////////////////////////////////////////

extern int bufftobuff(MSBuffer *sp,MSBuffer *dp,int n);
extern MSBuffer *createBufferFromAobj(const A &aobj);

#endif
