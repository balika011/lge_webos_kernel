/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*
    Copyright 2001, 2002 Georges Menie (www.menie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
    tz_putchar is the only external dependency for this file,
    if you have a working tz_putchar, just remove the following
    define. If the function should be called something else,
    replace outbyte(c) by your own function call.
*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_hal_5381.h"
#include "x_ckgen.h"
#include "x_bim.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define RS232_READ32(off)       IO_READ32(RS232_BASE, (off))
#define RS232_WRITE32(off, val) IO_WRITE32(RS232_BASE, (off), (val))

#define REG_U0_OUTPORT          (0x0000)
#define REG_U0_STATUS           (0x004c)
    #define STATUS_TXBUF        (0x1f00)

UINT32 _u4Debug = 0;

#ifndef NOLOG

void tz_putchar(UINT32 u4Char)
{
    while ((RS232_READ32(REG_U0_STATUS) & STATUS_TXBUF) != STATUS_TXBUF)
    { }
    
    RS232_WRITE32(REG_U0_OUTPORT, (UINT32)(u4Char & 0xff));
    if (u4Char == ((UINT32)'\n'))
    {
        RS232_WRITE32(REG_U0_OUTPORT, (UINT32)'\r');
    }
}

static void tz_printchar(char **str, int c)
{
    if (str)
    {
        **str = c;
        ++(*str);
    }
    else
    {
        (void)tz_putchar(c);
    }
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int tz_prints(char **out, const char *string, int width, int pad)
{
    register int pc = 0, padchar = ' ';

    if (width > 0)
    {
        register int len = 0;
        register const char *ptr;
        
        for (ptr = string; *ptr; ++ptr)
        {
            ++len;
        }
        
        if (len >= width)
        {
            width = 0;
        }
        else
        {
            width -= len;
        }
        
        if (pad & PAD_ZERO)
        {
            padchar = '0';
        }
    }
    
    if (!(pad & PAD_RIGHT)) 
    {
        for ( ; width > 0; --width) 
        {
            tz_printchar(out, padchar);
            ++pc;
        }
    }
    
    for ( ; *string ; ++string) 
    {
        tz_printchar(out, *string);
        ++pc;
    }
    
    for ( ; width > 0; --width) 
    {
        tz_printchar(out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int tz_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) 
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return tz_prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) 
    {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) 
    {
        t = u % b;
        if( t >= 10 )
        {
            t += letbase - '0' - 10;
        }
        *--s = t + '0';
        u /= b;
    }

    if (neg) 
    {
        if (width && (pad & PAD_ZERO)) 
        {
            tz_printchar (out, '-');
            ++pc;
            --width;
        }
        else 
        {
            *--s = '-';
        }
    }

    return pc + tz_prints(out, s, width, pad);
}

static int tz_print(char **out, int *varg)
{
    register int width, pad;
    register int pc = 0;
    register char *format = (char *)(*varg++);
    char scr[2];

    for (; *format != 0; ++format) 
    {
        if (*format == '%') 
        {
            ++format;
            width = pad = 0;
            if (*format == '\0') 
            {
                break;
            }
            
            if (*format == '%') 
            {
                goto out;
            }
            
            if (*format == '-') 
            {
                ++format;
                pad = PAD_RIGHT;
            }
            
            while (*format == '0') 
            {
                ++format;
                pad |= PAD_ZERO;
            }
            
            for ( ; *format >= '0' && *format <= '9'; ++format) 
            {
                width *= 10;
                width += *format - '0';
            }
            
            if (*format == 's') 
            {
                register char *s = *((char **)varg++);
                pc += tz_prints(out, s?s:"(null)", width, pad);
                continue;
            }
            
            if (*format == 'd') 
            {
                pc += tz_printi(out, *varg++, 10, 1, width, pad, 'a');
                continue;
            }
            
            if (*format == 'x') 
            {
                pc += tz_printi(out, *varg++, 16, 0, width, pad, 'a');
                continue;
            }
            
            if (*format == 'X') 
            {
                pc += tz_printi(out, *varg++, 16, 0, width, pad, 'A');
                continue;
            }
            
            if (*format == 'u') 
            {
                pc += tz_printi(out, *varg++, 10, 0, width, pad, 'a');
                continue;
            }
            
            if (*format == 'c') 
            {
                /* char are converted to int then pushed on the stack */
                scr[0] = *varg++;
                scr[1] = '\0';
                pc += tz_prints(out, scr, width, pad);
                continue;
            }
        }
        else 
        {
out:
            tz_printchar(out, *format);
            ++pc;
        }
    }
    
    if (out) 
    {
        **out = '\0';
    }
    
    return pc;
}

int tz_printf(const char *format, ...)
{
    int nRet;
    register int *varg = (int *)(&format);

    nRet = tz_print(0, varg);

    return nRet;
}

#else /* NOLOG */

int tz_printf(const char *format, ...)
{
}

#endif /* NOLOG */

#ifdef CC_TVP_SUPPORT
#include "tzst.h"

extern UINT32 sec_fbm_start;
extern UINT32 sec_fbm_end;

extern unsigned long sec_fbm_size ;
extern unsigned long sec_mjc_size ;
EXTERN UINT32 PA_LOAD_BASE;
#endif

void enableDebug(UINT32 args)
{
	UINT32 u4Enable = *(UINT32*)args;
#ifdef CC_ENABLE_TVP_DEBUG
	UINT32 sec_start = (*((UINT32*)args+1)^0xdeadcafe);
	UINT32 sec_end = (*((UINT32*)args+2))^0xdeadcafe;
#endif
	if(u4Enable == 1)
    	_u4Debug = u4Enable;
#ifdef CC_ENABLE_TVP_DEBUG
	if(u4Enable == 2)
	{
	    if(sec_start != 0 && sec_end!=0)
	    {
    		sec_fbm_size = PA_LOAD_BASE - sec_start;
    		sec_mjc_size = sec_end - PA_LOAD_BASE - TZ_DRAM_SC_SIZE;
        }
		tz_printf("sec_fbm_size changed to %x,sec_mjc_size change to %x\n",sec_fbm_size,sec_mjc_size);
	}
	if(u4Enable == 3)
	{
		_u4Debug = 3;
		sec_fbm_start = sec_start;
		sec_fbm_end = sec_end;	
		tz_printf("sec_fbm_start changed to %x,sec_fbm_start change to %x\n",sec_fbm_start,sec_fbm_end);
	}
#endif
}

int dprintf(const char *format, ...)
{
#ifndef NOLOG
    register int *varg = (int *)(&format);

    if (_u4Debug==1)
    {
        return tz_print(0, varg);
    }
#endif

    return 0;
}
