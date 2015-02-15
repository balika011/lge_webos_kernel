/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bignum.h"


/*
                File mpmul22.c.  Supplement to mpgcdex.c.

        mp_mul22u(mat, vec1, vec2, lvec, carrys) -- Multiplies the
                  multiple-precision numbers vec1 and vec2 (length lvec)
                  by a 2 x 2 matrix of unsigned scalars.

                        (carrys[0], vec1)   (m11  m12)  (vec1)
                                          =
                        (carrys[1], vec2)   (m21  m22)  (vec2)

                  where mat has m11, m12, m21, m22.
                  Require m11 + m12 < RADIX and m21 + m22 < RADIX.


        mp_mul22s(mat, vec1, vec2, lvec, carrys) -- Multiplies the
                  multiple-precision numbers vec1 and vec2 (length lvec)
                  by a 2 x 2 matrix with signed off-diagonal scalars.

                        (carrys[0], vec1)   (m11  -m12)  (vec1)
                                          =
                        (carrys[1], vec2)   (-m21  m22)  (vec2)

                  where mat has m11, m12, m21, m22.
                  Require m11, m12, m21, m22 all in [0, RADIX/2 - 1]

*/


#if defined(_M_IX86) && DRM_USE_ASSEMBLY

#if defined (_MSC_VER)
#pragma warning(disable: 4100) /* unreferenced formal parameter */
#endif /* _MSC_VER */


/*
            The following are temporary variables in
            some of the X86 assembly codes.
            The entire struct is stored on the stack
            at the start of each routine.
            Each routine uses the data it needs.
*/

typedef struct {
    digit_t carry1;
    digit_t carry2;
    digit_t mat[4];             // Copy of matrix
    __int32 saved_regs22[4];    // ebp, ebx, edi, esi
} mul22_locals_t;

/*
              Assembly language pseudocode for mp_mul22u
              (mp_mul22s is very similar -- carry1, carry2 are signed,
              and some adds of edx:eax are replaced by subtracts).

    save matrix and registers, in struct on stack
    carry1 = ecx = 0

    esp = adjusted stack pointer
    ebp = remaining loop count
    edi = @vec1[i]
    eii = @vec2[i]

loop
        Save carry2 (in ecx)

        edx:eax = m11*vec1[i]
        ebx:ecx = carry1 (zero extended)
        ebx:ecx += edx:eax

        edx:eac = m12*vec2[i]
        ebx:ecx += edx:eax

        edx:eax = m21*vec1[i]
        update carry1 and vec1[i]
        ecx:ebx = carry2 (zero extended)
        ecx:ebx += edx:eax

        edx:eax = m22*vec2[i]
        ecx:ebx += edx:eax

        update  vec2[i]
        adjust ebp, esi, edi
    branch if more iterations

    load carrys address, carry1 (carry2 is in ecx)
    restore ebp, esi, edi
    save carry1
    return
*/

