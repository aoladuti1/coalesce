#include <bytestore.hpp>
#include <huffer.hpp>
#include <map>
#include <tests.hpp>

using namespace csc;

std::map<std::byte, std::size_t> getByteFrequencies(const ByteStore& bs) {
    auto byteMap = std::map<std::byte, std::size_t>();
    for (std::size_t i = 0; i < bs.size(); i++) {
        std::byte bsVal = bs.get(i);
        byteMap[bsVal] = byteMap[bsVal] + 1;
    }
    return byteMap;
}

int main() {
    __run_tests();
}
