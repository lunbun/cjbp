#include <iostream>
#include <fstream>

#include <cjbp/cjbp.h>

int main() {
    std::ifstream file("Main.class", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    std::unique_ptr<cjbp::ClassFile> classFile = cjbp::ClassFile::read(file);
    std::cout << classFile->toString() << std::endl;
}
