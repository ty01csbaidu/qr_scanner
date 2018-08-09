#ifndef SCANNER_H_
#define SCANNER_H_
//
// Created by Andrew on 2018/7/20.
// Copyright (c) 2018 mgtv.com. All rights reserved.
//
// @file: scanner.h
// @author: tangye <tangye@mgtv.com | tangyel@126.com>
// @bref: scanner for qr_code
//

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <chrono>



using namespace cv;
using namespace zbar;

struct DecodeResult{
    char* data;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;

    void free(){
        delete [] data;
    }
    bool operator <(const DecodeResult & other) const
    {
        if((x/2 < other.x/2) || ((!(other.x/2 < x/2)) && (y/2 < other.y/2))){
            return true;
        }else{
            return false;
        }
    }
};

typedef std::vector<DecodeResult> DecodeResults;

class TScanner{
private:
    Mat origin_image;
    DecodeResults zbar_decode(Mat &);
    DecodeResults morphology_decode(Mat &);
    void display(DecodeResults &);
public:
    TScanner(const char*);
    TScanner(unsigned char*, int, int);
    virtual ~TScanner();
    DecodeResults decode();
};


#endif /* SCANNER_H_ */