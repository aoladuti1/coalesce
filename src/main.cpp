#include <bytestore.hpp>

using namespace csc;
int main() {
    auto x = ByteStore();
    const auto j = std::bitset<BYTE_SIZE>(0b00000001);
    x.pushEnd((const std::byte) 0b00000001);
    std::cout << x; // all smiles xxx
}