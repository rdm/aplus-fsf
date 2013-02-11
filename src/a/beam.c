/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1990-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>

#include <dap/balloc.h>

#include <a/development.h>

#include <a/f.h>
#include <a/fncdcls.h>
#include <a/ik.h>
#include <a/arthur.h>
#include <a/fir.h>

extern I dbg_tb, dbg_tnan;

extern C *findMapped32FileName();
extern C *findMapped64FileName();
extern C *findFileName();

#if defined(__alpha) || defined(__ia64)
char beamFileCPrefix[8]={'A','p','l','u','s','L','6','4'};
int *beamFilePrefix=(int *)beamFileCPrefix;
#elif (_MIPS_SZLONG == 64)
char beamFileCPrefix[8]={'A','p','l','u','s','B','6','4'};
int *beamFilePrefix=(int *)beamFileCPrefix;
#elif defined(__VISUAL_C_2_0__) || defined(__i386)
char beamFileCPrefix[8]={'A','p','l','u','s','l','3','2'};
int *beamFilePrefix=(int *)beamFileCPrefix;
#else
int  *beamFilePrefix=0;
#endif



#define ENDIAN_UNDEF  0
#define ENDIAN_LITTLE 1
#define ENDIAN_BIG    2

Z I mapOut(C *s,A z) ;

int isWrongEndian(A aobj)
{
  return (aobj->r)?(!(1<=aobj->r&&MAXR>=aobj->r)):1!=aobj->n;
}

typedef union {char *c;unsigned char *uc;u_long *u;I *i;F *f;} EndianUnion;

static EndianUnion EndianTag={"abcd"};

static C *endianString(I endian)
{
  return (endian==ENDIAN_BIG)?"big":(endian==ENDIAN_LITTLE)?"little":
    (endian==ENDIAN_UNDEF)?"undef":"unknown";
}

static int hostEndian(void)
{
  return (0x61626364==*EndianTag.u)?ENDIAN_BIG:
    (0x64636261==*EndianTag.u)?ENDIAN_LITTLE:
    ENDIAN_UNDEF;
}

/* non-overlapping endian-swapping copy!! - Method 1 - one loop */
static void ndnicopy(I *from,I *to,I nints)
{
  unsigned char *cfrom=(unsigned char *)from, *cto=(unsigned char *)to;
  for(;nints--;cfrom+=4,cto+=4)
  {
    cto[3]=cfrom[0];cto[2]=cfrom[1];cto[1]=cfrom[2];cto[0]=cfrom[3];
  }
}

static void ndnfcopy(F *from,F *to,I nfs)
{
  unsigned char *cfrom=(unsigned char *)from, *cto=(unsigned char *)to;
  for(;nfs--;cfrom+=8,cto+=8)
  {
    cto[7]=cfrom[0];cto[6]=cfrom[1];cto[5]=cfrom[2];cto[4]=cfrom[3];
    cto[3]=cfrom[4];cto[2]=cfrom[5];cto[1]=cfrom[6];cto[0]=cfrom[7];
  }
}


/* in-place endian swap */
static void ndniswap(I *start,I nints)
{
  I tempi;
  EndianUnion tu, eu;

  tu.i=&tempi;
  for(eu.i=start;nints--;++eu.i)
  {
    tempi=*eu.i;
    eu.uc[3]=tu.uc[0];
    eu.uc[2]=tu.uc[1];
    eu.uc[1]=tu.uc[2];
    eu.uc[0]=tu.uc[3];
  }
}

static void ndnfswap(F *start,I nfs)
{
  F tempf;
  EndianUnion tu, eu;

  tu.f=&tempf;
  for(eu.f=start;nfs--;++eu.f)
  {
    tempf=*eu.f;
    eu.uc[7]=tu.uc[0];
    eu.uc[6]=tu.uc[1];
    eu.uc[5]=tu.uc[2];
    eu.uc[4]=tu.uc[3];
    eu.uc[3]=tu.uc[4];
    eu.uc[2]=tu.uc[5];
    eu.uc[1]=tu.uc[6];
    eu.uc[0]=tu.uc[7];
  }
}

