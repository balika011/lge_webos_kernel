/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigdefs.h"
#include "ecurve.h"

//#include <drmpkcrypto.h>


//      File ecexpon.c.    Version 04 December, 2002.
//
//          Exponentiation on an elliptic curve,
//      with or without using tables.  This code uses
//      projective coordinates for intermediate points.
//
//          Let E be the elliptic curve
//
//              y^2      = x^3 + ax + b     modulo a large prime q,
//      or
//              y^2 + xy = x^3 + ax^2 + b   over GF(2^m).
//
//      Given a point Pbase on E, in affine form,
//      we want to find  exponent*Pbase, where
//      exponent is multiple precision and nonnegative.

//          More precisely we are given the affine points
//      (2^table_spacing)^j * Pbase for 0 <= j <= table_last.
//      [When there is no pre-existing table, set table_last = 0,
//      and store only Pbase = 2^0 * Pbase in the `table'.]
//      We allow exponent > (2^table_spacing)^table_last.
//      That is, we will generate beyond the end of the table if needed.

//          Denote table_radix = 2^table_spacing, with table_spacing > 0.
//      We write the exponent in base table_radix
//      with digits -table_radix/2 to table_radix/2.
//      Some exponents have multiple such representations.

//                                                    i
//          Suppose exponent = sum    e  * table_radix  ,
//                                i    i
//
//      where | e_i | <= table_radix/2.  There are 1 + table_radix/2
//      buckets, where bucket  j   has  +- (table_radix^i)*P
//      wherever   e_i = +- j.  Now we must compute
//
//             sum        j * sum                Q
//                j > 0           (Q in bucket j)


//          We do the inner summations (those within individual buckets)
//      in semi-parallel fashion, keeping everything in affine form.
//      If bucket j has b_j members, then try to do FLOOR(b_j/2)
//      independent elliptic curve additions semi-concurrently.
//      Additions within different buckets can occur
//      semi-concurrently, as can two independent additions
//      in one bucket.  ``Semi-concurrently'' means
//      we schedule the additions to be done back-to-back,
//      and the addition code arranges to do only one inversion
//      (over GF(q) or GF(2^m)).

//          After a few iterations
//      [at most ceil(log_2(largest bucket size))],
//      each bucket shrinks to at most a single point.
//      Then we start combining buckets, to evaluate the outer sum
//
//                  sum        j * (contents of bucket j, in affine form)
//                      j > 0
//
//      [In practice, we start this outer sum before all
//      buckets have shrunk, overlapping these additions with
//      the earlier additions.]

//          Suppose table_radix = 128, so the bucket numbers
//      go from 1 to 64 (we can ignore bucket 0).
//      When a bucket shrinks to a single point,
//      we usually move it to two other buckets:

//          Bucket 48 acquires buckets 32 to 64
//          Bucket 24 acquires 17 to 31
//          Bucket 16 acquires 16, -32, 64
//          Bucket 15 acquires 15, -33, 63
//          Bucket 14 acquires 14, -34, 62
//          Bucket 13 acquires 13, -35, 61
//          Bucket 12 acquires 12, -36, 60
//          Bucket 11 acquires 11, -37, 59
//          Bucket 10 acquires 10, -38, 58
//          Bucket  9 acquires  9, -39, 57
//          Bucket  8 acquires  8, -40, 56
//          Bucket  7 acquires  7, -17, 31, -41, 55
//          Bucket  6 acquires  6, -18, 30, -42, 54
//          Bucket  5 acquires  5, -19, 29, -43, 53
//          Bucket  4 acquires  4, -20, 28, -44, 52
//          Bucket  3 acquires  3, -21, 27, -45, 51
//          Bucket  2 acquires  2, -22, 26, -46, 50
//          Bucket  1 acquires  1, -23, 25, -47, 49

//      (a minus sign means we negate the point during the move).
//      For example, we represent 59*Q59 as 48*Q59 + 11*Q59
//      whereas 37*Q37 is represented as 48*Q37 - 11*Q37.

//          The last two sum to 48*(Q59 + Q37) + 11*(Q59 - Q37).
//      We can do Q59 +- Q37 with one elliptic curve combined affine
//      addition-subtraction, inverting the shared denominator only once.
//      The sum goes to bucket 48 and the difference to bucket 11.

//          After two more rounds of intra-bucket merges.
//      bucket 48 has at most 9 members,
//      bucket 24 at most 3 members,
//      buckets 8 to 16 at most 1 member, and
//      buckets 1 to 7 at most 2 members.  Next

//          Bucket 12 acquires 8, 9, 10, 11, 12, 13, 14, 15, 16.
//          Bucket  6 acquires 5, 6, 7 [May need to wait one round.]
//          Bucket  4 acquires 4, -8, 16
//          Bucket  3 acquires 3, -9, 15
//          Bucket  2 acquires 2, -10, 14
//          Bucket  1 acquires 1, -5, 7, -11, 13

//      After the buckets shrink again, we need

//         48*Q48 + 24*Q24 + 12*Q12 + 6*Q6 + 4*Q4 + 3*Q3 + 2*Q2 + Q1.

//      (the 4*Q4 term is not present if table_radix is an even
//      power of 2, such as 64).  We do this final summation
//      in projective form, starting with  2*(2*Q48 + Q24) + Q12.
//      After summing everything else into the Q3, Q2, Q1 terms,
//      we write 3*Q3 + 2*Q2 + Q1 as 2*(Q3 + Q2) + (Q3 + Q1).

//      ---------------   WITHOUT A TABLE -----------------

//           If table_last = 0, then only one point
//      Pbase_powers[0] = Pbase is supplied by the caller.
//      We select table_spacing based on the number of
//      bits in the exponent.

//          We repeatedly double Pbase, but
//      store only into odd-numbered buckets [except possibly
//      at the end, for the leading digits of the exponent].
//      We combine the buckets as above, but typically need
//      fewer steps since most even-numbered buckets are empty.
//      The doublings are done in projective mode,
//      but everything is converted to affine mode (together)
//      before buckets are combined.

//      ----------------   BASIC OPERATIONS ------------------------

//      AFFINIZATION.  Convert the projective point (X, Y, Z)
//      (where X, Y, Z are not all zero) to (x, y) = (X/Z^2, Y/Z^3).

//          IF Z == 0 THEN
//              Return point at infinity;
//          ELSE
//              zinv = 1/Z;
//              zinv2 = zinv^2;
//              x = X*zinv2;
//              y = Y*zinv*zinv2;
//          END IF

//      Costs 1 inversion, 1 squaring, and 3 more multiplications.


//      AFFINE ADDITION.  Given (x1, y1) and (x2, y2) on E,
//      compute (x3, y3) = (x1, y1) + (x2, y2) on E.

//          IF (x1, y1) is point at infinity THEN
//              Set result to (x2, y2);
//          ELSE IF (x2, y2) is point at infinity THEN
//              Set result to (x1, y1);
//          ELSE IF x1 == x2 THEN
//              IF y1 == y2 THEN
//                  Use AFFINE DOUBLING algorithm
//              ELSE
//                  Set result to point at infinity
//              END IF
//          ELSE
//              m = (y1 - y2)/(x1 - x2);
//              IF working over GF(q)
//                  x3 = m^2 - x1 - x2;
//                  y3 = m*(x1 - x3) - y1;
//              ELSE
//                  x3 = m^2 + m - x1 - x2 - a;
//                  y3 = m*(x1 - x3) - y1 - x3;
//              END IF
//          END IF

//      Usually the cost is one division (= multiplication + inversion)
//      plus a squaring and multiplication.


//      AFFINE ADDITION-SUBTRACTION.  Given two affine points
//      (x1, y1) and (x2, y2) on E, compute
//      (x3, y3) = (x1, y1) + (x2, y2) and
//      (x4, y4) = (x1, y1) - (x2, y2).

//          IF (x1, y1) or (x2, y2) is the point at infinity
//              Set answers to other point, or its negative.
//          ELSE IF x1 == x2 THEN
//              One result is the point at infinity.
//              Use AFFINE DOUBLING for the other.
//          ELSE
//              deltax_inv = 1/(x1 - x2)
//              IF working over GF(q) THEN
//                  m3 = (y1 - y2)*deltax_inv;
//                  m4 = (y1 + y2)*deltax_inv;
//                  x3 = m3^2 - x1 - x2;
//                  x4 = m4^2 - x1 - x2;
//                  y3 = m3*(x1 - x3) - y1;
//                  y4 = m4*(x1 - x4) - y1;
//              ELSE
//                  m3 = (y1 - y2)*deltax_inv;
//                  m4 = (y1 + x2 + y2)*deltax_inv;
//                  x3 = m3^2 + m3 - x1 - x2 - a;
//                  x4 = m4^2 + m4 - x1 - x2 - a;
//                  y3 = m3*(x1 - x3) - x3 - y1;
//                  y4 = m4*(x1 - x4) - x4 - y1;
//              END IF
//          END IF

//      Usually the cost is 1 inversion, 2 squarings, 4 multiplications


//      AFFINE DOUBLING.  Given (x1, y1) on E, compute
//      (x2, y2) = 2*(x1, y1) on E.

//          IF working over GF(q) THEN
//              IF (y1 == 0) THEN
//                  Set result to point at infinity
//              ELSE
//                  m = (3*x1^2 + a)/(2*y1);
//                  x2 = m^2 - 2*x1;
//                  y2 = m*(x1 - x2) - y1;
//              END IF
//          ELSE
//              IF (x1 == 0) THEN
//                  Set result to point at infinity
//              ELSE
//                  m = x1 + y1/x1;
//                                 (from (3*x1^2 + 2*a*x1 - y1)/(2*y1 + x1)
//                  x2 = m^2 + m - a;    (from m^2 + m - 2*x1 - a)
//                  y2 = m*(x1 - x2) - y1 - x2;
//              END IF
//          END IF

//      Usual cost is:
//              1 division (inversion + multiplication)
//              1 squaring over GF(2^m), 2 over GF(q)
//              1 multiplication


//      AFFINE NEGATION.  Given an affine point (x1, y1) on E,
//      compute its negative (x2, y2) = -(x1, y1) on E.

//           x2 = x1;
//           IF working over GF(q) THEN
//               y2 = -y1;
//           ELSE
//               y2 = -x1 - y1;
//           END IF

//      No multiplications or inversions needed.

