#include <iostream>
#include "include/nostl/string.h"

int main() {
    nostl::string s = "hello";
    
    // Test rfind with char
    size_t pos1 = s.rfind('l');
    std::cout << "rfind('l') in 'hello': " << pos1 << std::endl;  // Should be 3
    
    size_t pos2 = s.rfind('l', 2);
    std::cout << "rfind('l', 2) in 'hello': " << pos2 << std::endl;  // Should be 2
    
    size_t pos3 = s.rfind('x');
    std::cout << "rfind('x') in 'hello': " << pos3 << std::endl;  // Should be npos
    
    // Test rfind with string
    size_t pos4 = s.rfind("ll");
    std::cout << "rfind(\"ll\") in 'hello': " << pos4 << std::endl;  // Should be 2
    
    // Empty string test
    nostl::string empty_s = "";
    size_t pos5 = empty_s.rfind('a');
    std::cout << "rfind('a') in empty string: " << pos5 << std::endl;  // Should be npos
    
    return 0;
}
