#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "Hello" << 4 << "\n";
    printf("Hello2 %d\n", 4);
    
    int x = 5;
    float y = 8;
    double z = 1;
    int& f = x;
    
    std::string s = "ss";
    
    printf(std::to_string(x).c_str());
    
    return 0;
}
