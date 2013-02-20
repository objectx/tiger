/*
 * Tiger.h: Tiger hash.
 *
 * AUTHOR(S): objectx
 *
 * $Id$
 */

#ifndef tiger_h__a3fe984f_ce75_4ec5_8a17_6fc73fa1f64e
#define tiger_h__a3fe984f_ce75_4ec5_8a17_6fc73fa1f64e   1

#if defined (_MSC_VER) && (1200 <= _MSC_VER)
#pragma once
#endif

#include <sys/types.h>
#include <stdint.h>

namespace Tiger {
    const size_t DEFAULT_PASSES = 3 ;

    typedef uint64_t    sbox_t [4 * 256] ;

    /// <summary>
    /// Initializes sbox with default configuration.
    /// </summary>
    /// <param name="sbox">SBox to initialize</param>
    /// <returns><paramref name="sbox"/><returns>
    sbox_t &    InitializeSBox (sbox_t &sbox) ;

    /// <summary>
    /// Initializes SBox with specified seed.
    /// </summary>
    /// <param name="sbox">SBox to be initialized</param>
    /// <param name="seed">Seed for sbox initialization.</param>
    /// <param name="seed_size">The seed size</param>
    /// <returns><paramref name="sbox"/><returns>
    sbox_t &    InitializeSBox (sbox_t &sbox, const void *seed, size_t seed_size, size_t passes) ;

    class Generator ;

    /// <summary>
    /// The Tiger192 hash value.
    /// </summary>
    class Digest {
        friend class Generator ;
    private:
        uint8_t values_ [3 * 8] ;
    private:
        Digest (const uint64_t values [3]) ;
    public:
        /// <summary>
        /// The copy constructor.
        /// </summary>
        /// <param name="src">The source</param>
        Digest (const Digest &src) ;
        /// <summary>
        /// # of bytes in the hash value.
        /// </summary>
        /// <returns>The hash size</returns>
        size_t  Size () const {
            return sizeof (values_) ;
        }
        /// <summary>
        /// The accessor.
        /// </summary>
        /// <param name="index">Index</param>
        /// <returns><paramref name="index">'th value</returns>
        uint8_t At (size_t index) const {
            return values_ [index] ;
        }
        uint8_t operator [] (size_t index) const {
            return values_ [index] ;
        }
        const uint8_t * begin () const {
            return &values_ [0] ;
        }
        const uint8_t * end () {
            return begin () + sizeof (values_) ;
        }
        /// <summary>
        /// Computes hash value (for <see cref="std::unordered_set"/>, <see cref="std::unordered_map"/>, etc...)
        /// </summary>
        /// <returns>Computed hash value</returns>
        size_t  Hash () const ;
        Digest &        Assign (const Digest &src) ;
        Digest &        operator = (const Digest &src) {
            return Assign (src) ;
        }
    public:
        /// <summary>
        /// Equality test.
        /// </summary>
        /// <param name="a0"></param>
        /// <param name="a1"></param>
        /// <returns><c>true</c> if <paramref name="a0"/> == <paramref name="a1"/>.
        static bool     IsEqual (const Digest &a0, const Digest &a1) ;
        /// <summary>
        /// Non-Equality test.
        /// </summary>
        /// <param name="a0"></param>
        /// <param name="a1"></param>
        /// <returns><c>true</c> if <paramref name="a0"/> != <paramref name="a1"/>.
        static bool     IsNotEqual (const Digest &a0, const Digest &a1) ;
        /// <summary>
        /// Compares two digest values.
        /// </summary>
        /// <param name="a0">Value to compare</param>
        /// <param name="a1">Value to compare</param>
        /// <returns>Negative when <paramref name="a0"/> &lt; <paramref name="a1"/>, Positive when a0 &gt; a1 and Zero when a0 == a1.</returns>
        static int      Compare (const Digest &a0, const Digest &a1) ;
    } ;

    /// <summary>
    /// The Tiger192 generator.
    /// </summary>
    class Generator {
    private:
        enum Flags {
            BIT_FINALIZED = 0,
            BIT_TIGER2 = 1
        };
    private:
        const sbox_t &  sbox_ ;
        size_t  count_ ;
        size_t  cntPass_ ;
        uint32_t        flags_ ;
        uint64_t        hash_ [3] ;
        uint64_t        buffer_ [8] ;
    public:
        /// <summary>
        /// The constructor.
        /// </summary>
        /// <param name="sbox">The sbox</param>
        Generator (const sbox_t &sbox) ;
        /// <summary>
        /// The constructor with the explicit pass counts.
        /// </summary>
        /// <param name="sbox">The sbox</param>
        /// <param name="cntPass"># of iterations in the compression function.</param>
        Generator (const sbox_t &sbox, size_t cntPass) ;
        /// <summary>
        /// The constructor with the explicit pass counts.
        /// </summary>
        /// <param name="sbox">The sbox</param>
        /// <param name="cntPass"># of iterations in the compression function.</param>
        /// <param name="isTiger2">Use Tiger2 padding</param>
        Generator (const sbox_t &sbox, size_t cntPass, bool isTiger2) ;
        /// <summary>
        /// Resets the state.
        /// </summary>
        /// <returns>myself</returns>
        Generator &     Reset () ;
        /// <summary>
        /// Checks finalized or not.
        /// </summary>
        /// <returns><c>true</c> when finalized</returns>
        bool    IsFinalized () const ;
        /// <summary>
        /// Use Tiger2 padding or not.
        /// </summary>
        /// <returns><c>true</c> if Tiger2 padding was used.</returns>
        bool    IsTiger2 () const ;
        /// <summary>
        /// Updates states
        /// </summary>
        /// <param name="data">The input sequence</param>
        /// <param name="size"># of bytes in the input sequence</param>
        /// <returns>myself</returns>
        Generator &     Update (const void *data, size_t size) ;
        /// <summary>
        /// Updates states
        /// </summary>
        /// <param name="value">The input value</param>
        /// <returns>myself</returns>
        Generator &     Update (uint8_t value) ;
        /// <summary>
        /// Computes Tiger192 digest
        /// </summary>
        /// <returns>Computed digest</returns>
        /// <remarks>Once finalized, sucessive <see cref="Finalize()"/> returns the same.
        Digest  Finalize () ;
    } ;

} /* end of [namespace Tiger] */

inline bool     operator == (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::IsEqual (a0, a1) ;
}

inline bool     operator != (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::IsNotEqual (a0, a1) ;
}

inline bool     operator <  (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::Compare (a0, a1) < 0 ;
}

inline bool     operator <= (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::Compare (a0, a1) <= 0 ;
}

inline bool     operator >  (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::Compare (a0, a1) > 0 ;
}

inline bool     operator >= (const Tiger::Digest &a0, const Tiger::Digest &a1) {
    return Tiger::Digest::Compare (a0, a1) >= 0 ;
}

#endif  /* tiger_h__a3fe984f_ce75_4ec5_8a17_6fc73fa1f64e */

/*
 * $LastChangedRevision$
 * $LastChangedBy$
 * $HeadURL$
 */

