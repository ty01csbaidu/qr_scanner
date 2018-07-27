#include <iostream>
#include "scanner.h"


using namespace std;

int main(int argc, char **argv) {
    if( argc != 2){
        cout << "usage: " << argv[0] << " img_file" << endl;
    }
    TScanner scanner(argv[1]);
    DecodeResults decodedResult = scanner.decode();
    //free memorry
    for(int i = 0; i < decodedResult.size(); i++){
        decodedResult[i].free();
    }
    return 0;
}