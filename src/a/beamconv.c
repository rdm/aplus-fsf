/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1990-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* This file contains routines that identify and convert the different
 * formats of mmap'd files used by A+
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <k.h>

/* These need to be set correctly: */

#if (_MIPS_SZLONG == 64) || defined(__alpha) || defined(__ia64)
#define HAS_64BIT_TYPE   1
typedef int		 INT32;
typedef long		 INT64; 
#elif (_MIPS_SZLONG == 32)
#define HAS_64BIT_TYPE   1
typedef int		 INT32;
typedef long long	 INT64;
#else
typedef int		 INT32;
typedef long             INT64;	/* Need to FIX !!! */
#endif

typedef struct
{
  INT32 c, t, r, n, d[9], i, p[1];
} A32;

typedef struct
{
  INT64 c, t, r, n, d[9], i, p[1];
} A64;


#define ENDIAN_UNDEF  0
#define ENDIAN_LITTLE 1
#define ENDIAN_BIG    2

static char *endianString(int endian)
{
  return (endian==ENDIAN_BIG)?"big":(endian==ENDIAN_LITTLE)?"little":
    (endian==ENDIAN_UNDEF)?"undef":"unknown";
}

typedef union {
  char *c;
  unsigned char *uc;
  u_int *u;
  long *i;
  double *f;
} EndianUnion;

static EndianUnion EndianTag={"abcd"};

static INT32 SwapEndianInt32(INT32 in)
{
  INT32 out;
  int i;
  unsigned char *to = (unsigned char *)&out;
  unsigned char *from = (unsigned char *)&in;
  for (i = 0; i < sizeof(INT32); i++)
    to[i] = from[3-i];
  return out;
}

static INT64 SwapEndianInt64(INT64 in)
{
  INT64 out;
  int i;
  unsigned char *to = (unsigned char *)&out;
  unsigned char *from = (unsigned char *)&in;
  for (i = 0; i < sizeof(INT64); i++)
    {
      to[i] = from[7-i];
    }
  return out;
}

static double SwapEndianDouble(double in)
{
  double out;
  int i;
  unsigned char *to = (unsigned char *)&out;
  unsigned char *from = (unsigned char *)&in;
  for (i = 0; i < sizeof(double); i++)
    {
      to[i] = from[(sizeof(double)-1)-i];
    }
  return out;
}

static double SwapEndianDouble32(double in)
{
  double out;
  int i;
  unsigned char *to = (unsigned char *)&out;
  unsigned char *from = (unsigned char *)&in;
  for (i = 0; i < sizeof(INT32); i++)
    {
      to[i] = from[3-i];
    }
  return out;
}

static void SwapEndianHeader64(A64 *header)
{
  int i;
  header->c = SwapEndianInt64(header->c);
  header->t = SwapEndianInt64(header->t);
  header->r = SwapEndianInt64(header->r);
  header->n = SwapEndianInt64(header->n);
  header->i = SwapEndianInt64(header->i);
  for (i = 0; i < 9; i++)
    header->d[i] = SwapEndianInt64(header->d[i]);
}
		
static void SwapEndianHeader32(A32 *header)
{
  int i;
  header->c = SwapEndianInt32(header->c);
  header->t = SwapEndianInt32(header->t);
  header->r = SwapEndianInt32(header->r);
  header->n = SwapEndianInt32(header->n);
  header->i = SwapEndianInt32(header->i);
  for (i = 0; i < 9; i++)
    header->d[i] = SwapEndianInt32(header->d[i]);
}
		
/* tr64 and tr32 -- from k.c/tr  but deals with negative r */

static INT64 tr64(INT64 r, INT64 *d)
{
  INT64 n=1;
  INT64 *t;
	
  if (r>0)
    for (t = d + r , n = *d; ++d < t; n *= *d);
  return n;
}

static INT32 tr32(INT32 r, INT32 *d)
{
  INT32 n=1;
  INT32 *t;
	
  if (r>0)
    for (t = d + r , n = *d; ++d < t; n *= *d);
  return n;
}

