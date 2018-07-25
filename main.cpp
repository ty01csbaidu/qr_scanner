#include <iostream>
#include "scanner.h"


using namespace std;

int main(int argc, char **argv) {
    if( argc != 2){
        cout << "usage: " << argv[0] << " img_file" << endl;
    }
    TScanner scanner(argv[1]);
    DecodeResults decodedResult = scanner.decode();
    return 0;
}