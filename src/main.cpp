#include "application.h"
#include <iostream>
#include <exception>

int main() {
    try {
        Application app(1280, 720, "Pod Engine - OpenGL Learning");
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