static int aobjEndian(A aobj)
{
  int result=ENDIAN_UNDEF;
  I tempint;
  int hostndn=hostEndian();
  if(!isWrongEndian(aobj))result=hostndn;
  else 
  {
    ndnicopy(&aobj->r,&tempint,1);
    if(0<=tempint&&9>=tempint)
      result=(ENDIAN_BIG==hostndn)?ENDIAN_LITTLE:ENDIAN_BIG;
  }
  return(result);
}


static void doSwapEndianInPlace(A aobj, I targetndn)  
{
  int n,t;
  I sourcendn=aobjEndian(aobj);
  I hostndn=hostEndian();
  if(targetndn==sourcendn)return;
  if(targetndn==hostndn)
  {
    ndniswap((I *)aobj,AH/sizeof(I));
    n=aobj->n; t=aobj->t;
  } else {
    n=aobj->n; t=aobj->t;
    ndniswap((I *)aobj,AH/sizeof(I));
  }
  switch(t)
  {
  case It:
    ndniswap(aobj->p,n);
    break;
  case Ft:
    ndnfswap((F *)aobj->p,n);
    break;
  case Ct:
    break;
  default:
    printf("\343 error!!: doSwapEndianInPlace encountered bad type:%d\n",t);
    break;
  }
}

static A doCopySwitchEndian(A aobj,I targetndn)
{
  static struct a tempAobj;
  A z;
  I n,t;
  I sourcendn=aobjEndian(aobj);
  I hostndn=hostEndian();
  if(targetndn==sourcendn)return (A)ic(aobj);
  if(targetndn==hostndn)
  {
    ndnicopy((I *)aobj,(I *)&tempAobj,AH/sizeof(I));
    z=gd(tempAobj.t,&tempAobj);
    n=z->n; t=z->t;
  } else {
    n=aobj->n; t=aobj->t;
    z=gd(aobj->t,aobj);
    ndnicopy((I *)aobj,(I *)z,AH/sizeof(I));
  }
  switch(t)
  {
  case It:
    ndnicopy(aobj->p,z->p,n);
    break;
  case Ft:
    ndnfcopy((F *)aobj->p,(F *)z->p,n);
    break;
  case Ct:
    bcopy(aobj->p,z->p,n+1);
    break;
  default:
    printf("\343 error!!: doCopySwitchEndian encountered bad type:%ld\n",t);
    break;
  }
  return z;
}

A ep_hostEndian(void)
{
  return gsym(endianString(hostEndian()));
}

A ep_aobjEndian(A aobj)
{
  return gsym(endianString(aobjEndian(aobj)));
}

A ep_CopyRightEndian(A aobj)
{
  return doCopySwitchEndian(aobj,hostEndian());
}

void ep_SwapRightEndian(A aobj)  /* must use argument type 12, not 0!!! */
{
  if(!isWrongEndian(aobj))return;
  doSwapEndianInPlace(aobj,hostEndian());
}

A ep_CopyToEndian(A aobj,A andn)
{
  I targetndn;
  if(Et!=andn->t||1!=andn->n||!QS(*andn->p))ERROUT(ERR_TYPE);
  targetndn=(*andn->p==MS(si("big"))) ? ENDIAN_BIG :
    (*andn->p==MS(si("little")))? ENDIAN_LITTLE : ENDIAN_UNDEF;
  if(ENDIAN_UNDEF==targetndn)ERROUT(ERR_DOMAIN);
  return doCopySwitchEndian(aobj, targetndn);
}

A ep_SwapToEndian(A aobj,A andn) /* must use argument type 12, not 0!!! */
{
  I targetndn, sourcendn;
  if(Et!=andn->t||1!=andn->n||!QS(*andn->p))ERROUT(ERR_TYPE);
  targetndn=(*andn->p==MS(si("big"))) ? ENDIAN_BIG :
    (*andn->p==MS(si("little")))? ENDIAN_LITTLE : ENDIAN_UNDEF;
  if(ENDIAN_UNDEF==targetndn)ERROUT(ERR_DOMAIN);
  sourcendn=aobjEndian(aobj);
  if(targetndn!=sourcendn) doSwapEndianInPlace(aobj, targetndn);
  return aplus_nl;
}

