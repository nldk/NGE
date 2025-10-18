#include <iostream>
#include <NGE/nge.h>

int main() {
    std::cout << "NGE consumer example: headers found and linked." << std::endl;

    // We don't need to call any SDL functions here — just ensure code compiles and links
    // against the NGE::NGE imported target which transitively links SDL.
    return 0;
}

