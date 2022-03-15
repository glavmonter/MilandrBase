#pragma once

template<bool condition, class TypeIfTrue, class TypeIfFalse>
struct StaticIf
{
    typedef TypeIfTrue Result;
};

template<class TypeIfTrue, class TypeIfFalse>
struct StaticIf<false, TypeIfTrue, TypeIfFalse>
{
    typedef TypeIfFalse Result;
};


template<unsigned sizeBits>
struct SelectSize
{
    static const bool LessOrEq8 = sizeBits <= 8;
    static const bool LessOrEq16 = sizeBits <= 16;

    typedef typename StaticIf<
            LessOrEq8,
            uint8_t,
            typename StaticIf<LessOrEq16, uint16_t, uint32_t>::Result>
            ::Result Result;
};


template<unsigned size>
struct SelectSizeForLength
{
    static const bool LessOrEq8 = size <= 0xFF;
    static const bool LessOrEq16 = size <= 0xFFFF;

    typedef typename StaticIf<
            LessOrEq8,
            uint8_t,
            typename StaticIf<LessOrEq16, uint16_t, uint32_t>::Result>
            ::Result Result;
};

