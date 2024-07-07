#include <map>
#include <huffer.hpp>
#include <queue>
#include <cstring>
#include <algorithm>

using namespace csc;

// TODO: erase ByteStore? change constant names. 
// Ensure the 0xFF for numunique uses a byte_size agnostic mask
// e.g. unsigned char = (1 << BYTESIZE) - 1 (try uint if not working)
// Refactor, speedup.

HuffNode::HuffNode(std::byte character, std::size_t frequency) {
    data = character;
    freq = frequency;
    left = right = nullptr;
}

HuffNode::HuffNode(HuffNode* leftChild, HuffNode* rightChild) {
    left = leftChild;
    right = rightChild;
    freq = (left->freq) + (right->freq); 
}

void encodeFrequencies(
    HuffNode* root, std::string code, 
    std::map<std::byte, std::string>& output) {
    if (root == nullptr)
        return;
    auto left = root->left, right = root->right;
    if (left != nullptr)
        encodeFrequencies(root->left, code + "0", output);
    if (right != nullptr)
        encodeFrequencies(root->right, code + "1", output);
    if (left == nullptr && right == nullptr)
        output[root->data] = code;
}

// Encoding Tree
HuffNode* newTree(std::map<std::byte, std::size_t>& freqTable) {
    auto pq = std::priority_queue<HuffNode*, std::vector<HuffNode*>, Compare>();
    for (auto it = freqTable.begin(); it != freqTable.end(); it++)
        pq.push(new HuffNode(it->first,  it->second));
    while (pq.size() > 1) {
        auto lChild = topPop(pq);
        auto rChild = topPop(pq);
        pq.push(new HuffNode(lChild, rChild));
    }
    return pq.top();
}

HuffNode* remakeTree(std::map<std::byte, std::string>& codeTable) {
    HuffNode* root = new HuffNode((std::byte)0,0);
    HuffNode* curNode = root;
    for (auto it = codeTable.begin(); it != codeTable.end(); it++) {
        HuffNode* curNode = root;
        std::byte character = it->first;
        std::string code = it->second;
        for (unsigned short i = 0; i < code.length(); i++) {
            if (code[i] == '0') {
                if ((curNode->left) == nullptr) {
                    curNode->left = new HuffNode((std::byte) 0, 0L);
                }
                curNode = curNode->left;
            } else if (code[i] == '1'){
                if ((curNode->right) == nullptr) {
                    curNode->right = new HuffNode((std::byte) 0, 0L);
                }
                curNode = curNode->right;
            }
        }
        curNode->data = character;
    }
    return root;
}

void delTree(HuffNode* root) {
    if (root == nullptr)
        return;
    delTree(root->left);
    delTree(root->right);
    delete root;
}

std::map<std::byte, std::size_t> getByteFrequencies(
    std::ifstream& rf, std::size_t& counter) {
    auto byteMap = std::map<std::byte, std::size_t>();
    if(!rf) {
        throw std::invalid_argument("Can't open stream");
    }
    char b = 0;
    while(rf.get(b)) {
        std::byte byteB = (std::byte) b;
        byteMap[byteB] = byteMap[byteB] + 1;
        counter++;
    }
    rf.close();
    return byteMap;
    }

std::map<std::byte, std::size_t> getByteFrequencies(
    const std::string inputFile, std::size_t& counter) {
    auto byteMap = std::map<std::byte, std::size_t>();
    std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
    if(!rf) {
        throw std::invalid_argument("Can't open file " + inputFile);
    }
    char b = 0;
    while (rf.get(b)) {
        std::byte byteB = (std::byte) b;
        byteMap[byteB] = byteMap[byteB] + 1;
        counter++;
    }
    rf.close();
    return byteMap;
}

std::string padByteCode(const std::string code) {
    int rem = code.size() % BYTE_SIZE;
    int bext = 0;
    std::string ret = code;
    if (rem != 0)
        bext = BYTE_SIZE - rem;
    for (int i = 0; i < bext; i++)
        ret = ret + "0";
    return ret;
}

std::size_t paddedByteCount(std::size_t bits) {
    std::size_t byteNo = bits / BYTE_SIZE;
    return (bits % BYTE_SIZE != 0) ? byteNo + 1 : byteNo;
}

ByteStore stringToPaddedBytes(std::string str) {
    std::size_t byteNo = str.length() / BYTE_SIZE;
    auto ret = ByteStore((str.length() % BYTE_SIZE != 0) ? byteNo + 1 : byteNo);
    for (std::size_t i = 0, bcount = 0; i < str.length(); bcount += 1) {
        unsigned char nextByte = 0;
        for (std::size_t j = 0; j < BYTE_SIZE && i < str.length(); j++, i++)
            if (str[i] == '1')
                nextByte |= (1 << (BYTE_SIZE - 1 - j));
        ret.set(bcount, nextByte);
    }  
    return ret;
}

