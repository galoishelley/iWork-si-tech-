#ifndef __FERROR_H
#define __FERROR_H
	
char ferror_msg32[21][80]={
{" FMINVAL 0	/* bottom of error message codes */ "},
{" FALIGNERR 1  /* fielded buffer not aligned */"},
{" FNOTFLD 2    /* buffer not fielded */"},
{" FNOSPACE 3   /* no space in fielded buffer */"},
{" FNOTPRES 4   /* field not present */"},
{" FBADFLD 5    /* unknown field number or type */"},
{" FTYPERR 6    /* illegal field type */"},
{" FEUNIX 7     /* unix system call error */"},
{" FBADNAME 8   /* unknown field name */"},
{" FMALLOC 9    /* malloc failed */"},
{" FSYNTAX 10   /* bad syntax in boolean expression */"},
{" FFTOPEN 11   /* cannot find or open field table */"},
{" FFTSYNTAX 12 /* syntax error in field table */"},
{" FEINVAL 13   /* invalid argument to function */"},
{" FBADTBL 14   /* destructive concurrent access to field table */"},
{" FBADVIEW 15  /* cannot find or get view */"},
{" FVFSYNTAX 16 /* bad viewfile */"},
{" FVFOPEN 17   /* cannot find or open viewfile */"},
{" FBADACM 18   /* ACM contains negative value */"},
{" FNOCNAME 19  /* cname not found */"},
{" FMAXVAL  20  /* top of error message codes */"}
};

#endif /*__FERROR_H*/
