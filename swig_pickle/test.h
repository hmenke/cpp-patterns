#pragma once

#include <string>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

class Test {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_s;
    }

    std::string m_s;
public:
    Test() : m_s() {}
    void construct() { m_s = "Hëllö wørłd!"; }
    std::string get() { return m_s; }
};
