/*
 * Copyright (c) 2019  Masashi Fujita.  All rights reserved.
 */

#include "Tiger.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>

//#define FORCE_RUNTIME_BYTEORDER_CHECKING

namespace {

    const uint64_t init_state_0 = 0x0123456789ABCDEFuLL;
    const uint64_t init_state_1 = 0xFEDCBA9876543210uLL;
    const uint64_t init_state_2 = 0xF096A5B4C3B2E187uLL;
    const uint64_t schedule_0   = 0xA5A5A5A5A5A5A5A5uLL;
    const uint64_t schedule_1   = 0x0123456789ABCDEFuLL;

#ifndef FORCE_RUNTIME_BYTEORDER_CHECKING
#    if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#        if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    const bool TARGET_LITTLE_ENDIAN = true;
#        elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    const bool TARGET_LITTLE_ENDIAN = false;
#        else
#            define FORCE_RUNTIME_BYTEORDER_CHECKING 1
#        endif
#    endif
#endif /* FORCE_RUNTIME_BYTEORDER_CHECKING */

#ifdef FORCE_RUNTIME_BYTEORDER_CHECKING
    bool check_little_endian () noexcept {
        unsigned int val = 0x01020304;

        auto p = reinterpret_cast<const uint8_t*> (&val);

        if (p[0] == 0x04 && p[1] == 0x03 && p[2] == 0x02 && p[3] == 0x01) {
            return true;
        }
        else if (p[3] == 0x04 && p[2] == 0x03 && p[1] == 0x02 && p[0] == 0x01) {
            return false;
        }
        assert (false);
        return false;
    }
    const bool TARGET_LITTLE_ENDIAN = check_little_endian ();
#endif /* FORCE_RUNTIME_BYTEORDER_CHECKING */

    inline uint64_t as_uint64 (const void* addr) {
        auto p = static_cast<const uint8_t*> (addr);
        // clang-format off
        return ( (static_cast<uint64_t> (p[0]) <<  0u)
               | (static_cast<uint64_t> (p[1]) <<  8u)
               | (static_cast<uint64_t> (p[2]) << 16u)
               | (static_cast<uint64_t> (p[3]) << 24u)
               | (static_cast<uint64_t> (p[4]) << 32u)
               | (static_cast<uint64_t> (p[5]) << 40u)
               | (static_cast<uint64_t> (p[6]) << 48u)
               | (static_cast<uint64_t> (p[7]) << 56u)
               );
        // clang-format on
    }

    std::array<uint64_t, 8> make_work (const void* seed, size_t size) {
        std::array<uint8_t, 64> tmp;

        tmp.fill (0);
        ::memcpy (tmp.data (), seed, std::min (size, tmp.size ()));
        // clang-format off
        return std::array<uint64_t, 8> {{ as_uint64 (&tmp[8 * 0])
                                        , as_uint64 (&tmp[8 * 1])
                                        , as_uint64 (&tmp[8 * 2])
                                        , as_uint64 (&tmp[8 * 3])
                                        , as_uint64 (&tmp[8 * 4])
                                        , as_uint64 (&tmp[8 * 5])
                                        , as_uint64 (&tmp[8 * 6])
                                        , as_uint64 (&tmp[8 * 7])
                                        }};
        // clang-format on
    }

    void to_bytes (uint8_t* result, uint64_t value) {
        if (TARGET_LITTLE_ENDIAN) {
            memcpy (result, &value, sizeof (value));
        }
        else {
            auto p = reinterpret_cast<const uint8_t*> (&value);
            for (size_t i = 0; i < sizeof (value); ++i) {
                result[i ^ 0x07u] = p[i];
            }
        }
    }