Z int loopen(C *path,int flags,mode_t mode)
{
  I i=0,st=1,fd=-1;
  static C fcn[]="open";
  if(!path) R -1;
  while(i<10&&-1==(fd=open(path,flags,mode))&&EWOULDBLOCK==errno)
    {sleep(st);if(8>st)st*=2;++i;}
  if (i) errno=log_EWouldBlock(i,fd,errno,path,fcn);
  R fd;
}

struct a32 {
    int c,t,r,n,d[MAXR],i,p[1];
};

#define AH32 (sizeof(struct a32) - sizeof(int))

C *tmv32(int t,I *d,int *s,int n) {
  int i;

  switch(t) {
    case It:
    for (i = 0; i < n; i++) {
          *d++ = *s++;
        }
        R(C*)d;

    case Et:
        fprintf(stderr, "\343 Internal Error:beam.c/tmv32 - can't do enclosed types\n");
        R 0;

    case Ft: {
              F *a=(F*)d;
              F *b=(F*)s;
              DO(n,*a++=*b++)
              R(C*)a;
             }
    case Ct: {
              C *a=(C*)d;
              C *b=(C*)s;
              DO(n,*a++=*b++)
              R(C*)a;
             }
  }
  R 0;
}

void
ttmv32(int t,I *d,int *s,int n) {
  C *x=tmv32(t,d,s,n);

  if (x == 0) {
    fprintf(stderr, "\343 Internal Error:beam.c/ttmv32 tmv32 failed\n");
    R;
  }

  if(Ct==t)
    *x='\0';
}

void mv32(I *dest64,int *src32,int n) {
  int i;

  for (i = 0; i < n; i++) {
    *dest64++ = *src32++;
  }
}

A
gc32(int t,int r,int n,int d[],int *p) {
  I f ;
  A z ;

  /*printf("In gc32 (%d, %d, %d ...)\n", t, r, n) ; */
  f = (t==Ct) ;
  z = (A)mab(f+AH+Tt(t,n));

  z->c=1;
  z->t=t;
  z->r=r;
  z->n=n;

  mv32(z->d,d,r);
  ttmv32(t,z->p,p,n);
  if(f)
    ((C*)z->p)[n]=0;
  R z;
}

A
cvt64(struct a32 *aobj) {   /* mapped objects only - floats, chars and longs */
    A newa;
	int i = aobj->i ;
    /*printf("%d %d %d %d %d\n", aobj->c, aobj->t, aobj->r, aobj->n, aobj->i) ; */
    newa = gc32(aobj->t,aobj->r,aobj->n,aobj->d,aobj->p);
	newa->i = i ;
    R newa;
}

