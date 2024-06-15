#include <map>
#include <huffer.hpp>

using namespace csc;

HuffNode::HuffNode(std::byte data, std::size_t freq) { 
    this->data = data;
    this->freq = freq;
    left = right = nullptr;
}

HuffNode::HuffNode() {
    this->freq = 0;
    left = right = nullptr;
}

class Compare {
    public:
        bool operator()(const HuffNode& a, const HuffNode& b) {
            return a.freq > b.freq;
        }
};

std::map<std::byte, std::size_t> csc::processFile(
    const std::string inputFile, ByteStore& output) {
        std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
        auto byteMap = std::map<std::byte, std::size_t>();
        if(!rf) {
            throw std::invalid_argument("Can't open file " + inputFile);
        }
        for (unsigned char b; rf >> b;) {
            std::byte castb = (std::byte) b;
            output.pushEnd(castb);
            byteMap[castb] = byteMap[castb] + 1;
        }
        rf.close();
        return byteMap;
}
