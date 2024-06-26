#include <bytestore.hpp>

using namespace csc;
int main() {
    auto y = ByteStore(std::bitset<BYTE_SIZE>(0b01010100));
    std::cout << (int) y[0] << "\n";
    std::cout << (int) ByteStore("01010100")[0] << "\n";
    auto x = ByteStore();
    x.readFromFile("x.txt");
    std::cout << x;

}