    inline void round (const Tiger::sbox_t& sbox, uint64_t& a, uint64_t& b, uint64_t& c, uint64_t x, uint64_t mul) {
        c ^= x;
        // clang-format off
        auto const c0 = static_cast<uint8_t> (c >>  0u);
        auto const c1 = static_cast<uint8_t> (c >>  8u);
        auto const c2 = static_cast<uint8_t> (c >> 16u);
        auto const c3 = static_cast<uint8_t> (c >> 24u);
        auto const c4 = static_cast<uint8_t> (c >> 32u);
        auto const c5 = static_cast<uint8_t> (c >> 40u);
        auto const c6 = static_cast<uint8_t> (c >> 48u);
        auto const c7 = static_cast<uint8_t> (c >> 56u);
        // clang-format on

        a -= (sbox[0 * 256 + c0] ^ sbox[1 * 256 + c2] ^ sbox[2 * 256 + c4] ^ sbox[3 * 256 + c6]);
        b += (sbox[3 * 256 + c1] ^ sbox[2 * 256 + c3] ^ sbox[1 * 256 + c5] ^ sbox[0 * 256 + c7]);
        b *= mul;
    }

} // namespace

namespace Tiger {

    namespace {
        void Compress (state_t& state, const msgblock_t& input, const sbox_t& sbox, size_t passes) noexcept {
            uint_fast64_t a = state[0];
            uint_fast64_t b = state[1];
            uint_fast64_t c = state[2];

            uint_fast64_t x0 = input[0];
            uint_fast64_t x1 = input[1];
            uint_fast64_t x2 = input[2];
            uint_fast64_t x3 = input[3];
            uint_fast64_t x4 = input[4];
            uint_fast64_t x5 = input[5];
            uint_fast64_t x6 = input[6];
            uint_fast64_t x7 = input[7];

            auto schedule = [&x0, &x1, &x2, &x3, &x4, &x5, &x6, &x7]() {
                x0 -= x7 ^ schedule_0;
                x1 ^= x0;
                x2 += x1;
                x3 -= x2 ^ ((~x1) << 19u);
                x4 ^= x3;
                x5 += x4;
                x6 -= x5 ^ ((~x4) >> 23u);
                x7 ^= x6;
                x0 += x7;
                x1 -= x0 ^ ((~x7) << 19u);
                x2 ^= x1;
                x3 += x2;
                x4 -= x3 ^ ((~x2) >> 23u);
                x5 ^= x4;
                x6 += x5;
                x7 -= x6 ^ schedule_1;
            };

            auto pass = [&](const sbox_t& S, uint64_t& A, uint64_t& B, uint64_t& C, uint64_t MUL) {
                round (S, A, B, C, x0, MUL);
                round (S, B, C, A, x1, MUL);
                round (S, C, A, B, x2, MUL);
                round (S, A, B, C, x3, MUL);
                round (S, B, C, A, x4, MUL);
                round (S, C, A, B, x5, MUL);
                round (S, A, B, C, x6, MUL);
                round (S, B, C, A, x7, MUL);
            };

            pass (sbox, a, b, c, 5);
            schedule ();
            pass (sbox, c, a, b, 7);
            schedule ();
            pass (sbox, b, c, a, 9);

            for (size_t cnt = 3; cnt < passes; ++cnt) {
                schedule ();
                pass (sbox, a, b, c, 9);

                auto tmp = a;
                a        = c;
                c        = b;
                b        = tmp;
            }
            state[0] = a ^ state[0];
            state[1] = b - state[1];
            state[2] = c + state[2];
        }
    } // namespace

    sbox_t& InitializeSBox (sbox_t& sbox) noexcept {
        return InitializeSBox (sbox, "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham", 64, 5);
    }

