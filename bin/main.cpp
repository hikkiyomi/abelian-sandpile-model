#include "lib/interactor.h"
#include "lib/renderer/renderer.h"

#include <iostream>

int main(int argc, char* argv[]) {
    Interactor interactor(argc, argv);
    interactor.Run();

    return 0;
}
