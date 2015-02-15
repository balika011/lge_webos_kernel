/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */

/*==========================================================================*/
/**
 * @file
 * Trusted Time and System Time Management Library
 *
 * require:
 */
/*==========================================================================*/

//#include <time.h>

#include "basic_types_iptves.h"



#define	USHRT_MAX	0xffffU		/* max value for an unsigned short */
#define	SHRT_MAX	0x7fff		/* max value for a short */
#define SHRT_MIN        (-0x7fff-1)     /* min value for a short */

#define	UINT_MAX	0xffffffffU	/* max value for an unsigned int */
#define	INT_MAX		0x7fffffff	/* max value for an int */
#define	INT_MIN		(-0x7fffffff-1)	/* min value for an int */


#define INT32_MIN	(-2147483647-1)
#define INT32_MAX	(2147483647)
//#define INT_LEAST32_MIN	INT32_MIN
//#define INT_LEAST32_MAX	INT32_MAX
//#define INT_FAST32_MIN	INT32_MIN
//#define INT_FAST32_MAX	INT32_MAX

#define UINT32_MAX	(4294967295U)
//#define UINT_LEAST32_MAX UINT32_MAX
//#define UINT_FAST32_MAX UINT32_MAX


#ifndef _ASM_GENERIC_ERRNO_BASE_H
#define _ASM_GENERIC_ERRNO_BASE_H
#define EPERM 1
#define ENOENT 2
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#endif


/*
** Each file begins with. . .
*/

#define	TZ_MAGIC	"TZif"

struct tzhead {
	char	tzh_magic[4];		/* TZ_MAGIC */
	char	tzh_version[1];		/* '\0' or '2' as of 2005 */
	char	tzh_reserved[15];	/* reserved--must be zero */
	char	tzh_ttisgmtcnt[4];	/* coded number of trans. time flags */
	char	tzh_ttisstdcnt[4];	/* coded number of trans. time flags */
	char	tzh_leapcnt[4];		/* coded number of leap seconds */
	char	tzh_timecnt[4];		/* coded number of transition times */
	char	tzh_typecnt[4];		/* coded number of local time types */
	char	tzh_charcnt[4];		/* coded number of abbr. chars */
};

/*
** . . .followed by. . .
**
**	tzh_timecnt (char [4])s		coded transition times a la time(2)
**	tzh_timecnt (unsigned char)s	types of local time starting at above
**	tzh_typecnt repetitions of
**		one (char [4])		coded UTC offset in seconds
**		one (unsigned char)	used to set tm_isdst
**		one (unsigned char)	that's an abbreviation list index
**	tzh_charcnt (char)s		'\0'-terminated zone abbreviations
**	tzh_leapcnt repetitions of
**		one (char [4])		coded leap second transition times
**		one (char [4])		total correction after above
**	tzh_ttisstdcnt (char)s		indexed by type; if TRUE, transition
**					time is standard time, if FALSE,
**					transition time is wall clock time
**					if absent, transition times are
**					assumed to be wall clock time
**	tzh_ttisgmtcnt (char)s		indexed by type; if TRUE, transition
**					time is UTC, if FALSE,
**					transition time is local time
**					if absent, transition times are
**					assumed to be local time
*/

/*
** If tzh_version is '2' or greater, the above is followed by a second instance
** of tzhead and a second instance of the data in which each coded transition
** time uses 8 rather than 4 chars,
** then a POSIX-TZ-environment-variable-style string for use in handling
** instants after the last transition time stored in the file
** (with nothing between the newlines if there is no POSIX representation for
** such instants).
*/

/*
** In the current implementation, "tzset()" refuses to deal with files that
** exceed any of the limits below.
*/

#ifndef TZ_MAX_TIMES
#define TZ_MAX_TIMES	1200
#endif /* !defined TZ_MAX_TIMES */

