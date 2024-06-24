
#ifndef HUFFER
#define HUFFER
#include <map>
#include <bytestore.hpp>
#include <queue>
#include <filesystem>

constexpr std::size_t SIZE_T_BIT_SIZE = sizeof(std::size_t) * csc::BYTE_SIZE;
using namespace csc;

class HuffNode {
    public:
        HuffNode* left;
        HuffNode* right;
        std::byte data;
        std::size_t freq;

        HuffNode(std::byte data, std::size_t freq);

        HuffNode(HuffNode* leftChild, HuffNode* rightChild);

};

// Custom comparator class
class Compare {
public:
    bool operator()(HuffNode* a, HuffNode* b) {
        return a->freq > b->freq;
    }
};

std::vector<std::byte> stringToPaddedBytes(std::string str);

void encodeFrequencies(
    HuffNode* root, std::string code, std::map<std::byte, std::string>& output) ;

void encodeFrequencies(
    HuffNode* root, std::map<std::byte, std::string>& codeTable);

template <class A, class B, class C>
A topPop(std::priority_queue<A, B, C>& pq) {
         auto ret = pq.top();
         pq.pop();
         return ret;
}

template <std::size_t N>
std::vector<std::byte> bitsetToByteVect(std::bitset<N> bset) {
    if (N % BYTE_SIZE != 0) {
        throw std::invalid_argument(
            "N must be divisible by the number of bits in a byte");
    }
    auto ret = std::vector<std::byte>();
    for (std::size_t i = 0; i < N;) {
        auto nextByte = std::bitset<BYTE_SIZE>();
        for (int j = 0; j < BYTE_SIZE && i < N; j++, i++) {
            nextByte[BYTE_SIZE - 1 - j] = bset[N - 1 - i];
        }
        ret.push_back((std::byte) nextByte.to_ulong());
    }
    return ret;
}

HuffNode* genTree(std::priority_queue<HuffNode*,
                       std::vector<HuffNode*>,Compare>& pq);

void delTree(HuffNode* root);

std::map<std::byte, std::size_t> getByteFrequencies(const ByteStore& bs);

std::string padByteCode(const std::string code);

std::vector<std::byte> genHeaderBytes(
    std::string ext, std::size_t n_total_chars, 
    const std::map<std::byte, std::string>& codeTable);

std::size_t getTotalFrequency(std::map<std::byte, std::size_t> codeFreqs);

std::vector<std::string> filepathsInDir(std::string dir);

std::map<std::byte, std::size_t> processFile(
    const std::string inputFile, ByteStore& output);

#endif