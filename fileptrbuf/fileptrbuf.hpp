// This implementation is based on
// http://ilab.usc.edu/rjpeters/groovx/classrutz_1_1stdiobuf.html
// and therefore
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
#pragma once

#include <algorithm>
#include <array>
#include <ios>
#include <stdexcept>
#include <streambuf>

class fileptrbuf : public std::streambuf {
    ptrdiff_t const m_putback_size = 4UL;
    std::array<char_type, 4096> buffer;
    FILE *m_f;
    std::ios_base::openmode m_mode;

    int flush() {
        if (!(m_mode & std::ios::out)) {
            return EOF;
        }
        std::size_t const num = pptr() - pbase();
        std::size_t written = fwrite(pbase(), sizeof(char_type), num, m_f);
        if (written != num) {
            return EOF;
        }
        pbump(-num);
        return 0;
    }

protected:
    virtual int sync() override {
        if (flush() == EOF) {
            return -1;
        }
        return 0;
    }

public:
    explicit fileptrbuf(FILE *f, std::ios_base::openmode mode)
        : m_f{f}, m_mode{mode} {
        setg(buffer.data() + m_putback_size, buffer.data() + m_putback_size,
             buffer.data() + m_putback_size);
        setp(buffer.data(), buffer.data() + buffer.size());
    }

    virtual int_type underflow() override {
        if (!(m_mode & std::ios::in)) {
            return EOF;
        }
        if (gptr() < egptr()) {
            return *gptr();
        }

        // copy up to four characters previously read into the putback buffer
        size_t const n_putback = std::min(gptr() - eback(), m_putback_size);
        std::copy_n(gptr() - n_putback, n_putback,
                    buffer.data() + m_putback_size - n_putback);

        // read new characters
        size_t const num =
            std::fread(buffer.data() + m_putback_size, sizeof(char_type),
                       buffer.size() - m_putback_size, m_f);

        if (num == 0) {
            return EOF;
        }

        // reset buffer pointers
        setg(buffer.data() + m_putback_size - n_putback,
             buffer.data() + m_putback_size,
             buffer.data() + m_putback_size + num);

        // Avoid EOF being casted to int -1
        return static_cast<unsigned char>(*gptr());
    }
    virtual int_type overflow(int c) override {
        if (!(m_mode & std::ios::out)) {
            return EOF;
        }
        if (c != EOF) {
            *pptr() = c;
            pbump(1);
        }
        if (flush() == EOF) {
            return -1;
        }
        return c;
    }
};
