/*
 * Tiger.cpp:
 *
 * AUTHOR(S): objectx
 *
 */

#include "Tiger.hpp"
#include <assert.h>
#include <string.h>
#include <algorithm>

namespace {

    static const uint64_t       init_state_0 = 0x0123456789ABCDEFuLL ;
    static const uint64_t       init_state_1 = 0xFEDCBA9876543210uLL ;
    static const uint64_t       init_state_2 = 0xF096A5B4C3B2E187uLL ;
    static const uint64_t       schedule_0 = 0xA5A5A5A5A5A5A5A5uLL ;
    static const uint64_t       schedule_1 = 0x0123456789ABCDEFuLL ;

    bool    check_little_endian () {
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

    static const bool   TARGET_LITTLE_ENDIAN = check_little_endian () ;

    inline uint64_t asUInt64 (const void *addr) {
        auto p = static_cast<const uint8_t *> (addr) ;
        return ( (static_cast<uint64_t> (p [0]) <<  0)
               | (static_cast<uint64_t> (p [1]) <<  8)
               | (static_cast<uint64_t> (p [2]) << 16)
               | (static_cast<uint64_t> (p [3]) << 24)
               | (static_cast<uint64_t> (p [4]) << 32)
               | (static_cast<uint64_t> (p [5]) << 40)
               | (static_cast<uint64_t> (p [6]) << 48)
               | (static_cast<uint64_t> (p [7]) << 56)) ;
    }

    std::array<uint64_t, 8> make_work (const void *seed, size_t size) {
        std::array<uint8_t, 64> tmp ;

        tmp.fill (0) ;
        ::memcpy (tmp.data (), seed, std::min (size, tmp.size ())) ;

        return std::array<uint64_t, 8> {{ asUInt64 (&tmp [8 * 0])
                                        , asUInt64 (&tmp [8 * 1])
                                        , asUInt64 (&tmp [8 * 2])
                                        , asUInt64 (&tmp [8 * 3])
                                        , asUInt64 (&tmp [8 * 4])
                                        , asUInt64 (&tmp [8 * 5])
                                        , asUInt64 (&tmp [8 * 6])
                                        , asUInt64 (&tmp [8 * 7]) }} ;
    }

    static void ToByte (uint8_t *result, uint64_t value) {
        if (TARGET_LITTLE_ENDIAN) {
            memcpy (result, &value, sizeof (value)) ;
        }
        else {
            const uint8_t *     p = reinterpret_cast<const uint8_t *> (&value) ;
            for (size_t i = 0 ; i < sizeof (value) ; ++i) {
                result [i ^ 0x07] = p [i] ;
            }
        }
    }

}

namespace Tiger {

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

    static void   Compress ( state_t &          state
                           , const msgblock_t & input
                           , const sbox_t &     sbox
                           , size_t             passes) {
        uint_fast64_t   a = state [0] ;
        uint_fast64_t   b = state [1] ;
        uint_fast64_t   c = state [2] ;

        uint_fast64_t   x0 = input [0] ;
        uint_fast64_t   x1 = input [1] ;
        uint_fast64_t   x2 = input [2] ;
        uint_fast64_t   x3 = input [3] ;
        uint_fast64_t   x4 = input [4] ;
        uint_fast64_t   x5 = input [5] ;
        uint_fast64_t   x6 = input [6] ;
        uint_fast64_t   x7 = input [7] ;

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
        return InitializeSBox ( sbox
                              , "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham"
                              , 64
                              , 5) ;
    }

    sbox_t &    InitializeSBox (sbox_t &sbox, const void *seed, size_t seed_size, size_t passes) {
        state_t state {{ init_state_0
                       , init_state_1
                       , init_state_2 }};

        for (size_t i = 0 ; i < sbox.size () ; ++i) {
            auto make_fillval = [](size_t idx) -> uint64_t {
                auto v = static_cast<uint64_t> (idx & 0xFFu) ;
                return ( (v <<  0)
                       | (v <<  8)
                       | (v << 16)
                       | (v << 24)
                       | (v << 32)
                       | (v << 40)
                       | (v << 48)
                       | (v << 56)) ;
            } ;
            sbox [i] = make_fillval (i) ;
        }

        auto    work = make_work (seed, seed_size) ;

        int_fast32_t    abc = 2 ;
        auto    tp = reinterpret_cast<uint8_t *> (&sbox [0]) ;
        auto    sp = reinterpret_cast<uint8_t *> (&state [0]) ;
        for (size_t cnt = 0 ; cnt < passes ; ++cnt) {
            for (int_fast32_t i = 0 ; i < 256 ; ++i) {
                for (int_fast32_t sb = 0 ; sb < 1024 ; sb += 256) {
                    ++abc ;
                    if (abc == 3) {
                        abc = 0 ;
                        Compress (state, work, sbox, 3) ;
                    }
                    for (int_fast32_t col = 0 ; col < 8 ; ++col) {
                        std::swap ( tp [8 * (sb + i                 ) + col]
                                  , tp [8 * (sb + sp [8 * abc + col]) + col]) ;
                    }
                }
            }
        }
        return sbox ;
    }

    Generator::Generator (const sbox_t &sbox, size_t passes, bool isTiger2)
            : sbox_ { sbox }
            , cntPass_ { std::max (DEFAULT_PASSES, passes) }
            , hash_ {{ init_state_0, init_state_1, init_state_2 }} {
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

    Generator & Generator::Update (const void *data, size_t size) {
        auto    p = static_cast<const uint8_t *> (data) ;
        auto    q = reinterpret_cast<uint8_t *> (&buffer_ [0]) ;

        if (TARGET_LITTLE_ENDIAN) {
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
        if (TARGET_LITTLE_ENDIAN) {
            q [idx] = value ;
        }
        else {
            q [0x3F - idx] = value ;
        }
        ++count_ ;
        return *this ;
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

    Digest::Digest (const state_t &value) {
        ToByte (&values_ [ 0], value [0]) ;
        ToByte (&values_ [ 8], value [1]) ;
        ToByte (&values_ [16], value [2]) ;
    }

    Digest::Digest (const Digest &src) {
        ::memcpy (values_, src.values_, sizeof (values_)) ;
    }

    bool  Digest::IsEqual (const Digest &a0, const Digest &a1) {
        for (size_t i = 0 ; i < sizeof (a0.values_) ; ++i) {
            if (a0.values_ [i] != a1.values_ [i]) {
                return false ;
            }
        }
        return true ;
    }

    bool  Digest::IsNotEqual (const Digest &a0, const Digest &a1) {
        for (size_t i = 0 ; i < sizeof (a0.values_) ; ++i) {
            if (a0.values_ [i] != a1.values_ [i]) {
                return true ;
            }
        }
        return false ;
    }

    int Digest::Compare (const Digest &a0, const Digest &a1) {
        return ::memcmp (a0.values_, a1.values_, sizeof (a0.values_)) ;
    }

    Digest &    Digest::Assign (const Digest &src) {
        ::memcpy (values_, src.values_, sizeof (values_)) ;
        return *this ;
    }

    size_t Digest::Hash () const {
        auto v = ( asUInt64 (&values_ [ 0])
                 ^ asUInt64 (&values_ [ 8])
                 ^ asUInt64 (&values_ [16])) ;
        if (sizeof (size_t) == 4) {
            return static_cast<size_t> ((v >> 32) ^ v) ;
        }
        else if (sizeof (size_t) == 8) {
            return static_cast<size_t> (v) ;
        }
        else {
            assert (false) ;
            return 0 ;
        }
    }
} /* end of [namespace Tiger] */
/*
 * [END OF FILE]
 */
