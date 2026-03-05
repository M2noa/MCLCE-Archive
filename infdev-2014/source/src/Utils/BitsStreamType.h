#pragma once

//struct to hold the value:
template<typename T> struct bits_t
{
    T value;
}; //no constructor necessary


//functions to infer type, construct bits_t with a member initialization list
//use a reference to avoid copying. The non-const version lets us extract too
template<typename T> bits_t<T&> bits(T& t)
{
    return bits_t<T&>{t};
}

template<typename T> bits_t<const T&> bits(const T& t)
{
    return bits_t<const T&>{t};
}

//insertion operator to call ::write() on whatever type of stream
template<typename S, typename T>
S& operator<<(S& stream, bits_t<T> bitsValue)
{
    return stream.write((char*)&bitsValue.value, sizeof(T));
}

//extraction operator to call ::read(), require a non-const reference here
template<typename S, typename T>
S& operator>>(S& stream, bits_t<T&> bitsValue) {
    return stream.read((char*)&bitsValue.value, sizeof(T));
}