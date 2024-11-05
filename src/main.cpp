#include <tests.hpp>
#include <string.h>

void printHelp() {
    const std::string helpText = R"(
Coalesce
--------
Syntax: 
<csc|coalesce> <-c | -d | -h | -help> [-s] <FILES AND/OR DIRECTORIES> [--o <OUTPUT FILES AND/OR DIRECTORIES>]
...Where [] == optional, <> == required (if no help flag set), and | == OR.

Semantics: 
-h | -help: help
-c: compression mode 
-d: decompression mode
-s: silent standard output
--o: output list

    ++Basic Usage Example (compress and decompress the file testfile.txt):
csc -c testfile.txt --o Newfolder/testfile
csc -d new_folder/testfile --o Newfolder/original
(Check out new_folder, it'll hold both the compressed and uncompressed file.)

    ++Compressing and Decompressing the whole Current Working Directory Example:
csc -c . --o compression_folder
csc -d compression_folder --o decompression_folder
(NB: if coalesce.exe or csc.exe is not in the PATH variable, 
you may need to prepend '.\' to 'csc' or 'coalesce' commands...)
--------
Note: each input will be matched to the output in order and matched in type. E.g. if the first input is a file, 
the first specified output will be matched to it and also be a file.
Inputs without a specified output will be output to the current folder.
Decompression mode output and input files do not require a specified extension since it is stored in the respective compressed file.
Compression output files do not require a specified extension, since it will be set to )" + COMPRESSION_EXT + R"(.
Enabling the -s flag will still print a message about not being able to decompress to existing files in the standard error output.
)";
    std::cout << helpText << std::endl;
}

bool tryFindCompressed(std::filesystem::path& p) {
    std::string filePath = p.string();
    if (!std::filesystem::exists(p)) {
        if (!std::filesystem::exists(std::filesystem::path(filePath + COMPRESSION_EXT))) {
            std::cerr << "Error: Path to " << p.string() << " does not exist" << std::endl;
            return 1;
        } else {
            p = std::filesystem::path(filePath + COMPRESSION_EXT);
        }                
    }
    return 0;
}

int main(int argc, char* argv[]) {
    bool verbose = true;
    bool decode = false;
    bool isDir = false;
    std::vector<std::string> outputs;
    std::vector<std::string> targets;
    std::vector<bool> dirTracker;
    if (argc == 2 && (strcmp(argv[1], "-h") || strcmp(argv[1], "-help") )) {
        printHelp();
        return 0;
    }
    if (argc < 3) {
        std::cerr << "Not enough arguments. Use the -h or -help flag." << std::endl;
        return 1;         
    }
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0) {
            decode = true;
        } else if (strcmp(argv[i], "-c") == 0) {
            decode = false;
        } else if (strcmp(argv[i], "-s") == 0) {
            verbose = false;
        } else if (strcmp(argv[i], "--o") == 0) {
            i++;
            if (i >= argc) {
                std::cerr << "Error: --o option requires a filename or directory" << std::endl;
                return 1; 
            }
            for (; i < argc; i++) {
                std::filesystem::path p(argv[i]);
                if (decode) {
                    auto pNoExt = std::filesystem::path(p.string()).replace_extension("");
                    p = std::filesystem::exists(pNoExt) ? p : pNoExt;
                }                      
                outputs.push_back(p.string());
            }
        } else {
            std::filesystem::path p(argv[i]);
            if (decode && (tryFindCompressed(p) == 1)) {
                return 1;
            }
            dirTracker.push_back(std::filesystem::is_directory(p));
            targets.push_back(p.string());
        }
    }
    if (targets.size() == 0) {
        std::cerr << "Error: No files or directories passed" << std::endl;
        return 1;          
    }
    else if (outputs.size() > targets.size()) {
        (std::cerr << "Error: more outputs (" 
                   << outputs.size() << ") than targets "
                   << "(" << targets.size() << ")" << std::endl);
        return 1; 
    } else {
        for (int r = outputs.size(); r < targets.size(); r++) {
            if (dirTracker[r]) {
                outputs.push_back(std::filesystem::current_path().string());
            } else {
                std::string replExt = decode ? "" : COMPRESSION_EXT;
                outputs.push_back(
                    std::filesystem::path(targets[r]).replace_extension(
                        replExt).string());
            }
        }
    }

    // Process each target (file or directory)
    for (int i = 0; i < targets.size(); i++) {
        std::string output = outputs[i];
        std::string target = targets[i];
        if (dirTracker[i]) {
            processDirectory(target, output, decode, verbose);
        } else {
            processFile(target, output, decode, verbose);
        }
    }
    if (verbose)
        std::cout << "All done!" << std::endl;
    return 0;
}