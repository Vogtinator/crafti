#ifndef FIX_H
#define FIX_H

#include <cstdint>
#include <cmath>
#include <limits>
#include <cstring>
#include <cstdio>

template <unsigned int s, typename T=int32_t> class Fix
{   
public:
    constexpr Fix() : value(0) {}
    //constexpr Fix(const Fix<s,T> &other) : value(other.value) {}
    constexpr Fix(const float v) : value(static_cast<T>(v * static_cast<float>(1<<s))) {}
    constexpr Fix(const unsigned int v) : value(v << s) {}
    constexpr Fix(const int v) : value(v << s) {}
    constexpr operator float() { return toFloat(); }
    constexpr operator int() { return toInteger<int>(); }
    constexpr operator unsigned int() { return toInteger<unsigned int>(); }

    Fix<s,T>& operator=(const Fix<s,T> &other) { value = other.value; return *this; }

    T round() const { T ret = value >> (s-1); return (ret>>1) + (ret&1); }
    constexpr T floor() const { return value >> s; }

    Fix<s,T> wholes() const { Fix<s,T> ret; ret.value = value & ~((1<<s)-1); return ret; }

    Fix<s,T> operator -() const { Fix<s,T> ret; ret.value = -value; return ret; }
    Fix<s,T>& operator +() const { return *this; }

    Fix<s,T>& operator ++() { value += 1 << s; return *this; }
    Fix<s,T>& operator --() { value -= 1 << s; return *this; }

    template <typename U> Fix<s,T> operator >>(const U other) const { Fix<s,T> ret; ret.value = value >> other; return ret; }
    template <typename U> Fix<s,T> operator <<(const U other) const { Fix<s,T> ret; ret.value = value << other; return ret; }

    Fix<s,T> operator +(const Fix<s,T>& other) const { Fix<s,T> ret; ret.value = value + other.value; return ret; }
    template <typename U> Fix<s,T> operator +(const U other) const { Fix<s,T> ret; ret.value = value + (other<<s); return ret; }

    Fix<s,T> operator -(const Fix<s,T>& other) const { Fix<s,T> ret; ret.value = value - other.value; return ret; }
    template <typename U> Fix<s,T> operator -(const U other) const { Fix<s,T> ret; ret.value = value - (other<<s); return ret; }

    Fix<s,T> operator *(const Fix<s,T>& other) const { Fix<s,T> ret; ret.value = (value * other.value) >> s; return ret; }
    template <typename U> Fix<s,T> operator *(const U other) const { Fix<s,T> ret; ret.value = value * other; return ret; }

    Fix<s,T> operator /(const Fix<s,T>& other) const { Fix<s,T> ret; ret.value = (value << s) / other.value; return ret; }
    template <typename U> Fix<s,T> operator /(const U other) const { Fix<s,T> ret; ret.value = value / other; return ret; }

    Fix<s,T>& operator +=(const Fix<s,T>& other) { value += other.value; return *this; }
    Fix<s,T>& operator -=(const Fix<s,T>& other) { value -= other.value; return *this; }
    Fix<s,T>& operator *=(const float other) { value *= other; return *this; }
    Fix<s,T>& operator *=(const int other) { value *= other; return *this; }
    Fix<s,T>& operator *=(const Fix<s,T>& other) { value = (value * other.value) >> s; return *this; }
    Fix<s,T>& operator /=(const float other) { value /= other; return *this; }
    Fix<s,T>& operator /=(const int other) { value /= other; return *this; }
    Fix<s,T>& operator /=(const Fix<s,T>& other) { value = (value << s) / other.value; return *this; }

    constexpr bool operator >(const Fix<s,T>& other) { return value > other.value; }
    constexpr bool operator <(const Fix<s,T>& other) { return value < other.value; }
    template <typename U> constexpr bool operator >=(const U other) { return value >= other<<s; }
    constexpr bool operator >=(const Fix<s,T>& other) { return value >= other.value; }
    constexpr bool operator <=(const Fix<s,T>& other) { return value <= other.value; }
    constexpr bool operator ==(const Fix<s,T>& other) { return value == other.value; }
    constexpr bool operator !=(const Fix<s,T>& other) { return value != other.value; }

    template <typename U>
    constexpr U toInteger()
    {
        return value >> s;
    }

    constexpr float toFloat()
    {
        return static_cast<float>(value) / static_cast<float>(1<<s);
    }

    template <typename U>
    void fromInteger(const U v)
    {
        value = v << s;
    }

    void fromFloat(const float v)
    {
        value = static_cast<T>(v * static_cast<float>(1<<s));
    }

    template <unsigned int s2, typename T2>
    operator Fix<s2, T2>() const
    {
        Fix<s2, T2> ret;
        if(s2 > s)
            ret.value = value << (s2-s);
        else
            ret.value = value >> (s-s2);

        return ret;
    }

    Fix<s,T>& normaliseAngle()
    {
        while(*this < Fix<s,T>(0))
            *this += Fix<s,T>(360);

        while(*this >= 360)
            *this -= Fix<s,T>(360);

        return *this;
    }

    static Fix<s,T> minStep()
    {
        Fix<s,T> ret;
        ret.value = 1;
        return ret;
    }

    static Fix<s,T> minValue()
    {
        Fix<s,T> ret;
        ret.value = std::numeric_limits<T>::min();
        return ret;
    }

    static Fix<s,T> maxValue()
    {
        Fix<s,T> ret;
        ret.value = std::numeric_limits<T>::max();
        return ret;
    }

    void print() const
    {
        char str[32];

        double n = toFloat();

        if (n == 0.0)
            strcpy(str, "0");
        else {
            int digit, m, m1;
            char *c = str;
            int neg = (n < 0);
            if (neg)
                n = -n;
            m = log10(n);
            int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
            if (neg)
                *(c++) = '-';
            if (useExp) {
                if (m < 0)
                   m -= 1.0;
                n = n / pow(10.0, m);
                m1 = m;
                m = 0;}
            if (m < 1.0)
                m = 0;
            while (n > 0.001 || m >= 0) {
                double weight = pow(10.0, m);
                if (weight > 0 && !__builtin_isinf(weight)) {
                    digit = ::floor(n / weight);
                    n -= (digit * weight);
                    *(c++) = '0' + digit; }
                if (m == 0 && n > 0)
                    *(c++) = '.';
                m--; }
            if (useExp) {
                int i, j;
                *(c++) = 'e';
                if (m1 > 0) {
                    *(c++) = '+';
                } else {
                    *(c++) = '-';
                    m1 = -m1; }
                m = 0;
                while (m1 > 0) {
                    *(c++) = '0' + m1 % 10;
                    m1 /= 10;
                    m++; }
                c -= m;
                for (i = 0, j = m-1; i<j; i++, j--) {
                    c[i] ^= c[j];
                    c[j] ^= c[i];
                    c[i] ^= c[j];}
                c += m;}
            *(c) = '\0'; }

        fputs(str, stdout);
    }

    using type = T;
    constexpr static unsigned int precision = s;

    T value;
};

#endif // FIX_H