    sbox_t& InitializeSBox (sbox_t& sbox, const void* seed, size_t seed_size, size_t passes) noexcept {
        state_t state {{init_state_0, init_state_1, init_state_2}};

        for (size_t i = 0; i < sbox.size (); ++i) {
            auto make_fillval = [](size_t idx) -> uint64_t {
                auto v = static_cast<uint64_t> (idx & 0xFFu);
                // clang-format off
                return ((v <<  0u) |
                        (v <<  8u) |
                        (v << 16u) |
                        (v << 24u) |
                        (v << 32u) |
                        (v << 40u) |
                        (v << 48u) |
                        (v << 56u));
                // clang-format on
            };
            sbox[i] = make_fillval (i);
        }

        auto work = make_work (seed, seed_size);

        int_fast32_t abc = 2;
        auto         tp  = reinterpret_cast<uint8_t*> (&sbox[0]);
        auto         sp  = reinterpret_cast<uint8_t*> (&state[0]);
        for (size_t cnt = 0; cnt < passes; ++cnt) {
            for (int_fast32_t i = 0; i < 256; ++i) {
                for (int_fast32_t sb = 0; sb < 1024; sb += 256) {
                    ++abc;
                    if (abc == 3) {
                        abc = 0;
                        Compress (state, work, sbox, 3);
                    }
                    for (int_fast32_t col = 0; col < 8; ++col) {
                        std::swap (tp[8 * (sb + i) + col], tp[8 * (sb + sp[8 * abc + col]) + col]);
                    }
                }
            }
        }
        return sbox;
    }

    Generator::Generator (const sbox_t& sbox, size_t passes, bool isTiger2) noexcept
            : sbox_ {sbox}
            , cntPass_ {std::max (DEFAULT_PASSES, passes)}
            , hash_ {{init_state_0, init_state_1, init_state_2}} {
        if (isTiger2) {
            flags_ |= 1u << BIT_TIGER2;
        }
    }

    Generator& Generator::Reset () noexcept {
        count_   = 0;
        hash_[0] = init_state_0;
        hash_[1] = init_state_1;
        hash_[2] = init_state_2;
        flags_ &= ~(1u << BIT_FINALIZED);
        return *this;
    }

    Generator& Generator::Update (const void* data, size_t size) noexcept {
        auto p = static_cast<const uint8_t*> (data);
        auto q = reinterpret_cast<uint8_t*> (&buffer_[0]);

        if (TARGET_LITTLE_ENDIAN) {
            for (size_t i = 0; i < size; ++i) {
                size_t idx = count_ & 0x3Fu;
                if (0 < count_ && idx == 0) {
                    Compress (hash_, buffer_, sbox_, cntPass_);
                }
                q[idx] = p[i];
                ++count_;
            }
        }
        else {
            for (size_t i = 0; i < size; ++i) {
                size_t idx = count_ & 0x3Fu;
                if (0 < count_ && idx == 0) {
                    Compress (hash_, buffer_, sbox_, cntPass_);
                }
                q[0x3F - idx] = p[i];
                ++count_;
            }
        }
        return *this;
    }

    Generator& Generator::Update (uint8_t value) noexcept {
        size_t idx = count_ & 0x3Fu;

        if (0 < count_ && idx == 0) {
            Compress (hash_, buffer_, sbox_, cntPass_);
        }

        auto q = reinterpret_cast<uint8_t*> (&buffer_[0]);
        if (TARGET_LITTLE_ENDIAN) {
            q[idx] = value;
        }
        else {
            q[0x3F - idx] = value;
        }
        ++count_;
        return *this;
    }

    digest_t Generator::Finalize () noexcept {
        if (!IsFinalized ()) {
            uint8_t pad[64];

            ::memset (pad, 0, sizeof (pad));

            uint64_t bitcount = 8 * static_cast<uint64_t> (count_);

            if (IsTiger2 ()) {
                Update (0x80);
            }
            else {
                Update (0x01);
            }
            size_t remain = 0x40 - (count_ & 0x3Fu);
            if (8 <= remain) {
                Update (pad, remain - 8);
            }
            else {
                // No rooms to store the bit-length.  Requires extra block.
                Update (pad, remain);
                Update (pad, sizeof (pad) - 8);
            }
            uint8_t tmp[8];
            to_bytes (tmp, bitcount);
            Update (tmp, sizeof (tmp));
            assert ((count_ & 0x3Fu) == 0);
            Compress (hash_, buffer_, sbox_, cntPass_);
            flags_ |= (1u << BIT_FINALIZED);
        }
        digest_t result;
        to_bytes (&result[0], hash_[0]);
        to_bytes (&result[8], hash_[1]);
        to_bytes (&result[16], hash_[2]);
        return result;
    }
} // namespace Tiger