//      PROJECTIVE ADDITION.  Given two projective points (X1, Y1, Z1)
//      and (X2, Y2, Z2) on E, compute
//
//           (X3, Y3, Z3) = (X1, Y1, Z1) + (X2, Y2, Z2)
//
//           IF Z1 == 0 THEN
//               Copy (X2, Y2, Z2) to (X3, Y3, Z3)
//           ELSE IF Z2 == 0 THEN
//               Copy (X1, Y1, Z1) to (X3, Y3, Z3)
//           ELSE
//               U1 = X1*Z2^2;
//               S1 = Y1*Z2^3;
//               U2 = X2*Z1^2;
//               S2 = Y2*Z1^3;
//               W  = U1 - U2;
//               R  = S1 - S2;
//               IF (W == 0) THEN
//                   IF (R == 0) THEN
//                       Use projective doubling algorithm
//                   ELSE
//                       Set result to point at infinity.
//                   END IF
//               ELSE if working over GF(q) THEN
//                   Z3 = W*Z1*Z2
//                   T1 = U1*W^2
//                   T2 = U2*W^2;
//                   X3 = R^2 - T1 - T2;
//                   Y3 = S1*(T2 - T1) - R*(X3 - T1);
//                              // Same as Y3 = -R*X3 + S1*T2 - S2*T1
//               ELSE     // (working over GF(2^m))
//                   Z3 = W*Z1*Z2;
//                   T1 = U1*W^2;
//                   T2 = U2*W^2;
//                   T3 = R + Z3;
//                   X3 = -a Z3^2 + T3*R - T1 - T2;
//                   Y3 = -T3*X3 + S1*T2 - S2*T1
//                          N.B.  This uses one fewer squaring
//                                than the P1363 code of August, 1998.
//               END IF
//           END IF

//      PROJECTIVE-AFFINE ADDITION.  Add a projective point
//      (X1, Y1, Z1) to an affine point (x2, y2).  This is the
//      above code with (X2, Y2, Z2) = (x2, y2, 1).

//      PROJECTIVE DOUBLING.  Given a projective point (X1, Y1, Z1)
//      on E, compute (X2, Y2, Z2) = 2*(X1, Y1, Z1).
//
//          IF z1 == 0 THEN
//              Set (X2, Y2, Z2) to point at infinity
//          ELSE IF working over GF(2^m) THEN
//              X2 = X1^4 + b*Z1^8;
//              Z2 = X1*Z1^2;
//              Y2 = (b*Z1^8)*Z2 + (X1^2 + Y1*Z1)*X2;
//          ELSE
//              S = 4*X1*Y1^2;
//              M = 3*X1^2 + a*Z1^4;
//              X2 = M^2 - 2*S;
//              Y2 = M*(S - X2) - 8*Y1^4;
//              Z2 = 2*Y1*Z1;
//              IF doing several doublings in succession THEN
//                  a*Z2^4 = 2*(a*Z1^4)*(8*Y1^4);
//                           (an observation of Wei Dai)
//              END IF
//          END IF

//       Over GF(q), if we are doing several back-to-back
//       doublings (so a*Z1^4 is known in advance),
//       then the cost per doubling is

//           4 squarings:       X1^2, Y1^2, (2*Y1^2)^2, m^2
//           4 multiplications: X1*Y1^2, M*(S - X2), Y1*Z1, new a*Z1^4

//       Over GF(2^m), this cost is

//           5 squarings:       X1^2, X1^4, Z1^2, Z1^4, Z1^8
//           5 multiplications: b*Z1^8, X1*Z1^2, 3 more for Y2



#define TABLE_SPACING_MAX 7
#define BUCKET_NUMBER_MAX ((CRYPTO_LONG)1 << (TABLE_SPACING_MAX-1))
#define BUCKET_ZERO 0
#define MAX_SLOT 20

typedef enum {ECOP_AFFINIZATION = 1,
              ECOP_ADD,
              ECOP_DOUBLING,
              ECOP_DO_NOTHING} concurrent_op_t;

/*
    digit_t is used for some temporaries.
    It has five field entries packed together (total length 5*elng).
    The first three are a projective point (x, y, z),
    representing the point at infinity if z == 0
    or the affine point (x/z^2, y/z^3) if z != 0.

    The last two entries are optional, and used only over GF(q) fields.
    If nonzero, their product should be a*z^4.
    They are used to speed repeated doublings.
*/
#define ECPROJ5_TEMPS_COUNT 10
       // ecproj5_add_ecaffine needs 10
       // Variable p5temps and typedef digit_t * used
       // for digit_t arrays of length
       // elng*ECPROJ5_TEMPS_OOUNT + ecurve->ndigtemps.
       // (If this changes, be sure to alter test program.)

 /*
        The next struct has much of the temporary data during
        an exponentiation.  It has data shared by multiple
        procedures, as well as large arrays needed locally
        A copy of the struct is malloc-ed early
        in ecaffine_exponentiation_tabular
        and released before exiting.

        The pending_op array holds a queue of requests requiring
        inversions (see operations above).
        The AFFINIZATION, DOUBLING, ADD requests
        take one or two inputs, and produce one output.
        New queue requests may (recursively) be generated
        as outputs are sent to their intended destinations.

        Each bucket (see comments at start of file)
        has a current contents, which may be the identity
        (point at infinity).  It also has an _anticipating_ count.
        The anticipating count is the number of outputs
        in pending_addsubs or pending_op destined for that bucket.
*/

typedef struct {
           CRYPTO_BOOL         OK;          // Error indicator (TRUE if no error)
           CRYPTO_BOOL         char2;       // Characteristic 2?
           CRYPTO_DWORD        slots_in_use;
                             // Next available index in pending_ops
           CRYPTO_LONG         bucket_max;  // Largest bucket number in use
           const ecurve_t  *ecurve;      // Elliptic curve information
           struct {
               CRYPTO_LONG out1;        /* Larger output index */
               CRYPTO_LONG out2;        /* Smaller output index */
                     /* Input buckets are out1 +- out2.
                        Outputs are out1 and out2.  */
           } pending_addsubs[BUCKET_NUMBER_MAX/2];

           struct {
               digit_t    *contents;     // x and y.  Length 2*slng
               CRYPTO_INT     anticipating;
           } bucket[BUCKET_NUMBER_MAX + 1];

           struct {
               digit_t         *inputs;             // Length 4*elng
               CRYPTO_INT          negate_flag;
               concurrent_op_t  operation;
               CRYPTO_LONG         output_bucket;

               CRYPTO_DWORD        denom_index;  // For ec_concurrent_operations
           } pending_op[MAX_SLOT];

           digit_t             *latest_power_known;  // Length 5*elng
           digit_t             *output_point;   // Length 5*elng
           digit_t             *p5temps;        // pointer
           digit_t             *concurrent_ops_temps;  // Length 4*elng
           digit_t             *denoms;         // Length elng*MAX_SLOT
           digit_t             *denom_inverses; // Length elng*MAX_SLOT
           CRYPTO_DWORD            aib_recursion_level;  // Nesting level in
                                            // affine_insert_bucket
           digit_t             *aib_temps[2];   // Two arrays, each length 2*elng
    } ecexpon_data_t;

static CRYPTO_VOID affine_insert_bucket
     (const digit_t *, const CRYPTO_LONG, ecexpon_data_t*, const CRYPTO_INT, struct bigctx_t *f_pBigCtx);

