#include <bytestore.hpp>
#include <map>
#include <huffer.hpp>
bool _HeaderEncodeTest();
bool _RunTests();
std::map<std::byte, std::size_t> getByteFrequencies(const csc::ByteStore& bs);
