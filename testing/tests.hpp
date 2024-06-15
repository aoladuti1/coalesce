#include <bytestore.hpp>
#include <map>
#include <huffer.hpp>
bool __run_tests();
std::map<std::byte, std::size_t> getByteFrequencies(const csc::ByteStore& bs);