#ifndef TZ_MAX_TYPES
#ifndef NOSOLAR
#define TZ_MAX_TYPES	256 /* Limited by what (unsigned char)'s can hold */
#endif /* !defined NOSOLAR */
#ifdef NOSOLAR
/*
** Must be at least 14 for Europe/Riga as of Jan 12 1995,
** as noted by Earl Chew.
*/
#define TZ_MAX_TYPES	20	/* Maximum number of local time types */
#endif /* !defined NOSOLAR */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS	50	/* Maximum number of abbreviation characters */
				/* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#define SECSPERMIN	60
#define MINSPERHOUR	60
#define HOURSPERDAY	24
#define DAYSPERWEEK	7
#define DAYSPERNYEAR	365
#define DAYSPERLYEAR	366
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	((int_fast32_t) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR	12

#define TM_SUNDAY	0
#define TM_MONDAY	1
#define TM_TUESDAY	2
#define TM_WEDNESDAY	3
#define TM_THURSDAY	4
#define TM_FRIDAY	5
#define TM_SATURDAY	6

#define TM_JANUARY	0
#define TM_FEBRUARY	1
#define TM_MARCH	2
#define TM_APRIL	3
#define TM_MAY		4
#define TM_JUNE		5
#define TM_JULY		6
#define TM_AUGUST	7
#define TM_SEPTEMBER	8
#define TM_OCTOBER	9
#define TM_NOVEMBER	10
#define TM_DECEMBER	11

#define TM_YEAR_BASE	1900

#define EPOCH_YEAR	1970
#define EPOCH_WDAY	TM_THURSDAY

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

/*
** Since everything in isleap is modulo 400 (or a factor of 400), we know that
**	isleap(y) == isleap(y % 400)
** and so
**	isleap(a + b) == isleap((a + b) % 400)
** or
**	isleap(a + b) == isleap(a % 400 + b % 400)
** This is true even if % means modulo rather than Fortran remainder
** (which is allowed by C89 but not C99).
** We use this to avoid addition overflow problems.
*/

#define isleap_sum(a, b)	isleap((a) % 400 + (b) % 400)


struct lsinfo {             /* leap second information */
    time_t       ls_trans;   /* transition time */
    int_fast64_t ls_corr;    /* correction to apply */
};


#define BIGGEST(a, b)   (((a) > (b)) ? (a) : (b))

#ifdef TZNAME_MAX
#define MY_TZNAME_MAX   TZNAME_MAX
#endif /* defined TZNAME_MAX */
#ifndef TZNAME_MAX
#define MY_TZNAME_MAX   255
#endif /* !defined TZNAME_MAX */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#ifndef WRONG
#define WRONG   (-1)
#endif /* !defined WRONG */




#ifndef YEARSPERREPEAT
#define YEARSPERREPEAT		400	/* years before a Gregorian repeat */
#endif /* !defined YEARSPERREPEAT */

/*
** The Gregorian year averages 365.2425 days, which is 31556952 seconds.
*/

#ifndef AVGSECSPERYEAR
#define AVGSECSPERYEAR		31556952L
#endif /* !defined AVGSECSPERYEAR */

#ifndef SECSPERREPEAT
#define SECSPERREPEAT		((int_fast64_t) YEARSPERREPEAT * (int_fast64_t) AVGSECSPERYEAR)
#endif /* !defined SECSPERREPEAT */

#ifndef SECSPERREPEAT_BITS
#define SECSPERREPEAT_BITS	34	/* ceil(log2(SECSPERREPEAT)) */
#endif /* !defined SECSPERREPEAT_BITS */


#define EPOCH_YEAR	1970


#ifndef WILDABBR
/*
** Someone might make incorrect use of a time zone abbreviation:
**  1.  They might reference tzname[0] before calling tzset (explicitly
**      or implicitly).
**  2.  They might reference tzname[1] before calling tzset (explicitly
**      or implicitly).
**  3.  They might reference tzname[1] after setting to a time zone
**      in which Daylight Saving Time is never observed.
**  4.  They might reference tzname[0] after setting to a time zone
**      in which Standard Time is never observed.
**  5.  They might reference tm.TM_ZONE after calling offtime.
** What's best to do in the above cases is open to debate;
** for now, we just set things up so that in any of the five cases
** WILDABBR is used. Another possibility: initialize tzname[0] to the
** string "tzname[0] used before set", and similarly for the other cases.
** And another: initialize tzname[0] to "ERA", with an explanation in the
** manual page of what this "time zone abbreviation" means (doing this so
** that tzname[0] has the "normal" length of three characters).
*/
#define WILDABBR    "   "
#endif /* !defined WILDABBR */



struct ttinfo {             /* time type information */
    int_fast32_t tt_gmtoff;  /* UTC offset in seconds */
    int          tt_isdst;   /* used to set tm_isdst */
    int          tt_abbrind; /* abbreviation list index */
    int          tt_ttisstd; /* TRUE if transition is std time */
    int          tt_ttisgmt; /* TRUE if transition is UTC */
};



/* time api */



static const char   gmt[] = "GMT";
static char     wildabbr[] = WILDABBR;