/* from k.h */
#define Tt64(t,x) ((x)<<((((t>>1)&1)+3)&3))
#define Tt32(t,x) ((x)<<(t+2&3))

static void GetHostInformation(int *width, int *endian)
{
  /* 1. Determine what endian we are */
  if (0x61626364==*EndianTag.u)
    *endian = ENDIAN_BIG;
  else
    {
      if (0x64636261==*EndianTag.u)
	*endian = ENDIAN_LITTLE;
      else
	*endian = ENDIAN_UNDEF;
    }

  /* 2. Determine what width we are */
  *width = sizeof(long) * 8;
}

	
int GetSrcInformation(void *src, I iBytesRead, 
		      int *width, int *endian,
		      int iHostWidth, int iHostEndian)
{
  long iPredLen;
  long iLen;
  char caHeaderBuf[sizeof(A64)];
  A32 a32BigHeader;
  A64 a64BigHeader;
  A32 a32LittleHeader;
  A64 a64LittleHeader;
  struct stat statbuf;
	

  if( iBytesRead>0 && (iBytesRead<sizeof(A32)-sizeof(INT32)) )
    {
      printf("\343 Error:File too small to be a mapped file\n");
      return -1;
    }

  /* Read in the header - use the worst case (64-bit) */
  memcpy(caHeaderBuf, src, sizeof(A64));

  memcpy((char *)&a32BigHeader,    (char *)caHeaderBuf, sizeof(A32)); 
  memcpy((char *)&a32LittleHeader, (char *)caHeaderBuf, sizeof(A32)); 
  memcpy((char *)&a64BigHeader,    (char *)caHeaderBuf, sizeof(A64)); 
  memcpy((char *)&a64LittleHeader, (char *)caHeaderBuf, sizeof(A64)); 

  if (iHostEndian == ENDIAN_BIG)
    {
      SwapEndianHeader32(&a32LittleHeader);
      SwapEndianHeader64(&a64LittleHeader);
    }
  else
    {
      SwapEndianHeader32(&a32BigHeader);
      SwapEndianHeader64(&a64BigHeader);
    }
		
	
  iLen = iBytesRead;

  /* 3. Look at the file. 
   * Analyze the possibilities:
   * 		32-bit big endian
   *		32-bit little endian
   *		64-bit big endian
   *		64-bit little endian
   */

  /* The format of a 32-bit file is:
   * ccccttttrrrrnnnnd...iiiip...
   * big endian
   * 0000000t000rnnnnd...iiiip...
   * little endian
   * 0000t000r000nnnnd...iiiip...
   *
   * The format of a 64-bit file is:
   * ccccccccttttttttrrrrrrrrnnnnnnnnd...
   * big endian
   * 000000000000000t0000000rnnnnnnnnd...
   * little endian
   * 00000000t0000000r0000000nnnnnnnnd...
   *
   * Format		Type	Rank	Num
   * 32/big:		8		12		13-16
   * 32/little:	5		9		13-16
   * 64/big:		16		24		25-32
   * 64/little:	9		17		25-32
   * The conflict here is between 32 and 64 bit little-endian files;
   * an int-type 32/little may appear to be a 64/little file.
   * Fortunately in this case the num field will distinguish as the
   * 32-bit num field corresponds to 0 in the 64/little file.
   */

  /*
    printf("64 bit big endian:\n");
    printf("Count: %lx\n", a64BigHeader.c);
    printf("Type: %lx\n", a64BigHeader.t);
    printf("Rank: %lx\n", a64BigHeader.r);
    printf("Num: %lx\n", a64BigHeader.n);
    printf("Items: %lx\n", a64BigHeader.i);
    printf("Size: %ld\n", sizeof(A64) - sizeof(INT64) + Tt64(a64BigHeader.t,
    a64BigHeader.i) + (a64BigHeader.t == 2));
    printf("32 bit big endian:\n");
    printf("Count: %lx\n", a32BigHeader.c);
    printf("Type: %lx\n", a32BigHeader.t);
    printf("Rank: %lx\n", a32BigHeader.r);
    printf("Num: %lx\n", a32BigHeader.n);
    printf("Items: %lx\n", a32BigHeader.i);
    printf("Size: %ld\n", 
    sizeof(A32) - sizeof(INT32) + Tt32(a32BigHeader.t, a32BigHeader.i *
    tr32(a32BigHeader.r - 1, a32BigHeader.d + 1)) +
    (a32BigHeader.t == 2));
  */

#define isAObject(AObject) \
      (AObject.t >= 0) && \
      (AObject.t <= 8) && \
      (AObject.r >= 0)  && \
      (AObject.r <= 9) && \
      ((AObject.n > 0) || (AObject.n ==0 && (AObject.r >= 1))) && \
      ((AObject.i > 0) || (AObject.i ==0 && (AObject.r >= 1))) && \
      (AObject.c == 0)

#define checkDims(AObject) \
       { \
	 int dims=1,i; \
	 if( AObject.r==0 && AObject.n!=1 ) \
	   dimsOK=0; \
	 else \
	   { \
	     for(i=0; i<AObject.r; i++) \
	       dims *= AObject.d[i]; \
	     if(dims!=AObject.n) \
	       dimsOK=0; \
	   } \
       }

#define TR32(x) ((x).r ? tr32( ((x).r-1), ((x).d+1)) : 1)
#define TR64(x) ((x).r ? tr64( ((x).r-1), ((x).d+1)) : 1)

#if defined(__i386)
  /* Try 32-bit little endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
  if ( isAObject(a32LittleHeader) )
    {
       int dimsOK=1;
       /* Check dimensions */
       checkDims(a32LittleHeader);

      /* Check file length */
      if (dimsOK && 
	  (sizeof(A32)-sizeof(INT32)+
	   Tt32(a32LittleHeader.t, a32LittleHeader.i *
		TR32(a32LittleHeader)) +
	  (a32LittleHeader.t == 2) <= iLen))
	{
	  *width = 32;
	  *endian = ENDIAN_LITTLE;
	  return 0;
	}
    }

  /* Try 32-bit big endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
     if( isAObject(a32BigHeader) )
     {
       
       int dimsOK=1;
       /* Check dimensions */
       checkDims(a32BigHeader);

      /* Check file length */
      if (dimsOK && 
	  (sizeof(A32)-sizeof(INT32)+
	   Tt32(a32BigHeader.t, a32BigHeader.i *
		TR32(a32BigHeader)) +
	  (a32BigHeader.t == 2) <= iLen))
	{
	  *width = 32;
	  *endian = ENDIAN_BIG;
	  return 0;
	}
    }
		