static CRYPTO_BOOL ecaffine_to_ecproj5(const digit_t *, digit_t *,
                                const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

static CRYPTO_VOID ec_concurrent_operations(ecexpon_data_t*, struct bigctx_t *f_pBigCtx);

static CRYPTO_DWORD get_ec_concurrent_slot(ecexpon_data_t*, struct bigctx_t *f_pBigCtx);

static CRYPTO_VOID proj5_insert_bucket
        (const digit_t*, const CRYPTO_LONG, ecexpon_data_t*, const CRYPTO_INT, struct bigctx_t *f_pBigCtx);

static CRYPTO_VOID schedule_affine_add_and_sub
        (const digit_t *, const digit_t *, const CRYPTO_LONG, const CRYPTO_LONG,
            ecexpon_data_t*, struct bigctx_t *f_pBigCtx);

static CRYPTO_VOID schedule_affine_add_or_sub
          (const digit_t *, const digit_t *,
           const CRYPTO_LONG, ecexpon_data_t*, const CRYPTO_INT, struct bigctx_t *f_pBigCtx);


static CRYPTO_BOOL ecproj5_add_ecaffine
            (const digit_t *, const digit_t *,
            digit_t *, const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

static CRYPTO_BOOL ecproj5_add_ecproj5(const digit_t *, const digit_t *,
                                       digit_t *, const struct ecurve_t*,
                                       digit_t *, struct bigctx_t *f_pBigCtx);

static CRYPTO_BOOL ecproj5_doubling(const digit_t *, digit_t *,
                                    const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

static CRYPTO_BOOL ecproj5_on_curve(const digit_t *, const struct ecurve_t*,
                                    const CRYPTO_CHAR *, digit_t *, struct bigctx_t *f_pBigCtx);

static CRYPTO_BOOL ecproj5_set_infinite
             (digit_t *, const struct ecurve_t*, digit_t *);

static CRYPTO_BOOL ecproj5_to_ecaffine(const digit_t *, digit_t *,
                                const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

CRYPTO_BOOL ecaffine_exponentiation_tabular
                (const digit_t *Pbase_powers,
                 const CRYPTO_DWORD    table_spacing,
                 const CRYPTO_DWORD    table_last,
                 const digit_t     exponent[],
                 const CRYPTO_DWORD    lexpon,
                 digit_t  *result,
                 const ecurve_t   *ecurve,
                 struct bigctx_t *f_pBigCtx)
{
    const CRYPTO_DWORD      nbit_exponent
                       = mp_significant_bit_count(exponent, lexpon);
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD      elng  = fdesc->elng;
    const CRYPTO_LONG  bucket_max
                         = (CRYPTO_LONG)1 << (table_spacing - 1);

    CRYPTO_LONG carry_bits, ibucket;
    CRYPTO_DWORD iaddsub, ibatch, ibit, iop, lg2_latest_power_known, naddsub;
    CRYPTO_BOOL progress;
    CRYPTO_BOOL OK = TRUE;
    const digit_t *Pbase_powers_next;
    digit_t     *dtemps = NULL;
    ecexpon_data_t ecxdat, *pecxdat = &ecxdat;
    const CRYPTO_DWORD num_p5temps = elng*ECPROJ5_TEMPS_COUNT + ecurve->ndigtemps;
    const CRYPTO_DWORD num_dtemps = num_p5temps + 6*MAX_SLOT*elng
             + 2*elng*(bucket_max + 1) + 18*elng;

    /*
    ** Overflow/Underflow check
    */
    if ( num_p5temps < ecurve->ndigtemps
      || elng*ECPROJ5_TEMPS_COUNT < elng
      || num_p5temps < elng*ECPROJ5_TEMPS_COUNT )
    {
        OK = FALSE;
    }

    if (!OK) {
    } else if (table_spacing < 3 || table_spacing > TABLE_SPACING_MAX) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                       "ec_exponentiaton_tabular");

        TRACE( ( "Invalid table_spacing = %ld in ec exponentiation_tabular -- not in [3, %ld]\n",
               ( CRYPTO_LONG )table_spacing, ( CRYPTO_LONG )TABLE_SPACING_MAX ) );      
    } else {
        dtemps = digit_allocate(num_dtemps,(CRYPTO_CHAR *)"ecaffine_exponentiation_tabular");
        if (dtemps == NULL) OK = FALSE;
    }
    if (OK) {
        digit_t * p5temps = dtemps;
        digit_t *pending_op_inputs = p5temps + num_p5temps;
        digit_t *bucket_contents = pending_op_inputs + 4*MAX_SLOT*elng;
        pecxdat->denoms = bucket_contents + 2*elng*(bucket_max + 1);
        pecxdat->denom_inverses = pecxdat->denoms + MAX_SLOT*elng;
        pecxdat->aib_temps[0] = pecxdat->denom_inverses + MAX_SLOT*elng;
        pecxdat->aib_temps[1] = pecxdat->aib_temps[0] + 2*elng;
        pecxdat->latest_power_known = pecxdat->aib_temps[1] + 2*elng;
        pecxdat->output_point = pecxdat->latest_power_known + 5*elng;
        pecxdat->concurrent_ops_temps = pecxdat->output_point + 5*elng;
        DRMASSERT (pecxdat->concurrent_ops_temps + 4*elng == dtemps + num_dtemps);

        pecxdat->aib_recursion_level = 0;
        pecxdat->bucket_max = bucket_max;
        pecxdat->char2 = CHARACTERISTIC_2(fdesc);
                                          // Characteristic 2?
        pecxdat->ecurve = ecurve;
        pecxdat->OK = OK;
        pecxdat->p5temps = p5temps;
        pecxdat->slots_in_use = 0;

        for (iop = 0; iop != MAX_SLOT; iop++) {
            pecxdat->pending_op[iop].inputs = pending_op_inputs + 4*iop*elng;
        }

        for (ibucket = 0; ibucket <= bucket_max; ibucket++) {
            pecxdat->bucket[ibucket].anticipating = 0;
                 // Not anticipating any exports from other buckets
            pecxdat->bucket[ibucket].contents
                   = bucket_contents + 2*ibucket*elng;
                 // Address for (x, y)
            pecxdat->OK = pecxdat->OK
                 && ecaffine_set_infinite
                         (pecxdat->bucket[ibucket].contents,
                         ecurve, f_pBigCtx);
                     // Empty bucket (i.e., set contents to point at infinity)
         } // for ibucket

/*
            Initialize table of addition-subtraction pairs.
            If table_spacing = 7 (i.e,, bucket_max = 64), we pair

                 48-i  with  48+i  for  1 <= i <= 16
                 24-i  with  24+i  for  1 <= i <= 7
                 12-i  with  12+i  for  1 <= i <= 4
                  6-i  with   6+i  for  1 <= i <= 1

            and have {1, 2, 3, 4, 6, 12, 24, 48} remaining.
            If instead table spacing = 6 (bucket_max = 32), then we pair

                 24-i  with  24+i  for  1 <= i <= 8
                 12-i  with  12+i  for  1 <= i <= 3
                  6-i  with   6+i  for  1 <= i <= 2

            and have {1, 2, 3, 6, 12, 24} remaining.
*/

        naddsub = 0;
        for (ibatch = 0;
             ibatch != table_spacing - 3 && pecxdat->OK;
             ibatch++) {
            const CRYPTO_LONG ipow2 = (CRYPTO_LONG)1
                                  << (table_spacing - 3 - ibatch);

            for (ibucket = 1;
                 ibucket <= ipow2 - (CRYPTO_LONG)(ibatch & 1);
                 ibucket++) {

                /* Pair 3*ipow2 +- ibucket */

                pecxdat->pending_addsubs[naddsub].out1 = 3*ipow2;
                pecxdat->pending_addsubs[naddsub].out2 = ibucket;
                naddsub++;
                pecxdat->bucket[3*ipow2].anticipating++;
                pecxdat->bucket[ibucket].anticipating++;
            }
        } /* for ibatch */
        if (!pecxdat->OK) {
        } else if (naddsub >   sizeof(pecxdat->pending_addsubs)
                              /sizeof(pecxdat->pending_addsubs[0])) {
            pecxdat->OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "ecexpon pending_addsubs overflow");
            TRACE( ( "Pending_addsubs overflow in ecexpon, table_spacing = %ld, naddsub = %ld\n",
                   ( CRYPTO_LONG )table_spacing, ( CRYPTO_LONG )naddsub ) );
        }

               // Plan to use the first point in the
               // supplied table as our latest point.

        lg2_latest_power_known = 0-table_spacing;
        Pbase_powers_next = Pbase_powers;

             // Loop through bits of exponent, right to left.
             // Entire loop is skipped if nbit_exponent = 0.

        carry_bits = 0;
        for (ibit = 0;
            (ibit < nbit_exponent || carry_bits != 0) && pecxdat->OK;
             ibit++) {
            if (ibit < nbit_exponent) {
                carry_bits +=
                    (CRYPTO_LONG)mp_getbit(exponent, ibit)
                 << (ibit - lg2_latest_power_known);
            }
                // We need to add
                // (remaining bits ++ carry_bits)*latest_power_known,
                // where ++ denotes adddition after shifting left operand.
                // The parenthesized quantity is known to be strictly positive.

/*
              As long as the supplied table (Pbase_powers)
              still has useful data, we multiply each table entry
              (or its negative) by an integer up to
              bucket_max = 2^(table_spacing - 1) .
              If the exponent is beyond the range of the table.
              then (after the table has been exhausted),
              we repeatedly double the last entry from the table
              and occasionally multiply that result by an integer.

              As long as ibit <= lg2_latest_power2 + table_spacing,
              we procrastinate deciding which multiple of
              latest_power_known to use
              (i.e., which bucket to deposit it in).

              If carry_bits exceeds bucket_max, or if
              there are more exponent bits to process, then we know
              we'll be multiplying latest_power_known by a large number.
              When there are no more table entries, and we know that
              this multiplier will be even, we divide it by 2
              and double latest_power_known now, to
              reduce the size of the multiplier.
*/
            if (ibit >= lg2_latest_power_known + table_spacing) {
                  /*
                     Choose ibucket == carry_bits (mod 2*bucket_max),
                     with  ABS(ibucket) <= bucket_max.

                     If carry_bits == bucket_max (mod 2*bucket_max),
                     we can choose ibucket = +- bucket_max.
                     Choose the sign so that
                     ibucket == carry_bits (mod 4*bucket_max).
                     Then the new carry_bits
                     (after dividing by 2*bucket_max on next iteration)
                     will be even, and more likely to be zero.

                     When ibit = 0, latest_power_known will be uninitialized,
                     but this is OK since ibucket = 0.
                 */

                ibucket = ((carry_bits + bucket_max) & (4*bucket_max - 1))
                            - bucket_max;  // In [-bucket_max, 3*bucket_max - 1]

                if (ibucket > bucket_max) ibucket -= 2*bucket_max;
                                       // In [-bucket_max, bucket_max]
                carry_bits -= ibucket;

                if (ibucket != BUCKET_ZERO) {
                    CRYPTO_INT negate_flag;
                    if (ibucket < 0) {
                        negate_flag = -1;
                        ibucket = -ibucket;
                    } else {
                        negate_flag = +1;
                    }

                    proj5_insert_bucket(pecxdat->latest_power_known,
                               ibucket, pecxdat, negate_flag, f_pBigCtx);
                }

                if (   carry_bits < 0
                    || carry_bits != (carry_bits >> table_spacing)
                                         << table_spacing) {
                    pecxdat->OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "ec_exponentiation_tabular carry bits");
                    TRACE( ( "Invalid carry_bits = %ld in ec_exponentiation_tabular\n",
                           ( CRYPTO_LONG )carry_bits ) );
                }

                if (   lg2_latest_power_known + table_spacing
                    <= table_last*table_spacing) {
                                  /* More entries in supplied table */

                    lg2_latest_power_known += table_spacing;
                    carry_bits >>= table_spacing; /* Divide by 2*bucket_max */

                    pecxdat->OK = pecxdat->OK &&
                        ecaffine_to_ecproj5(Pbase_powers_next,
                                            pecxdat->latest_power_known,
                                            ecurve, p5temps, f_pBigCtx);
                    Pbase_powers_next += 2*elng;
                } /* New table entry? */
            } /* if */

            while (     ibit >= lg2_latest_power_known + table_spacing
                    &&  IS_EVEN(carry_bits)
                    &&  pecxdat->OK
                    && (    ibit < nbit_exponent - 1
                         || carry_bits > bucket_max)) {
                            /* Go beyond user-supplied table */
                carry_bits /= 2;
                pecxdat->OK = pecxdat->OK
                        && ecproj5_doubling(pecxdat->latest_power_known,
                                        pecxdat->latest_power_known,
                                        ecurve, p5temps, f_pBigCtx);
                lg2_latest_power_known++;
            } /* while */

            if (   ibit < lg2_latest_power_known
                || ibit > lg2_latest_power_known + table_spacing) {
                pecxdat->OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "ecaffine_exponentiation_tabular -- bad ibit");
                TRACE( ( "ecaffine_exponentiation_tabular -- ibit = %ld, lg2_latest_power_known = %ld\n",
                       ( CRYPTO_LONG )ibit, ( CRYPTO_LONG )lg2_latest_power_known ) );
            }
        } /* for ibit */

/*
        Various powers +- 2*j * Pbase have been inserted into buckets
        (perhaps with an affinization scheduled first).
        Where a bucket has multiple entries,
        start summing those entries, two at a time
        [so its population drops approximately in half].
        When a bucket shrinks to one entry, and nothing else
        is waiting to go there, move the bucket's output elsewhere.
 */
        progress = TRUE;
        while (pecxdat->OK && progress) {
            CRYPTO_DWORD naddsub_new = 0;

            for (iaddsub = 0; iaddsub != naddsub; iaddsub++) {
                const CRYPTO_LONG out1
                      = pecxdat->pending_addsubs[iaddsub].out1;
                const CRYPTO_LONG out2
                      = pecxdat->pending_addsubs[iaddsub].out2;
                const CRYPTO_LONG in1 = out1 + out2;
                const CRYPTO_LONG in2 = out1 - out2;

                if (   pecxdat->bucket[in1].anticipating == 0
                    && pecxdat->bucket[in2].anticipating == 0) {
                    digit_t *p1 = pecxdat->bucket[in1].contents;
                    digit_t *p2 = pecxdat->bucket[in2].contents;
                     /* If neither bucket is anticipating, */
                     /* then both are available to be moved. */
                    schedule_affine_add_and_sub(p1, p2, out1,
                                            out2, pecxdat, f_pBigCtx);
                    pecxdat->OK = pecxdat->OK
                        && ecaffine_set_infinite(p1, ecurve, f_pBigCtx)
                        && ecaffine_set_infinite(p2, ecurve, f_pBigCtx);
                } else {
                    pecxdat->pending_addsubs[naddsub_new].out1 = out1;
                    pecxdat->pending_addsubs[naddsub_new].out2 = out2;
                    naddsub_new++;    /* Put at start of table. */
                                      /* Try again next iteration. */
                }
            } /* for iaddsub */
            progress = (naddsub_new != naddsub);
            naddsub = naddsub_new;

            if (!progress && pecxdat->slots_in_use > 0) {
                progress = TRUE;
                ec_concurrent_operations(pecxdat, f_pBigCtx);
            }
        } // while

        for (ibucket = 1; ibucket <= bucket_max; ibucket++) {
            const CRYPTO_INT antic = pecxdat->bucket[ibucket].anticipating;
            if (antic != 0 && pecxdat->OK) {
                pecxdat->OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                    "ecaffine_exponentiation_tabular -- anticipating nonzero");
                
                TRACE( ( "ecaffine_exponentiation_tabular -- bucket %ld has anticipating = %d\n",
                       ( CRYPTO_LONG )ibucket, ( CRYPTO_INT )antic ) );
            }
        } // for ibucket

        //  Form output_point = 16*Q48 + 8*Q24 + 4*Q12 + 2*Q6 + Q3 (say)

        pecxdat->OK = pecxdat->OK
            && ecproj5_set_infinite(pecxdat->output_point, ecurve,
                                    p5temps);
        for (ibatch = 0;
             ibatch <= table_spacing - 3 && pecxdat->OK;
             ibatch++) {

            const CRYPTO_LONG ipow2 = (CRYPTO_LONG)1
                                    << (table_spacing - 3 - ibatch);
            digit_t *p1 = pecxdat->bucket[3*ipow2].contents;

            pecxdat->OK = pecxdat->OK
                && ecproj5_doubling(pecxdat->output_point,
                                    pecxdat->output_point,
                                    ecurve, p5temps, f_pBigCtx)
                && ecproj5_add_ecaffine(pecxdat->output_point, p1,
                                        pecxdat->output_point,
                                        ecurve, p5temps, f_pBigCtx)
                && ecaffine_set_infinite(p1, ecurve, f_pBigCtx);
        } // for ibatch

