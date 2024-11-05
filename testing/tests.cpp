#include <tests.hpp>

bool _printPassAndReturn(std::string name, bool success) {
    std::cout << name << " -> " << ((success) ? "passed" : "failed") << "\n";
    return success;
}

bool _AllWriteTest() {
    std::string stem = "y";
    std::string ext = ".txt";
    writeCompFile(stem + ext, stem + ".csc", true);
    writeDecompFile(stem + ".csc", stem + ".jpg", true);
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
    successTracker.push_back(_AllWriteTest());
    for (auto x : successTracker)
        if (x == false) return false;
    return _printPassAndReturn("All tests", true);
}