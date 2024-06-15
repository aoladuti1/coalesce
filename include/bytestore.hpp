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
    std::byte getByte(const std::size_t pos, const std::bitset<N>& bset) {
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
        return (std::byte) byteHolder.to_ullong();
    }
    // TODO: in namespace csc declare/define byteToBitset()

    class ByteStore {
        public:
            std::vector<std::byte> store;

            inline std::size_t size()    const { return store.size(); }
            inline std::size_t bitsize() const { return size() * BYTE_SIZE; }

            void readFromFile(const std::string inputFile);
            void writeToFile(const std::string outputFile, 
                                    const bool append = true) const;

            ByteStore() noexcept;
            ByteStore(const std::size_t n_bytes) noexcept;
            template <std::size_t N>
            ByteStore(const std::bitset<N>& bset) {
                std::size_t numBsetBytes = getByteCount(bset);
                store = std::vector<std::byte>(numBsetBytes);
                std::size_t sizeVal = size();
                for (std::size_t i = 0; i < numBsetBytes; i++) {
                    set(sizeVal-1-i, getByte(i, bset));
                }                    
            }
            ByteStore(const std::string inputFile) noexcept;
            ByteStore(const std::byte& b) noexcept;
            virtual ~ByteStore() {};

            std::string toString() const;

            void set(const std::size_t pos, const unsigned char byte);
            void set(const std::size_t pos, const std::byte byte);
            void set(const std::size_t pos, const std::bitset<BYTE_SIZE>);

            const std::byte get(const std::size_t pos) const;
            std::byte get(const std::size_t pos);

            const std::bitset<BYTE_SIZE> getBitset(const std::size_t pos) const;
            std::bitset<BYTE_SIZE> getBitset(const std::size_t pos);

            bool getBit(const std::size_t pos) const;
            
            void insert(const std::size_t pos, const std::byte& byte);
            void insert(const std::size_t pos, const std::bitset<BYTE_SIZE>& bset);
            void insert(const std::size_t pos, const unsigned char& byte);

            void pushEnd(const std::byte& byte);
            void pushEnd(const std::bitset<BYTE_SIZE>& bset);
            void pushEnd(const unsigned char& byte);

            std::byte popEnd();
            
            std::bitset<BYTE_SIZE> popEndBitset();

            void extend(const ByteStore& bs);

            std::byte&       operator[](const std::size_t i);
            const std::byte& operator[](const std::size_t i) const;

            friend std::ostream& operator<<(std::ostream& os, const ByteStore& bs);
            
            friend std::istream& operator>>(std::istream& is, ByteStore& bs);

    };
}

#endif