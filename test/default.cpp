
#include <Tiger.hpp>

#include "fixture.hpp"
#include "to_string.hpp"

#include <catch2/catch.hpp>

#include <cstdint>


TEST_CASE_METHOD (TigerFixture, "Test Generator", "[generator]") {
    using namespace fmt::literals;

    Tiger::Generator    gen (sbox ()) ;

    SECTION ("(empty string)") {
        gen.Update ("", 0) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "3293AC630C13F0245F92BBB1766E16167A4E58492DDE73F3") ;
    }
    SECTION ("a") {
        gen.Update ("a", 1) ;

        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "77BEFBEF2E7EF8AB2EC8F93BF587A7FC613E247F5F247809") ;
    }
    SECTION ("abc") {
        gen.Update ("abc", 3) ;

        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "2AAB1484E8C158F2BFB8C5FF41B57A525129131C957B5F93") ;
    }
    SECTION ("message digest") {
        std::string     src ("message digest") ;

        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "D981F8CB78201A950DCF3048751E441C517FCA1AA55A29F6") ;
    }
    SECTION ("abcdefghijklmnopqrstuvwxyz") {
        std::string     src ("abcdefghijklmnopqrstuvwxyz") ;
        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "1714A472EEE57D30040412BFCC55032A0B11602FF37BEEE9") ;
    }
    SECTION ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") {
        std::string     src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ;
        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "0F7BF9A19B9C58F2B7610DF7E84F0AC3A71C631E7B53F78E") ;
    }
    SECTION ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") {
        std::string     src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ;

        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "8DCEA680A17583EE502BA38A3C368651890FFBCCDC49A8CC") ;
    }
    SECTION ("\"1234567890\" * 8") {
        std::string     src ;
        for (int i = 0 ; i < 8 ; ++i) {
            src.append ("1234567890") ;
        }
        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "1C14795529FD9F207A958F84C52F11E887FA0CABDFD91BFD") ;
    }
    SECTION ("1 million \"a\"") {
        std::string     src (1000000, 'a') ;

        gen.Update (src.c_str (), src.size ()) ;
        auto const &    result = gen.Finalize () ;

        REQUIRE ("{}"_format (result) == "6DB0E2729CBEAD93D715C6A7D36302E9B3CEE0D2BC314B41") ;
    }
}
