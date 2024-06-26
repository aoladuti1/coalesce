#include "bytestore.hpp"

using namespace csc;

void ByteStore::readFromFile(const std::string inputFile) {
    std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
    if(!rf) {
        throw std::invalid_argument("Can't open file " + inputFile);
    }
    for (unsigned char b; rf >> b;) {
        store.push_back((std::byte)b);
    }
    rf.close();
}

void ByteStore::writeToFile(const std::string outputFile,
                            const bool append) const {
    const auto flags = std::ios::out | std::ios::binary;
    std::ofstream wf(outputFile, append ? flags | std::ios::app : flags);
    if(!wf) {
        throw std::invalid_argument("Can't open file " + outputFile);
    }
    std::size_t sizeVal = size();
    for (std::size_t i = 0; i < sizeVal; i++) {
        wf.put((unsigned char) at(i));
    }
    wf.close();
}

ByteStore::ByteStore() noexcept {
    store = std::vector<std::byte>();
}

ByteStore::ByteStore(const std::size_t nBytes) noexcept {
    store = std::vector<std::byte>(nBytes);
}

ByteStore::ByteStore(const std::string bitStr) noexcept {
    store = std::vector<std::byte>();
    extend(bitStr);
}

ByteStore::ByteStore(const std::byte& b) noexcept {
    store = std::vector<std::byte>();
    store.push_back(b);
}

std::string ByteStore::to_string() const {
    std::string ret = "";
    for (std::size_t i = 0; i < size(); i++) {
        ret += (unsigned char) at(i);
    }
    return ret;    
}

void ByteStore::set(const std::size_t pos, const unsigned char b) {
    store[pos] = (std::byte) b;
}

void ByteStore::set(const std::size_t pos, const std::byte byte) {
    store[pos] = byte;
}

void ByteStore::set(const std::size_t pos, const std::bitset<BYTE_SIZE> bset) {
    store[pos] = (std::byte) bset.to_ulong();
}

const std::byte ByteStore::at(const std::size_t pos) const {
    return store.at(pos);
}

std::byte ByteStore::at(const std::size_t pos) {
    return store.at(pos);
}

const std::bitset<BYTE_SIZE> ByteStore::getBitset(const std::size_t pos) const {
    return std::bitset<BYTE_SIZE>((unsigned char) store.at(pos));
}

std::bitset<BYTE_SIZE> ByteStore::getBitset(const std::size_t pos) {
    return std::bitset<BYTE_SIZE>((unsigned char) store.at(pos));
}

bool ByteStore::getBit(const std::size_t pos) const {
    const std::bitset<BYTE_SIZE> bset = getBitset(pos / BYTE_SIZE);
    return bset[BYTE_SIZE-1-(pos % BYTE_SIZE)];
}

void ByteStore::insert(const std::size_t pos, const std::byte& byte) {
    store.insert(store.begin() + pos, byte);
}

void ByteStore::insert(const std::size_t pos, const std::bitset<BYTE_SIZE>& bset) {
    insert(pos, (std::byte) bset.to_ulong());
}

void ByteStore::insert(const std::size_t pos, const unsigned char& byte) {
    insert(pos, (std::byte) byte);
}

void ByteStore::push_back(const std::byte& byte) {
    store.push_back(byte);
}

void ByteStore::push_back(const std::bitset<BYTE_SIZE>& bset) {
    push_back((std::byte) bset.to_ulong());
}

void ByteStore::push_back(const unsigned char& b) {
    push_back((std::byte) b);
}

std::byte ByteStore::top_pop_back() {
    auto ret = store.back();
    store.pop_back();
    return ret;
}

std::bitset<BYTE_SIZE> ByteStore::top_pop_back_bitset() {
    return std::bitset<BYTE_SIZE>((std::size_t) top_pop_back());
}

void ByteStore::extend(const ByteStore& bs) {
    store.insert(std::end(store), std::begin(bs.store), std::end(bs.store));
}

void ByteStore::extend(std::string bitStr) {
    for (int i = 0; i < bitStr.length();) {
        auto nextByte = std::bitset<BYTE_SIZE>();
        for (int j = 0; j < BYTE_SIZE && i < bitStr.length(); j++, i++) {
            nextByte[BYTE_SIZE - 1 - j] = std::stoi(std::string{bitStr[i]});
        }
        push_back(nextByte);
    }  

}

std::byte& ByteStore::operator[](const std::size_t i) {
    return store[i];
}

const std::byte& ByteStore::operator[](const std::size_t i) const {
    return store[i];
}


namespace csc {

    bool operator==(const ByteStore& bs1, const ByteStore& bs2) {
        return bs1.store == bs2.store;
    }   
    
    std::ostream& operator<<(std::ostream& os, const ByteStore& bs) {
        if (bs.size() > 0) {
            unsigned char b = (unsigned char) bs.at(0);
            for (std::size_t i = 1; os << b && i < bs.size(); i++) {
                b = (unsigned char) bs.at(i);
            }
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, ByteStore& bs) {
        for (unsigned char b; is >> b;) {
            bs.push_back((std::byte)b);
        }
        return is;
    }
}