#else
  /* Try 64-bit big endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
  if ( isAObject(a64BigHeader) )
    {

       int dimsOK=1;
       /* Check dimensions */
       checkDims(a64BigHeader);

      /*
	printf("In 64 bit big endian\n");
	printf("Count: %lx\n", a64BigHeader.c);
	printf("Type: %lx\n", a64BigHeader.t);
	printf("Rank: %lx\n", a64BigHeader.r);
	printf("Num: %lx\n", a64BigHeader.n);
	printf("Items: %lx\n", a64BigHeader.i);
	printf("Size: %ld\n", sizeof(A64) - sizeof(INT64) + Tt64(a64BigHeader.t,
	a64BigHeader.i) + (a64BigHeader.t == 2));
      */
      /* Check file length */
      if (dimsOK && 
	  (sizeof(A64)-sizeof(INT64)+
	   Tt64(a64BigHeader.t, a64BigHeader.i *
		TR64(a64BigHeader)) +
	  (a64BigHeader.t == 2) <= iLen))
	{
	  *width = 64;
	  *endian = ENDIAN_BIG;
	  return 0;
	}
    }
		

  /* Try 64-bit little endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
  if ( isAObject(a64LittleHeader) )
    {
       int dimsOK=1;
       /* Check dimensions */
       checkDims(a64LittleHeader);

      /* Check file length */
      if (dimsOK && 
	  (sizeof(A64)-sizeof(INT64)+
	   Tt64(a64LittleHeader.t,a64LittleHeader.i *
		TR64(a64LittleHeader)) +
	   (a64LittleHeader.t == 2) <= iLen))
	{
	  *width = 64;
	  *endian = ENDIAN_LITTLE;
	  return 0;
	}
    }

  /* Try 32-bit big endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
     if( isAObject(a32BigHeader) )
     {
       
       int dimsOK=1;
       /* Check dimensions */
       checkDims(a32BigHeader);

      /* Check file length */
      if (dimsOK && 
	  (sizeof(A32)-sizeof(INT32)+
	   Tt32(a32BigHeader.t, a32BigHeader.i *
		TR32(a32BigHeader)) +
	  (a32BigHeader.t == 2) <= iLen))
	{
	  *width = 32;
	  *endian = ENDIAN_BIG;
	  return 0;
	}
    }
		

  /* Try 32-bit little endian */
  /* Valid values: 0 <= type <= 8, 0 < rank <= 9, n > 0 */
  if ( isAObject(a32LittleHeader) )
    {
       int dimsOK=1;
       /* Check dimensions */
       checkDims(a32LittleHeader);

      /* Check file length */
      if (dimsOK && 
	  (sizeof(A32)-sizeof(INT32)+
	   Tt32(a32LittleHeader.t, a32LittleHeader.i *
		TR32(a32LittleHeader)) +
	  (a32LittleHeader.t == 2) <= iLen))
	{
	  *width = 32;
	  *endian = ENDIAN_LITTLE;
	  return 0;
	}
    }
