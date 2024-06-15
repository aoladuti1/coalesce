
#ifndef HUFFER
#define HUFFER
#include <map>
#include <bytestore.hpp>
namespace csc {
    class HuffNode {
        public:
            HuffNode* left;
            HuffNode* right;
            std::byte data;
            std::size_t freq;

            HuffNode(std::byte data, std::size_t freq);

            HuffNode();
    };

std::map<std::byte, std::size_t> processFile(
    const std::string inputFile, ByteStore& output);
}
#endif