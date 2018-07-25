# qr_scanner

### prerequisite
You have to install zbar and opencv first

### make
g++ main.cpp scanner.cpp -o main -lzbar $(pkg-config --cflags --libs opencv)
