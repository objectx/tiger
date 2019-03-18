#ifndef TIGER_FIXTURE_HPP
#define TIGER_FIXTURE_HPP  1

#include <Tiger.hpp>

class TigerFixture {
    Tiger::sbox_t   sbox_ ;

public:
    TigerFixture () {
        Tiger::InitializeSBox (sbox_);
    }

    const Tiger::sbox_t & sbox () const { return sbox_ ; }
} ;

#endif /* TIGER_FIXTURE_HPP */
