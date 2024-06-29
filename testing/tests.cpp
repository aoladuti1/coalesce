#include <tests.hpp>

using namespace csc;

bool _printPassAndReturn(std::string name, bool success) {
    std::cout << name << " -> " << ((success) ? "passed" : "failed") << "\n";
    return success;
}

bool _HeaderEncodeTest() {
    auto x = std::map<std::byte, std::size_t>();
    x[(std::byte)'a'] = 8;
    x[(std::byte)'b'] = 9;
    x[(std::byte)'c'] = 12;
    x[(std::byte)'d'] = 13;
    x[(std::byte)'e'] = 16;
    x[(std::byte)'f'] = 45;
    auto codex = std::map<std::byte, std::string>();
    HuffNode* root = newTree(x);
    encodeFrequencies(root, codex);
    auto header = genHeaderBytes(".mp3", getTotalFrequency(x), codex);
    std::string testString = "0000000000000000000"
    "0000000000000000000000000000000000000011001"
    "11000001000010111001101101011100000011001100"
    "0001100110000100000100111000000110001000000100"
    "11110000011000110000001110000000011001000000"
    "001110100000011001010000001111000000011001100000000100000000";
    std::string str = "";
    for (int i = 0; i < 32; i++)
        str += std::bitset<8>((int)header.at(i)).to_string();
    bool success = str == testString && header.size() == 32;
    std::size_t j = 0;
    delTree(root);
    return _printPassAndReturn("HeaderEncodeTest", success);
}

bool _FileWrite() {
    std::string file = "x.txt";
    std::filesystem::path p = std::filesystem::path(file);    
    std::string ext = p.extension().string();
    std::size_t total_chars = 0;
    std::map<std::byte, std::size_t> freqTable = getByteFrequencies(file, total_chars);
    HuffNode* root = newTree(freqTable);
    auto codeTable = std::map<std::byte, std::string>();
    encodeFrequencies(root, codeTable);
    auto header = genHeaderBytes(ext, total_chars, codeTable);
    for (const auto& x : codeTable)
        std::cout << ((unsigned char) x.first) << " " << (x.second) << "\n";
    header.writeToFile("x.csc", false);
    return true;
}

bool _RunTests() {
    auto successTracker = std::vector<bool>();
    successTracker.push_back(_HeaderEncodeTest());
    successTracker.push_back(_FileWrite());
    for (auto x : successTracker)
        if (x == false) return false;
    return _printPassAndReturn("All tests", true);
}