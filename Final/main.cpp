#include <iostream>
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " archivo.m0" << endl;
        return 1;
    }

    Parser p;
    p.parse(argv[1]);

    return 0;
}
