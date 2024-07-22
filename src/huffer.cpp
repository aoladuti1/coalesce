#include <map>
#include <huffer.hpp>
#include <queue>
#include <algorithm>

const std::string OS_SEP(1, std::filesystem::path::preferred_separator);

#if __cplusplus >= 202002L
    #include <bit>
    #define CSC_BIG_ENDIAN std::endian::big == std::endian::native
#else 
    #define CSC_BIG_ENDIAN ENDIAN::big == ENDIAN::native
#endif

/* Hiya, this is a basic general-purpose Huffman Coding file
compression algorithm implementation,
using modern C++, the standard library (STL) and bitwise arithmetic.
This is a small project generally made for C++ practice, and 
to be extremely generalized (endian-independent, 
loose byte representation requirements etc.)

Compilation and System Reqs:
-System must have at least 8 bits in a byte
-Both sizeof(std::size_t) and sizeof(unsigned short) must match in alternate compiled binaries
-Filenames must be 255 characters or less (including the extension)
-C++17 minimum (G++, MSVC, Clang), or C++20 minimum (any). */

HuffNode::HuffNode(const std::byte character, const std::size_t frequency) {
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

HuffNode* newTree(const std::map<std::byte, std::size_t>& freqTable) {
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

HuffNode* remakeTree(const std::map<std::byte, std::string>& codeTable) {
    HuffNode* root = new HuffNode((std::byte) 0, 0);
    HuffNode* curNode = root;
    for (auto it = codeTable.begin(); it != codeTable.end(); it++) {
        HuffNode* curNode = root;
        std::byte character = it->first;
        std::string code = it->second;
        for (int i = 0; i < code.length(); i++) {
            if (code[i] == '0') {
                if (curNode->left == nullptr) {
                    curNode->left = new HuffNode((std::byte) 0, 0L);
                }
                curNode = curNode->left;
            } else {
                if (curNode->right == nullptr) {
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
    if (root != nullptr) {
        delTree(root->left);
        delTree(root->right);
        delete root;
    }
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
    int rem = code.size() % CHAR_BIT;
    int bext = 0;
    std::string ret = code;
    if (rem != 0)
        bext = CHAR_BIT - rem;
    for (int i = 0; i < bext; i++)
        ret = ret + "0";
    return ret;
}

std::vector<std::byte> stringToPaddedBytes(const std::string str) {
    std::vector<std::byte> ret = std::vector<std::byte>(minByteCount(str.length()));
    for (std::size_t i = 0, bcount = 0; i < str.length(); bcount += 1) {
        unsigned char nextByte = 0;
        for (std::size_t j = 0; j < CHAR_BIT && i < str.length(); j++, i++)
            if (str[i] == '1')
                nextByte |= (1 << (CHAR_BIT - 1 - j));
        ret[bcount] = (std::byte) nextByte;
    }  
    return ret;
}

std::size_t fillBuffer(
    std::byte* arr, const std::size_t arrLen, const std::string code) {
    std::size_t bcount = 0;
    for (std::size_t i = 0; bcount < arrLen; bcount++) {
        unsigned char nextByte = 0;
        for (std::size_t j = 0; j < CHAR_BIT; j++, i++) {
            if (code[i] == '1')
                nextByte |= (1 << (CHAR_BIT - 1 - j));
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
Header Notation: ITEM [BYTE LENGTH OF ITEM]
#####
NUMBER_CHARS_TOTAL [SIZEOF(STD::SIZE_T)]
NUM_EXT_CHARS [1] 
EXT_CHARS [NUM_EXT_CHARS]
NUM_UNIQUE_CHARS [SIZEOF(UNSIGNED SHORT)]
(  CHAR [1] 
   CHAR_CODE_LENGTH [1] 
   PADDED_CODE [MIN BYTES TO STORE CHAR_CODE_LENGTH BITS]  )[NUM_UNIQUE_CHARS] 
*/
std::vector<std::byte> genHeaderBytes(
    const std::string ext, const std::size_t n_total_chars, 
    const std::map<std::byte, std::string>& codeTable) {
    std::vector<std::byte> ret = std::vector<std::byte>();
    ret.reserve(codeTable.size() * 2);
    // +NUMBER_CHARS_TOTAL
    for (int i = 0; i < sizeof(std::size_t); i++) 
        ret.push_back((std::byte) (n_total_chars >> (i * CHAR_BIT)));
    // +NUM_EXT_CHARS
    ret.push_back((std::byte) ext.length());
    // +EXT_CHARS
    for (int i = 0; i < ext.length(); i++)
        ret.push_back((std::byte) ext[i]);
    // +NUM_UNIQUE_CHARS
    for (int i = 0; i < sizeof(unsigned short); i++) 
        ret.push_back((std::byte) (codeTable.size() >> (i * CHAR_BIT)));
    for (auto it = codeTable.cbegin(); it != codeTable.cend(); it++) {
        // +CHAR
        ret.push_back(it->first); 
        // +CHAR_CODE_LENGTH
        ret.push_back((std::byte) codeTable.at(it->first).length());
        std::string paddedCodeStr = padByteCode(it->second);
        std::vector<std::byte> paddedCodeBytes = stringToPaddedBytes(paddedCodeStr);
        // +PADDED_CODE
        ret.reserve(ret.size() + paddedCodeBytes.size());
        ret.insert(ret.end(), paddedCodeBytes.begin(), paddedCodeBytes.end());
    }
    return ret;
}

HuffNode* tryGetLeaf(
    HuffNode* curNode, const std::string encoding, std::size_t& moveCounter) {
    std::size_t i = 0;
    for (; i < encoding.length(); ++i) {
        if (curNode == nullptr || isTreeLeaf(curNode)) {
            break;
        } else if (encoding[i] == '0') {
            curNode = curNode->left;
        } else {
            curNode = curNode->right;
        }
        ++moveCounter;
    }
    return curNode;
}

std::vector<std::string> filepathsInDir(const std::string dir) {
    namespace fs = std::filesystem;
    auto ret = std::vector<std::string>();
    for (const auto & entry : fs::directory_iterator(dir))
        ret.push_back(entry.path().string());
    return ret;
}

void writeToFile(const std::vector<std::byte>& bytes,
                 const std::string outputFile, const bool append)  {
    const auto flags = std::ios::out | std::ios::binary;
    std::ofstream wf(outputFile, append ? flags | std::ios::app : flags);
    if(!wf) {
        throw std::invalid_argument("Can't open file " + outputFile);
    }
    std::size_t sizeVal = bytes.size();
    for (std::size_t i = 0; i < sizeVal; i++) {
        wf.put((unsigned char) bytes.at(i));
    }
    wf.close();
}

void writeCodesToFile(const std::string inputFile, const std::string outputFile) {
    constexpr std::size_t bufferSize = IO_BUFFER_SIZE;
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
    writeToFile(header, outputFile, false);
    std::ofstream wf(outputFile, std::ios::out | std::ios::binary | std::ios::app);
    std::ifstream rf(inputFile,  std::ios::in  | std::ios::binary );
    std::string encoding = "";
    std::string fullCode = "";
    if (!rf) {
        throw std::invalid_argument("Can't read " + inputFile);
    }
    if (!wf) {
        throw std::invalid_argument("Can't compress to " + outputFile);
    }
    for (char b; rf.get(b);) {
        encoding += codeTable[(std::byte) b];
        if (encoding.length() >= bufferSize * CHAR_BIT) {
            fillBuffer(buffer, bufferSize, encoding);
            for (std::size_t i = 0; i < bufferSize; i++) {
                wf.put((unsigned char) buffer[i]);
            }
            encoding = encoding.substr(bufferSize * CHAR_BIT);
        }
    }
    if (encoding.length() > 0) {
        std::vector<std::byte> remainingBytes = stringToPaddedBytes(encoding);
        for (std::size_t i = 0; i < remainingBytes.size(); i++) {
            wf.put((unsigned char) remainingBytes[i]);
        }
    }
    rf.close();
    wf.close();
}

void writeDecodedFile(const std::string codeFile, 
                      const std::string decodeFilename,
                      bool errorOnExistingOutput) {
    std::ifstream rf(codeFile, std::ios::in  | std::ios::binary);
    if (!rf) {
        throw std::invalid_argument("Can't read " + codeFile);
    }
    constexpr std::size_t bufferSize = IO_BUFFER_SIZE;
    char b;
    std::size_t originalLen;
    std::byte originalLenBytes[sizeof(std::size_t)];
    std::byte numUniqueBytes[sizeof(unsigned short)];
    unsigned short extLen;
    unsigned short numUnique;
    std::string ext = "";
    std::map<std::byte, std::string> codeTable = std::map<std::byte, std::string>();
    rf.read(reinterpret_cast<char*>(originalLenBytes), sizeof(std::size_t));
    if constexpr(CSC_BIG_ENDIAN) {
        std::reverse(originalLenBytes, originalLenBytes + sizeof(std::size_t));
    }
    memcpy(&originalLen, originalLenBytes, sizeof(std::size_t));
    rf.get(b);
    extLen = (unsigned short) b;
    for (int i = 0; i < extLen; i++) {
        rf.get(b);
        ext += b;
    }
    rf.read(reinterpret_cast<char*>(numUniqueBytes), sizeof(unsigned short));
    if constexpr(CSC_BIG_ENDIAN) {
        std::reverse(numUniqueBytes, numUniqueBytes + sizeof(unsigned short));
    }
    memcpy(&numUnique, numUniqueBytes, sizeof(unsigned short));
    std::string outputFile = decodeFilename + ext;
    if (std::filesystem::exists(outputFile) && errorOnExistingOutput) {
        std::cerr << ("Can't decompress to existing file " + outputFile + "\n");
        return;
    }
    std::ofstream wf(outputFile, std::ios::out | std::ios::binary | std::ios::app);
    if (!wf) {
        throw std::invalid_argument("Can't decompress to " + outputFile);
    }
    for (int i = 0; i < numUnique; i++) {
        rf.get(b);
        std::byte character = (std::byte) b;
        rf.get(b);
        unsigned short codeLen = (unsigned short) b;
        unsigned short codeByteLen = minByteCount(codeLen);
        std::string paddedCode = "";
        for (int i = 0; i < codeByteLen; i++) {
            rf.get(b);
            paddedCode += std::bitset<CHAR_BIT>(b).to_string();
        }
        codeTable[character] = paddedCode.substr(0, codeLen);
    }
    HuffNode* root = remakeTree(codeTable);
    HuffNode* curNode = root;
    std::string encoding = "";
    std::size_t writeCount = 0;
    while (writeCount < originalLen) {
        for (int i = 0; (i < IO_BUFFER_SIZE * CHAR_BIT) && rf.get(b); i++) {
            encoding += std::bitset<CHAR_BIT>(b).to_string();
        }
        while (writeCount < originalLen && !encoding.empty()) {
            std::size_t moveCount = 0;
            curNode = tryGetLeaf(curNode, encoding, moveCount);
            encoding = encoding.substr(moveCount);
            if (isTreeLeaf(curNode)) {
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

void processFile(
        const std::string& filePath, 
        const std::string& outputFile, 
        const bool decode, 
        const bool verbose) {
    if (std::filesystem::exists(outputFile)) {
        if (verbose)
            std::cout << outputFile << " already exists -- skipping\n";
        return;
    }    
    // TODO: Put messages inside write functions
    if (decode) {
        if (verbose)
            std::cout << "Decompressing " << filePath << " to " << outputFile << " ...\n";
        writeDecodedFile(filePath, outputFile);
    } else {
        if (verbose)
            std::cout << "Compressing " << filePath << " to " << outputFile << " ...\n";
        writeCodesToFile(filePath, outputFile);
    }
}

void processFile(
        const std::string& filePath, const std::string& outputFile, const bool decode) {
    processFile(filePath, outputFile, decode, false);
}

void processDirectory(
        const std::string& dirPath, 
        const std::string& outputDir, 
        const bool decode, 
        const bool verbose) {
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (!entry.is_regular_file() || (decode && entry.path().extension() != COMPRESSION_EXT))
            continue;
        std::string replExt = decode ? "" : COMPRESSION_EXT;
        std::string output = std::filesystem::path(entry.path().string()).filename().replace_extension(
                replExt).string();
        if (!std::filesystem::exists(outputDir)) {
            // Attempt to create the directory
            if (!std::filesystem::create_directories(outputDir)) {
                throw std::runtime_error("Failed to create directory: " + outputDir);
            }
        }
        processFile(entry.path().string(), 
                    outputDir + OS_SEP + output, decode, verbose);
    }
}

void processDirectory(
        const std::string& dirPath, const std::string& outputDir, const bool decode) {
    processDirectory(dirPath, outputDir, decode, false);
}