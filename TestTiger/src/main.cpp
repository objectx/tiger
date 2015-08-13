/*
 * main.cpp:
 *
 * AUTHOR(S): objectx
 *
 * $Id$
 */

#include "common.h"
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

    class TestLogger {
    private:
        int     errors_ ;
    public:
        TestLogger () : errors_ (0) {/* NO-OP */}
        void    AreEqual (const std::string &actual, const std::string &expected, const std::string &msg) ;
        int     getErrors () const { return errors_ ; }
        void    Reset () { errors_ = 0 ; }
    } ;

    void        TestLogger::AreEqual (const std::string &actual, const std::string &expected, const std::string &msg) {
        if (std::equal (actual.begin (), actual.end (), expected.begin ())) {
            std::cout << "Test [" << msg << "] was successed." << std::endl ;
        }
        else {
            std::cout <<
                "Test [" << msg << "] was failed." << std::endl <<
                "\tExpected: " << expected << std::endl <<
                "\t  Actual: " << actual   << std::endl ;
            ++errors_ ;
        }
    }
}

static Tiger::sbox_t    sbox ;

static void     Test () {
    std::cout << "Tiger test" << std::endl ;

    Tiger::Generator    gen (sbox) ;

    TestLogger  logger ;

    {
        gen.Reset () ;
        gen.Update ("", 0) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("3293AC630C13F0245F92BBB1766E16167A4E58492DDE73F3") ;

        logger.AreEqual (out.str (), expected, "\"\" (empty string)") ;
    }
    {
        gen.Reset () ;
        gen.Update ("a", 1) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("77BEFBEF2E7EF8AB2EC8F93BF587A7FC613E247F5F247809") ;

        logger.AreEqual (out.str (), expected, "a") ;
    }
    {
        std::string     src ("abc") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("2AAB1484E8C158F2BFB8C5FF41B57A525129131C957B5F93") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("message digest") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("D981F8CB78201A950DCF3048751E441C517FCA1AA55A29F6") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("abcdefghijklmnopqrstuvwxyz") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("1714A472EEE57D30040412BFCC55032A0B11602FF37BEEE9") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("0F7BF9A19B9C58F2B7610DF7E84F0AC3A71C631E7B53F78E") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("8DCEA680A17583EE502BA38A3C368651890FFBCCDC49A8CC") ;

        logger.AreEqual (out.str (), expected, "A..Za..z0..9") ;
    }
    {
        std::string     src ;
        for (int i = 0 ; i < 8 ; ++i) {
            src.append ("1234567890") ;
        }
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("1C14795529FD9F207A958F84C52F11E887FA0CABDFD91BFD") ;

        logger.AreEqual (out.str (), expected, "8 times \"1234567890\"") ;
    }
    {
        std::string     src (1000000, 'a') ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("6DB0E2729CBEAD93D715C6A7D36302E9B3CEE0D2BC314B41") ;
        logger.AreEqual (out.str (), expected, "1 million times \"a\"") ;
    }
}

static void     Test2 () {
    std::cout << "Tiger2 test" << std::endl ;

    Tiger::Generator    gen (sbox, Tiger::DEFAULT_PASSES, true) ;

    TestLogger  logger ;

    {
        gen.Reset () ;
        gen.Update ("", 0) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("4441BE75F6018773C206C22745374B924AA8313FEF919F41") ;

        logger.AreEqual (out.str (), expected, "\"\" (empty string)") ;
    }
    {
        gen.Reset () ;
        gen.Update ("a", 1) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("67E6AE8E9E968999F70A23E72AEAA9251CBC7C78A7916636") ;

        logger.AreEqual (out.str (), expected, "a") ;
    }
    {
        std::string     src ("abc") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("F68D7BC5AF4B43A06E048D7829560D4A9415658BB0B1F3BF") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("message digest") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("E29419A1B5FA259DE8005E7DE75078EA81A542EF2552462D") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("abcdefghijklmnopqrstuvwxyz") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("F5B6B6A78C405C8547E91CD8624CB8BE83FC804A474488FD") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("A6737F3997E8FBB63D20D2DF88F86376B5FE2D5CE36646A9") ;

        logger.AreEqual (out.str (), expected, src) ;
    }
    {
        std::string     src ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("EA9AB6228CEE7B51B77544FCA6066C8CBB5BBAE6319505CD") ;

        logger.AreEqual (out.str (), expected, "A..Za..z0..9") ;
    }
    {
        std::string     src ;
        for (int i = 0 ; i < 8 ; ++i) {
            src.append ("1234567890") ;
        }
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("D85278115329EBAA0EEC85ECDC5396FDA8AA3A5820942FFF") ;

        logger.AreEqual (out.str (), expected, "8 times \"1234567890\"") ;
    }
    {
        std::string     src (1000000, 'a') ;
        gen.Reset () ;
        gen.Update (src.c_str (), src.size ()) ;
        Tiger::Digest     result = gen.Finalize () ;

        std::ostringstream      out ;
        out << result ;

        std::string     expected ("E068281F060F551628CC5715B9D0226796914D45F7717CF4") ;
        logger.AreEqual (out.str (), expected, "1 million times \"a\"") ;
    }
}

int     main (int argc, char **argv)
{
    Tiger::InitializeSBox (sbox) ;

    Test () ;
    Test2 () ;
#if defined (_WIN32) || defined (_WIN64)
    if (IsDebuggerPresent ()) {
        DebugBreak () ;
    }
#endif
    return 0 ;
}

/*
 * $LastChangedRevision$
 * $LastChangedBy$
 * $HeadURL$
 */