//      Finish output_point = 4*Q4 + 3*output_point + 2*Q2 + Q1
//      Q4 will often be the point at infinity.
//      Use latest_power_known for 2*Q4 + Q2.
//      Then write 3*output_point + 2*latest_power_known + Q1
//      as 2*(output_point + latest_power_known) + (output_point + Q1).

        pecxdat->OK = pecxdat->OK
            && ecaffine_to_ecproj5(pecxdat->bucket[4].contents,
                                   pecxdat->latest_power_known,
                                   ecurve, p5temps, f_pBigCtx) // Q4
            && ecproj5_doubling(pecxdat->latest_power_known,
                                pecxdat->latest_power_known,
                                ecurve, p5temps, f_pBigCtx)    // 2*Q4
            && ecproj5_add_ecaffine(pecxdat->latest_power_known,
                                    pecxdat->bucket[2].contents,
                                    pecxdat->latest_power_known,
                                    ecurve, p5temps, f_pBigCtx) // 2*Q4 + Q2
            && ecproj5_add_ecproj5(pecxdat->latest_power_known,
                                   pecxdat->output_point,
                                   pecxdat->latest_power_known,
                                   ecurve, p5temps, f_pBigCtx)
                                                             // 2*Q4 + Q2 + Q3
            && ecproj5_add_ecaffine(pecxdat->output_point,
                                    pecxdat->bucket[1].contents,
                                    pecxdat->output_point,
                                    ecurve, p5temps, f_pBigCtx);   // Q3 + Q1

    //   Now need 2*latest_power_known + output_point.
    //   Convert final output to affine form.

        pecxdat->OK = pecxdat->OK
            && ecproj5_doubling(pecxdat->latest_power_known,
                                pecxdat->latest_power_known,
                                ecurve, p5temps, f_pBigCtx)
            && ecproj5_add_ecproj5(pecxdat->latest_power_known,
                                   pecxdat->output_point,
                                   pecxdat->output_point,
                                   ecurve, p5temps, f_pBigCtx)
            && ecproj5_to_ecaffine(pecxdat->output_point, result,
                                   ecurve, p5temps, f_pBigCtx);

//   Check that we did not neglect any data in the buckets.

        for (ibucket = 1;
             ibucket <= bucket_max && pecxdat->OK; ibucket++) {
            const CRYPTO_INT antic = pecxdat->bucket[ibucket].anticipating;
            const digit_t *contents = pecxdat->bucket[ibucket].contents;

            if (antic != 0) {
                pecxdat->OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                       "ecaffine_exponentiation_tabular, bad anticipating");
                TRACE( ( "ecaffine_exponentiation_tabular -- bucket %ld has anticipating = %d\n",
                       ( CRYPTO_LONG )ibucket, antic ) );
            }
            if (ibucket > 4 && !ecaffine_is_infinite(contents, ecurve,
                                                     f_pBigCtx)) {
                pecxdat->OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                         "ecaffine_exponentiation_tabular -- bucket not used");
                TRACE( ( "ecaffine_exponentiation_tabular -- bucket %ld output not used\n", 
                       ( CRYPTO_LONG )ibucket ) );
            }
        } // for ibucket
             OK = pecxdat->OK;
    }
    if (dtemps != NULL) {Free_Temporaries(dtemps);}
    return OK;
} // end ecaffine_exponentiation_tabular
/* --------------------------------------------------------  */
CRYPTO_BOOL ecaffine_table_construction
        (const digit_t *p0,          // Input point
         const CRYPTO_DWORD   table_spacing,
         const CRYPTO_DWORD   table_last,
         digit_t *table,        // OUT -- size 2*elng*(table_last+1)
         const ecurve_t  *ecurve,
         struct bigctx_t *f_pBigCtx)
/*
     In some problems one point on an elliptic curve is
     raised to multiple powers.  An example is the
     first phase of Diffie-Hellman, where a public
     generator is raised to a random power,
     and may be raised to a different random power later.

     ecaffine_exponentiation_tabular allows the user
     to input a table containing

           g,  g^e, g^(e^2), g^(e^3),

     where e = 2^(table_spacing).

     This routine constructs that table.
     The output array (table) has p0 in its first entry (2*elng words).
     The second entry has e*p0, then (e^2)*p0, ... up to
     (e^table_spacing)*p0.

     The choice of table_spacing depends upon space and speed
     requirements.  For moderate exponents (e.g., 160 bits)
     4 is adequate.  5 or 6 is better for 512-bit exponents.
*/

{
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_DWORD i, num_finite_points;

    CRYPTO_BOOL OK = TRUE;

    digit_t *denoms;      // Length elng*table_last
    digit_t *drecips;     // Length elng*table_last
    digit_t *ecpoint;     // Length 5*elng
    digit_t *ectemps;     // Length ecurve->ndigtemps (affine EC operations)
                          // Overlaps ftemps
    digit_t *ftemps;      // Length ecurve->fdesc->ndigtemps_arith;
    digit_t *ftemp1;      // Length elng
    digit_t *p5temps;     // Length ECPROJ5_TEMPS_COUNT * elng

    const CRYPTO_DWORD numdtemps =   elng*(6 + ECPROJ5_TEMPS_COUNT + 2*table_last)
                          + ecurve->ndigtemps;
    digit_t *dtemps = NULL;

    if( numdtemps >= ecurve->ndigtemps 
     && elng*(6 + ECPROJ5_TEMPS_COUNT + 2*table_last) >= elng
     && numdtemps >= elng*(6 + ECPROJ5_TEMPS_COUNT + 2*table_last) )
    {
        dtemps = digit_allocate(numdtemps,(CRYPTO_CHAR *)"ecaffine_table_construction");
    }
    if (dtemps == NULL) {
        OK = FALSE;
        denoms = drecips = ecpoint = ftemp1 = p5temps
               = ftemps = ectemps = NULL;
    } else {
        denoms = dtemps;
        drecips = denoms + elng*table_last;
        ecpoint = drecips + elng*table_last;
        ftemp1 = ecpoint + 5*elng;
    // p5temps, ftemps, ectemps must remain together
        p5temps = ftemp1 + elng;
        ftemps = ectemps = p5temps + ECPROJ5_TEMPS_COUNT*elng;
        DRMASSERT(ectemps + ecurve->ndigtemps == dtemps + numdtemps);
    }
    OK = OK && ecaffine_on_curve(p0, ecurve,
                      (CRYPTO_CHAR *)"",
                      ectemps, f_pBigCtx);
    memcpy( table,p0,( 2* fdesc->elng)*sizeof( digit_t ));// Copy given point
    OK = OK && ecaffine_to_ecproj5(p0, ecpoint, ecurve, p5temps, f_pBigCtx);

    num_finite_points = 0;
    for (i = 1; OK && i <= table_last; i++) {
        digit_t *tablei = table + 2*elng*i;
        digit_t *z = ecpoint + 2*elng;
        CRYPTO_DWORD j;

        for (j = 0; j != table_spacing; j++) {
            OK = OK && ecproj5_doubling(ecpoint, ecpoint, ecurve,
                                        p5temps, f_pBigCtx);
        }

        if (Kiszero(z, fdesc, f_pBigCtx)) {// Doubling gave point at infinity
            OK = OK && ecaffine_set_infinite(tablei, ecurve, f_pBigCtx);
        } else {
            num_finite_points++;
            memcpy( tablei,ecpoint,( 2* fdesc->elng)*sizeof( digit_t ));        // x and y
            memcpy( denoms + elng*(i-1),z,fdesc->elng*sizeof( digit_t )); // z (nonzero)
        }
    } // for i

    //  Invert denominators (z values).

    if (!OK) {
    } else if (num_finite_points > table_last) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                       "ecaffine_table_construction");

    } else {      // Invert z values
        OK = OK && Kinvert_many(denoms, drecips,
                                num_finite_points,
                                fdesc, ftemps, f_pBigCtx);

                  // Set each (x, y) to (x/z^2, y/z^3)

        for (i = 0; i != num_finite_points; i++) {
            const digit_t *zinv = drecips + i*elng;
            digit_t *x = table + 2*(i+1)*elng;
            digit_t *y = x + elng;

            OK = OK && Kmul(zinv, zinv, ftemp1, fdesc, ftemps,
                                                       f_pBigCtx)  // 1/z^2
                    && Kmul(y,  zinv, y,        fdesc, ftemps,
                                                       f_pBigCtx)  // y/z
                    && Kmul(y, ftemp1, y,       fdesc, ftemps,
                                                       f_pBigCtx)  // y/z^3
                    && Kmul(x, ftemp1, x,       fdesc, ftemps,
                                                       f_pBigCtx); // x/z^2
        } // for i
    }

