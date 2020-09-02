
#include <Tiger.hpp>

#include "fixture.hpp"
#include "to_string.hpp"

#include <doctest/doctest.h>

TEST_CASE_FIXTURE (TigerFixture, "Test Generator (Type 2)") {
    using namespace fmt::literals;

    Tiger::Generator gen (sbox (), Tiger::DEFAULT_PASSES, true);

    SUBCASE ("(empty string)") {
        gen.Update ("", 0);
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "4441BE75F6018773C206C22745374B924AA8313FEF919F41");
    }
    SUBCASE ("a") {
        gen.Update ("a", 1);

        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "67E6AE8E9E968999F70A23E72AEAA9251CBC7C78A7916636");
    }
    SUBCASE ("abc") {
        gen.Update ("abc", 3);

        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "F68D7BC5AF4B43A06E048D7829560D4A9415658BB0B1F3BF");
    }
    SUBCASE ("message digest") {
        std::string src ("message digest");

        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "E29419A1B5FA259DE8005E7DE75078EA81A542EF2552462D");
    }
    SUBCASE ("abcdefghijklmnopqrstuvwxyz") {
        std::string src ("abcdefghijklmnopqrstuvwxyz");
        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "F5B6B6A78C405C8547E91CD8624CB8BE83FC804A474488FD");
    }
    SUBCASE ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") {
        std::string src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "A6737F3997E8FBB63D20D2DF88F86376B5FE2D5CE36646A9");
    }
    SUBCASE ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") {
        std::string src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "EA9AB6228CEE7B51B77544FCA6066C8CBB5BBAE6319505CD");
    }
    SUBCASE ("\"1234567890\" * 8") {
        std::string src;
        for (int i = 0; i < 8; ++i) {
            src.append ("1234567890");
        }
        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "D85278115329EBAA0EEC85ECDC5396FDA8AA3A5820942FFF");
    }
    SUBCASE ("1 million \"a\"") {
        std::string src (1000000, 'a');

        gen.Update (src.c_str (), src.size ());
        auto const &result = gen.Finalize ();

        REQUIRE ("{}"_format(result) == "E068281F060F551628CC5715B9D0226796914D45F7717CF4");
    }
}