struct state {
    int     leapcnt;
    int     timecnt;
    int     typecnt;
    int     charcnt;
    int     goback;
    int     goahead;
    time_t      ats[TZ_MAX_TIMES];
    unsigned char   types[TZ_MAX_TIMES];
    struct ttinfo   ttis[TZ_MAX_TYPES];
    char        chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof gmt),
                (2 * (MY_TZNAME_MAX + 1)))];
    struct lsinfo   lsis[TZ_MAX_LEAPS];
    int defaulttype; /* for early times or if no transitions */
};

static struct state lclmem;
static struct state gmtmem;
#define lclptr      (&lclmem)
#define gmtptr      (&gmtmem)

typedef int int_fast32_t;
typedef long long	int_fast64_t;

#ifndef TZ_STRLEN_MAX
#define TZ_STRLEN_MAX 255
#endif /* !defined TZ_STRLEN_MAX */


int errno = 0;

static const int    mon_lengths[2][MONSPERYEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int    year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};


static char     lcl_TZname[TZ_STRLEN_MAX + 1];
static int      lcl_is_set;
static int      gmt_is_set;

char *          tzname[2] = {
    wildabbr,
    wildabbr
};




#ifndef TYPE_SIGNED
#define TYPE_SIGNED(type) (((type) -1) < 0)
#endif /* !defined TYPE_SIGNED */

#ifndef TYPE_INTEGRAL
#define TYPE_INTEGRAL(type) (((type) 0.5) != 0.5)
#endif /* !defined TYPE_INTEGRAL */

#define	CHAR_BIT	8		/* number of bits in a char */

#define DBL_DIG	15
#define DBL_MAX	1.7976931348623157E+308
#define DBL_MIN	2.2250738585072014E-308

#define FLT_DIG	6
#define FLT_MAX	3.40282347E+38F
#define FLT_MIN	1.17549435E-38F


#ifndef TYPE_BIT
#define TYPE_BIT(type)	(sizeof (type) * CHAR_BIT)
#endif /* !defined TYPE_BIT */


/* The minimum and maximum finite time values.  */
static time_t const time_t_min =
  (TYPE_SIGNED(time_t)
   ? (time_t) -1 << (CHAR_BIT * sizeof (time_t) - 1)
   : 0);
static time_t const time_t_max =
  (TYPE_SIGNED(time_t)
   ? - (~ 0 < 0) - ((time_t) -1 << (CHAR_BIT * sizeof (time_t) - 1))
   : -1);




#include "x_ckgen.h"
#include "x_chipreg.h"
#include "x_bim.h"

#define TIMER_HZ	(27000000)		//27MHz

typedef unsigned long long int CRYPTO_UINT64;

#define CRYPTO_UI64HL(a,b) (((CRYPTO_UINT64)(a)<<32) | (b))

unsigned long long time_start_count;

/*
	secs 
*/

u_int32_t time_start_sec = 0;

time_t mtk_gmtoff = 0;

time_t mtk_mktime(struct tm * const tmp);

//double mtk_difftime(const time_t time_1, const time_t time_0);


static void dumpbits(char *name, unsigned long long n)
{
	int i = 0;
	
	dprintf("dump %s:\n", name);

	for(i=63; i>=0; i--)
		dprintf("%x", (n>>i)&0x0000000000000001);

	dprintf("\n");

}


static void dumplong(char *name, unsigned long long n)
{
	int i = 0;

	unsigned int high = 0;
	unsigned int low = 0;	
	
	dprintf("dump %s:\n", name);

	high = (n>>32)&0xffffffff;
	low = n&0xffffffff;
	
	dprintf("%08x_%08x\n", high, low);
}


int time_set_start_time(u_int32_t in_start_sec, int gmtoff)
{
	//dprintf(">>%s:\n", __FUNCTION__);

	/* init the start time */
	time_start_sec = in_start_sec;


	/* get the timer count: start */

	time_start_count = CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));
	

	//dumplong("start_count", time_start_count);

	mtk_gmtoff = gmtoff;

	//dprintf("start sec: %x\n",  time_start_sec);

	//dprintf("gmtoff: %x\n",   mtk_gmtoff);	
	
	return 0;
}