#define mp_mul22u_asm mp_mul22u
__declspec(naked) CRYPTO_BOOL __stdcall mp_mul22u_asm
              (const digit_t mat[4],            // IN  (2 x 2 matrix of scalars)
               digit_t  vec1[],            // INOUT
               digit_t  vec2[],            // INOUT
               const CRYPTO_DWORD lvec,             // IN
               digit_t  carrys[2])         // OUT  (array of 2 scalars)
{
    __asm {
        mov edx,[esp+4]                          ; edx = mat
        sub esp,TYPE mul22_locals_t              ; Allocate space on stack

        xor ecx,ecx                              ; ecx = carry2 = 0
                             ; AGI delay here

        mov [esp].saved_regs22,ebp               ; Save ebp/ebx/edi/esi
        mov [esp+4].saved_regs22,ebx
        mov [esp+8].saved_regs22,edi
        mov [esp+12].saved_regs22,esi

        mov eax,[edx]                            ; Load first two matrix entries
        mov ebx,[edx+4]

        mov ebp,[esp+TYPE mul22_locals_t+4+12]
                                         ; ebp = lvec = remaining loop count
        mov edi,[esp+TYPE mul22_locals_t+4+4]    ; edi = vec1 address

        mov [esp].mat,eax               ; Copy first two matrix entries to stack
        mov [esp+4].mat,ebx

        mov eax,[edx+8]                          ; Load rest of matrix
        mov ebx,[edx+12]

        mov esi,[esp+TYPE mul22_locals_t+4+8]    ; esi = vec2 address
        test ebp,ebp                             ; Check whether loop count == 0

        mov [esp+8].mat,eax                      ; Store rest of matrix
        mov [esp+12].mat,ebx

        mov [esp].carry1,ecx             ; carry1 = 0  (and ecx has carry2 = 0)
        jz mul22u_exit0                          ; Exit if lvec == 0

mul22u_loop1:

        mov edx,[edi]                             ; edx = vec1[i]
        mov eax,[esp].mat                         ; eax = m11

        mov [esp].carry2,ecx                      ; Update carry2
        mov ecx,[esp].carry1

        mul edx                                   ; edx:eax = m11*vec1[i]

        xor ebx,ebx                               ; ebx:ecx = carry1
        add edi,4                                 ; Advance vec1[i] address

        add ecx,eax
        mov eax,[esp+4].mat                       ; eax = m12

        adc ebx,edx                          ; ebx:ecx = m11*vec1[i] + carry1
        mov edx,[esi]                             ; edx = vec2[i]

        mul edx                                   ; edx:eax = m12*vec2[i]

        add ecx,eax
        mov eax,[esp+8].mat                       ; eax = m21

        adc ebx,edx                               ; ebx:ecx += m12*vec2[i]
        mov edx,[edi-4]                           ; edx = vec1[old i]

        mul edx                                   ; edx:eax = m21*vec1[i]

        mov [edi-4],ecx                           ; Update vec1[old i]
        xor ecx,ecx

        mov [esp].carry1,ebx                      ; Update carry1
        mov ebx,[esp].carry2                      ; ecx:ebx = carry2

        add ebx,eax
        mov eax,[esp+12].mat                      ; eax = m22

        adc ecx,edx                           ; ecx:ebx += m21*(old vec1[i])
        mov edx,[esi]                             ; edx = vec2[i]

        mul edx                                   ; edx:eax = m22*vec2[i]

        add esi,4                                 ; Advance vec2[i] address
        add ebx,eax

        adc ecx,edx                               ; ecx:ebx += m22*vec2[i]
        dec ebp                                   ; Decrement loop counter

        mov [esi-4],ebx                           ; Update vec2[old i]
        jnz mul22u_loop1                          ; Continue until ebp == 0

        ; N.B.  ecx = carry2
mul22u_exit0:
        mov eax,[esp+TYPE mul22_locals_t+4+16] ; eax = address of carrys array
        mov edx,[esp].carry1

        mov ebp,[esp].saved_regs22
        mov ebx,[esp+4].saved_regs22

        mov edi,[esp+8].saved_regs22
        mov esi,[esp+12].saved_regs22

        add esp,TYPE mul22_locals_t               ; Remove struct from stack
        mov [eax],edx                             ; carrys[0] = carry1

        mov [eax+4],ecx                           ; carrys[1] = carry2
        mov eax,TRUE
        ret 20                            ; return, remove 5 args from stack

    }  /* end _asm IX86 */
} // mp_mul22u_asm


