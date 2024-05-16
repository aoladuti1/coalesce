#include <iostream>
#include "bitstore.h"

template <std::size_t N>
BitStore<N>::BitStore() noexcept {
    bitset = std::bitset<N>();
} 

template <std::size_t N>
BitStore<N>::BitStore(std::size_t bits) noexcept {
    bitset = std::bitset<N>(bits);
}

template <std::size_t N>
BitStore<N>::BitStore(std::bitset<N>& bset) noexcept {
    bitset = bset;
}

template <std::size_t N> 
bit BitStore<N>::operator[](std::size_t index) const {
    return bitset[index];
}

template <std::size_t N>
std::bitset<BYTE_SIZE> BitStore<N>::getByte(std::size_t pos) const {
    short limit = BYTE_SIZE;
    if (pos == n_bytes - 1){
        if (!exactByteFit) {
            limit = remainder_bits;
        }
    }
    auto byteHolder = std::bitset<BYTE_SIZE>();
    for (short i = 0; i < limit; i++) {
        byteHolder[i] = bitset[i + (pos * BYTE_SIZE)];
    }
    return byteHolder;
}


template <std::size_t N>
bit BitStore<N>::get(std::size_t pos) const {
    return bitset[pos];
}

template <std::size_t N>
bit BitStore<N>::getLtoR(std::size_t pos) const {
    return get(N-1-pos);
}

template <std::size_t N> 
byte* BitStore<N>::toByteArr(byte output[]) const {
    if (N == 0) { return output; }
    for (std::size_t i = 0; i < n_bytes; i++)
        output[i] = getByte(i).to_ulong();
    return output;
}

// returns 0 == good
template <std::size_t N>
int BitStore<N>::writeBytesToFile(std::string outputFile, bool append) const {
    byte bytes[n_bytes];
    toByteArr(bytes);
    const auto flags = std::ios::out | std::ios::binary;
    std::ofstream wf(outputFile, append ? flags | std::ios_base::app : flags);
    if(!wf) {
        std::cerr << "Cannot open file!" << std::endl;
        return 1;
    }
    for (std::size_t i = 0; i < n_bytes; i++) {
        wf.put(bytes[i]);
    }
    wf.close();
    if(!wf.good()) {
        std::cerr << "Error occurred at writing time!" << std::endl;
        return 1;
    }
    return 0;
}

template <std::size_t N>
int BitStore<N>::writeBytesToFile(std::string outputFile) const  {
   return writeBytesToFile(outputFile, false);
}

/**
 * Send a copy of the internal bitset to an ostream.
*/
template <std::size_t N>
std::ostream& operator<<(std::ostream &os, const BitStore<N> &bs) {
    return os << std::bitset<N>(bs.bitset);
}

int main() {
    unsigned long x = 0b1010101011;
    std::bitset bset = std::bitset<10>(x);
    BitStore bs = (BitStore<10>)(bset);
    const std::size_t n_bytes = bs.byteCount();
    byte y[n_bytes];
    bs.toByteArr(y);
    bs.writeBytesToFile("X.dat");
    auto bstrue = bs.getBitset();

    std::cout << (std::bitset<8>)(y[1]) << std::endl << n_bytes;
    return 0;
}