#pragma once
#ifndef to_string_hpp__341F47F3_918E_48E2_83E4_4D6B0A3A8718
#define to_string_hpp__341F47F3_918E_48E2_83E4_4D6B0A3A8718 1

#include <Tiger.hpp>

#include <fmt/format.h>

namespace fmt {
    template <>
        struct formatter<Tiger::digest_t> {
            template <typename ParseContext>
                constexpr auto parse (ParseContext& ctx) { return ctx.begin (); }

            template <typename FormatContext>
                auto format (const Tiger::digest_t& digest, FormatContext& ctx) {
                    auto it = ctx.begin ();
                    for (auto const v : digest) {
                        it = format_to (it, "{:02X}", v);
                    }
                    return it;
                }
        };
} // namespace fmt

#endif /* to_string_hpp__341F47F3_918E_48E2_83E4_4D6B0A3A8718 */
