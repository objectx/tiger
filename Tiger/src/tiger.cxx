/*
 * Tiger.cpp:
 *
 * AUTHOR(S): objectx
 *
 */

#include "Tiger.h"
#include <assert.h>
#include <string.h>

namespace Tiger {

    typedef uint64_t    state_t [3] ;

    static const uint64_t       init_state_0 = 0x0123456789ABCDEFuLL ;
    static const uint64_t       init_state_1 = 0xFEDCBA9876543210uLL ;
    static const uint64_t       init_state_2 = 0xF096A5B4C3B2E187uLL ;
    static const uint64_t       schedule_0 = 0xA5A5A5A5A5A5A5A5uLL ;
    static const uint64_t       schedule_1 = 0x0123456789ABCDEFuLL ;

    static bool check_little_endian () {
        unsigned int    val = 0x01020304 ;

        const uint8_t * p = reinterpret_cast<const uint8_t *> (&val) ;

        if (p [0] == 0x04 && p [1] == 0x03 && p [2] == 0x02 && p [3] == 0x01) {
            return true ;
        }
        else if (p [3] == 0x04 && p [2] == 0x03 && p [1] == 0x02 && p [0] == 0x01) {
            return false ;
        }
        assert (false) ;
        return false ;
    }

    static const bool   LITTLE_ENDIAN = check_little_endian () ;

    static void FillWork (uint64_t work [8], const void *seed, size_t size) {
        uint8_t tmp [64] ;
        ::memset (tmp, 0, sizeof (tmp)) ;
        if (size < sizeof (tmp)) {
            ::memcpy (tmp, seed, size) ;
        }
        else {
            ::memcpy (tmp, seed, sizeof (tmp)) ;
        }
        ::memset (work, 0, sizeof (work)) ;

        for (int i = 0 ; i < 8 ; ++i) {
            work [i] = ((static_cast<uint64_t> (tmp [8 * i + 0]) <<  0) |
                        (static_cast<uint64_t> (tmp [8 * i + 1]) <<  8) |
                        (static_cast<uint64_t> (tmp [8 * i + 2]) << 16) |
                        (static_cast<uint64_t> (tmp [8 * i + 3]) << 24) |
                        (static_cast<uint64_t> (tmp [8 * i + 4]) << 32) |
                        (static_cast<uint64_t> (tmp [8 * i + 5]) << 40) |
                        (static_cast<uint64_t> (tmp [8 * i + 6]) << 48) |
                        (static_cast<uint64_t> (tmp [8 * i + 7]) << 56)) ;
        }
    }

#define SCHEDULE do {                   \
        x0 -= x7 ^ schedule_0 ;         \
        x1 ^= x0 ;                      \
        x2 += x1 ;                      \
        x3 -= x2 ^ ((~x1) << 19) ;      \
        x4 ^= x3 ;                      \
        x5 += x4 ;                      \
        x6 -= x5 ^ ((~x4) >> 23) ;      \
        x7 ^= x6 ;                      \
        x0 += x7 ;                      \
        x1 -= x0 ^ ((~x7) << 19) ;      \
        x2 ^= x1 ;                      \
        x3 += x2 ;                      \
        x4 -= x3 ^ ((~x2) >> 23) ;      \
        x5 ^= x4 ;                      \
        x6 += x5 ;                      \
        x7 -= x6 ^ schedule_1 ;         \
    } while (false)

#define ROUND(A_, B_, C_, X_, MUL_) do {                                        \
        (C_) ^= (X_) ;                                                          \
        (A_) -= (sbox [0 * 256 + static_cast<uint8_t> ((C_) >> (0 * 8))] ^      \
                 sbox [1 * 256 + static_cast<uint8_t> ((C_) >> (2 * 8))] ^      \
                 sbox [2 * 256 + static_cast<uint8_t> ((C_) >> (4 * 8))] ^      \
                 sbox [3 * 256 + static_cast<uint8_t> ((C_) >> (6 * 8))]) ;     \
        (B_) += (sbox [3 * 256 + static_cast<uint8_t> ((C_) >> (1 * 8))] ^      \
                 sbox [2 * 256 + static_cast<uint8_t> ((C_) >> (3 * 8))] ^      \
                 sbox [1 * 256 + static_cast<uint8_t> ((C_) >> (5 * 8))] ^      \
                 sbox [0 * 256 + static_cast<uint8_t> ((C_) >> (7 * 8))]) ;     \
        (B_) *= static_cast<uint64_t> (MUL_) ;                                  \
    } while (false)

