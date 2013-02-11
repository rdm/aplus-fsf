///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved.
// See .../src/LICENSE for terms of distribution.
//
//
///////////////////////////////////////////////////////////////////////////////
//
// pString_Connection
//
// This class is subclassed from pA_Connection.  It contains the
// code for the read and send functions using the `string protocol.
// 
// The `string protocol is to send messages with two parts: the first
// part is a four-byte integer of the length of the second part, which
// is the contents of the message, as a char string.
// 

#if defined(__edgfe) || defined( __sgi) || defined(_AIX) || defined(SOLARIS_CSET)
#include <errno.h>
#endif
#include <pString_Connection.H>

#include <BufferUtilities.H>

static inline void mfbuffer(MSBuffer *bp)
{
  mf((I *)bp->minofbuffer());
  bp->minofbuffer(0);
}

A pString_Connection::getAobjFromBuffer(MSBuffer *bb)
{
  ipcWarn(wrnlvl(),"%t pString_Connection::getAobjFromBuffer\n");
  MSBuffer *hb=headBuffer();
  MSBuffer *db=readBuffer();
  A d,z;
  I s;

  if(4>(s=hb->put()-hb->get()))
  {
    if(0>bufftobuff(bb,hb,4-s)) R(A)0;
    if(4>(s=hb->put()-hb->get())) R(A)0;
    s=longAt(hb->get());
    if(0>=s)
    {
      Warn("\343 IPC warning: zero-length message found.  s=%d [%d]\n",
	s,handle());
      hb->reset();
      turnInReadOff();
      R(A)0;
    }

    // create read buffer, doubled as A-object char array
    d=gv(Ct,s);
    db->minofbuffer((C *)d); db->get(db->minofbuffer()); 
    db->put((C *)(d->p)); db->maxofbuffer(db->put()+s);
  }

  if(0>bufftobuff(bb,db,db->maxofbuffer()-db->put())) R(A)0;
  if(db->put()==db->maxofbuffer())
  {
    z=(A)db->minofbuffer();
    hb->reset();
    db->minofbuffer(0);
    db->clear();
    return z;
  }
  R(A)0;
}

A pString_Connection::readBurst(void)
{
  ipcWarn(wrnlvl(),"%t pString_Connection::readBurst\n");
  MSBuffer bbuff;
  A d,z=(A)0;
  I slen=readFileLength(),slen1,n,s,count;
  if(-1==slen)R(A)0;
  if(0==slen)
  {
    Warn("\343 IPC warning: pA::ReadBurst: read event with no data [%d]\n",
      handle());
  }

  /* create buff to hold it.  Fill buffer */
  slen1=slen?slen:4;
  bbuff.minofbuffer(mab(slen1));
  bbuff.maxofbuffer(bbuff.minofbuffer()+slen1);
  bbuff.reset();
  if(0>(n=readTheBuffer(&bbuff,slen1))) {mfbuffer(&bbuff); R(A)0;}
  if(0==n&&0==slen) {turnInReadOff(); mfbuffer(&bbuff); R(A)0;}

  d=getAobjFromBuffer(&bbuff);
  if((A)0==d){mfbuffer(&bbuff); R(A)0;}

  // determine how many more complete A-objects lie in bbuff
  count=1;
  for(C *cp=bbuff.get();cp<bbuff.put();cp+=s)
  {
    s=longAt(cp);
    cp+=sizeof(long);
    if(s<=bbuff.put()-cp)++count;
  }

  // create result
  z=gv(Et,count);
  for(int i=0;i<count;++i)z->p[i]=(I)aplus_nl;
  int idx=0;
  z->p[idx++]=(I)d;

  // retrieve additional A-objects from bbuff, fill in z
  while(idx<count)
  {
    d=getAobjFromBuffer(&bbuff);
    if((A)0==d)break;
    z->p[idx++]=(I)d;
  }
  if(idx<count)
  {
    ipcWarn(wrnlvl(),"%t burst mode aborted.  Possible data loss.\n");
  }

  // run once more to clear out bbuff and move partial object into connection
  // buffers
  if(bbuff.get()==bbuff.put())turnInReadOff();
  else
  {
    d=getAobjFromBuffer(&bbuff);
    if((A)0!=d || bbuff.get()!=bbuff.put())
    {
      ipcWarn(wrnlvl(),"%t burst buffer not cleared: %d %d %d\n",
	    d,bbuff.get(),bbuff.put());
    }
  }

  // free bbuff;
  mfbuffer(&bbuff);

  return z;     
}

A pString_Connection::readOne(void)
{
  ipcWarn(wrnlvl(),"%t pString_Connection::readOne\n");
  MSBuffer *hb=headBuffer();
  MSBuffer *db=readBuffer();
  int slen;
  A d,z=(A)0;

  if(4>(slen=hb->put()-hb->get()))
  {
    if(0>readTheBuffer(hb,4-slen)) return (A)0;
    if(4>(slen=hb->put()-hb->get())) return (A)0;
    
    slen=longAt(hb->get());
    if(0>=slen)
    {
      Warn("\343 IPC warning: zero-length string message.  slen=%d [%d]\n",
	slen,handle());
      hb->reset();
      turnInReadOff();
      return (A)0;
    }
    d=gv(Ct,slen);
    db->minofbuffer((C *)d); db->get(db->minofbuffer()); 
    db->put((C *)(d->p)); db->maxofbuffer(db->put()+slen);
  }
  if(0>readTheBuffer(db,db->maxofbuffer()-db->put())) return (A)0;
  if(db->put()==db->maxofbuffer())
  {
    z=(A)db->minofbuffer();
    hb->reset();
    db->minofbuffer(0);
    db->clear();
    return z;
  }
  return (A)0;
}

int pString_Connection::send(const A &msg_)
{
  ipcWarn(wrnlvl(),"%t pString_Connection::send\n");
  if(isInReset()||readChannel()==0) return -1;
  if(Ct!=msg_->t) return -1;

  MSBuffer *sb=new MSBuffer(msg_->n+sizeof(I));
  if(NULL==sb) return -1;
  sb->stuff((char *)(&msg_->n),sizeof(I));
  sb->stuff((const char *)msg_->p, msg_->n);
  sendTheBuffer(sb);
  if (MSFalse==isWritePause()) writeChannel()->enable();
  return doWrite(MSFalse);
}
