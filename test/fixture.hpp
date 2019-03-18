#pragma once
#ifndef fixture_hpp__B91CE273_5E78_44AC_BFCA_B39803B4018D
#define fixture_hpp__B91CE273_5E78_44AC_BFCA_B39803B4018D  1

#include <Tiger.hpp>

class TigerFixture {
    Tiger::sbox_t   sbox_ ;

public:
    TigerFixture () {
        Tiger::InitializeSBox (sbox_);
    }

    const Tiger::sbox_t & sbox () const { return sbox_ ; }
} ;

#endif /* fixture_hpp__B91CE273_5E78_44AC_BFCA_B39803B4018D */