#if defined(__alpha) || defined(__i386) || defined(__ia64)
Z A vetteMappedFile(I fsize,I aarg,int mode,C *name)
{
  A aobj=(A)aarg,z;

/*   return  0, no conversion,         z is set to aobj */
/*   return  1, conversion successful, z is new object in memory */
/*   return -1, conversion failed,     z is set to NULL */

  if( 1==cvtIfNeeded(aobj, &z, fsize) )	/* Conversion Successful */
    1; /* need to unmap aobj */
  
  return z;

}
#else
Z A vetteMappedFile(I fsize,I aarg,int mode,C *name)
{
  A aobj=(A)aarg,z=(A)0;
  static struct a tempheader;
  /*printf("In vetteMappedFile(%ld, %ld, %d, %s)\n", 
		fsize, aarg, mode, name) ; */
  if (aarg!=-1 && 0==aobj->c)
  {
    if(isWrongEndian(aobj))
    {
      if(ENDIAN_UNDEF!=aobjEndian(aobj))
      {
	ndnicopy((I *)aobj,(I *)&tempheader,AH/sizeof(I));
	if(Ct>=tempheader.t&&fsize>=AH+tempheader.n)
	{
	  if(BEAM_RO==mode)
	  {
	    H("%s[warn]: Beam: File %s is wrong endian.  Making local copy\n",
	      CC,name);
	    z=ep_CopyRightEndian(aobj);
	    dc(aobj);
	  }
	  else
	  {
	    H("%s[warn]: Beam: File %s is wrong endian.  Aborting map.\n",
	      CC,name);
	    z=0;
	  }
	}
      }
    }
    else if(Ct>=aobj->t&&fsize>=AH+Tt(aobj->t,aobj->n))z=aobj;
  }
  if((I)aobj == -1)
	{
	printf("\343 Internal error vetteMappedFile(): This is being doubly converted. Bad.\n") ;
	return (A)0 ;
	}
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  if(!z)
	{
	I m, t, items ;
	int ret, fd;
	char * dotPos ;

	printf("\343 Converting 32 bit file.\n") ; 
    z=cvt64((struct a32 *)aobj);
	t = z->t ;
  	 /* printf("z->c: %ld z->t: %ld z->r: %ld z->n: %ld z->d[0]: %ld z->d[1]: %ld z->i: %ld\n",
			z->c, t, z->r, z->n, z->d[0], z->d[1], z->i) ; 
	*/
    if (z==NULL)
	  {
	  H("%s[warn]: Beam error: not an `a object\n",CC);
      z = NULL;
	  }
	/* ok it's converted. now we can unlink */
	/* printf("Creating 64 bit mapped file (.m64)\n") ; */
	dotPos = strrchr(name, '.') ;
	if (dotPos)
		*dotPos = 0 ;
	name=findFileName(name,"m64");

	/* printf("Attempting to write the file out to %s\n", name) ;  */
	fd=loopen(name,O_CREAT|O_WRONLY,0666) ;
	if (!fd)
		{
		printf("\343 Error in loopen(%s)\n", name) ;
		return (A)0 ;
		}
	/* we hacked the i correctly already so ignore d[0] */
	z->c = 0 ;
	items = z->i ;
	if (z->n > z->i)
		items = z->n ;
	ret=writeAobjToFile(fd,(C *)z,AH+Tt(t,items)+(Ct==t?1:0),1);
	if (ret < 0)
		{
		printf("\343 Error in writeAobjToFile: %d\n", ret) ;
		return (A)0 ;
		}
	m=z->i*tr(z->r-1,z->d+1);
	if(z->i<*z->d)
	  {
	  *z->d=z->i ;
	  z->n=m;
	  }
	/* printf("m = %ld\n", m) ; */
	/* printf("Setting file len to %ld\n", AH+Tt(t,m)+(t==Ct)) ; */
	/* if(-1==flen(fd,AH+Tt(t,m)+(t==Ct)))
		{
		perror(name) ;
		close(fd);
		q=9 ;
		return (A)0;
		}
	*/
/*	if(-1==lseek(fd,0,0))  R perror(name),close(fd),q=9,(A)0;
	if(-1==write(fd,(C *)z,AH))R perror(name),close(fd),q=9,(A)0; */

  	 /* printf("z->c: %ld z->t: %ld z->r: %ld z->n: %ld z->d[0]: %ld z->d[1]: %ld z->i: %ld\n",
			z->c, t, z->r, z->n, z->d[0], z->d[1], z->i) ; */

	printf("\343 Wrote %s.\n", name) ;
	close(fd) ;
	dc(z) ; /* This should free */
	return (A)-1 ;
	}
#else
	if (!z)
		H("%s[warn]: Beam error: not an `a object\n",CC);
#endif
  R z;
}
#endif

typedef struct{I a,c,n,w;C *s,*t;}MFInfo;  
  /* n field used for "next" in freelist */

