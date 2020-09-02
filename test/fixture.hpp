#pragma once

#include <Tiger.hpp>

class TigerFixture {
    Tiger::sbox_t   sbox_ ;

public:
    TigerFixture () {
        Tiger::InitializeSBox (sbox_);
    }

    const Tiger::sbox_t & sbox () const { return sbox_ ; }
} ;