#if BIGNUM_EXPENSIVE_DEBUGGING
    for (i = 0; OK && i <= table_last; i++) {
        OK = OK && ecaffine_on_curve(table + 2*elng*i, ecurve,
                           "ecaffine_table_construction output",
                           ectemps, f_pBigCtx);
    } // for i
#endif

    if (dtemps != NULL) {Free_Temporaries(dtemps);}
    return OK;
} // end ecaffine_exponentiation
/* --------------------------------------------------------  */
static CRYPTO_VOID affine_insert_bucket
        (const digit_t     *EC_point,
         const CRYPTO_LONG    jbucket,
         ecexpon_data_t  *pecxdat,
         const CRYPTO_INT        negate_flag,
         struct bigctx_t *f_pBigCtx)
{
    const ecurve_t *ecurve = pecxdat->ecurve;
    digit_t *bucket_point = pecxdat->bucket[jbucket].contents;
    CRYPTO_DWORD recursion_level;    
    CRYPTO_BOOL OK;   

#if BIGNUM_EXPENSIVE_DEBUGGING
    CRYPTO_DWORD elng    = ecurve->fdesc->elng;
    digit_t  *ectemps = pecxdat->p5temps + ECPROJ5_TEMPS_COUNT*elng;

    pecxdat->OK = pecxdat->OK && ecaffine_on_curve(EC_point, ecurve,
                                 "affine_insert_bucket input",
                                 ectemps, f_pBigCtx);
#endif

    OK = pecxdat->OK;
    if (OK) {
        recursion_level = pecxdat->aib_recursion_level;
        pecxdat->aib_recursion_level++;
        if (recursion_level != 0 && recursion_level != 1) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "affine_insert_bucket recursion");
            TRACE( ( "affine_insert_bucket, recursion_level = %ld\n",
                   ( CRYPTO_LONG )recursion_level ) );
        } else if (ecaffine_is_infinite(EC_point, ecurve, f_pBigCtx)) {
                      // do nothing
        } else if (ecaffine_is_infinite(bucket_point, ecurve, f_pBigCtx)) {
                      // Vacant destination bucket
            OK = OK && ecaffine_multiply_pm1(EC_point, bucket_point,
                                             negate_flag, ecurve, f_pBigCtx);
        } else {
            digit_t *p1 = pecxdat->aib_temps[recursion_level];
            memcpy( p1,bucket_point,( 2*  ecurve->fdesc->elng)*sizeof( digit_t ));
            OK = OK && ecaffine_set_infinite(bucket_point,
                                             ecurve, f_pBigCtx);
            schedule_affine_add_or_sub(p1, EC_point,
                                   jbucket, pecxdat, negate_flag, f_pBigCtx);
        }
        pecxdat->OK = pecxdat->OK && OK;
        pecxdat->aib_recursion_level--;
    } // if OK
} // end affine_insert_bucket
// ---------------------------------------------------------------------
static CRYPTO_VOID ec_concurrent_operations(
    ecexpon_data_t  *pecxdat, 
    struct bigctx_t *f_pBigCtx )
{
    const ecurve_t     *ecurve  = pecxdat->ecurve;
    const field_desc_t *fdesc   = ecurve->fdesc;
    const CRYPTO_DWORD      elng    = fdesc->elng;
    const CRYPTO_BOOL     char2   = pecxdat->char2;  // Characteristic 2?
    const CRYPTO_DWORD num_slot_old = pecxdat->slots_in_use;
    digit_t *ectemps = pecxdat->p5temps + ECPROJ5_TEMPS_COUNT*elng;
    digit_t *ftemps = ectemps;

    CRYPTO_DWORD inversion_count, jslot;

    if (!pecxdat->OK) return;
    inversion_count = 0;

    for (jslot = 0; jslot != num_slot_old; jslot++) {
        CRYPTO_DWORD denom_index = inversion_count;
        digit_t* den = pecxdat->denoms + denom_index*elng;
        concurrent_op_t operation
                        = pecxdat->pending_op[jslot].operation;
        const digit_t *x1 = pecxdat->pending_op[jslot].inputs;
        const digit_t *y1 = x1 + elng;
        const digit_t *x2 = y1 + elng;
        const digit_t *y2 = x2 + elng;
        const digit_t *z1 = x2;       // Affinization input

        CRYPTO_BOOL OK = pecxdat->OK;

        switch (operation) {
        case ECOP_ADD:
            OK = OK && Ksub(x1, x2, den, fdesc, f_pBigCtx);
            if (Kiszero(den, fdesc, f_pBigCtx)) {
                if (Kequal(y1, y2, fdesc, f_pBigCtx)) {
                    operation = ECOP_DOUBLING;
                    if (char2) {
                        memcpy( den,x1, fdesc->elng*sizeof( digit_t ));
                    } else {
                        OK = OK && Kadd(y1, y1, den, fdesc, f_pBigCtx);
                    }
                } else {
                    // Points are negatives -- Keep den = 0
                }
            }
            break;
        case ECOP_AFFINIZATION:
            memcpy( den,z1, fdesc->elng*sizeof( digit_t ));
            break;
        case ECOP_DO_NOTHING:
        case ECOP_DOUBLING:
        default:
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "ec_concurrent_operations");
            TRACE( ( "ec_concurrent_operations -- bad operation = %ld\n", 
                   ( CRYPTO_LONG )operation ) );
            break;
        }
              // A denominator of 0 means the result is
              // the point at infinity, which need not be
              // put in the destination bucket.

              // Sometimes we compute P1 + P2 and P1 + (-P2)
              // back-to-back, where P1 and P2 are points on
              // our elliptic curve.  Both computations require
              // us to invert x(P1) - x(P2).  Test for a duplicate
              // denominator to avoid doing this twice.