std::size_t fillBuffer(std::string code, std::byte* arr, std::size_t arrLen) {
    std::size_t bcount = 0;
    for (std::size_t i = 0; bcount < arrLen; bcount++) {
        unsigned char nextByte = 0;
        for (std::size_t j = 0; j < BYTE_SIZE; j++, i++) {
            if (code[i] == '1')
                nextByte |= (1 << (BYTE_SIZE - 1 - j));
        }
        arr[bcount] = (std::byte) nextByte;
    }
    return bcount;
}

void encodeFrequencies(
    HuffNode* root, std::map<std::byte, std::string>& codeTable) {
    encodeFrequencies(root, "", codeTable);
}

/*
HEADER (ITEM [BYTE LENGTH OF ITEM] (NOTE: NO NEWLINES IN HEADER))===
NUMBER_CHARS_TOTAL [8]
NUM_EXT_CHARS [1] EXT_CHARS [NUM_EXT_CHARS]
NUM_UNIQUE_CHARS [2]
(CHAR [1] CHAR_CODE_LENGTH [1] 
    PADDEDCODE [MIN BYTES TO STORE CHAR_CODE_LENGTH BITS])[NUM_UNIQUE_CHARS]
*/
ByteStore genHeaderBytes(
    std::string ext, std::size_t n_total_chars, 
    const std::map<std::byte, std::string>& codeTable) {
    // Make returned container
    ByteStore ret = ByteStore();
    // TODO: reserve 64
    ByteStore totalLenBytes = ByteStore(
        std::bitset<SIZE_T_BIT_SIZE>(n_total_chars));
    // NOTE THAT EXTLEN CAN BE 0
    std::size_t extLen = ext.length();
    for (int i = 0; i < SIZE_T_BIT_SIZE / BYTE_SIZE; i++) {
        ret.push_back(totalLenBytes[i]);
    }
    ret.push_back(extLen);
    for (int i = 0; i < ext.length(); i++) {
        ret.push_back(ext[i]);
    }
    constexpr unsigned char zmask = ~0;
    unsigned short numUnique = codeTable.size();
    unsigned char highNum = (numUnique >> BYTE_SIZE) & zmask;
    unsigned char lowNum = numUnique & zmask;
    ByteStore numUniqueBytes = ByteStore(
        std::bitset<sizeof(unsigned short)>(numUnique));
    ret.push_back(highNum);
    ret.push_back(lowNum);
    for (int i = 2; i < sizeof(unsigned short); i++) {
        ret.push_back(0);
    }
    for (auto it = codeTable.cbegin(); it != codeTable.cend(); it++) {
        std::byte characterB = it->first;
        std::size_t codeLen = codeTable.at(it->first).length();
        ret.push_back(characterB);
        ret.push_back(codeLen);
        std::string paddedCodeStr = padByteCode(it->second);
        ByteStore paddedCodeBytes = stringToPaddedBytes(paddedCodeStr);
        ret.extend(paddedCodeBytes);
    }
    return ret;
}

bool finishedSearch(HuffNode* huffNode) {
    // TODO remove the dual-side check
    if (huffNode->left == nullptr && huffNode->right == nullptr) {
        return true;
    } else {
        return false;
    }
}

HuffNode* tryGetLeaf(HuffNode* curNode, const std::string& encoding, std::size_t& moveCounter) {
    std::size_t i = 0;
    for (; i < encoding.length(); ++i) {
        if (curNode == nullptr || (curNode->left == nullptr && curNode->right == nullptr)) {
            break;
        }
        if (encoding[i] == '0') {
            curNode = curNode->left;
        } else if (encoding[i] == '1') {
            curNode = curNode->right;
        } else {
            std::cerr << "Unexpected encoding value\n";
            break;
        }
        ++moveCounter;
    }
    return curNode;
}

/*
HEADER (ITEM [BYTE LENGTH OF ITEM] (NOTE: NO NEWLINES IN HEADER))===
NUMBER_CHARS_TOTAL [8]
NUM_EXT_CHARS [1] EXT_CHARS [NUM_EXT_CHARS]
NUM_UNIQUE_CHARS [2]
(CHAR [1] CHAR_CODE_LENGTH [1] 
    PADDEDCODE [MIN BYTES TO STORE CHAR_CODE_LENGTH BITS])[NUM_UNIQUE_CHARS]
*/

std::size_t getTotalFrequency(std::map<std::byte, std::size_t> codeFreqs) {
    std::size_t total = 0;
    for (auto it = codeFreqs.begin(); it != codeFreqs.end(); it++)
        total += (it->second);
    return total;
}

std::vector<std::string> filepathsInDir(std::string dir) {
    namespace fs = std::filesystem;
    auto ret = std::vector<std::string>();
    for (const auto & entry : fs::directory_iterator(dir))
        ret.push_back(entry.path().string());
    // /*see*/ std::cout << fs::is_directory("./");
    return ret;
}