#endif		
		
  printf("\343 Error:File does not match any known type.\n");
  return -1;
}

static int Convert32to64(A32 *from, A64 *to)
{
  long items = from->n;
  int i;

  to->c = from->c;
  to->t = from->t;
  to->r = from->r;
  to->n = from->n;
  for (i = 0; i < 9; i++)
    to->d[i] = from->d[i];
  to->i = (from->r) ? from->d[0] : from->n;
  switch(from->t)
    {
    case 0: /* Int */
      for (i = 0; i < items; i++)
	{
	  to->p[i] = from->p[i];
	}
      break;
    case 1: /* Float */
      {
	double *a = (double *)from->p;
	double *b = (double *)to->p;
	for (i = 0; i < items; i++)
	  b[i] = a[i];
	break;
      }
    case 2: /* Char */
      {
	char *a = (char *)from->p;
	char *b = (char *)to->p;
	for (i = 0; i < items; i++)
	  b[i] = a[i];
	b[i] = 0;
	break;
      }
    default: /* Can't handle it */
      printf("\343 Error:Type %d not translatable.\n", from->t);
      return -1;
    }
  return 0;
}

static int ConvertEndian32(A32 *from, A32 *to)
{
  int i;
  long items;
  memcpy(to, from, sizeof(A32) - sizeof(INT32));
  SwapEndianHeader32(to);
  items=to->n;
  switch(to->t)
    {
    case 0: /* Int */
      for (i = 0; i < items; i++)
	to->p[i] = SwapEndianInt32(from->p[i]);
      break; 
    case 1: /* Float */
      {
	double *out = (double *)to->p;
	double *in =  (double *)from->p;
	for (i = 0; i < items; i++)
	  out[i] = SwapEndianDouble(in[i]);
	break;
      }
    case 2: /* Char */
      {
	char *in = (char *)from->p;
	char *out = (char *)to->p;
	for (i = 0; i < items; i++)
	  out[i] = in[i];
	break;
      }
    default: /* Bad type */
      printf("\343 Error:Type %d not translatable.\n", from->t);
      return -1;
    }
  return 0;
}
		