Z I maxMFAlimit=2000;
Z I maxMFAindex=0;
Z I freeMFAindex=0;
Z MFInfo *MappedFileArray=0;
Z C MFAErrorMsg[128];

Z int MFArealloc(I newlim)
{
  Z MFInfo *newMFA;
  int i, count=0;
  if(0==newlim||newlim<maxMFAindex)
  {
    strcpy(MFAErrorMsg,"New limit is less than Mapped File Array length");
    R -1;
  }
  newMFA=(MFInfo *)ma(newlim*(sizeof(MFInfo)/sizeof(I)));
  if(0==newMFA)
  {
    strcpy(MFAErrorMsg,"Error allocating new Mapped File Array");
    R -1;
  }
  if(MappedFileArray) 
  {
    for(i=0;i<maxMFAindex;i++)if(MappedFileArray[i].a)
      tmv(It,(I*)(newMFA+count++),(I*)(MappedFileArray+i),
	  sizeof(MFInfo)/sizeof(I));
  }    
  if (newlim>count)
  {
    bzero(newMFA+count,sizeof(MFInfo)*(newlim-count));
    for(i=count;i<newlim;i++)newMFA[i].n=i+1;
  }
  if(0!=MappedFileArray)mf((I*)MappedFileArray);
  MappedFileArray=newMFA;
  maxMFAlimit=newlim;
  freeMFAindex=maxMFAindex=count;
  R 0;
}

void MFALimitSysCmd(I newlim)
{
  int rc;
  if(0>newlim)
  {
    H("%ld\n",maxMFAlimit);
    R;
  }
  if(0==MappedFileArray){maxMFAlimit=newlim;R;}
  rc=MFArealloc(newlim);
  if(rc) H("\343  maplim error: %s\n",MFAErrorMsg);
}

I MFALimitGet(void){R maxMFAlimit;}

void MFALimitSet(I newlim)
{
  int rc;
  rc=MFArealloc(newlim);
  if(rc) {H("\343  maplim error: %s\n",MFAErrorMsg);qs="maplim";q=-1;R;}
  R;
}

Z MFInfo *findMFInfoStruct(A aobj)
{
  if(aplus_nl==aobj||0==MappedFileArray)R 0;
  DO(maxMFAindex,if(MappedFileArray[i].a==(I)aobj)R MappedFileArray+i);
  R 0;
}

Z MFInfo *getFreeMFInfoStruct(void)
{
  MFInfo *p;
  if (0==MappedFileArray&&0!=maxMFAlimit) MFArealloc(maxMFAlimit);
  if(freeMFAindex>=maxMFAlimit)R 0;
  p=MappedFileArray+freeMFAindex;
  if(freeMFAindex>=maxMFAindex)maxMFAindex=freeMFAindex+1;
  freeMFAindex=p->n;
  R p;
}

Z void unmapDotMFile(A a,MFInfo *p)
{
  /* printf("In unmapDotMFile(%s, %lx, %ld)\n", p->t, a, p->n) ; */
  if(dbg_tb) beamtrc(p->t,2,0);
  if(munmap((caddr_t)a,(int)p->n)) 
  {
    I nern=errno;
    syslog(LOG_INFO,"A+ munmap() failed for %s with args (%#lx,%ld) with %m",
	   p->t,a,p->n);
    H("\343 A+ munmap() failed for %s with args (%#lx,%ld) and errno=%ld\n",
      p->t,a,p->n,nern);
  }
  p->a=0;
  bfree(p->s);
  bfree(p->t);
  p->s=p->t=(C *)0;
  p->n=freeMFAindex;
  freeMFAindex=(p-MappedFileArray);
  if(p==MappedFileArray+(maxMFAindex-1))--maxMFAindex;
}

