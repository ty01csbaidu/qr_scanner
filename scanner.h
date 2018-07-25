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



using namespace cv;
using namespace zbar;

struct DecodeResult{
    const char *data;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
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