#pragma once

#include <Tiger.hpp>

#include <fmt/format.h>

template <>
    struct fmt::formatter<Tiger::digest_t> {
        template <typename ParseContext>
            constexpr auto parse (ParseContext& ctx) { return ctx.begin (); }

        template <typename FormatContext>
            auto format (const Tiger::digest_t& digest, FormatContext& ctx) {
                for (auto const v : digest) {
                    format_to (ctx.out (), "{:02X}", v);
                }
                return ctx.out ();
            }
    };
