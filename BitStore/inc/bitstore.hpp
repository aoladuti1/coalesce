#include <iostream>
#include <bitset>
#include <fstream>
#ifndef BITSTORE
#define BITSTORE
typedef bool bit;
typedef char byte;

const std::size_t BYTE_SIZE = 8;
// Immutable container of std::bitsize
template <std::size_t N>
class BitStore {
    std::bitset<N> bitset;
    const short remainder_bits = N % BYTE_SIZE;
    const bool exactByteFit = remainder_bits == 0;
    const std::size_t n_bytes = byteCount();
    
    public:
        BitStore() noexcept;
        BitStore(std::size_t bits) noexcept;
        BitStore(std::bitset<N>& bset) noexcept;

        bit operator[](std::size_t index) const;
        template <std::size_t T> friend std::ostream& operator<<(
            std::ostream &os, const BitStore<T> &bs);

        inline std::bitset<N> getBitset() const { return bitset; }
        constexpr std::size_t byteCount() const noexcept {
            std::size_t byteNo = N / BYTE_SIZE;
            return (N % BYTE_SIZE != 0) ? byteNo + 1 : byteNo;
        }
        std::bitset<8> getByte(std::size_t pos) const;

        bit get(std::size_t pos) const;
        bit getLtoR(std::size_t pos) const;

        byte* toByteArr(byte output[]) const;
        int writeBytesToFile(std::string outputFile, bool append) const;
        int writeBytesToFile(std::string outputFile) const;

};
#endif