#define PASS(A_, B_, C_, MUL_)  do {            \
        ROUND ((A_), (B_), (C_), x0, (MUL_)) ;  \
        ROUND ((B_), (C_), (A_), x1, (MUL_)) ;  \
        ROUND ((C_), (A_), (B_), x2, (MUL_)) ;  \
        ROUND ((A_), (B_), (C_), x3, (MUL_)) ;  \
        ROUND ((B_), (C_), (A_), x4, (MUL_)) ;  \
        ROUND ((C_), (A_), (B_), x5, (MUL_)) ;  \
        ROUND ((A_), (B_), (C_), x6, (MUL_)) ;  \
        ROUND ((B_), (C_), (A_), x7, (MUL_)) ;  \
    } while (false)

    static void   Compress (uint64_t *state, const uint64_t *input, const sbox_t &sbox, size_t passes) {
        uint64_t        a = state [0] ;
        uint64_t        b = state [1] ;
        uint64_t        c = state [2] ;

        uint64_t        x0 = input [0] ;
        uint64_t        x1 = input [1] ;
        uint64_t        x2 = input [2] ;
        uint64_t        x3 = input [3] ;
        uint64_t        x4 = input [4] ;
        uint64_t        x5 = input [5] ;
        uint64_t        x6 = input [6] ;
        uint64_t        x7 = input [7] ;

        PASS (a, b, c, 5) ;
        SCHEDULE ;
        PASS (c, a, b, 7) ;
        SCHEDULE ;
        PASS (b, c, a, 9) ;

        for (size_t cnt = 3 ; cnt < passes ; ++cnt) {
            SCHEDULE ;
            PASS (a, b, c, 9) ;
            uint64_t    tmp = a ;
            a = c ;
            c = b ;
            b = tmp ;
        }
        state [0] = a ^ state [0] ;
        state [1] = b - state [1] ;
        state [2] = c + state [2] ;
    }

    sbox_t &    InitializeSBox (sbox_t &sbox) {
        return InitializeSBox (sbox, "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham", 64, 5) ;
    }

    sbox_t &    InitializeSBox (sbox_t &sbox, const void *seed, size_t seed_size, size_t passes) {
        state_t state ;

        state [0] = init_state_0 ;
        state [1] = init_state_1 ;
        state [2] = init_state_2 ;

        uint64_t        work [8] ;

        FillWork (work, seed, seed_size) ;

        for (int i = 0 ; i < (sizeof (sbox) / sizeof (sbox [0])) ; ++i) {
            uint8_t *   p = reinterpret_cast<uint8_t *> (&sbox [i]) ;

            uint8_t     fillval = static_cast<uint8_t> (i) ;

            for (int j = 0 ; j < 8 ; ++j) {
                p [j] = fillval ;
            }
        }

        int abc = 2 ;
        uint8_t *       tp = reinterpret_cast<uint8_t *> (&sbox [0]) ;
        uint8_t *       sp = reinterpret_cast<uint8_t *> (&state [0]) ;
        for (size_t cnt = 0 ; cnt < passes ; ++cnt) {
            for (int i = 0 ; i < 256 ; ++i) {
                for (int sb = 0 ; sb < 1024 ; sb += 256) {
                    ++abc ;
                    if (abc == 3) {
                        abc = 0 ;
                        Compress (state, work, sbox, 3) ;
                    }
                    for (int col = 0 ; col < 8 ; ++col) {
                        int     idx0 = 8 * (sb + i) + col ;
                        int     idx1 = 8 * (sb + sp [8 * abc + col]) + col ;
                        uint8_t  tmp = tp [idx0] ;
                        tp [idx0] = tp [idx1] ;
                        tp [idx1] = tmp ;
                    }
                }
            }
        }
        return sbox ;
    }

    Generator::Generator (const sbox_t &sbox) :
        sbox_ (sbox),
        count_ (0),
        cntPass_ (DEFAULT_PASSES),
        flags_ (0) {
        hash_ [0] = init_state_0 ;
        hash_ [1] = init_state_1 ;
        hash_ [2] = init_state_2 ;
    }

    Generator::Generator (const sbox_t &sbox, size_t passes) :
        sbox_ (sbox),
        count_ (0),
        cntPass_ (DEFAULT_PASSES),
        flags_ (0) {
        if (DEFAULT_PASSES < passes) {
            cntPass_ = passes ;
        }
        hash_ [0] = init_state_0 ;
        hash_ [1] = init_state_1 ;
        hash_ [2] = init_state_2 ;
    }

    Generator::Generator (const sbox_t &sbox, size_t passes, bool isTiger2) :
        sbox_ (sbox),
        count_ (0),
        cntPass_ (DEFAULT_PASSES),
        flags_(0) {
        if (DEFAULT_PASSES < passes) {
            cntPass_ = passes ;
        }
        hash_ [0] = init_state_0 ;
        hash_ [1] = init_state_1 ;
        hash_ [2] = init_state_2 ;
        if (isTiger2) {
            flags_ |= 1u << BIT_TIGER2 ;
        }
    }

    Generator & Generator::Reset () {
        count_ = 0 ;
        hash_ [0] = init_state_0 ;
        hash_ [1] = init_state_1 ;
        hash_ [2] = init_state_2 ;
        flags_ &= ~(1u << BIT_FINALIZED) ;
        return *this ;
    }

    bool        Generator::IsFinalized () const {
        return (flags_ & (1u << BIT_FINALIZED)) != 0 ;
    }

    bool        Generator::IsTiger2 () const {
        return (flags_ & (1u << BIT_TIGER2)) != 0 ;
    }

    Generator & Generator::Update (const void *data, size_t size) {
        const uint8_t * p = static_cast<const uint8_t *> (data) ;
        uint8_t *       q = reinterpret_cast<uint8_t *> (&buffer_ [0]) ;

        if (LITTLE_ENDIAN) {
            for (size_t i = 0 ; i < size ; ++i) {
                size_t  idx = count_ & 0x3F ;
                if (0 < count_ && idx == 0) {
                    Compress (hash_, buffer_, sbox_, cntPass_) ;
                }
                q [idx] = p [i] ;
                ++count_ ;
            }
        }
        else {
            for (size_t i = 0 ; i < size ; ++i) {
                size_t  idx = count_ & 0x3F ;
                if (0 < count_ && idx == 0) {
                    Compress (hash_, buffer_, sbox_, cntPass_) ;
                }
                q [0x3F - idx] = p [i] ;
                ++count_ ;
            }
        }
        return *this ;
    }

    Generator & Generator::Update (uint8_t value) {
        size_t  idx = count_ & 0x3F ;

        if (0 < count_ && idx == 0) {
            Compress (hash_, buffer_, sbox_, cntPass_) ;
        }

        uint8_t *       q = reinterpret_cast<uint8_t *> (&buffer_ [0]) ;
        if (LITTLE_ENDIAN) {
            q [idx] = value ;
        }
        else {
            q [0x3F - idx] = value ;
        }
        ++count_ ;
        return *this ;
    }

    static void ToByte (uint8_t *result, uint64_t value) {
        if (LITTLE_ENDIAN) {
            memcpy (result, &value, sizeof (value)) ;
        }
        else {
            const uint8_t *     p = reinterpret_cast<const uint8_t *> (&value) ;
            for (size_t i = 0 ; i < sizeof (value) ; ++i) {
                result [i ^ 0x07] = p [i] ;
            }
        }
    }

    Digest      Generator::Finalize () {
        if (! IsFinalized ()) {
            uint8_t     pad [64] ;

            ::memset (pad, 0, sizeof (pad)) ;

            uint64_t    bitcount = 8 * static_cast<uint64_t> (count_) ;

            if (IsTiger2 ()) {
                Update (0x80) ;
            }
            else {
                Update (0x01) ;
            }
            size_t      remain = 0x40 - (count_ & 0x3F) ;
            if (8 <= remain) {
                Update (pad, remain - 8) ;
            }
            else {
                // No rooms to store the bit-length.  Requires extra block.
                Update (pad, remain) ;
                Update (pad, sizeof (pad) - 8) ;
            }
            uint8_t     tmp [8] ;
            ToByte (tmp, bitcount) ;
            Update (tmp, sizeof (tmp)) ;
            assert ((count_ & 0x3F) == 0) ;
            Compress (hash_, buffer_, sbox_, cntPass_) ;
            flags_ |= (1u << BIT_FINALIZED) ;
        }
        return Digest (hash_) ;
    }

    Digest::Digest (const uint64_t values [3]) {
        ToByte (&values_ [ 0], values [0]) ;
        ToByte (&values_ [ 8], values [1]) ;
        ToByte (&values_ [16], values [2]) ;
    }

    Digest::Digest (const Digest &src) {
        ::memcpy (values_, src.values_, sizeof (values_)) ;
    }

    bool  Digest::IsEqual (const Digest &a0, const Digest &a1) {
        for (int i = 0 ; i < sizeof (a0.values_) ; ++i) {
            if (a0.values_ [i] != a1.values_ [i]) {
                return false ;
            }
        }
        return true ;
    }
    bool  Digest::IsNotEqual (const Digest &a0, const Digest &a1) {
        for (int i = 0 ; i < sizeof (a0.values_) ; ++i) {
            if (a0.values_ [i] != a1.values_ [i]) {
                return true ;
            }
        }
        return false ;
    }

    int Digest::Compare (const Digest &a0, const Digest &a1) {
        return ::memcmp (a0.values_, a1.values_, sizeof (a0.values_)) ;
    }

} /* end of [namespace Tiger] */
/*
 * [END OF FILE]
 */