int cvtIfNeeded(void *src, A *dest, I ilen)
{
  /* Converts the src object to the host format if needed   */
  /*  arguments:                                            */
  /*    src  - pointer to the source data                   */
  /*    dest - address of the pointer to the destination    */
  /*    ilen - if src is a mapped file then the file length */
  /*  Returns:                                              */
  /*    0 - no conversion required (dest==NULL)             */
  /*    1 - Conversion successful  (dest==*convertedAobj)   */
  /*   -1 - Conversion failed      (dest==NULL              */

  static int hostWidth = -1;
  static int hostEndian= -1;
  int srcWidth;
  int srcEndian;
  int rc;

  /* Determine host characteristics */
  if(hostWidth==-1)
    GetHostInformation(&hostWidth, &hostEndian);
  
  if(hostEndian == ENDIAN_UNDEF) 
    { 
      printf("\343 Error:Unable to determine host endian\n"); 
      dest=NULL;
      return rc=-1; 
    }

  /* Check and convert source */
  rc=GetSrcInformation(src, ilen, 
		       &srcWidth, &srcEndian, 
		       hostWidth, hostEndian);
  if(rc!=0)
    return -1;

  /* START CONVERSIONS */
  /* Case 1: No Conversions */
  if( srcEndian == hostEndian &&  srcWidth  == hostWidth)
    {
      *dest=src;      /* *dest=src; */
      return 0;
    }

  /* Case 2: Big/32 -> Big/64 */
  if ( srcEndian  == ENDIAN_BIG && hostEndian == ENDIAN_BIG &&
       srcWidth   == 32         && hostWidth  == 64 )
    {
      A32 *from = (A32 *)src;
      long items = from->n;
      long size = (from->t==2) + sizeof(A64) - sizeof(INT64) + Tt64(from->t, items);
      A64 *to = (A64 *)mab(size);

      rc = Convert32to64(from, to);
      if (rc)
	{
	  printf("\343 Error:Convert32to64 failed\n");
	  *dest=NULL;
	  mf(to);
	  return -1;
	}
      else
	{
	  to->i= (to->r) ? to->d[0] : to->n; /* fix items */ 
	  *dest=(A)to;
	  return 1;
	}
    }

  /* Case 3: Big/32 -> Little/32 */
  if ( srcEndian == ENDIAN_BIG && hostEndian == ENDIAN_LITTLE &&
       srcWidth  == 32         && hostWidth  == 32)
    {
      
      A32 *to, *from = (A32 *)src;
      A32 wrk32;
      long items,size32;

      memcpy(&wrk32, src, sizeof(A32) - sizeof(INT32) ); /* copy header */
      SwapEndianHeader32(&wrk32);                /* correct endian */

      items=wrk32.n;		/* Get the number of elements */
      
      size32=(wrk32.t==2) + sizeof(A32) - sizeof(INT32) + Tt32(wrk32.t, items);

      to = (A32 *)mab(size32);

      rc = ConvertEndian32(from, to);
      if (rc)
	{
	  printf("\343 Error:ConvertEndian32 failed\n");
	  *dest=(A)NULL;
	  mf(to);
	  return -1;
	}
	
      else 
	{
	  to->i= (to->r) ? to->d[0] : to->n; /* fix items */ 
	  *dest=(A)to;
	  return 1;
	}
    }

  /* Case 4: Big/32 -> Little/64  or Little/32 -> Big64 */
  if ( srcEndian !=  hostEndian &&
       srcWidth  == 32          && hostWidth  == 64)
    {
      A32 *from = (A32 *)src;
      A32 wrk;
      A32 *toa;
      A64 *tob;
      long items;
      long size32,size64;

      memcpy(&wrk, src, sizeof(A32) - sizeof(INT32) ); /* copy header */
      SwapEndianHeader32(&wrk);                /* correct endian */

      items=wrk.n;		/* Get the number of elements */

      size32=(wrk.t==2) + sizeof(A32) - sizeof(INT32) + Tt32(wrk.t, items);
      size64=(wrk.t==2) + sizeof(A64) - sizeof(INT64) + Tt64(wrk.t, items);

      toa = (A32 *)mab(size32);
      tob = (A64 *)mab(size64);

      rc = ConvertEndian32(from, toa);
      if (rc)
	{
	  printf("\343 Error:ConvertEndian32 failed\n");
	  *dest=NULL;
	  mf(toa);
	  mf(tob);
	  return -1;
	}
      else
	{
	  rc = Convert32to64(toa, tob);
	  if (rc)
	    {
	      printf("\343 Error:Convert32to64 failed\n");
	      *dest=(A)NULL;
	      mf(toa);
	      mf(tob);
	      return -1;
	    }
	  else
	    {
	      mf(toa);
	      tob->i= (tob->r) ? tob->d[0] : tob->n; /* fix items */ 
	      *dest=(A)tob;
	      return 1;
	    }
	}
    }

  /* TODO: downward conversions */
  /* Case 5: Big/64    -> Big/32    */
  /* Case 6: Big/64    -> Little/32 */
  /* Case 7: Little/64 -> Little/32 */
  /* Case 8: Little/64 -> Big/32    */

  
  return -1;	
}

