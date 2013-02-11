#ifndef included_cxsys_impexp_h
#define included_cxsys_impexp_h

/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1990-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
extern char *ExportAObject(A, char *, I, I *);
extern int ExportAObjectSizePass(A, const char *,const I, I*, I*);
extern int ExportAObjectFillPass(A, const char *,const I, I, char *);
extern A AExportAObject(A, char *, I, I *);
extern A ImportAObject(char *, I, char *);
#ifdef __cplusplus
}
#endif

#endif

