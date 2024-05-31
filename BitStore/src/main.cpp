#include "bytestore.hpp"

using namespace csc;

void ByteStore::readFromFile(std::string inputFile) {
    std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
    if(!rf) {
        throw std::invalid_argument("Can't open file " + inputFile);
    }
    for (unsigned char b; rf >> b;) {
        store.push_back((std::byte)b);
    }
    rf.close();
}

void ByteStore::writeToFile(std::string outputFile,
                                 bool append) const {
    const auto flags = std::ios::out | std::ios::binary;
    std::ofstream wf(outputFile, append ? flags | std::ios::app : flags);
    if(!wf) {
        throw std::invalid_argument("Can't open file " + outputFile);
    }
    std::size_t sizeVal = size();
    for (std::size_t i = 0; i < sizeVal; i++) {
        wf.put((unsigned char) get(i));
    }
    wf.close();
}

ByteStore::ByteStore() noexcept {
    store = std::vector<std::byte>();
}

ByteStore::ByteStore(std::size_t nBytes) noexcept {
    store = std::vector<std::byte>(nBytes);
}

ByteStore::ByteStore(const std::string inputFile) noexcept {
    store = std::vector<std::byte>();
    try {
        readFromFile(inputFile);
    } catch (...) {
        std::cerr << "Empty ByteStore init. due to failure to open " 
                    + inputFile << "\n";
    }
}

ByteStore::ByteStore(const std::byte b) noexcept {
    store = std::vector<std::byte>();
    store.push_back(b);
}

std::string ByteStore::toString() const {
    std::string ret = "";
    for (std::size_t i = 0; i < size(); i++) {
        ret += (unsigned char) get(i);
    }
    return ret;    
}

void ByteStore::set(std::size_t pos, const unsigned char b) {
    store[pos] = (std::byte) b;
}

void ByteStore::set(std::size_t pos, const std::byte byte) {
    store[pos] = byte;
}

void ByteStore::set(std::size_t pos, const std::bitset<BYTE_SIZE> bset) {
    store[pos] = (std::byte) bset.to_ullong();
}

const std::byte ByteStore::get(std::size_t pos) const {
    return store.at(pos);
}

std::byte ByteStore::get(std::size_t pos) {
    return store.at(pos);
}

const std::bitset<BYTE_SIZE> ByteStore::getBitset(std::size_t pos) const {
    return std::bitset<BYTE_SIZE>((unsigned char) store.at(pos));
}

std::bitset<BYTE_SIZE> ByteStore::getBitset(std::size_t pos) {
    return std::bitset<BYTE_SIZE>((unsigned char) store.at(pos));
}

bool ByteStore::getBit(std::size_t pos) const {
    const std::bitset<BYTE_SIZE> bset = getBitset(pos / BYTE_SIZE);
    return bset[BYTE_SIZE-1-(pos % BYTE_SIZE)];
}

inline void ByteStore::insert(std::size_t pos, const std::byte byte) {
    store.insert(store.begin() + pos, byte);
}

inline void ByteStore::insert(std::size_t pos, const std::bitset<BYTE_SIZE> bset) {
    insert(pos, (std::byte) bset.to_ullong());
}

inline void ByteStore::insert(std::size_t pos, unsigned char byte) {
    insert(pos, (std::byte) byte);
}

inline void ByteStore::pushEnd(const std::byte byte) {
    store.push_back(byte);
}

inline void ByteStore::pushEnd(const std::bitset<BYTE_SIZE> bset) {
    pushEnd((std::byte) bset.to_ullong());
}

inline void ByteStore::pushEnd(unsigned char b) {
    pushEnd((std::byte) b);
}

inline std::byte ByteStore::popEnd() {
    auto ret = store.back();
    store.pop_back();
    return ret;
}

inline std::bitset<BYTE_SIZE> ByteStore::popEndBitset() {
    return std::bitset<BYTE_SIZE>((unsigned long long) popEnd());
}

inline void ByteStore::extend(const ByteStore& bs) {
    store.insert(std::end(store), std::begin(bs.store), std::end(bs.store));
}

std::byte& ByteStore::operator[](std::size_t i) {
    return store[i];
}

const std::byte& ByteStore::operator[](std::size_t i) const {
    return store[i];
}

namespace csc {
    std::ostream& operator<<(std::ostream& os, const ByteStore& bs) {
        if (bs.size() > 0) {
            unsigned char b = (unsigned char) bs.get(0);
            for (std::size_t i = 1; os << b && i < bs.size(); i++) {
                b = (unsigned char) bs.get(i);
            }
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, ByteStore& bs) {
        for (unsigned char b; is >> b;) {
            bs.pushEnd((std::byte)b);
        }
        return is;
    }
}