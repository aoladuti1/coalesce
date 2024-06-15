#include <tests.hpp>

using namespace csc;
bool __run_tests() {
    auto n = ByteStore((std::byte) 0b01010101);
    n.pushEnd(0b01010101);
    n.pushEnd(0b01010100);
    n.writeToFile("okay.txt", false);
    ByteStore bs = ByteStore();
    auto m = processFile("okay.txt", bs);
    for(auto it = m.cbegin(); it != m.cend(); ++it) {
        (std::cout << (unsigned char) it->first 
            << " " << (std::size_t) it->second << "\n");
    }
    std::cout << bs;
    return true; // Visual Test 
}