        if (Kiszero(den, fdesc, f_pBigCtx)) {
            operation = ECOP_DO_NOTHING;
            denom_index = 0;
        } else if (   denom_index != 0
                   && Kequal(den - elng, den, fdesc, f_pBigCtx)) {
            denom_index--;  // Duplicate denominator
        } else {
            inversion_count++;  /* inversion_count = denom_index + 1 */
        }
        pecxdat->OK = pecxdat->OK && OK;
        pecxdat->pending_op[jslot].denom_index = denom_index;
        pecxdat->pending_op[jslot].operation = operation;
    } // for jslot



         // Invert denominators.

    pecxdat->OK = pecxdat->OK && Kinvert_many(pecxdat->denoms,
                                              pecxdat->denom_inverses,
                                              inversion_count, fdesc,
                                              ftemps, f_pBigCtx);

          // Finish the requested operations

    pecxdat->slots_in_use = 0;

    for (jslot = 0;
         jslot != num_slot_old && pecxdat->OK;
         jslot++) {
        const concurrent_op_t operation
                  = pecxdat->pending_op[jslot].operation;
        const CRYPTO_DWORD denom_index
                  = pecxdat->pending_op[jslot].denom_index;
        const CRYPTO_LONG ibucket
                  = pecxdat->pending_op[jslot].output_bucket;
        CRYPTO_INT negate_flag = pecxdat->pending_op[jslot].negate_flag;
        const digit_t *inverse = pecxdat->denom_inverses + denom_index*elng;
        const digit_t *x1 = pecxdat->pending_op[jslot].inputs;
        const digit_t *y1 = x1 + elng;
        const digit_t *x2 = y1 + elng;
        const digit_t *y2 = x2 + elng;
//        const digit_t *z1 = x2;       // Affinization input

        digit_t *new_point = pecxdat->concurrent_ops_temps;     // Length 2*elng
        digit_t *ftemp1 = pecxdat->concurrent_ops_temps + 2*elng; // Length elng
        digit_t *ftemp2 = pecxdat->concurrent_ops_temps + 3*elng; // Length elng
        CRYPTO_BOOL OK = pecxdat->OK;

           // The calls to affine_insert_bucket below may
           // cause table slots to be used.
           // However, each such call needs at most one slot.
           // New requests are stored at the start of the array.

        if (OK && jslot < pecxdat->slots_in_use) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "ec_concurrent_operations slot");
            TRACE( ( "ec_concurrent_operations -- slots re-allocated too fast.\n" ) );
        }
        switch(operation) {
        case ECOP_DO_NOTHING:
            break;
        case ECOP_AFFINIZATION:
                   // Multiply x by inverse^2 and y by inverse^3

            OK = OK && Kmul(inverse, inverse, ftemp1, fdesc,
                            ftemps, f_pBigCtx);
                                                // inverse^2
            OK = OK && Kmul(x1, ftemp1, new_point, fdesc,
                            ftemps, f_pBigCtx);
            OK = OK && Kmul(inverse, ftemp1, ftemp1, fdesc,
                            ftemps, f_pBigCtx);
                                                // inverse^3
            OK = OK && Kmul(y1, ftemp1, new_point + elng, fdesc,
                            ftemps, f_pBigCtx);
            break;
        case ECOP_ADD:
        case ECOP_DOUBLING:
            if (operation == ECOP_ADD) {  // m = (y1 - y2)/(x1 - x2)
                OK = OK && Ksub(y1, y2, ftemp1, fdesc, f_pBigCtx);
                OK = OK && Kmul(inverse, ftemp1, ftemp1, fdesc,
                                ftemps, f_pBigCtx);
            } else if (char2) {          // m = x1 + y1/x1
                OK = OK && Kmul(y1, inverse, ftemp1, fdesc,
                                ftemps, f_pBigCtx);
                OK = OK && Kadd(x1, ftemp1, ftemp1, fdesc, f_pBigCtx);
            } else {          // m = (3*x1^2 + a)/(2*y1)
                OK = OK && Kmul(x1, x1, ftemp1, fdesc,
                                ftemps, f_pBigCtx);
                OK = OK && Kadd(ftemp1, ftemp1, ftemp2, fdesc, f_pBigCtx);
                OK = OK && Kadd(ftemp1, ftemp2, ftemp1, fdesc, f_pBigCtx);
                OK = OK && Kadd(ftemp1, ecurve->a, ftemp1,
                                                   fdesc, f_pBigCtx);
                OK = OK && Kmul(ftemp1, inverse, ftemp1,
                                fdesc, ftemps, f_pBigCtx);
            }
                  // Now ftemp1 = m (slope)

            OK = OK && Kmul(ftemp1, ftemp1, ftemp2, fdesc,
                            ftemps, f_pBigCtx);  // m^2

            if (char2) {          // xnew = m^2 + m - a - x1 - x2
                OK = OK && Kadd(ftemp1, ftemp2, ftemp2, fdesc, f_pBigCtx);
                OK = OK && Ksub(ftemp2, ecurve->a, ftemp2, fdesc, f_pBigCtx);
                OK = OK && Ksub(ftemp2, x1, ftemp2, fdesc, f_pBigCtx);
                OK = OK && Ksub(ftemp2, x2, new_point, fdesc, f_pBigCtx);
                     // TBD -- Write one EOR loop
            } else {              // xnew = m^2 - x1 - x2
                OK = OK && Ksub(ftemp2, x1, ftemp2, fdesc, f_pBigCtx);
                OK = OK && Ksub(ftemp2, x2, new_point, fdesc, f_pBigCtx);
            }
                            // ynew = y1 + m*(xnew - x1)
                            // (xnew, ynew) is the negative sum
                            // Therefore we alter negate_flag.

            OK = OK && Ksub(new_point, x1, ftemp2, fdesc, f_pBigCtx);
            OK = OK && Kmul(ftemp1, ftemp2, ftemp2,
                            fdesc, ftemps, f_pBigCtx);
            OK = OK && Kadd(y1, ftemp2, new_point + elng,
                            fdesc, f_pBigCtx);
            negate_flag = -negate_flag;
            break;
        default:
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "ec_concurrent_operations 2nd loop");
            TRACE( ( "ec_concurrent_operations -- bad operation = %ld in second loop\n",
                   ( CRYPTO_LONG )operation ) );
            break;

        } // end switch
        if (!OK) {
        } else if (ibucket <= 0 || ibucket > pecxdat->bucket_max) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "ec_concurrent_operations -- ibucket");
            TRACE( ( "ec_concurrent_operations -- bad ibucket = %ld\n", 
                   ( CRYPTO_LONG )ibucket ) );
        } else if (pecxdat->bucket[ibucket].anticipating <= 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "ec_concurrent_operations, anticipating <= 0");
            TRACE( ( "ec_concurrent_operations -- anticipating = %ld not positive\n",
                   ( CRYPTO_LONG )pecxdat->bucket[ibucket].anticipating ) );
        } else {
            pecxdat->bucket[ibucket].anticipating--;
        }
        if (OK && operation != ECOP_DO_NOTHING) {
#if BIGNUM_EXPENSIVE_DEBUGGING
            OK = OK && ecaffine_on_curve(new_point, ecurve,
                      "ec_concurrent operations new point",
                       ectemps, f_pBigCtx);
            if (!OK) {
                TRACE( ( "New point not on curve, operation = %ld, char2 = %ld\n",
                       ( CRYPTO_LONG )operation, ( CRYPTO_LONG )char2 ) );
            }
#endif
            affine_insert_bucket(new_point, ibucket,
                                 pecxdat, negate_flag, f_pBigCtx);
        }
        pecxdat->OK = pecxdat->OK && OK;
    } // for jslot
} // end ec_concurrent_operations
/* --------------------------------------------------------- */
static CRYPTO_DWORD get_ec_concurrent_slot
       (ecexpon_data_t *pecxdat,
       struct bigctx_t *f_pBigCtx)
{
    while (pecxdat->OK && pecxdat->slots_in_use == MAX_SLOT) {
        ec_concurrent_operations(pecxdat, f_pBigCtx);
    }
    if (pecxdat->OK) {
        const CRYPTO_DWORD islot = pecxdat->slots_in_use;
        pecxdat->slots_in_use++;
        return islot;
    } else {
        return 0;
    }
} // end get_ec_concurrent_slot
// --------------------------------------------------------------------
static CRYPTO_VOID proj5_insert_bucket
        (const digit_t      *EC_point,
         const CRYPTO_LONG    ibucket,
         ecexpon_data_t  *pecxdat,
         const CRYPTO_INT        negate_flag,
         struct bigctx_t *f_pBigCtx)
/*
    Insert the projective point EC_point into
    bucket jbucket, possibly negating it.
*/
{
    const ecurve_t     *ecurve = pecxdat->ecurve;
    const field_desc_t *fdesc  = ecurve->fdesc;
    const CRYPTO_DWORD      elng   = fdesc->elng;
    const digit_t      *pz     = EC_point + 2*elng;

    if (!pecxdat->OK) {
        // Return if earlier error
    } else if (Kiszero(pz, fdesc, f_pBigCtx)) {
                                       // Check for point at infinity
    } else if (ibucket == BUCKET_ZERO || ibucket > BUCKET_NUMBER_MAX) {
                                       // Skip bucket zero
    } else if (Kequal(pz, fdesc->one, fdesc, f_pBigCtx)) {
        affine_insert_bucket(EC_point, ibucket, pecxdat,
                             negate_flag, f_pBigCtx);
                          // N.B. -- x and y occur first in EC_point,
                          // so we can treat it as affine if z == 1.
    } else {
        const CRYPTO_DWORD islot = get_ec_concurrent_slot(pecxdat, f_pBigCtx);

        pecxdat->pending_op[islot].operation = ECOP_AFFINIZATION;
        pecxdat->pending_op[islot].output_bucket = ibucket;
        pecxdat->pending_op[islot].negate_flag = negate_flag;
        pecxdat->bucket[ibucket].anticipating++;
        memcpy( pecxdat->pending_op[islot].inputs,EC_point,( 3* fdesc->elng)*sizeof( digit_t ));
                              // Copy x, y, z
    }
} // end proj5_insert_bucket
// ----------------------------------------------------------------------
static CRYPTO_BOOL ecaffine_to_ecproj5
        (const digit_t      *p1,
         digit_t        *p2,
         const ecurve_t        *ecurve,
         digit_t *   p5temps,
         struct bigctx_t *f_pBigCtx)
// Convert p1 from affine to projective form, result in p2.
{
    const field_desc_t *fdesc  = ecurve->fdesc;
    const CRYPTO_DWORD      elng   = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;

    if (ecaffine_is_infinite(p1, ecurve, f_pBigCtx)) {
        OK = OK && ecproj5_set_infinite(p2, ecurve, p5temps);
    } else {
        memcpy( p2,p1,( 2* fdesc->elng)*sizeof( digit_t ));
        memcpy( p2 + 2*elng,fdesc->one, fdesc->elng*sizeof( digit_t ));
        OK = OK && Kclear_many(p2 + 3*elng, 2, fdesc, f_pBigCtx);
    }
    return OK;
} // end ecaffine_to_ecproj5
// ----------------------------------------------------------------------
static CRYPTO_BOOL ecproj5_add_ecaffine
        (const digit_t      *p1,
         const digit_t     *p2,
         digit_t       *psum,
         const ecurve_t       *ecurve,
         digit_t *  p5temps,
         struct bigctx_t *f_pBigCtx)
// Add p1 + p2, where p1 is projective and p2 is affine.
// Result (psum) is projective.
{
    const CRYPTO_DWORD elng  = ecurve->fdesc->elng;
    CRYPTO_BOOL OK = TRUE;
// CAUTION -- temp_point must not overlap storage in
//            procedures called by this function.
    digit_t *temp_point = p5temps + 5*elng;
#if ECPROJ5_TEMPS_COUNT < 10
#error "ecproj5_add_ecaffine -- Temps array too short"
#endif

    OK = OK && ecaffine_to_ecproj5(p2, temp_point, ecurve,
                                   p5temps, f_pBigCtx)
            && ecproj5_add_ecproj5(p1, temp_point, psum, ecurve,
                                   p5temps, f_pBigCtx);
    return OK;
} // end ecproj5_add_ecaffine
//-----------------------------------------------------------------------

static CRYPTO_BOOL ecproj5_add_ecproj5
        (const digit_t      *p1,
         const digit_t      *p2,
         digit_t       *psum,
         const ecurve_t       *ecurve,
         digit_t *  p5temps,
         struct bigctx_t *f_pBigCtx)