int getItems(void *src, I *itemCount, I *rank, I *items, I ilen)
{
  /* Converts the src object to the host format if needed   */
  /*  arguments:                                            */
  /*    src  - pointer to the source data                   */
  /*    itemCount - src->i                                  */
  /*    rank - src->r                                       */
  /*    items - src->d[0]                                   */
  /*    ilen - if src is a mapped file then the file length */
  /*  Returns:                                              */
  /*    0 - no conversion required (dest==NULL)             */
  /*    1 - Conversion successful  (dest==*convertedAobj)   */
  /*   -1 - Conversion failed      (dest==NULL              */

  static int hostWidth = -1;
  static int hostEndian= -1;
  int srcWidth;
  int srcEndian;
  int rc;

  /* Determine host characteristics */
  if(hostWidth==-1)
    GetHostInformation(&hostWidth, &hostEndian);
  
  if(hostEndian == ENDIAN_UNDEF) 
    { 
      printf("\343 Error:Unable to determine host endian\n"); 
      return rc=-1; 
    }

  /* Check and convert source */
  rc=GetSrcInformation(src, ilen, 
		       &srcWidth, &srcEndian, 
		       hostWidth, hostEndian);
  if(rc!=0)
    return -1;

  /* START CONVERSIONS */
  /* Case 1: No Conversions */
  if( srcEndian == hostEndian &&  srcWidth  == hostWidth)
    {
      *itemCount = ((A)src)->i;   
      *items     = ((A)src)->d[0];   
      *rank      = ((A)src)->r;   
      return 0;
    }

  /* Case 2: Big/32 -> Big/64 */
  if ( srcEndian  == ENDIAN_BIG && hostEndian == ENDIAN_BIG &&
       srcWidth   == 32         && hostWidth  == 64 )
    {
      *itemCount = ((A32 *)src)->i;
      *items     = ((A32 *)src)->d[0];
      *rank      = ((A32 *)src)->r;
      return 1;
    }

  /* Case 3: Big/32 -> Little/32 */
  if ( srcEndian == ENDIAN_BIG && hostEndian == ENDIAN_LITTLE &&
       srcWidth  == 32         && hostWidth  == 32)
    {
      
      A32 *to, *from = (A32 *)src;
      A32 wrk32;

      memcpy(&wrk32, src, sizeof(A32) - sizeof(INT32) ); /* copy header */
      SwapEndianHeader32(&wrk32);                /* correct endian */
      *itemCount = wrk32.i;		
      *items     = wrk32.d[0];		
      *rank      = wrk32.r;		
      return 1;
    }

  /* Case 4: Big/32 -> Little/64 */
  if ( srcEndian == ENDIAN_BIG && hostEndian == ENDIAN_LITTLE &&
       srcWidth  == 32         && hostWidth  == 64)
    {
      A32 *from = (A32 *)src;
      A32 wrk;
      A32 *toa;
      A64 *tob;

      memcpy(&wrk, src, sizeof(A32) - sizeof(INT32) ); /* copy header */
      SwapEndianHeader32(&wrk);                /* correct endian */

      *itemCount = wrk.n;	
      *items     = wrk.d[0];	
      *rank      = wrk.r;	

      return 1;
    }

  /* TODO: downward conversions */
  /* Case 5: Big/64    -> Big/32    */
  /* Case 6: Big/64    -> Little/32 */
  /* Case 7: Little/64 -> Little/32 */
  /* Case 8: Little/64 -> Big/32    */

  
  return -1;	
}

	
	
