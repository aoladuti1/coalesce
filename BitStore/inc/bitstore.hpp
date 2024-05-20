#include <iostream>
#include <bitset>
#include <fstream>
#include <stdexcept>
#ifndef BITSTORE
#define BITSTORE
typedef bool bit;
typedef char byte;

constexpr std::size_t BYTE_SIZE = 8;
// Immutable container of std::bitsize
template <std::size_t N>
class BitStore {
    std::bitset<N> bitset;
    const short remainder_bits = N % BYTE_SIZE;
    const bool exactByteFit = remainder_bits == 0;
    const std::size_t n_bytes = byteCount();
    
    private:
        void setByteInplace(std::size_t pos, byte b) {
            short limit = BYTE_SIZE;
            if (pos == n_bytes - 1){
                if (!exactByteFit) {
                    limit = remainder_bits;
                }
            } else if (pos >= n_bytes) {
                throw std::invalid_argument("pos greater than bytecount " + b);
            }
            auto byteHolder = std::bitset<BYTE_SIZE>(b);
            for (short i = 0; i < limit; i++) {
                bitset[i + (pos * BYTE_SIZE)] = byteHolder[i];
            }
        }
    public:
        BitStore() noexcept {
            bitset = std::bitset<N>();
        } 

        BitStore(std::size_t bits) noexcept {
            bitset = std::bitset<N>(bits);
        }

        BitStore(std::bitset<N>& bset) noexcept {
            bitset = bset;
        }

        BitStore(std::string inputFile) {
            const auto flags = std::ios::in | std::ios::binary;
            std::ifstream rf(inputFile, flags);
            if(!rf) {
                std::cerr << "Cannot open " + inputFile + "!" << std::endl;
                throw std::invalid_argument("Can't open file");
            }
            std::size_t pos = 0;
            for (byte b; rf >> b && pos < n_bytes; pos++) {
                setByteInplace(pos, b);
            }
            rf.close();
        } 

        template <std::size_t T> friend std::ostream& operator<<(
            std::ostream &os, const BitStore<T> &bs);

        bit operator[](std::size_t index) const {
            return bitset[index];
        }

        std::bitset<BYTE_SIZE> getByte(std::size_t pos) const {
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

        inline std::bitset<N> getBitset() const { return bitset; }
        constexpr std::size_t byteCount() const noexcept {
            std::size_t byteNo = N / BYTE_SIZE;
            return (N % BYTE_SIZE != 0) ? byteNo + 1 : byteNo;
        }

        bit get(std::size_t pos) const {
            return bitset[pos];
        }

        bit getLtoR(std::size_t pos) const {
            return get(N-1-pos);
        }

        byte* toByteArr(byte output[]) const {
            if (N == 0) { return output; }
            for (std::size_t i = 0; i < n_bytes; i++)
                output[i] = getByte(i).to_ulong();
            return output;
        }

        // returns 0 == good
        int writeBytesToFile(std::string outputFile, bool append) const {
            byte bytes[n_bytes];
            toByteArr(bytes);
            const auto flags = std::ios::out | std::ios::binary;
            std::ofstream wf(outputFile, append ? flags | std::ios_base::app : flags);
            if(!wf) {
                std::cerr << "Cannot open " + outputFile + "!" << std::endl;
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

        int writeBytesToFile(std::string outputFile) const {
            return writeBytesToFile(outputFile, false);
        }

};

/**
 * Send a copy of the internal bitset to an ostream.
*/
template <std::size_t N>
std::ostream& operator<<(std::ostream &os, const BitStore<N> &bs) {
    return os << std::bitset<N>(bs.bitset);
}

#endif