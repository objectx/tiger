/*
 * main.cpp:
 *
 * AUTHOR(S): objectx
 *
 * $Id$
 */

#include "common.h"
#define CATCH_CONFIG_RUNNER 1
#include <catch.hpp>
#include "Tiger.h"

namespace {
    template <typename T>
        class put_hex_
        {
        private:
            const T &   value_ ;
            size_t      width_ ;
        public:
            put_hex_ (const T &value, size_t width) : value_ (value), width_ (width) {
                /* NO-OP */
            }
            friend std::ostream &       operator << (std::ostream &output, const put_hex_<T> &arg) {
                char    fill = output.fill ('0') ;
                std::ios::fmtflags      flag = output.setf (std::ios::hex | std::ios::uppercase,
                                                            std::ios::basefield | std::ios::uppercase) ;
                output.width (arg.width_) ;
                output << arg.value_ ;
                output.setf (flag, std::ios::basefield | std::ios::uppercase) ;
                output.fill (fill) ;
                return output ;
            }
        } ;

    template <typename T>
        static put_hex_<T>      put_hex (const T &value, int width) {
            return put_hex_<T> (value, width) ;
        }

    std::ostream &      operator << (std::ostream &output, const Tiger::Digest &result) {
        char    fill = output.fill ('0') ;
        std::ios::fmtflags      flag = output.setf (std::ios::hex | std::ios::uppercase, std::ios::basefield | std::ios::uppercase) ;

        for (size_t i = 0 ; i < result.Size () ; ++i) {
            output.width (2) ;
            output << (static_cast<int> (result [i]) & 0xFF) ;
        }
        output.setf (flag) ;
        output.fill (fill) ;
        return output ;
    }
}

static Tiger::sbox_t    sbox ;

TEST_CASE ("Test Generator", "[generator]") {
    Tiger::Generator    gen (sbox) ;

    std::ostringstream      out ;

    SECTION ("(empty string)") {
        gen.Update ("", 0) ;
        Tiger::Digest   result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "3293AC630C13F0245F92BBB1766E16167A4E58492DDE73F3") ;
    }
    SECTION ("a") {
        gen.Update ("a", 1) ;

        Tiger::Digest     result = gen.Finalize () ;
        out << result ;

        REQUIRE (out.str () == "77BEFBEF2E7EF8AB2EC8F93BF587A7FC613E247F5F247809") ;
    }
    SECTION ("abc") {
        gen.Update ("abc", 3) ;

        Tiger::Digest     result = gen.Finalize () ;
        out << result ;

        REQUIRE (out.str () == "2AAB1484E8C158F2BFB8C5FF41B57A525129131C957B5F93") ;
    }
    SECTION ("message digest") {
        std::string     src ("message digest") ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "D981F8CB78201A950DCF3048751E441C517FCA1AA55A29F6") ;
    }
    SECTION ("abcdefghijklmnopqrstuvwxyz") {
        std::string     src ("abcdefghijklmnopqrstuvwxyz") ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "1714A472EEE57D30040412BFCC55032A0B11602FF37BEEE9") ;
    }
    SECTION ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") {
        std::string     src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "0F7BF9A19B9C58F2B7610DF7E84F0AC3A71C631E7B53F78E") ;
    }
    SECTION ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") {
        std::string     src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "8DCEA680A17583EE502BA38A3C368651890FFBCCDC49A8CC") ;
    }
    SECTION ("\"1234567890\" * 8") {
        std::string     src ;
        for (int i = 0 ; i < 8 ; ++i) {
            src.append ("1234567890") ;
        }
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "1C14795529FD9F207A958F84C52F11E887FA0CABDFD91BFD") ;
    }
    SECTION ("1 million \"a\"") {
        std::string     src (1000000, 'a') ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "6DB0E2729CBEAD93D715C6A7D36302E9B3CEE0D2BC314B41") ;
    }
}

TEST_CASE ("Test Generator (Type 2)", "[generator]") {
    Tiger::Generator    gen (sbox, Tiger::DEFAULT_PASSES, true) ;

    std::ostringstream      out ;

    SECTION ("(empty string)") {
        gen.Update ("", 0) ;
        Tiger::Digest   result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "4441BE75F6018773C206C22745374B924AA8313FEF919F41") ;
    }
    SECTION ("a") {
        gen.Update ("a", 1) ;

        Tiger::Digest     result = gen.Finalize () ;
        out << result ;

        REQUIRE (out.str () == "67E6AE8E9E968999F70A23E72AEAA9251CBC7C78A7916636") ;
    }
    SECTION ("abc") {
        gen.Update ("abc", 3) ;

        Tiger::Digest     result = gen.Finalize () ;
        out << result ;

        REQUIRE (out.str () == "F68D7BC5AF4B43A06E048D7829560D4A9415658BB0B1F3BF") ;
    }
    SECTION ("message digest") {
        std::string     src ("message digest") ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "E29419A1B5FA259DE8005E7DE75078EA81A542EF2552462D") ;
    }
    SECTION ("abcdefghijklmnopqrstuvwxyz") {
        std::string     src ("abcdefghijklmnopqrstuvwxyz") ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "F5B6B6A78C405C8547E91CD8624CB8BE83FC804A474488FD") ;
    }
    SECTION ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") {
        std::string     src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "A6737F3997E8FBB63D20D2DF88F86376B5FE2D5CE36646A9") ;
    }
    SECTION ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") {
        std::string     src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "EA9AB6228CEE7B51B77544FCA6066C8CBB5BBAE6319505CD") ;
    }
    SECTION ("\"1234567890\" * 8") {
        std::string     src ;
        for (int i = 0 ; i < 8 ; ++i) {
            src.append ("1234567890") ;
        }
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "D85278115329EBAA0EEC85ECDC5396FDA8AA3A5820942FFF") ;
    }
    SECTION ("1 million \"a\"") {
        std::string     src (1000000, 'a') ;

        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        out << result ;

        REQUIRE (out.str () == "E068281F060F551628CC5715B9D0226796914D45F7717CF4") ;
    }
}

int main (int argc, char **argv) {
    Tiger::InitializeSBox (sbox) ;

    int result = Catch::Session ().run (argc, argv) ;

    return result ;
}

/* [END OF FILE] */
