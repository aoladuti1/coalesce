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
    std::string testString = "00000000000000000000000000000000000000000000000"
        "00000000001100111000001000010111001101101011100000011001100000000000"
        "00110011000010000010011100000011000100000010011110000011000110000001"
        "11000000001100100000000111010000001100101000000111100000001100110000"
        "0000100000000";
    std::string str = "";
    for (int i = 0; i < header.size(); i++)
        str += std::bitset<8>((int)header.at(i)).to_string();
    bool success = str == testString;
    std::size_t j = 0;
    delTree(root);
    return _printPassAndReturn("HeaderEncodeTest", success);
}

bool _FileWriteTest() {
    std::string file = "x.txt";
    std::filesystem::path p = std::filesystem::path(file);    
    std::string ext = p.extension().string();
    std::size_t total_chars = 0;
    std::map<std::byte, std::size_t> freqTable = getByteFrequencies(file, total_chars);
    HuffNode* root = newTree(freqTable);
    auto codeTable = std::map<std::byte, std::string>();
    encodeFrequencies(root, codeTable);
    auto header = genHeaderBytes(ext, total_chars, codeTable);
    bool succ = codeTable[(std::byte) 'a'] == "0" && codeTable[(std::byte) 'b'] == "1";
    header.writeToFile("x.csc", false);
    return _printPassAndReturn("FileWriteTest", succ);
}

bool _AllWriteTest() {
    std::string stem = "y";
    std::string ext = ".txt";
    writeCodesToFile(stem + ext, stem + ".csc");
    writeDecodedFile(stem + ".csc", stem + ".jpg");
    std::ifstream rf1(stem + ext, std::ios::binary | std::ios::in);
    std::ifstream rf2(stem + ".jpg", std::ios::binary | std::ios::in);
    char b1, b2;
    bool success = true;
    while (rf1.get(b1)) {
        rf2.get(b2);
        if ( b1 != b2 ) {
            success = false;
        }
    }
    return _printPassAndReturn("AllWriteTest", success);
}

bool _RunTests() {
    auto successTracker = std::vector<bool>();
    successTracker.push_back(_HeaderEncodeTest());
    successTracker.push_back(_FileWriteTest());
    successTracker.push_back(_AllWriteTest());
    for (auto x : successTracker)
        if (x == false) return false;
    return _printPassAndReturn("All tests", true);
}