#pragma once
#ifndef TIGER_PUT_HEX_HPP
#define TIGER_PUT_HEX_HPP 1

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>

template <typename T>
class put_hex_ {
private:
    const T& value_;
    size_t   width_;

public:
    put_hex_ (const T& value, size_t width)
        : value_ (value)
        , width_ (width) {
        /* NO-OP */
    }
    friend std::ostream& operator<< (std::ostream&      output,
                                     const put_hex_<T>& arg) {
        char               fill = output.fill ('0');
        std::ios::fmtflags flag = output.setf (std::ios::hex | std::ios::uppercase,
                                               std::ios::basefield | std::ios::uppercase);
        output.width (arg.width_);
        output << arg.value_;
        output.setf (flag, std::ios::basefield | std::ios::uppercase);
        output.fill (fill);
        return output;
    }
};

template <typename T>
static put_hex_<T> put_hex (const T& value, int width) {
    return put_hex_<T> (value, width);
}

inline std::ostream& operator<< (std::ostream& output, const Tiger::digest_t& result) {
    char               fill = output.fill ('0');
    std::ios::fmtflags flag = output.setf (std::ios::hex | std::ios::uppercase,
                                           std::ios::basefield | std::ios::uppercase);

    for (size_t i = 0; i < result.size (); ++i) {
        output.width (2);
        output << (static_cast<int> (result[i]) & 0xFF);
    }
    output.setf (flag);
    output.fill (fill);
    return output;
}

#endif /* TIGER_PUT_HEX_HPP */