void writeCodesToFile(std::string inputFile, std::string outputFile) {
    constexpr std::size_t bufferSize = 512;
    std::byte buffer[bufferSize];
    std::filesystem::path p = std::filesystem::path(inputFile);    
    std::string ext = p.extension().string();
    std::size_t total_chars = 0;
    std::map<std::byte, std::size_t> freqTable = getByteFrequencies(
        inputFile, total_chars);
    HuffNode* root = newTree(freqTable);
    auto codeTable = std::map<std::byte, std::string>();
    encodeFrequencies(root, codeTable);
    auto header = genHeaderBytes(ext, total_chars, codeTable);
    header.writeToFile(outputFile, false);
    std::ofstream wf(outputFile, std::ios::out | std::ios::binary | std::ios::app);
    std::ifstream rf(inputFile,  std::ios::in  | std::ios::binary );
    std::string encoding = "";
    std::string fullCode = "";
    if (!rf) {
        std::cerr << "Can't open " + inputFile;
    }
    if (!wf) {
        std::cerr << "Can't open " + outputFile;
    }
    for (char b; rf.get(b);) {
        encoding += codeTable[(std::byte) b];
        if (encoding.length() >= bufferSize * BYTE_SIZE) {
            fillBuffer(encoding, buffer, bufferSize);
            for (std::size_t i = 0; i < bufferSize; i++) {
                wf.put((unsigned char) buffer[i]);
            }
            encoding = encoding.substr(bufferSize * BYTE_SIZE);
        }
    }
    if (encoding.length() > 0) {
        ByteStore remainingBytes = stringToPaddedBytes(encoding);
        for (std::size_t i = 0; i < remainingBytes.size(); i++) {
            wf.put((unsigned char) remainingBytes[i]);
        }
    }
    rf.close();
    wf.close();
}

void writeDecodedFile(std::string codeFile, std::string decodeFile) {
    std::ifstream rf(codeFile,  std::ios::in  | std::ios::binary );
    if (std::filesystem::exists(decodeFile)) {
        std::ofstream file;
        file.open(decodeFile, std::ios::out);
        file.close();
    }
    std::ofstream wf(decodeFile, std::ios::out | std::ios::binary | std::ios::app);
    if (!rf) {
        std::cerr << "Can't open " + codeFile;
    }
    if (!wf) {
        std::cerr << "Can't open " + decodeFile;
    }
    char b = 0;
    std::size_t originalLen = 0;
    constexpr std::size_t bufferSize = 512;
    std::byte originalLenBytes[sizeof(std::size_t)];
    std::byte numUniqueBytes[sizeof(unsigned short)];
    unsigned short extLen = 0;
    unsigned short numUnique = 0;
    std::string ext = "";
    std::map<std::byte, std::string> codeTable = std::map<std::byte, std::string>();
    // TODO: Be endian agnostic
    rf.read(reinterpret_cast<char*>(originalLenBytes), 8);
    std::reverse(originalLenBytes, originalLenBytes + sizeof(std::size_t));
    std::memcpy(&originalLen, originalLenBytes, 8);
    rf.get(b);
    extLen = (unsigned short) b;
    for (unsigned short i = 0; i < extLen; i++) {
        rf.get(b);
        ext += b;
    }
    // TODO: Be endian agnostic
    rf.read(reinterpret_cast<char*>(numUniqueBytes), sizeof(unsigned short));
    std::reverse(numUniqueBytes, numUniqueBytes + sizeof(unsigned short));
    std::memcpy(&numUnique, numUniqueBytes, sizeof(unsigned short));
    for (unsigned short i = 0; i < numUnique; i++) {
        rf.get(b);
        std::byte character = (std::byte) b;
        rf.get(b);
        unsigned short codeLen = (unsigned short) b;
        unsigned short codeByteLen = paddedByteCount(codeLen);
        std::string paddedCode = "";
        for (unsigned short i = 0; i < codeByteLen; i++) {
            rf.get(b);
            paddedCode += std::bitset<BYTE_SIZE>(b).to_string();
        }
        codeTable[character] = paddedCode.substr(0, codeLen);
    }
    HuffNode* root = remakeTree(codeTable);
    HuffNode* curNode = root;
    std::string encoding = "";
    std::size_t writeCount = 0;
    while (writeCount < originalLen) {
        for (int i = 0; i < bufferSize * BYTE_SIZE && rf.get(b); i++) {
            encoding += std::bitset<BYTE_SIZE>(b).to_string();
        }
        while (writeCount < originalLen && !encoding.empty()) {
            std::size_t moveCount = 0;
            curNode = tryGetLeaf(curNode, encoding, moveCount);
            encoding = encoding.substr(moveCount);
            if (curNode->left == nullptr && curNode->right == nullptr) {
                wf.put((unsigned char)(curNode->data));
                writeCount++;
                curNode = root;
            }
        }
    }
    rf.close();
    wf.close();
    delTree(root);
}
//000011001111110101100010