extern I map(int,int);
Z I mapDotMFile(int fd,int mode,C *fname,C *t)
{
  A aobj;
  I mfile;
  MFInfo *p;
  I n;
  (void) setStickyBit(fd,0);
  n=lseek(fd,0,SEEK_END);
  mfile=map(fd,mode);		/* map returns a 0 on failure    */
  Q(!mfile,9);			/* Set domain error if map fails */
  /* printf("mfile = %ld, n = %ld\n", mfile, n) ; */
  aobj=vetteMappedFile(n,mfile,mode,fname);
  Q(!aobj,9);
  p=getFreeMFInfoStruct();
  if(!p)
  {
    R H("maplim\n"),dc(aobj),q=9,0;
  }
    p->c=1,p->n=n,p->w=mode,p->s=bstring(fname),p->t=bstring(t),p->a=mfile;
    if(aobj!=(A)mfile) unmapDotMFile((A)mfile,p);
  R (I)aobj;
}

I isWritableFile(I a){MFInfo *p=findMFInfoStruct((A)a);R p?p->w:0;}
I im(I a){MFInfo *p=findMFInfoStruct((A)a);if(p)++p->c;R a;}
void dm(A a)
{
  MFInfo *p=findMFInfoStruct(a);
  if(p){
    if(!--p->c){
      unmapDotMFile(a,p);
    } 
  } else if(0<a->c) if (!--a->c)dec(a);
}

I mf_length(A aobj){MFInfo *p=findMFInfoStruct(aobj);R p?p->n:0;}

I mf_info(A aobj,I *pw,C **pt)
{
  MFInfo *p=findMFInfoStruct(aobj);
  if(p){*pw=p->w;*pt=p->t;R 0;}
  R 1;
}

Z void dbg_mfapp(MFInfo *p)
{H(" [%s]  refcnt:%ld\n",p->t,p->c);}

void dbg_mfa(void)
{
  int i;
  H("\343 maplim:%ld  maxIndex:%ld  freeIndex:%ld\n",maxMFAlimit,maxMFAindex,
    freeMFAindex);
  if(0==MappedFileArray){H("\343 Mapped File Array not initialized.\n");R;}
  DO(maxMFAindex,H("\343  %ld: ",i);
     if(MappedFileArray[i].a)dbg_mfapp(MappedFileArray+i);
     else H("<free>  next:%ld\n",MappedFileArray[i].n));
  H("\343  --  -----------------\n");
  for(i=maxMFAindex;i<maxMFAindex+10;i++)
  {
    if(i>=maxMFAlimit)break;
    H("\343  %d: ",i);
    if(MappedFileArray[i].a)dbg_mfapp(MappedFileArray+i);
    else H("<free>  next:%ld\n",MappedFileArray[i].n);
  }
}

Z void dbg_mfrpp(MFInfo *p)
{H("\343 %ld\340\"%s\": [%s]  addr:%lu  refcnt:%ld  bytes:%ld\n",
		      p->w,p->s,p->t,p->a,p->c,p->n);}
void dbg_mfr(void)
{
  if(0==MappedFileArray)R;
  DO(maxMFAindex,if(MappedFileArray[i].a)dbg_mfrpp(MappedFileArray+i));
}

A dbg_mfrsf(void)
{
  int n=0;
  A z,modes,args,fnames,addrs,refcnts,bytes;
  MFInfo *p;
  z=gv(Et,2);
  z->p[0]=(I)gvi(Et,6,MS(si("mode")),MS(si("arg")),MS(si("fname")),
		 MS(si("addr")),MS(si("refcnt")),MS(si("bytes")));
  if(MappedFileArray){DO(maxMFAindex,if(MappedFileArray[i].a)++n);}
  modes=gv(It,n);
  args=gv(Et,n);
  fnames=gv(Et,n);
  addrs=gv(It,n);
  refcnts=gv(It,n);
  bytes=gv(It,n);
  n=0;
  if(MappedFileArray)
  {
    DO(maxMFAindex,
       if(MappedFileArray[i].a){p=MappedFileArray+i;
				modes->p[n]=p->w;
				args->p[n]=(I)gsv(0,p->s);
				fnames->p[n]=(I)gsv(0,p->t);
				addrs->p[n]=p->a;
				refcnts->p[n]=p->c;
				bytes->p[n]=p->n;
				n++;});
  }
  z->p[1]=(I)gvi(Et,6,modes,args,fnames,addrs,refcnts,bytes);
  R z;
} 