//  Add p1 + p2 where both are projective.  Sum goes into psum.
//  psum may overlap p1 or p2
{
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD       elng  = fdesc->elng;
    const digit_t *p1x = p1;
    const digit_t *p1y = p1 +   elng;
    const digit_t *p1z = p1 + 2*elng;
    const digit_t *p2x = p2;
    const digit_t *p2y = p2 +   elng;
    const digit_t *p2z = p2 + 2*elng;
    const CRYPTO_BOOL char2 = CHARACTERISTIC_2(fdesc);

    CRYPTO_BOOL OK = TRUE;
    digit_t  *p3x = psum;
    digit_t  *p3y = psum + elng;
    digit_t  *p3z = psum + 2*elng;
    digit_t  *ftemps = p5temps + elng*ECPROJ5_TEMPS_COUNT;

    if (Kiszero(p1z, fdesc, f_pBigCtx)) {
        memcpy( psum,p2,5* ecurve->fdesc->elng*sizeof( digit_t ));
    } else if (Kiszero(p2z, fdesc, f_pBigCtx)) {
        memcpy( psum,p1,5* ecurve->fdesc->elng*sizeof( digit_t ));
    } else {
        const CRYPTO_BOOL p1affine = Kequal(p1z, fdesc->one, fdesc, f_pBigCtx);
        const CRYPTO_BOOL p2affine = Kequal(p2z, fdesc->one, fdesc, f_pBigCtx);

        digit_t *T0 = p5temps;
        digit_t *U1 = p5temps + elng;
        digit_t *U2 = p5temps + 2*elng;
        digit_t *S1 = p5temps + 3*elng;
        digit_t *S2 = p5temps + 4*elng;
    // CAUTION -- ecproj5_add_ecaffine assumes nothing
    // beyond here is modified until x, y inputs have been read.
        digit_t *W  = p5temps + 5*elng;
        digit_t *R  = p5temps + 6*elng;

        if (p1affine) {
            memcpy( U2,p2x, fdesc->elng*sizeof( digit_t ));
            memcpy( S2,p2y, fdesc->elng*sizeof( digit_t ));
        } else {
            OK = OK && Kmul(p1z, p1z, T0, fdesc,
                            ftemps, f_pBigCtx)  // z1^2
                    && Kmul(p2x, T0, U2, fdesc,
                            ftemps, f_pBigCtx)   // U2 = x2 * z1^2
                    && Kmul(p1z, T0, T0, fdesc,
                            ftemps, f_pBigCtx)   // z1^3
                    && Kmul(p2y, T0, S2, fdesc,
                            ftemps, f_pBigCtx);  // S2 = y2 * z1^3
        }
        if (p2affine) {
            memcpy( U1,p1x, fdesc->elng*sizeof( digit_t ));
            memcpy( S1,p1y, fdesc->elng*sizeof( digit_t ));
        } else {
            OK = OK && Kmul(p2z, p2z, T0, fdesc,
                            ftemps, f_pBigCtx)   // z2^2
                    && Kmul(p1x, T0, U1, fdesc,
                            ftemps, f_pBigCtx)    // U1 = x1 * z2^2
                    && Kmul(p2z, T0, T0, fdesc,
                            ftemps, f_pBigCtx)    // z2^3
                    && Kmul(p1y, T0, S1, fdesc,
                            ftemps, f_pBigCtx);   // S1 = t1 * z2^2
        }
        if (Kequal(U1, U2, fdesc, f_pBigCtx)) {
            if (Kequal(S1, S2, fdesc, f_pBigCtx)) {
                OK = OK && ecproj5_doubling(p2, psum, ecurve,
                                            p5temps, f_pBigCtx);
            } else {
                OK = OK && ecproj5_set_infinite(psum, ecurve,
                                                p5temps);
            }
        } else {
            OK = OK && Ksub(U1, U2, W, fdesc, f_pBigCtx)
                    && Ksub(S1, S2, R, fdesc, f_pBigCtx);

            if (p1affine) {
                memcpy( T0,W, fdesc->elng*sizeof( digit_t ));
            } else {
                OK = OK && Kmul(W, p1z, T0, fdesc, ftemps, f_pBigCtx);
            }
            if (p2affine) {
                memcpy( p3z,T0, fdesc->elng*sizeof( digit_t ));
            } else {
                OK = OK && Kmul(T0, p2z, p3z, fdesc, ftemps, f_pBigCtx);
            }                              // z3 = W * z1 * z2 (nonzero)

            OK = OK && Kmul(W, W, T0, fdesc, ftemps, f_pBigCtx)
                && Kmul(U1, T0, U1, fdesc,
                        ftemps, f_pBigCtx)    // T1 = U1 * W^2
                    && Kmul(U2, T0, U2, fdesc,
                        ftemps, f_pBigCtx);   // T2 = U2 * W^2
            OK = OK && Kadd(U1, U2, T0, fdesc,
                        f_pBigCtx);   // T1 + T2 */

            if (char2) {
                OK = OK && Kmul(S2, U1, U1, fdesc,
                                ftemps, f_pBigCtx)     // S2 * T1
                        && Kmul(S1, U2, U2, fdesc,
                                ftemps, f_pBigCtx)     // S1 * T2
                        && Ksub(U2, U1, U2,
                                fdesc, f_pBigCtx)          // S1*T2 - S2*T1
                        && Kmul(p3z, p3z, p3x, fdesc,
                                ftemps, f_pBigCtx)  // z3^2
                        && Kmul(ecurve->a, p3x, p3x, fdesc,
                                ftemps, f_pBigCtx) // a*z3^2
                        && Kadd(p3x, T0, p3x, fdesc,
                                f_pBigCtx)  // a*z3^2 + T1 + T2
                        && Kadd(R, p3z, T0, fdesc,
                                f_pBigCtx)    // T3 = R + z3
                        && Kmul(T0, R, R, fdesc, ftemps,
                                f_pBigCtx)      // R*(R + z3)
                        && Ksub(R, p3x, p3x, fdesc,
                                f_pBigCtx)   // New x3
                        && Kmul(T0, p3x, T0, fdesc, ftemps,
                                f_pBigCtx)   // T3*x3
                        && Ksub(U2, T0, p3y, fdesc,
                                f_pBigCtx);  // New y3
            } else {
                OK = OK && Kmul(R, R, p3x, fdesc,
                                ftemps, f_pBigCtx)
                        && Ksub(p3x, T0, p3x, fdesc,
                                f_pBigCtx)          // x3 = R^2 - T1 - T2
                        && Ksub(U2, U1, U2, fdesc,
                                f_pBigCtx)            // T2 - T1
                        && Kmul(S1, U2, U2, fdesc,
                                ftemps, f_pBigCtx)    // S1*(T2 - t1)
                        && Ksub(p3x, U1, T0, fdesc,
                                f_pBigCtx)           // x3 - T1
                        && Kmul(T0, R, T0, fdesc,
                                ftemps, f_pBigCtx)     // R*(x3 - T1)
                        && Ksub(U2, T0, p3y, fdesc,
                                f_pBigCtx);          // New y3
            } // end if characteristic
        } // end if doubling or infinite sum
    } // end if input point infinite
    return OK;
} // end ecproj5_add_ecproj5
// ----------------------------------------------------------------------
static CRYPTO_BOOL ecproj5_doubling
        (const digit_t       *p1,
         digit_t        *p2,
         const ecurve_t        *ecurve,
         digit_t *   p5temps,
         struct bigctx_t *f_pBigCtx)
// Double a projective point on an elliptic curve.
// Return p2 = 2*p1.  p1 and p2 may overlap
{
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD      elng  = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;
    const digit_t *p1x = p1;
    const digit_t *p1y = p1 +   elng;
    const digit_t *p1z = p1 + 2*elng;
    digit_t  *p2x = p2;
    digit_t  *p2y = p2 +   elng;
    digit_t  *p2z = p2 + 2*elng;
    digit_t  *ftemps = p5temps + ECPROJ5_TEMPS_COUNT*elng;

    if (!OK) {
    } else if (Kiszero(p1z, fdesc, f_pBigCtx)) {
        OK = OK && ecproj5_set_infinite(p2, ecurve,
                                        p5temps);
    } else if (CHARACTERISTIC_2(fdesc)) {
        digit_t *xsq   = p5temps;
        digit_t *zsq   = p5temps + elng;
        digit_t *bz8   = p5temps + 2*elng;
        digit_t *temp1 = p5temps + 3*elng;

        OK = OK && Kmul(p1x, p1x, xsq, fdesc,
                        ftemps, f_pBigCtx)
                && Kmul(p1z, p1z, zsq, fdesc,
                        ftemps, f_pBigCtx)
                && Kmul(zsq, zsq, bz8, fdesc,
                        ftemps, f_pBigCtx)        // Z^4
                && Kmul(bz8, bz8, bz8, fdesc,
                        ftemps, f_pBigCtx)        // Z^8
                && Kmul(ecurve->b, bz8, bz8, fdesc,
                        ftemps, f_pBigCtx)  // b * Z^8
                && Kmul(p1y, p1z, temp1, fdesc,
                        ftemps, f_pBigCtx)
                && Kadd(xsq, temp1, temp1, fdesc,
                        f_pBigCtx)            // X^2 + Y*Z
                && Kmul(p1x, zsq, p2z, fdesc, ftemps,
                        f_pBigCtx)        // Z2 = X * Z^2
                && Kmul(xsq, xsq, xsq, fdesc, ftemps,
                        f_pBigCtx)        // X^4
                && Kadd(xsq, bz8, p2x, fdesc,
                        f_pBigCtx)                // X2 = X^4 + bZ^8
                && Kmul(bz8, p2z, bz8, fdesc, ftemps,
                        f_pBigCtx)        // (b Z^8) * Z2
                && Kmul(temp1, p2x, temp1, fdesc, ftemps,
                        f_pBigCtx)    // (X^2 + Y*Z)*X2
                && Kadd(bz8, temp1, p2y, fdesc,
                        f_pBigCtx);             // Y2
    } else {
        // TBD -- check fourth and fifth components of vector
        digit_t *xsq  = p5temps;
        digit_t *S    = p5temps + elng;
        digit_t *az4  = p5temps + 2*elng;
        digit_t *M    = p5temps + 3*elng;
        digit_t *y4th = p5temps + 4*elng;

        OK = OK && Kmul(p1x, p1x, xsq, fdesc,
                        ftemps, f_pBigCtx)     //
                && Kadd(xsq, xsq, M, fdesc,
                        f_pBigCtx)
                && Kadd(xsq, M, M, fdesc,
                        f_pBigCtx)                 // 3 * X^2
                && Kmul(p1z, p1z, az4, fdesc,
                        ftemps, f_pBigCtx)     // Z^2
                && Kmul(az4, az4, az4, fdesc,
                        ftemps, f_pBigCtx)     // Z^4
                && Kmul(ecurve->a, az4, az4, fdesc,
                        ftemps, f_pBigCtx)  // a*Z^4
                && Kadd(M, az4, M, fdesc,
                        f_pBigCtx)                 // M = 3 X^2 + a Z^4
                && Kadd(p1y, p1y, y4th, fdesc,
                        f_pBigCtx)            // 2*Y
                && Kmul(y4th, p1z, p2z, fdesc,
                        ftemps, f_pBigCtx)    // Z2 = (2*Y)*Z
                && Kmul(y4th, y4th, y4th, fdesc,
                        ftemps, f_pBigCtx)  // 4 * Y^2
                && Kmul(p1x, y4th, S, fdesc,
                        ftemps, f_pBigCtx)      // S = 4 * X * Y^2
                && Kmul(y4th, y4th, y4th, fdesc,
                        ftemps, f_pBigCtx)  // 16*Y^4
                && Kmul(M, M, p2x, fdesc,
                        ftemps, f_pBigCtx)
                && Ksub(p2x, S, p2x, fdesc,
                        f_pBigCtx)
                && Ksub(p2x, S, p2x, fdesc,
                        f_pBigCtx)               // X2 = M^2 - 2*S
                && Ksub(S, p2x, S, fdesc,
                        f_pBigCtx)
                && Kmul(M, S, p2y, fdesc,
                        ftemps, f_pBigCtx)         // M*(S - X2)
                && Kmulpower2(y4th, -1, y4th, fdesc,
                       f_pBigCtx)      // 8*Y^4
                && Ksub(p2y, y4th, p2y, fdesc,
                        f_pBigCtx);       // Y2 = M*(S - X2) - 8 Y^4
    }
    return OK;
} // end ecproj5_doubling
// ----------------------------------------------------------------------
static CRYPTO_BOOL ecproj5_on_curve
        (const digit_t      *p1,
         const ecurve_t       *ecurve,
         const CRYPTO_CHAR      *pdebug_info,
         digit_t *  p5temps,
         struct bigctx_t *f_pBigCtx)