#define mp_mul22s_asm mp_mul22s
__declspec(naked) CRYPTO_BOOL __stdcall mp_mul22s_asm
              (const digit_t mat[4],            // IN  (2 x 2 matrix of scalars)
               digit_t  vec1[],            // INOUT
               digit_t  vec2[],            // INOUT
               const CRYPTO_DWORD  lvec,            // IN
               sdigit_t scarrys[2])        // OUT  (array of 2 scalars)
{
    __asm {
        mov edx,[esp+4]                          ; edx = mat
        sub esp,TYPE mul22_locals_t              ; Allocate space on stack

        xor ecx,ecx                              ; ecx = carry2 = 0
                             ; AGI delay here

        mov [esp].saved_regs22,ebp               ; Save ebp/ebx/edi/esi
        mov [esp+4].saved_regs22,ebx
        mov [esp+8].saved_regs22,edi
        mov [esp+12].saved_regs22,esi

        mov eax,[edx]                       ; Load first two matrix entries
        mov ebx,[edx+4]

        mov ebp,[esp+TYPE mul22_locals_t+4+12]
                                        ; ebp = lvec = remaining loop count
        mov edi,[esp+TYPE mul22_locals_t+4+4]    ; edi = vec1 address

        mov [esp].mat,eax               ; Copy first two matrix entries to stack
        mov [esp+4].mat,ebx

        mov eax,[edx+8]                          ; Load rest of matrix
        mov ebx,[edx+12]

        mov esi,[esp+TYPE mul22_locals_t+4+8]    ; esi = vec2 address
        test ebp,ebp                          ; Check whether loop count == 0

        mov [esp+8].mat,eax                      ; Store rest of matrix
        mov [esp+12].mat,ebx

        mov [esp].carry1,ecx              ; carry1 = 0  (and ecx has carry2 = 0)
        jz mul22s_exit0                          ; Exit if lvec == 0

mul22s_loop1:

        mov edx,[edi]                             ; edx = vec1[i]
        mov eax,[esp].mat                         ; eax = m11

        mov ebx,[esp].carry1
        mov [esp].carry2,ecx                      ; Update carry2

        mul edx                                   ; edx:eax = m11*vec1[i]

        sar ebx,31
        mov ecx,[esp].carry1                ; ebx:ecx = carry1 (sign extended)

        add ecx,eax
        mov eax,[esp+4].mat                       ; eax = m12

        adc ebx,edx                          ; ebx:ecx = m11*vec1[i] + carry1
        mov edx,[esi]                             ; edx = vec2[i]

        mul edx                                   ; edx:eax = m12*vec2[i]

        sub ecx,eax
        mov eax,[esp+8].mat                       ; eax = m21

        sbb ebx,edx                               ; ebx:ecx -= m12*vec2[i]
        mov edx,[edi]                             ; edx = vec1[i]

        mul edx                                   ; edx:eax = m21*vec1[i]

        mov [esp].carry1,ebx                      ; Update carry1
        mov ebx,[esp].carry2                      ; carry2

        mov [edi],ecx                             ; Update vec1[i]
        mov ecx,ebx                               ; carry2

        sar ecx,31                          ; ecx:ebx = carry2 (sign extended)
        add edi,4                                 ; Advance vec1[i] address

        sub ebx,eax
        mov eax,[esp+12].mat                      ; eax = m22

        sbb ecx,edx                               ; ecx:ebx -= m21*(old vec1[i])
        mov edx,[esi]                             ; edx = vec2[i]

        mul edx                                   ; edx:eax = m22*vec2[i]

        add esi,4                                 ; Advance vec2[i] address
        add ebx,eax

        adc ecx,edx                               ; ecx:ebx += m22*vec2[i]
        dec ebp                                   ; Decrement loop counter

        mov [esi-4],ebx                           ; Update vec2[old i]
        jnz mul22s_loop1                          ; Continue until ebp == 0

        ; N.B.  ecx = carry2
mul22s_exit0:
        mov eax,[esp+TYPE mul22_locals_t+4+16]  ; eax = address of scarrys array
        mov edx,[esp].carry1

        mov ebp,[esp].saved_regs22
        mov ebx,[esp+4].saved_regs22

        mov edi,[esp+8].saved_regs22
        mov esi,[esp+12].saved_regs22

        add esp,TYPE mul22_locals_t               ; Remove struct from stack
        mov [eax],edx                             ; carrys[0] = carry1

        mov [eax+4],ecx                           ; carrys[1] = carry2
        mov eax,TRUE
        ret 20                              ; return, remove 5 args from stack
    }  /* end _asm IX86 */
} // mp_mul22s_asm
#endif
/*
          No assembly code presently (1999) for non-Intel architectures.
          To add others, write an external procedure mp_mul22u or mp_mul22s,
          Then #define mp_mul22u_asm and/or mp_mul22s_asm to be
          the alternate procedure.
*/

#if !defined(mp_mul22u_asm)
#define mp_mul22u_asm mp_mul22u_c
#define mp_mul22u_c mp_mul22u
#endif

#if defined(mp_mul22u_c) 

CRYPTO_BOOL mp_mul22u_c(
            const digit_t mat[4],            // IN  (2 x 2 matrix of scalars)
            digit_t  vec1[],            // INOUT
            digit_t  vec2[],            // INOUT
            CRYPTO_DWORD lvec,             // IN
            digit_t  carrys[2] )         // OUT  (array of 2 scalars)