I mapIn(C *name,I mode){
  I fd=-1,z;
  I tmode = mode ;
  C*t=0;
  /* printf("In mapIn(%s, %d)\n", name, mode) ;*/
  Q(!name||BEAM_RO>mode||BEAM_LOCAL<mode,9);
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  /* First look for a 64 bit file */
  t = findMapped64FileName(name, (BEAM_RW==tmode)) ;
  if (!t)
	{
	/* We are only reading */
	tmode = BEAM_RW ;
	t = findMapped32FileName(name, (BEAM_RW==tmode)) ;
	}
#else
  t = findMapped32FileName(name, (BEAM_RW==tmode)) ;
#endif
  Q(!t,9);
  if(dbg_tb)beamtrc(t,1,tmode);
  ERR(t,fd=loopen(t,BEAM_RW==tmode?O_RDWR:O_RDONLY,0666));
  z=mapDotMFile(fd, tmode, name, t) ;
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  switch(z)
	{
	case 0:
	  R 0 ;
	case -1:
	  /* printf("Converted 32 bit file, mapping file again\n") ; */
	  close(fd) ;
	  t = findMapped64FileName(name, (BEAM_RW==mode)) ;
	  /* printf("t = %s\n", t) ; */
	  ERR(t,fd=loopen(t,BEAM_RW==mode?O_RDWR:O_RDONLY,0666));
	  /* printf("Calling mapDotMFile(%d, %d, %s, %s)\n",
			fd, mode, name, t) ; */
	  if(0==(z=mapDotMFile(fd,mode,name,t)))R 0;
	  break ;
	}
#endif
  if(dbg_tnan)nanbeamchk(t,(A)z);
  /* printf("Returning z = %ld\n", (I)z) ; */
  R z;
}

Z writeAobjToFile(int fd,C *s,unsigned n,I c)
{
  I t;
  A a=(A) s; 
  /* printf("in writeAobj - n = %u\n", n) ; */
  do t=write(fd,s,n); while(s+=t,t!=-1&&(n-=t));
  if((!c) && t!=-1 && a->i!=a->d[0]) 
  { 
    /* if beaming out a mapped variable, writeover a->i with a->d[0]  */
    t=lseek(fd,(AH-sizeof(I)),SEEK_SET);
    if(t!=-1) t=write(fd,(C *)a->d,4);
  }
  fsync(fd);R t;
} /* IBM write fix */

Z I mapOut(C *s,A z)
{
  I fd,rc,c=0;
  C r[MAXPATHLEN];
  C *dotPos, *slashPos;
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  static C defaultSuffix[]={"m64"} ;
#else
  static C defaultSuffix[]={"m"};
#endif
  Q(Ct<z->t,6);

/* On solaris this comparison was failing when '.' was zero and the address */
/* returned from strrchr was a negative signed long */
/* This was caused by missing #include string.h */

/*   s=strrchr((DEV_STRARG)s,'.')>strrchr((DEV_STRARG)s,'/') ?  */
/*     s : findFileName(s,"m"); */

  dotPos=strrchr((DEV_STRARG)s,'.');
  if( dotPos==NULL )
    {
      s=findFileName(s,defaultSuffix);
    }
  else
    {
      slashPos=strrchr((DEV_STRARG)s,'/');
      if(slashPos!=NULL && dotPos<slashPos)
	s=findFileName(s,defaultSuffix);
    }

  if(dbg_tb)beamtrc(s,0,0);
  strcpy(r,(DEV_STRARG)s),strcat(r,(DEV_STRARG)"!@#");
  ERR(s,fd=loopen(r,O_CREAT|O_WRONLY,0666));
  if(z->c)c=z->c,z->c=0,z->i=z->r?*z->d:1;
  rc=writeAobjToFile(fd,(C *)z,AH+Tt(z->t,z->n)+(Ct==z->t?1:0),c);
  if(c)z->c=c;
  close(fd);
  ERR(s,rc);
  ERR(s,rename(r,s));
  R 1;
}

