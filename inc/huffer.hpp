
#ifndef HUFFER
#define HUFFER
#include <map>
#include <queue>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <bitset>
#include <climits>

constexpr std::size_t IO_BUFFER_SIZE = 512; // 512 bytes

class HuffNode {
    public:
        HuffNode* left;
        HuffNode* right;
        std::byte data;
        std::size_t freq;

        HuffNode(const std::byte character, const std::size_t frequency);

        HuffNode(HuffNode* leftChild, HuffNode* rightChild);

};

// Custom comparator class
class Compare {
public:
    bool operator()(HuffNode* a, HuffNode* b) {
        return a->freq > b->freq;
    }
};

enum class ENDIAN
{
#if defined(_MSC_VER) && !defined(__clang__)
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

std::vector<std::byte> stringToPaddedBytes(const std::string str);

std::size_t fillBuffer(
    std::byte* arr, const std::size_t arrLen, const std::string code);

void encodeFrequencies(
    HuffNode* root, std::string code, std::map<std::byte, std::string>& output);

void encodeFrequencies(
    HuffNode* root, std::map<std::byte, std::string>& codeTable);

template <class A, class B, class C>
A topPop(std::priority_queue<A, B, C>& pq) {
         auto top = pq.top();
         pq.pop();
         return top;
}

inline bool isTreeLeaf(HuffNode* node) {
    return node->left == nullptr;
}

HuffNode* newTree(const std::map<std::byte, std::size_t>& freqTable);

void delTree(HuffNode* root);

std::map<std::byte, std::size_t> getByteFrequencies(
    std::ifstream& rf, std::size_t& counter);

std::map<std::byte, std::size_t> getByteFrequencies(
    const std::string inputFile, std::size_t& counter);

std::string padByteCode(const std::string code);

inline std::size_t minByteCount(const std::size_t nBits) {
    return (nBits % CHAR_BIT != 0) ? (nBits / CHAR_BIT) + 1 : (nBits / CHAR_BIT);
}

std::vector<std::byte> genHeaderBytes(
    const std::string ext, const std::size_t n_total_chars, 
    const std::map<std::byte, std::string>& codeTable);

std::vector<std::string> filepathsInDir(const std::string dir);

void writeToFile(const std::vector<std::byte>& bytes,
                 const std::string outputFile, const bool append);

void writeCodesToFile(
    const std::string inputFile, const std::string outputFile);
void writeDecodedFile(
    const std::string inputFile, const std::string outputFile);

#endif