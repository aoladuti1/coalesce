#include <map>
#include <huffer.hpp>
#include <queue>

using namespace csc;

HuffNode::HuffNode(std::byte character, std::size_t frequency) {
    data = character;
    freq = frequency;
    left = right = NULL;
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
    for (unsigned char b; rf >> b;) {
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
    for (unsigned char b; rf >> b;) {
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

ByteStore stringToPaddedBytes(std::string str) {
    std::size_t byteNo = str.length() / BYTE_SIZE;
    auto ret = ByteStore((str.length() % BYTE_SIZE != 0) ? byteNo + 1 : byteNo);
    for (std::size_t i = 0, bcount = 0; i < str.length(); bcount+=1) {
        unsigned char nextByte; // uninit friendly
        for (std::size_t j = 0; j < BYTE_SIZE && i < str.length(); j++, i++) {
            if (str[i] == '1') {
                nextByte |= (1 << (BYTE_SIZE - 1 - j));
            }
        }
        ret.set(bcount, nextByte);
    }  
    return ret;
}

void encodeFrequencies(
    HuffNode* root, std::map<std::byte, std::string>& codeTable) {
    encodeFrequencies(root, "", codeTable);
}

/*
    HEADER===
    NUMBER_CHARS_TOTAL[8BYTES]
    NUM_EXT_CHARS[1BYTE]CHARBYTES[NUM_EXT_CHARS]
    NUMBER_OF_UNIQUE_CHARS[1BYTE]
    (CHAR[1BYTE]LENGTH[1BYTE]PADDEDCODE..N) .. NUMBER_CHARS_TOTAL
    
*/
ByteStore genHeaderBytes(
    std::string ext, std::size_t n_total_chars, 
    const std::map<std::byte, std::string>& codeTable) {
    // Make returned container
    ByteStore ret = ByteStore();
    ByteStore totalLenBytes = ByteStore(
        std::bitset<SIZE_T_BIT_SIZE>(n_total_chars));
    // TODO: NOTE THAT EXTLEN CAN BE 0
    std::size_t extLen = ext.length();
    std::size_t numUnique = codeTable.size();
    for (int i = 0; i < SIZE_T_BIT_SIZE / BYTE_SIZE; i++) {
        ret.push_back(totalLenBytes[i]);
    }
    ret.push_back(extLen);
    for (int i = 0; i < ext.length(); i++) {
        ret.push_back(ext[i]);
    }
    ret.push_back(numUnique);
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

std::size_t getTotalFrequency(std::map<std::byte, std::size_t> codeFreqs) {
    std::size_t total = 0;
    for (auto it = codeFreqs.begin(); it != codeFreqs.end(); it++)
        total += (it->second);
    return total;
}

std::map<std::byte, std::size_t> processFile(
    const std::string inputFile, ByteStore& output) {
    std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
    auto byteMap = std::map<std::byte, std::size_t>();
    if(!rf) {
        throw std::invalid_argument("Can't open file " + inputFile);
    }
    for (unsigned char b; rf >> b;) {
        std::byte castb = (std::byte) b;
        output.push_back(castb);
        byteMap[castb] = byteMap[castb] + 1;
    }
    rf.close();
    return byteMap;
}

std::vector<std::string> filepathsInDir(std::string dir) {
    namespace fs = std::filesystem;
    auto ret = std::vector<std::string>();
    for (const auto & entry : fs::directory_iterator(dir))
        ret.push_back(entry.path().string());
    // /*see*/ std::cout << fs::is_directory("./");
    return ret;
}

void writeCodesToFile(std::string inputFile, std::string outputFile, 
                      std::map<std::byte, std::string>& codeTable) {
    std::ifstream rf(inputFile, std::ios::in | std::ios::binary);
    std::byte bytes[BYTE_SIZE * 64];

    // concat code strings while nextwritesize less than N
    // if nextwritesize >= N set sparebits to the remainder
    // place remainder in buffer
    // continue
    rf.close();

}