C *stringFromAobj(A a){R a->t==Ct?(C*)a->p:a->t==Et&&QS(*a->p)?XS(*a->p)->n:0;}

H1(monadicBeam){ND1 R mapIn(stringFromAobj(a),0);}
H2(dyadicBeam){
  C *s;
  ND2;
  s=stringFromAobj(a);
  R !s?mapIn(stringFromAobj(w),*a->p):mapOut(s,w)?(I)aplus_nl:0;
}

Z I setSizeOfFile(int fd,off_t n)
{
#ifdef __VISUAL_C_2_0__
  I j=CLBYTES,k=lseek(fd,0,SEEK_END);
#else
  I j=getpagesize(),k=lseek(fd,0,SEEK_END);
#endif
  C junk[4];
  /* printf("In setSizeOfFile; n = %ld, j = %ld, k = %ld\n",
		n, j, k) ;*/
  junk[0]='\0';
  if(-1==k)
    R k;
  n=((n+j-1)/j)*j;
  for(;n<k;n+=j)
  {
    if(-1==lseek(fd,n,SEEK_SET))
      R -1;
    if(-1==write(fd,junk,1))
      R -1;
  }
  R 0;
}


Z I items(I n,A z)
{
  C *s;Z struct a a;I fd,t,m,j,w=n!=-1;
  C *name ;
  Q(-2>n,9);
  NDC1(z);s=stringFromAobj(z);Q(!s,9);
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  name = findMapped64FileName(s, w) ;
  if (!name)
	name = findMapped32FileName(s, w) ;
#else
  name = findMapped32FileName(s, w) ;
#endif
  if(-1==(fd=loopen(name,w?O_RDWR:O_RDONLY,0666))) 
    return q=9,0;
  if(-1==read(fd,(C *)&a,AH)) 
    return perror(s),close(fd),q=9,0;
#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
  {
    I ret, totsize,rank,itemCount,items;
    struct stat statbuf;
    ret = fstat(fd, &statbuf) ;
    if (ret)
    {
      close(fd);
      perror("items (fstat):") ;
      return 0;
    }
    
    ret=getItems(&a, &itemCount, &rank, &items, statbuf.st_size);
    
    /* domain error if conversion error or */
    /* conversion and trying to set        */
    if( ret==-1 || (ret && n!=-1) )
      {
	 close(fd);
	 q=9;
	 return 0;
      }
    else if( ret==1 )		/* Conversion was required     */
      {
	close(fd);
	Q( !rank, 7);		/* Must be not be a scalar */
	return itemCount>items?itemCount:items;
      }
  }
#endif		
  if(!a.r) R close(fd),q=7,0;
  m=*a.d;j=a.i;if(m>j)j=m;
  if(w){
    t=a.t;
    if(n==-2) 
    {
      if(-1==setSizeOfFile(fd,AH+Tt(t,a.n)+(t==Ct)))
	R perror(s),close(fd),q=9,0;
    }
    else
    {
      a.i=n,m=n*tr(a.r-1,a.d+1);if(n<*a.d)*a.d=n,a.n=m;
      if(-1==flen(fd,AH+Tt(t,m)+(t==Ct))) R perror(s),close(fd),q=9,0;
      if(-1==lseek(fd,0,0))  R perror(s),close(fd),q=9,0;
      if(-1==write(fd,(C *)&a,AH))R perror(s),close(fd),q=9,0;
    }
  }
  R close(fd),j;
}

void beamInstall(void){
  install((PFI)items,        "_items",     9,2,9,0,0,0,0,0,0,0);
  install((PFI)ep_hostEndian,"_hostendian",0,0,0,0,0,0,0,0,0,0);
  install((PFI)ep_aobjEndian,"_endian",    0,1,0,0,0,0,0,0,0,0);
  R;
}

