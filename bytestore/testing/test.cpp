#include <bytestore.hpp>

using namespace csc;
int main() {
    std::ifstream rf("x.txt", std::ios::in | std::ios::binary);
    auto y = ByteStore(std::bitset<BYTE_SIZE>(0b01010100));
    std::cout << (int) y[0] << "\n";
    rf >> y;
    std::cout << (char) y[1] << (char) y[2] << "\n";
    std::cout << "Should print Tab: " << y;
    std::ofstream("y.txt", std::ios::out | std::ios::binary) << y;
}