static int time_get_end_time(u_int32_t *out_end_sec)
{

	unsigned long long time_end_count = 0;
	unsigned long long time_counts_passed = 0;
	unsigned long long secs_passed = 0;

	
	//dprintf("\n\n>>%s:\n", __FUNCTION__);


	time_end_count =  CRYPTO_UI64HL(BIM_READ32(REG_RW_TIMER2_HIGH),BIM_READ32(REG_RW_TIMER2_LOW));

	//dumplong("end_count", time_end_count);



	//secs passed : shoube be ness than 0xffffffff
	time_counts_passed =  time_start_count - time_end_count;
	
	//dump end time passed
	//dumplong("time_counts_passed", time_counts_passed);
	
	secs_passed = time_counts_passed/TIMER_HZ;

	//
	//dprintf("sec passed:%d\n", secs_passed);


	*out_end_sec = time_start_sec + secs_passed;

	//dprintf("out_end_sec%d\n", secs_passed);



	




	
	return 0;
}


int time_clear(void)
{
	time_start_count = 0;
	
	time_start_sec = 0;

	return 0;
}








/*
	returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
*/
time_t mtk_time(time_t *t)
{
	int ret = 0;
	
	u_int32_t time_sec;

	ret = time_get_end_time(&time_sec);

	return time_sec;
}







static time_t
time2(struct tm * const tmp,
      struct tm * (*const funcp)(const time_t *, int_fast32_t, struct tm *, const struct state *), // android-changed: added sp.
      const int_fast32_t offset,
      int *const okayp, const struct state* sp); // android-changed: added sp.




static int
leaps_thru_end_of(  const int y)
{
    return (y >= 0) ? (y / 4 - y / 100 + y / 400) :
        -(leaps_thru_end_of(-(y + 1)) + 1);
}



static int
increment_overflow(int *const ip, int j)
{
	  int const	i = *ip;

	/*
	** If i >= 0 there can only be overflow if i + j > INT_MAX
	** or if j > INT_MAX - i; given i >= 0, INT_MAX - i cannot overflow.
	** If i < 0 there can only be overflow if i + j < INT_MIN
	** or if j < INT_MIN - i; given i < 0, INT_MIN - i cannot overflow.
	*/
	if ((i >= 0) ? (j > INT_MAX - i) : (j < INT_MIN - i))
		return TRUE;
	*ip += j;
	return FALSE;
}

static int
increment_overflow32(int_fast32_t *const lp, int const m)
{
	  int_fast32_t const	l = *lp;

	if ((l >= 0) ? (m > INT_FAST32_MAX - l) : (m < INT_FAST32_MIN - l))
		return TRUE;
	*lp += m;
	return FALSE;
}

static int
normalize_overflow(int *const tensptr, int *const unitsptr, const int base)
{
	  int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow(tensptr, tensdelta);
}

static int
normalize_overflow32(int_fast32_t *const tensptr, int *const unitsptr,
		     const int base)
{
	  int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow32(tensptr, tensdelta);
}

