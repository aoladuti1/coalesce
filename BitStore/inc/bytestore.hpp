#include <iostream>
#include <bitset>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <limits>
#ifndef BITSTORE
#define BITSTORE

namespace csc {

    constexpr std::size_t BYTE_SIZE = std::numeric_limits<unsigned char>::digits;
    
    template <std::size_t N>
    constexpr std::size_t getByteCount(const std::bitset<N>& bset) noexcept {
        std::size_t byteNo = N / BYTE_SIZE;
        return (N % BYTE_SIZE != 0) ? byteNo + 1 : byteNo;
    }

    template <std::size_t N>
    std::byte getByte(std::size_t pos, const std::bitset<N>& bset) {
        short limit = BYTE_SIZE;
        if (pos == getByteCount(bset)-1){
            constexpr short remainder_bits = N % 8;
            if (!(remainder_bits == 0)) {
                limit = remainder_bits;
            }
        }
        auto byteHolder = std::bitset<BYTE_SIZE>();
        for (short i = 0; i < limit; i++) {
            byteHolder[i] = bset[i + (pos * BYTE_SIZE)];
        }
        return (std::byte) byteHolder.to_ulong();
    }
    
    class ByteStore {
        public:
            std::vector<std::byte> store;

            ByteStore() noexcept;
            ByteStore(const std::size_t n_bytes) noexcept;
            template <std::size_t N>
            ByteStore(const std::bitset<N>& bset) {
                std::size_t numBsetBytes = getByteCount(bset);
                store = std::vector<std::byte>(numBsetBytes);
                std::size_t size_ = size();
                for (std::size_t i = 0; i < numBsetBytes; i++) {
                    set(size_-1-i, getByte(i, bset));
                }                    
            }
            ByteStore(const std::string inputFile);
            ByteStore(const std::byte b);
            virtual ~ByteStore() {};

            std::size_t size() const { return store.size(); }

            void set(const std::size_t pos, const unsigned char byte);
            void set(const std::size_t pos, std::byte byte);
            void set(const std::size_t pos, const std::bitset<BYTE_SIZE>);
            std::byte get(const std::size_t pos) const;
            std::bitset<BYTE_SIZE> getBitset(const std::size_t pos) const;

            void push(const std::size_t pos, const unsigned char byte);
            void push(const std::size_t pos, std::byte byte);
            void push(const std::size_t pos, const std::bitset<BYTE_SIZE> bs);
            std::byte pop();
            std::bitset<BYTE_SIZE> popBitset();

            void extend(ByteStore& bs);

            void readFromFile(std::string inputFile);
            void writeToFile(std::string outputFile, bool append = true);

            std::byte&       operator[](std::size_t i);
            const std::byte& operator[](std::size_t i) const;

            friend std::ostream&  operator<<(std::ostream& os, const ByteStore& bs);
            friend std::ostream&  operator>>(const ByteStore& bs, std::ostream& os);
            
            friend std::ofstream& operator<<(std::ofstream& ofs, const ByteStore& bs);
            friend std::ofstream& operator>>(const ByteStore& bs, std::ofstream& ofs);
            
            friend std::istream&  operator>>(std::istream& is, ByteStore& bs);
            friend std::istream&  operator<<(ByteStore& bs, std::istream& is);

            friend std::ifstream& operator>>(std::ifstream& ifs, ByteStore& bs);
            friend std::ifstream& operator<<(ByteStore& bs, std::ifstream& ifs);

    };
}

#endif