// Test whether a projective point is on the curve.
//
//       GF(2^m) -- Is Y^2 + XYZ == X^3 + a X^2 Z^2 + b Z^6 ?
//
//
//       GF(q) --   Is Y^2       == X^3 + a X Z^4   + b Z^6 ?



{
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD      elng  = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_BOOL char2 = CHARACTERISTIC_2(fdesc);
    const digit_t *p1x = p1;
    const digit_t *p1y = p1 +   elng;
    const digit_t *p1z = p1 + 2*elng;
// Avoid using over five temporaries.
// to prevent conflict with ecproj5_add_ecaffine.
    digit_t *temp1 = p5temps;
    digit_t *zsq   = p5temps + elng;
    digit_t *xsq   = p5temps + 2*elng;
    digit_t *ysq   = p5temps + 3*elng;
    digit_t *z4th  = p5temps + 4*elng;
    digit_t *ftemps = p5temps + ECPROJ5_TEMPS_COUNT*elng;

    if (    Kiszero(p1x, fdesc, f_pBigCtx)
         && Kiszero(p1y, fdesc, f_pBigCtx)
         && Kiszero(p1z, fdesc, f_pBigCtx)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_ZERO_OPERAND,
                        "ecproj5_on_curve");
    }
    OK = OK && Kmul(p1x, p1x, xsq, fdesc,
                    ftemps, f_pBigCtx)          // X^2
            && Kmul(p1y, p1y, ysq, fdesc,
                    ftemps, f_pBigCtx)
            && Kmul(p1z, p1z, zsq, fdesc,
                    ftemps, f_pBigCtx)
            && Kmul(zsq, zsq, z4th, fdesc,
                    ftemps, f_pBigCtx)         // Z^4
            && Kmul(p1x, xsq, temp1, fdesc,
                    ftemps, f_pBigCtx)
            && Ksub(ysq, temp1, ysq, fdesc,
                    f_pBigCtx)                // Y^2 - X^3
            && Kmul(ecurve->b, zsq, zsq, fdesc,
                    ftemps, f_pBigCtx);   // b*Z^2

    if (char2) {
        OK = OK && Kmul(zsq, z4th, temp1, fdesc,
                        ftemps, f_pBigCtx)   // b*Z^6
                && Ksub(ysq, temp1, ysq, fdesc,
                        f_pBigCtx)            // Y^2 - X^3 - bZ^6
                && Kmul(p1x, p1z, temp1, fdesc,
                        ftemps, f_pBigCtx)    // X*Z
                && Kmul(ecurve->a, temp1, zsq, fdesc,
                        ftemps, f_pBigCtx)
                && Ksub(zsq, p1y, zsq, fdesc,
                        f_pBigCtx)              // a*X*Z - Y
                && Kmul(temp1, zsq, temp1, fdesc,
                        ftemps, f_pBigCtx); // X*Z*(a*X*Z - Y)
    } else {
        OK = OK && Kmul(ecurve->a, p1x, temp1, fdesc,
                        ftemps, f_pBigCtx)
                && Kadd(temp1, zsq, temp1, fdesc,
                        f_pBigCtx)           // a*X + b*Z^2
                && Kmul(temp1, z4th, temp1, fdesc,
                        ftemps, f_pBigCtx); // (a*X + b*Z^2)*Z^4
    }
    if (OK) {
        const CRYPTO_BOOL on_curve = Kequal(ysq, temp1, fdesc, f_pBigCtx);
        if (!on_curve) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_NOT_ON_CURVE,
                            pdebug_info);
        }
    }
    return OK;
} // end ecproj5_on_curve
// --------------------------------------------------------------------
static CRYPTO_BOOL ecproj5_set_infinite
        (digit_t       *p1,
         const ecurve_t       *ecurve,
         digit_t *  p5temps)
{
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;

    memcpy( p1,fdesc->one, fdesc->elng*sizeof( digit_t ));
    memcpy( p1 + elng,fdesc->one, fdesc->elng*sizeof( digit_t ));
    Kclear_many(p1 + 2*elng, 3, fdesc, NULL);
    return OK;
} // end ecproj5_set_infinite
// ---------------------------------------------------------------------
static CRYPTO_BOOL ecproj5_to_ecaffine
        (const digit_t       *p1,
         digit_t       *p2,
         const ecurve_t        *ecurve,
         digit_t *   p5temps,
         struct bigctx_t *f_pBigCtx)
// Convert p1 from projective to affine form, result in p2.
// If p1 = (x, y, z, *, *), return (x/z^2, y/z^3)
{
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = ecurve->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    const digit_t *p1z = p1 + 2*elng;
    digit_t *ftemps = p5temps + elng*ECPROJ5_TEMPS_COUNT;

    if (Kiszero(p1z, fdesc, f_pBigCtx)) {
        OK = OK && ecaffine_set_infinite(p2, ecurve, f_pBigCtx);
    } else {
        digit_t *z1inv = p5temps;
        digit_t *z1invsq = p5temps + elng;

        OK = OK && Kinvert(p1z, z1inv, fdesc,
                        ftemps, f_pBigCtx)
                && Kmul(z1inv, z1inv, z1invsq, fdesc,
                        ftemps, f_pBigCtx)
                && Kmul(p1, z1invsq, p2, fdesc,
                        ftemps, f_pBigCtx)       // x2 = X/Z^2
                && Kmul(z1inv, z1invsq, z1invsq, fdesc,
                        ftemps, f_pBigCtx)  // 1/Z^3
                && Kmul(p1 + elng, z1invsq, p2 + elng, fdesc,
                        ftemps, f_pBigCtx);      // y2 = Y/Z^3
    }
    return OK;
} // end ecproj5_to_ecaffine
/* ------------------------------------------------------------ */
static CRYPTO_VOID schedule_affine_add_and_sub(
                      const digit_t     *point1,
                      const digit_t     *point2,
                      const CRYPTO_LONG    out1,
                      const CRYPTO_LONG    out2,
                      ecexpon_data_t  *pecxdat,
                      struct bigctx_t *f_pBigCtx)
/*
          Insert point1 + point2 in bucket out1
          and    point1 - point2 in bucket out2.

          We are replacing

                (out1 + out2)*point1 + (out1 - out2)*point2
          by
                out1*(point1 + point2) + out2*(point1 - point2)

          Buckets out1 +- out2 will acquire no more entries
          during this exponentiation.  We send the data elsewhere
          (i.e., to out1, out2) as we funnel everything into
          fewer buckets, until we do final processing.
*/
{
    const ecurve_t *ecurve = pecxdat->ecurve;
    if (!pecxdat->OK) {
    } else if (    out1 <= 0
                || out2 <= 0
                || out1 > pecxdat->bucket_max
                || out2 > pecxdat->bucket_max
                || pecxdat->bucket[out1].anticipating <= 0
                || pecxdat->bucket[out2].anticipating <= 0) {
        pecxdat->OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "schedule_affine_add_and_sub 1");
        TRACE( ( "schedule_affine_add_and_sub -- Bad output buckets = %ld %ld\n",
               ( CRYPTO_LONG )out1, ( CRYPTO_LONG )out2 ) );
    } else {
        pecxdat->bucket[out1].anticipating--;
                  // Cancel transfers to out1 and out2 scheduled
                  // when pending_addsubs table was created.
                  // They will be rescheduled below if still needed.
        pecxdat->bucket[out2].anticipating--;

        if (   !ecaffine_is_infinite(point1, ecurve, f_pBigCtx)
            && !ecaffine_is_infinite(point2, ecurve, f_pBigCtx) ) {
            if (pecxdat->slots_in_use > MAX_SLOT - 2) {
                ec_concurrent_operations(pecxdat, f_pBigCtx);
                   /* Try to schedule addition and subtraction
                      in adjacent slots, since both need
                      to invert x(point1) - x(point2).
                    */
            }
        }

        schedule_affine_add_or_sub(point1, point2, out1,
                                   pecxdat, +1, f_pBigCtx);
        schedule_affine_add_or_sub(point1, point2, out2,
                                   pecxdat, -1, f_pBigCtx);
    }
} // end schedule_affine_add_and_sub
/* ------------------------------------------------------------ */
static CRYPTO_VOID schedule_affine_add_or_sub(
    const digit_t    *point1,
    const digit_t    *point2,
    const CRYPTO_LONG    jbucket,
    ecexpon_data_t   *pecxdat,
    const CRYPTO_INT     negate_second,
    struct bigctx_t  *f_pBigCtx)
//
//       If negate_second = +1, add affine points point1 and point2.
//       If negate_second = -1, subtract point1 - point2.
//       In either case send the result to bucket jbucket.
//

{
    const ecurve_t *ecurve = pecxdat->ecurve;
    const CRYPTO_DWORD elng = ecurve->fdesc->elng;
    CRYPTO_BOOL OK = pecxdat->OK;
    
    if (!OK) {
    } else if (jbucket < 0 || jbucket > pecxdat->bucket_max) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "schedule_affine_add_or_sub");
        TRACE( ( "schedule_affine_add_or_sub, jbucket = %ld\n",
               ( CRYPTO_LONG )jbucket ) );
    }

    if (!OK) {
    }else if (ecaffine_is_infinite(point1, ecurve, f_pBigCtx)) {
        affine_insert_bucket(point2, jbucket, pecxdat,
                             negate_second, f_pBigCtx);
    } else if (ecaffine_is_infinite(point2, ecurve, f_pBigCtx)) {
        affine_insert_bucket(point1, jbucket, pecxdat,
                             +1, f_pBigCtx);
    } else {
        CRYPTO_DWORD islot = get_ec_concurrent_slot(pecxdat, f_pBigCtx);
        digit_t *inputs = pecxdat->pending_op[islot].inputs;
        memcpy( inputs,point1,( 2*  ecurve->fdesc->elng)*sizeof( digit_t ));
        OK = OK && ecaffine_multiply_pm1(point2,
            inputs + 2*elng, negate_second, ecurve, f_pBigCtx);
        pecxdat->pending_op[islot].negate_flag = +1;
        pecxdat->pending_op[islot].operation = ECOP_ADD;
        pecxdat->pending_op[islot].output_bucket = jbucket;
        pecxdat->bucket[jbucket].anticipating++;
    }
    pecxdat->OK = pecxdat->OK && OK;
} // end schedule_affine_add_or_sub
