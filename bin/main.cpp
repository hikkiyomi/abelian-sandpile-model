#include "lib/interactor.h"
#include "lib/renderer/renderer.h"

#include <iostream>

int main(/* int argc, char* argv[] */) {
    int argc = 11;
    char* argv[] = {
        "./lab3",
        "--input",
        "input.tsv",
        "--output=C:/ProgrammingWorkspace/itmo_learning/prog_basics/lab3-out/",
        "-l",
        "480",
        "-w",
        "640",
        "-m=0",
        "--freq",
        "1"
    };
    
    Interactor interactor(argc, argv);

    return 0;
}
