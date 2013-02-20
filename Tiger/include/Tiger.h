/*
 * Tiger.h: Tiger hash.
 *
 * AUTHOR(S): objectx
 *
 */
#ifndef	tiger_h__a324fb584c589244176c2e965aa5cc34
#define	tiger_h__a324fb584c589244176c2e965aa5cc34	1

#if defined (_MSC_VER) && (1200 <= _MSC_VER)
#pragma once
#endif

#include <sys/types.h>
#include <stdint.h>

namespace Tiger {
    const size_t DEFAULT_PASSES = 3 ;

    typedef uint64_t    sbox_t [4 * 256] ;

    /**
     * Initializes sbox with default configuration.
     *
     * @param sbox   SBox to be initialized
     *
     * @return SBOX
     */
    sbox_t &    InitializeSBox (sbox_t &sbox) ;

    /**
     * Initializes SBox with specified seed.
     *
     * @param sbox      SBox to be initialized
     * @param seed      Seed for sbox initialization
     * @param seed_size The seed size
     * @param passes    # of passes to apply
     *
     * @return SBOX
     */
    sbox_t &    InitializeSBox (sbox_t &sbox, const void *seed, size_t seed_size, size_t passes) ;

    class Generator ;

    /** The Tiger192 hash value.  */
    class Digest {
        friend class Generator ;
    private:
        uint8_t	values_ [3 * 8] ;
    private:
        Digest (const uint64_t values [3]) ;
    public:
	/**
	 * The copy constructor.
	 *
	 * @param src The source
	 */
	Digest (const Digest &src) ;
	/**
	 * # of bytes in the hash value.
	 *
	 * @return The hash size
	 */
	size_t  Size () const {
            return sizeof (values_) ;
        }
	/**
	 * The accessor.
	 *
	 * @param index Index
	 *
	 * @return INDEX'th value
	 */
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
	/**
	 * Computes hash value (for std::unordered_set, std::unordered_map, etc...).
	 *
	 * @return Computed hash value
	 * @see std::unordered_set
	 * @see std::unordered_map
	 */
	size_t  Hash () const ;
        Digest &        Assign (const Digest &src) ;
        Digest &        operator = (const Digest &src) {
            return Assign (src) ;
        }
    public:
	/**
	 * Equality test.
	 */
	static bool     IsEqual (const Digest &a0, const Digest &a1) ;

	/**
	 * Non-Equality test.
	 */
	static bool     IsNotEqual (const Digest &a0, const Digest &a1) ;
	/**
	 * Compares two digest values.
	 */
	static int      Compare (const Digest &a0, const Digest &a1) ;
    } ;

    /** The Tiger192 generator.  */
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
	/**
	 * The constructor.
	 *
	 * @param sbox The sbox
	 */
	Generator (const sbox_t &sbox) ;
	/**
	 * The constructor with the explicit pass counts.
	 *
	 * @param sbox    The sbox
	 * @param cntPass # of iterations in the compression function
	 */
	Generator (const sbox_t &sbox, size_t cntPass) ;
	/**
	 * The constructor with the explicit pass counts.
	 *
	 * @param sbox     The sbox
	 * @param cntPass  # of iterations in the compression function.
	 * @param isTiger2 Use Tiger2 padding
	 */
	Generator (const sbox_t &sbox, size_t cntPass, bool isTiger2) ;
	/** Resets the state.  */
	Generator &     Reset () ;
	/** Checks finalized or not.  */
	bool    IsFinalized () const ;
	/** Use Tiger2 padding or not.  */
	bool    IsTiger2 () const ;
	/**
	 * Updates states
	 *
	 * @param data   The input sequence
	 * @param size   # of bytes in the input sequence
	 *
	 * @return *this
	 */
	Generator &     Update (const void *data, size_t size) ;
	/**
	 * Updates states
	 *
	 * @param value  The input value
	 *
	 * @return *this
	 */
	Generator &     Update (uint8_t value) ;
	/**
	 * Computes Tiger192 digest
	 *
	 * @remarks Once finalized, successive Finalize() returns the same value.
	 * @return Computed digest
	 */
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

#endif  /* tiger_h__a324fb584c589244176c2e965aa5cc34 */
/*
 * [END OF FILE]
 */
