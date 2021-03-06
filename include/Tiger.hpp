/*
 * Copyright (c) 2019  Masashi Fujita.  All rights reserved.
 */
/// @file
/// @brief Tiger hash function.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Tiger {
    const size_t DEFAULT_PASSES = 3;

    using sbox_t     = std::array<uint64_t, 4 * 256>;
    using state_t    = std::array<uint64_t, 3>;
    using msgblock_t = std::array<uint64_t, 8>;
    using digest_t   = std::array<uint8_t, 3 * 8>;

    /**
     * Initializes sbox with default configuration.
     *
     * @param sbox   SBox to be initialized
     *
     * @return SBOX
     */
    sbox_t &InitializeSBox (sbox_t &sbox) noexcept;

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
    sbox_t &InitializeSBox (sbox_t &sbox, const void *seed, size_t seed_size, size_t passes) noexcept;

    /** The Tiger192 generator.  */
    class Generator {
    private:
        enum Flags { BIT_FINALIZED = 0, BIT_TIGER2 = 1 };

    private:
        const sbox_t &sbox_;
        size_t        count_ = 0;
        size_t        cntPass_;
        uint32_t      flags_ = 0;
        state_t       hash_;
        msgblock_t    buffer_;

    public:
        /**
         * The constructor.
         *
         * @param sbox The sbox
         */
        explicit Generator (const sbox_t &sbox) : Generator (sbox, DEFAULT_PASSES, false) { /* NO-OP */
        }

        /**
         * The constructor with the explicit pass counts.
         *
         * @param sbox    The sbox
         * @param cntPass # of iterations in the compression function
         */
        Generator (const sbox_t &sbox, size_t cntPass) : Generator (sbox, cntPass, false) { /* NO-OP */
        }

        /**
         * The constructor with the explicit pass counts.
         *
         * @param sbox     The sbox
         * @param cntPass  # of iterations in the compression function.
         * @param isTiger2 Use Tiger2 padding
         */
        Generator (const sbox_t &sbox, size_t cntPass, bool isTiger2) noexcept;

        /** Resets the state.  */
        Generator &Reset () noexcept;

        bool IsFinalized () const { return (flags_ & (1u << BIT_FINALIZED)) != 0; }

        bool IsTiger2 () const { return (flags_ & (1u << BIT_TIGER2)) != 0; }
        /**
         * Updates states
         *
         * @param data   The input sequence
         * @param size   # of bytes in the input sequence
         *
         * @return *this
         */
        Generator &Update (const void *data, size_t size) noexcept;

        /**
         * Updates states
         *
         * @param value  The input value
         *
         * @return *this
         */
        Generator &Update (uint8_t value) noexcept;

        /**
         * Computes Tiger192 digest
         *
         * @remarks Once finalized, successive Finalize() returns the same value.
         * @return Computed digest
         */
        digest_t Finalize () noexcept;
    };
}  // namespace Tiger
