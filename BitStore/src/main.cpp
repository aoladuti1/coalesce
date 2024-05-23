#include "bytestore.hpp"

csc::ByteStore::ByteStore() noexcept {
    store = std::vector<std::byte>();
}

void csc::ByteStore::set(const std::size_t pos, const unsigned char b) {
    store[pos] = (std::byte) b;
}

void csc::ByteStore::set(const std::size_t pos, const std::byte byte) {
    store[pos] = byte;
}

csc::ByteStore::ByteStore(const std::string inputFile) {
    const auto flags = std::ios::in | std::ios::binary;
    std::ifstream rf(inputFile, flags);
    if(!rf) {
        std::cerr << "Cannot open " + inputFile + "!" << std::endl;
        throw std::invalid_argument("Can't open file");
    }
    store = std::vector<std::byte>();
    std::size_t pos = 0;
    for (unsigned char b; rf >> b; pos++) {
        store.push_back((std::byte)b);
    }
    rf.close();
}