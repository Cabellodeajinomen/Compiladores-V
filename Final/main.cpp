#include <iostream>
#include <string>
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    bool trace = false;
    const char* filename = nullptr;

    if (argc == 2) {
        filename = argv[1];
    } else if (argc == 3 && string(argv[1]) == "--trace") {
        trace = true;
        filename = argv[2];
    } else {
        cerr << "Uso: " << argv[0] << " [--trace] archivo.m0" << endl;
        return 1;
    }

    Parser p;
    if (trace)
        p.setTrace(true);

    p.parse(filename);
    return p.hasErrors() ? 1 : 0;
}