static int
tmcomp(  const struct tm * const atmp,
         const struct tm * const btmp)
{
      int    result;

    if ((result = (atmp->tm_year - btmp->tm_year)) == 0 &&
        (result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
        (result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
        (result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
        (result = (atmp->tm_min - btmp->tm_min)) == 0)
            result = atmp->tm_sec - btmp->tm_sec;
    return result;
}




static struct tm *
timesub(const time_t *const timep, const int_fast32_t offset,
          const struct state *const sp,
          struct tm *const tmp)
{
      const struct lsinfo *  lp;
      time_t         tdays;
      int            idays;  /* unsigned would be so 2003 */
      int_fast64_t           rem;
    int             y;
      const int *        ip;
      int_fast64_t           corr;
      int            hit;
      int            i;

    corr = 0;
    hit = 0;


    i = sp->leapcnt;

    while (--i >= 0) {
        lp = &sp->lsis[i];
        if (*timep >= lp->ls_trans) {
            if (*timep == lp->ls_trans) {
                hit = ((i == 0 && lp->ls_corr > 0) ||
                    lp->ls_corr > sp->lsis[i - 1].ls_corr);
                if (hit)
                    while (i > 0 &&
                        sp->lsis[i].ls_trans ==
                        sp->lsis[i - 1].ls_trans + 1 &&
                        sp->lsis[i].ls_corr ==
                        sp->lsis[i - 1].ls_corr + 1) {
                            ++hit;
                            --i;
                    }
            }
            corr = lp->ls_corr;
            break;
        }
    }
    y = EPOCH_YEAR;
    tdays = *timep / SECSPERDAY;
    rem = *timep - tdays * SECSPERDAY;
    while (tdays < 0 || tdays >= year_lengths[isleap(y)]) {
        int     newy;
          time_t tdelta;
          int    idelta;
          int    leapdays;

        tdelta = tdays / DAYSPERLYEAR;
        idelta = tdelta;
        if (tdelta - idelta >= 1 || idelta - tdelta >= 1)
            return NULL;
        if (idelta == 0)
            idelta = (tdays < 0) ? -1 : 1;
        newy = y;
        if (increment_overflow(&newy, idelta))
            return NULL;
        leapdays = leaps_thru_end_of(newy - 1) -
            leaps_thru_end_of(y - 1);
        tdays -= ((time_t) newy - y) * DAYSPERNYEAR;
        tdays -= leapdays;
        y = newy;
    }
    {
          int_fast32_t   seconds;
          time_t half_second = 0.5;

        seconds = tdays * SECSPERDAY + half_second;
        tdays = seconds / SECSPERDAY;
        rem += seconds - tdays * SECSPERDAY;
    }
    /*
    ** Given the range, we can now fearlessly cast...
    */
    idays = tdays;
    rem += offset - corr;
    while (rem < 0) {
        rem += SECSPERDAY;
        --idays;
    }
    while (rem >= SECSPERDAY) {
        rem -= SECSPERDAY;
        ++idays;
    }
    while (idays < 0) {
        if (increment_overflow(&y, -1))
            return NULL;
        idays += year_lengths[isleap(y)];
    }
    while (idays >= year_lengths[isleap(y)]) {
        idays -= year_lengths[isleap(y)];
        if (increment_overflow(&y, 1))
            return NULL;
    }
    tmp->tm_year = y;
    if (increment_overflow(&tmp->tm_year, -TM_YEAR_BASE))
        return NULL;
    tmp->tm_yday = idays;
    /*
    ** The "extra" mods below avoid overflow problems.
    */
    tmp->tm_wday = EPOCH_WDAY +
        ((y - EPOCH_YEAR) % DAYSPERWEEK) *
        (DAYSPERNYEAR % DAYSPERWEEK) +
        leaps_thru_end_of(y - 1) -
        leaps_thru_end_of(EPOCH_YEAR - 1) +
        idays;
    tmp->tm_wday %= DAYSPERWEEK;
    if (tmp->tm_wday < 0)
        tmp->tm_wday += DAYSPERWEEK;
    tmp->tm_hour = (int) (rem / SECSPERHOUR);
    rem %= SECSPERHOUR;
    tmp->tm_min = (int) (rem / SECSPERMIN);
    /*
    ** A positive leap second requires a special
    ** representation. This uses "... ??:59:60" et seq.
    */
    tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
    ip = mon_lengths[isleap(y)];
    for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
        idays -= ip[tmp->tm_mon];
    tmp->tm_mday = (int) (idays + 1);
    tmp->tm_isdst = 0;
#ifdef TM_GMTOFF
    tmp->TM_GMTOFF = offset;
#endif /* defined TM_GMTOFF */
    return tmp;
}





/*
** The easy way to behave "as if no library function calls" localtime
** is to not call it--so we drop its guts into "localsub", which can be
** freely called. (And no, the PANS doesn't require the above behavior--
** but it *is* desirable.)
**
** The unused offset argument is for the benefit of mktime variants.
*/

/*ARGSUSED*/
static struct tm *
localsub(const time_t * const timep, const int_fast32_t offset,
         struct tm * const tmp, const struct state * sp) // android-changed: added sp.
{
      const struct ttinfo *  ttisp;
      int            i;
      struct tm *        result;
    const time_t            t = *timep;

    // BEGIN android-changed: support user-supplied sp.
    if (sp == NULL) {
        sp = lclptr;
    }
    // END android-changed
#ifdef ALL_STATE
    if (sp == NULL)
        return gmtsub(timep, offset, tmp, sp); // android-changed: added sp.
#endif /* defined ALL_STATE */
    if ((sp->goback && t < sp->ats[0]) ||
        (sp->goahead && t > sp->ats[sp->timecnt - 1])) {
            time_t          newt = t;
              time_t     seconds;
              time_t     tcycles;
              int_fast64_t   icycles;

            if (t < sp->ats[0])
                seconds = sp->ats[0] - t;
            else    seconds = t - sp->ats[sp->timecnt - 1];
            --seconds;
            tcycles = seconds / YEARSPERREPEAT / AVGSECSPERYEAR;
            ++tcycles;
            icycles = tcycles;
            if (tcycles - icycles >= 1 || icycles - tcycles >= 1)
                return NULL;
            seconds = icycles;
            seconds *= YEARSPERREPEAT;
            seconds *= AVGSECSPERYEAR;
            if (t < sp->ats[0])
                newt += seconds;
            else    newt -= seconds;
            if (newt < sp->ats[0] ||
                newt > sp->ats[sp->timecnt - 1])
                    return NULL;    /* "cannot happen" */
            result = localsub(&newt, offset, tmp, sp); // android-changed: added sp.
            if (result == tmp) {
                  time_t newy;

                newy = tmp->tm_year;
                if (t < sp->ats[0])
                    newy -= icycles * YEARSPERREPEAT;
                else    newy += icycles * YEARSPERREPEAT;
                tmp->tm_year = newy;
                if (tmp->tm_year != newy)
                    return NULL;
            }
            return result;
    }
    if (sp->timecnt == 0 || t < sp->ats[0]) {
        i = sp->defaulttype;
    } else {
          int    lo = 1;
          int    hi = sp->timecnt;

        while (lo < hi) {
              int    mid = (lo + hi) >> 1;

            if (t < sp->ats[mid])
                hi = mid;
            else    lo = mid + 1;
        }
        i = (int) sp->types[lo - 1];
    }
    ttisp = &sp->ttis[i];
    /*
    ** To get (wrong) behavior that's compatible with System V Release 2.0
    ** you'd replace the statement below with
    **  t += ttisp->tt_gmtoff;
    **  timesub(&t, 0L, sp, tmp);
    */
    result = timesub(&t, ttisp->tt_gmtoff, sp, tmp);
    tmp->tm_isdst = ttisp->tt_isdst;
    tzname[tmp->tm_isdst] = &sp->chars[ttisp->tt_abbrind];

    //tmp->TM_ZONE = &sp->chars[ttisp->tt_abbrind];

    return result;
}








static time_t
time1(struct tm * const tmp,
      struct tm * (* const funcp) (const time_t *, int_fast32_t, struct tm *, const struct state *), // android-changed: added sp.
      const int_fast32_t offset, const struct state * sp) // android-changed: added sp.
{
      time_t         t;
      int            samei, otheri;
      int            sameind, otherind;
      int            i;
      int            nseen;
    int             seen[TZ_MAX_TYPES];
    int             types[TZ_MAX_TYPES];
    int             okay;

    if (tmp == NULL) {
        errno = EINVAL;
        return WRONG;
    }
    if (tmp->tm_isdst > 1)
        tmp->tm_isdst = 1;
    t = time2(tmp, funcp, offset, &okay, sp); // android-changed: added sp.


    if (okay || tmp->tm_isdst < 0)
        return t;

    /*
    ** We're supposed to assume that somebody took a time of one type
    ** and did some math on it that yielded a "struct tm" that's bad.
    ** We try to divine the type they started from and adjust to the
    ** type they need.
    */
    // BEGIN android-changed: support user-supplied sp.
    if (sp == NULL) {
        sp = (const struct state *) ((funcp == localsub) ?  lclptr : gmtptr);
    }
    // BEGIN android-changed
#ifdef ALL_STATE
    if (sp == NULL)
        return WRONG;
#endif /* defined ALL_STATE */


    for (i = 0; i < sp->typecnt; ++i)
        seen[i] = FALSE;
    nseen = 0;
    for (i = sp->timecnt - 1; i >= 0; --i)
        if (!seen[sp->types[i]]) {
            seen[sp->types[i]] = TRUE;
            types[nseen++] = sp->types[i];
        }
    for (sameind = 0; sameind < nseen; ++sameind) {
        samei = types[sameind];
        if (sp->ttis[samei].tt_isdst != tmp->tm_isdst)
            continue;
        for (otherind = 0; otherind < nseen; ++otherind) {
            otheri = types[otherind];
            if (sp->ttis[otheri].tt_isdst == tmp->tm_isdst)
                continue;
            tmp->tm_sec += sp->ttis[otheri].tt_gmtoff -
                    sp->ttis[samei].tt_gmtoff;
            tmp->tm_isdst = !tmp->tm_isdst;
            t = time2(tmp, funcp, offset, &okay, sp); // android-changed: added sp.
            if (okay)
                return t;
            tmp->tm_sec -= sp->ttis[otheri].tt_gmtoff -
                    sp->ttis[samei].tt_gmtoff;
            tmp->tm_isdst = !tmp->tm_isdst;
        }
    }
    return WRONG;
}






static time_t
time2sub(struct tm * const tmp,
         struct tm *(*const funcp)(const time_t*, int_fast32_t, struct tm*, const struct state*),
         const int_fast32_t offset,
         int * const okayp,
         const int do_norm_secs, const struct state * sp) // android-changed: added sp
{
      int            dir;
      int            i, j;
      int            saved_seconds;
      int_fast32_t           li;
      time_t         lo;
      time_t         hi;
    int_fast32_t                y;
    time_t              newt;
    time_t              t;
    struct tm           yourtm, mytm;

    *okayp = FALSE;
    yourtm = *tmp;
    if (do_norm_secs) {
        if (normalize_overflow(&yourtm.tm_min, &yourtm.tm_sec,
            SECSPERMIN))
                return WRONG;
    }
    if (normalize_overflow(&yourtm.tm_hour, &yourtm.tm_min, MINSPERHOUR))
        return WRONG;
    if (normalize_overflow(&yourtm.tm_mday, &yourtm.tm_hour, HOURSPERDAY))
        return WRONG;
    y = yourtm.tm_year;
    if (normalize_overflow32(&y, &yourtm.tm_mon, MONSPERYEAR))
        return WRONG;
    /*
    ** Turn y into an actual year number for now.
    ** It is converted back to an offset from TM_YEAR_BASE later.
    */
    if (increment_overflow32(&y, TM_YEAR_BASE))
        return WRONG;
    while (yourtm.tm_mday <= 0) {
        if (increment_overflow32(&y, -1))
            return WRONG;
        li = y + (1 < yourtm.tm_mon);
        yourtm.tm_mday += year_lengths[isleap(li)];
    }
    while (yourtm.tm_mday > DAYSPERLYEAR) {
        li = y + (1 < yourtm.tm_mon);
        yourtm.tm_mday -= year_lengths[isleap(li)];
        if (increment_overflow32(&y, 1))
            return WRONG;
    }
    for ( ; ; ) {
        i = mon_lengths[isleap(y)][yourtm.tm_mon];
        if (yourtm.tm_mday <= i)
            break;
        yourtm.tm_mday -= i;
        if (++yourtm.tm_mon >= MONSPERYEAR) {
            yourtm.tm_mon = 0;
            if (increment_overflow32(&y, 1))
                return WRONG;
        }
    }
    if (increment_overflow32(&y, -TM_YEAR_BASE))
        return WRONG;
    yourtm.tm_year = y;
    if (yourtm.tm_year != y)
        return WRONG;
    if (yourtm.tm_sec >= 0 && yourtm.tm_sec < SECSPERMIN)
        saved_seconds = 0;
    else if (y + TM_YEAR_BASE < EPOCH_YEAR) {
        /*
        ** We can't set tm_sec to 0, because that might push the
        ** time below the minimum representable time.
        ** Set tm_sec to 59 instead.
        ** This assumes that the minimum representable time is
        ** not in the same minute that a leap second was deleted from,
        ** which is a safer assumption than using 58 would be.
        */
        if (increment_overflow(&yourtm.tm_sec, 1 - SECSPERMIN))
            return WRONG;
        saved_seconds = yourtm.tm_sec;
        yourtm.tm_sec = SECSPERMIN - 1;
    } else {
        saved_seconds = yourtm.tm_sec;
        yourtm.tm_sec = 0;
    }
    /*
    ** Do a binary search (this works whatever time_t's type is).
    */
    if (!TYPE_SIGNED(time_t)) {
        lo = 0;
        hi = lo - 1;
    } else if (!TYPE_INTEGRAL(time_t)) {
        if (sizeof(time_t) > sizeof(float))
            hi = (time_t) DBL_MAX;
        else    hi = (time_t) FLT_MAX;
        lo = -hi;
    } else {
        lo = 1;
        for (i = 0; i < (int) TYPE_BIT(time_t) - 1; ++i)
            lo *= 2;
        hi = -(lo + 1);
    }
    for ( ; ; ) {
        t = lo / 2 + hi / 2;
        if (t < lo)
            t = lo;
        else if (t > hi)
            t = hi;
        if ((*funcp)(&t, offset, &mytm, sp) == NULL) { // android-changed: added sp.
            /*
            ** Assume that t is too extreme to be represented in
            ** a struct tm; arrange things so that it is less
            ** extreme on the next pass.
            */
            dir = (t > 0) ? 1 : -1;
        } else  dir = tmcomp(&mytm, &yourtm);
        if (dir != 0) {
            if (t == lo) {
                if (t == time_t_max)
                    return WRONG;
                ++t;
                ++lo;
            } else if (t == hi) {
                if (t == time_t_min)
                    return WRONG;
                --t;
                --hi;
            }
            if (lo > hi)
                return WRONG;
            if (dir > 0)
                hi = t;
            else    lo = t;
            continue;
        }
        if (yourtm.tm_isdst < 0 || mytm.tm_isdst == yourtm.tm_isdst)
            break;
        /*
        ** Right time, wrong type.
        ** Hunt for right time, right type.
        ** It's okay to guess wrong since the guess
        ** gets checked.
        */
        // BEGIN android-changed: support user-supplied sp
        if (sp == NULL) {
            sp = (const struct state *)
                ((funcp == localsub) ? lclptr : gmtptr);
        }
        // END android-changed
#ifdef ALL_STATE
        if (sp == NULL)
            return WRONG;
#endif /* defined ALL_STATE */
        for (i = sp->typecnt - 1; i >= 0; --i) {
            if (sp->ttis[i].tt_isdst != yourtm.tm_isdst)
                continue;
            for (j = sp->typecnt - 1; j >= 0; --j) {
                if (sp->ttis[j].tt_isdst == yourtm.tm_isdst)
                    continue;
                newt = t + sp->ttis[j].tt_gmtoff -
                    sp->ttis[i].tt_gmtoff;
                if ((*funcp)(&newt, offset, &mytm, sp) == NULL) // android-changed: added sp.
                    continue;
                if (tmcomp(&mytm, &yourtm) != 0)
                    continue;
                if (mytm.tm_isdst != yourtm.tm_isdst)
                    continue;
                /*
                ** We have a match.
                */
                t = newt;
                goto label;
            }
        }
        return WRONG;
    }
label:
    newt = t + saved_seconds;
    if ((newt < t) != (saved_seconds < 0))
        return WRONG;
    t = newt;
    if ((*funcp)(&t, offset, tmp, sp)) // android-changed: added sp.
        *okayp = TRUE;
    return t;
}




static time_t
time2(struct tm * const tmp,
      struct tm * (*const funcp)(const time_t *, int_fast32_t, struct tm *, const struct state *), // android-changed: added sp.
      const int_fast32_t offset,
      int *const okayp, const struct state* sp) // android-changed: added sp.
{
    time_t  t;

    /*
    ** First try without normalization of seconds
    ** (in case tm_sec contains a value associated with a leap second).
    ** If that fails, try with normalization of seconds.
    */
    t = time2sub(tmp, funcp, offset, okayp, FALSE, sp);
    return *okayp ? t : time2sub(tmp, funcp, offset, okayp, TRUE, sp);
}




time_t
mtk_mktime(struct tm * const tmp)
{
    
    time_t result = time1(tmp, localsub, 0L, NULL); // android-changed: extra parameter.
    
    return result;
}




#if 0
double mtk_difftime(const time_t time_1, const time_t time_0)
{

	/*
	** If (sizeof (double) > sizeof (time_t)) simply convert and subtract
	** (assuming that the larger type has more precision).
	*/
	if (sizeof (double) > sizeof (time_t))
		return (double) time_1 - (double) time_0;
	if (!TYPE_INTEGRAL(time_t)) {
		/*
		** time_t is floating.
		*/
		return time_1 - time_0;
	}
	if (!TYPE_SIGNED(time_t)) {
		/*
		** time_t is integral and unsigned.
		** The difference of two unsigned values can't overflow
		** if the minuend is greater than or equal to the subtrahend.
		*/
		if (time_1 >= time_0)
			return            time_1 - time_0;
		else	return -(double) (time_0 - time_1);
	}
	/*
	** time_t is integral and signed.
	** Handle cases where both time_1 and time_0 have the same sign
	** (meaning that their difference cannot overflow).
	*/
	if ((time_1 < 0) == (time_0 < 0))
		return time_1 - time_0;
	/*
	** time_1 and time_0 have opposite signs.
	** Punt if uintmax_t is too narrow.
	** This suffers from double rounding; attempt to lessen that
	** by using long double temporaries.
	*/
	if (sizeof (uintmax_t) < sizeof (time_t))
		return (long double) time_1 - (long double) time_0;
	/*
	** Stay calm...decent optimizers will eliminate the complexity below.
	*/
	if (time_1 >= 0 /* && time_0 < 0 */)
		return    (uintmax_t) time_1 + (uintmax_t) (-1 - time_0) + 1;
	return -(double) ((uintmax_t) time_0 + (uintmax_t) (-1 - time_1) + 1);
}
#endif