/*
      This routine is given a 2 x 2 matrix mat with elements m11, m12, m21, m22
      in order.  It is also given two multi-precision numbers vec1 and vec2
      of length lvec.  It does the matrix x vector multiplication

                ( m11  m12 )  ( vec1 )

                ( m21  m22 )  ( vec2 )

      and stores the results back in [vec1, vec2].
      Any carries (beyond lvec words) go into the carrys vector.

      RESTRICTION:  The scalars m11, m12, m21, m22 must be nonnegative
                    and satisfy m11 + m12 < RADIX,  m21 + m22 < RADIX.
                    Given these bounds, it is easy to check by induction
                    that carry1, carry2 are bounded by RADIX - 2.
*/
{
    CRYPTO_DWORD i;
    CRYPTO_BOOL OK = TRUE;
    digit_t carry1 = 0, carry2 = 0;
    const digit_t m11 = mat[0], m12 = mat[1], m21 = mat[2], m22 = mat[3];

    if (m12 > RADIXM1 - m11 || m21 > RADIXM1 - m22) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA, "mp_mul22u");
    }
    if (OK) {
        for (i = 0; i != lvec; i++) {
            const CRYPTO_UINT64 prod11 = MULTIPLY_ADD1(m11, vec1[i], carry1);
            const CRYPTO_UINT64 prod21 = MULTIPLY_ADD1(m21, vec1[i], carry2);
            const CRYPTO_UINT64 prod12 = MULTIPLY_ADD1(m12, vec2[i], CRYPTO_UI64Low32(prod11));
            const CRYPTO_UINT64 prod22 = MULTIPLY_ADD1(m22, vec2[i], CRYPTO_UI64Low32(prod21));
            vec1[i] = CRYPTO_UI64Low32(prod12);
            vec2[i] = CRYPTO_UI64Low32(prod22);
            carry1 = CRYPTO_UI64High32(prod11) + CRYPTO_UI64High32(prod12);
            carry2 = CRYPTO_UI64High32(prod21) + CRYPTO_UI64High32(prod22);
        } // for i
    }
    carrys[0] = carry1;
    carrys[1] = carry2;
    return OK;
} // end mp_mul22u

#endif /* if C code compiled */


#if !defined(mp_mul22s_asm)
#define mp_mul22s_asm mp_mul22s_c
#define mp_mul22s_c mp_mul22s
#endif

#if defined(mp_mul22s_c) 

CRYPTO_UINT64 SDBLINT(sdigit_t s)  // Convert signed value to DBLINT
{
    CRYPTO_INT64 i64 = CRYPTO_I64( s );
    CRYPTO_UINT64 ui64;

    ui64 = *(CRYPTO_UINT64*)&i64;
    return ui64;
}  // SDBLINT

CRYPTO_BOOL mp_mul22s_c(
            const digit_t mat[4],       // IN  (2 x 2 matrix of scalars)
            digit_t  vec1[],            // INOUT
            digit_t  vec2[],            // INOUT
            const CRYPTO_DWORD lvec,       // IN
            sdigit_t carrys[2] )        // OUT  (array of 2 scalars)
/*
      This routine resembles mp_mul22u except that the off-diagonal
      elements m12, m21 are treated as negative (with their absolute values
      passed in the array).  It does the matrix x vector multiplication

                (  m11  -m12 )  ( vec1 )

                ( -m21   m22 )  ( vec2 )

      and stores the results back in [vec1, vec2].
      Any carries (beyond lvec words) go into the carrys vector.

      Here carrys has type sdigit_t rather than digit_t.

      RESTRICTION:  The scalars m11, m12, m21, m22 must be nonnegative
                    and at most M, where M < RADIX/2.
                    This is more restrictive than mp_mul22u,
                    which checks only m11 + m12 and m21 + m22.
                    It is easy to check by induction that
                    carry1, carry2 are in [-M, M-1].
*/
{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD i;
    sdigit_t carry1 = 0, carry2 = 0;
    const digit_t m11 = mat[0], m12 = mat[1], m21 = mat[2], m22 = mat[3];

    if ((m11 | m12 | m21 | m22) & RADIX_HALF) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA, "mp_mul22s\n");
    }
    if (OK) {
        for (i = 0; i != lvec; i++) {
            const CRYPTO_UINT64 prod11 = DPRODUU(m11, vec1[i]);
            const CRYPTO_UINT64 prod12 = DPRODUU(m12, vec2[i]);
            const CRYPTO_UINT64 prod21 = DPRODUU(m21, vec1[i]);
            const CRYPTO_UINT64 prod22 = DPRODUU(m22, vec2[i]);
            CRYPTO_UINT64 prod1, prod2;

            prod1 = CRYPTO_UI64Add(prod11, SDBLINT(carry1));
            prod2 = CRYPTO_UI64Add(prod22, SDBLINT(carry2));
            prod1 = CRYPTO_UI64Sub(prod1, prod12);
            prod2 = CRYPTO_UI64Sub(prod2, prod21);
            vec1[i] = CRYPTO_UI64Low32(prod1);
            vec2[i] = CRYPTO_UI64Low32(prod2);
            carry1 = (sdigit_t)CRYPTO_UI64High32(prod1);
            carry2 = (sdigit_t)CRYPTO_UI64High32(prod2);
        } // for i
    }
    carrys[0] = carry1;
    carrys[1] = carry2;
    return OK;
} // end mp_mul22s

#endif /* if C code compiled */

