//
// Created by Andrew on 2018/7/20.
// Copyright (c) 2018 mgtv.com. All rights reserved.
//
// @file: scanner.cpp
// @author: tangye <tangye@mgtv.com | tangyel@126.com>
// @bref:
//


#include "scanner.h"

const int MAX_IMG_SIZE = 10000000;

TScanner::TScanner(const char * f) {
    origin_image = imread(f, 0);
}

TScanner::TScanner(unsigned char * img, int width, int height) {
    Mat tmp(width, height, CV_8UC1, img);
    origin_image = tmp;
}

TScanner::~TScanner() {
    origin_image.release();
}


DecodeResults TScanner::zbar_decode(Mat &img) {
    std::vector<DecodeResult> decodedObjects;

    // Create zbar scanner
    ImageScanner scanner;

    // Configure scanner
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);


    // Wrap image data in a zbar image
    Image image(img.cols, img.rows, "Y800", (uchar *)img.data, img.cols * img.rows);

    // Scan the image for barcodes and QRCodes
    int n = scanner.scan(image);

    // Print results
    for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
    {
        DecodeResult obj;

        std::string s = symbol->get_data();
        obj.data = new char[s.length()+1];
        strcpy(obj.data, s.c_str());
        //obj.data = symbol->get_data();

        // Print type and data
        // std::cout << "Data : " << obj.data << std::endl;

        if(symbol->get_location_size() != 4){
            continue;
        }
        // Obtain bounding box
        int x_min = MAX_IMG_SIZE;
        int x_max = -1;
        int y_min = MAX_IMG_SIZE;
        int y_max = -1;
        for(int i = 0; i< symbol->get_location_size(); i++)
        {
            int x = symbol->get_location_x(i);
            int y = symbol->get_location_y(i);
            if(x < x_min){
                x_min = x;
            }
            if(x > x_max){
                x_max = x;
            }
            if(y < y_min){
                y_min = y;
            }
            if(y > y_max){
                y_max = y;
            }
        }

        if (x_max <= x_min || y_max <= y_min){
            continue;
        }

        obj.x = x_min;
        obj.y = y_min;
        obj.width = x_max - x_min;
        obj.height = y_max - y_min;
        decodedObjects.push_back(obj);
    }

    return decodedObjects;
}


DecodeResults TScanner::morphology_decode(Mat &img) {
    DecodeResults decodedObjects = zbar_decode(img);
    if(decodedObjects.size() <= 0){
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3), Point(-1, -1));
        Mat dil_img, open;
        dilate(img, dil_img, kernel, Point(-1,-1), 3);
        morphologyEx(dil_img, open, CV_MOP_OPEN, kernel);
        decodedObjects = zbar_decode(open);
    }
    /*
    if(decodedObjects.size() > 0){
        display(decodedObjects);
    }
     */
    return decodedObjects;
}


void TScanner::display(DecodeResults & decodeResults) {
    std::cout << "find " << decodeResults.size() << " items" << std::endl;
    for(int i = 0; i < decodeResults.size(); i++){
        std::cout << "Data: " << decodeResults[i].data << std::endl;
        std::cout << "Location: (" << decodeResults[i].x << "," << decodeResults[i].y <<
        "," << decodeResults[i].width << "," << decodeResults[i].height << ")" << std::endl;
    }
}



DecodeResults TScanner::decode() {
    /*
     *   1. mean-filter blur + decode
     *   2. mean-filter blur + diff + inpaint + decode
     *   3. mean-filter blur + otsu + decode
     *   4. mean-filter blur + otsu + adpativethreshold + decode
        decode = zbar_decode or (morphologyEx + zbar_decode)
     */

    if(!origin_image.data){
        DecodeResults ret;
        return ret;
    }

    //img to gray im
    //Mat gray;
    //cvtColor(origin_image, gray, CV_BGR2GRAY);

    //decode
    DecodeResults decodeObject = morphology_decode(origin_image);
    if(decodeObject.size() > 0){
        std::cout << "direct" << std::endl;
        std::cout << decodeObject.size() << std::endl;
        //return decodeObject;
    }

    //otsu
    Mat origin_otsu_im;
    threshold(origin_image, origin_otsu_im, 26, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    DecodeResults intermediateResults = morphology_decode(origin_otsu_im);
    if(intermediateResults.size() > 0){
        std::cout << "otsu" << std::endl;
        std::cout << intermediateResults.size() << std::endl;
        decodeObject.insert(decodeObject.end(), intermediateResults.begin(), intermediateResults.end());
    }

    //adaptive
    Mat origin_adaptive_im;
    adaptiveThreshold(origin_image, origin_adaptive_im, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 41, 0);
    intermediateResults = morphology_decode(origin_adaptive_im);
    if(intermediateResults.size() > 0){
        std::cout << "adaptive filter" << std::endl;
        std::cout << intermediateResults.size() << std::endl;
        decodeObject.insert(decodeObject.end(), intermediateResults.begin(), intermediateResults.end());
    }


    //mean-filter
    Mat filter_img;
    blur(origin_image, filter_img, Size(5,5));

    intermediateResults = morphology_decode(filter_img);
    if(intermediateResults.size() > 0){
        std::cout << "mean-filter" << std::endl;
        std::cout << intermediateResults.size() << std::endl;
        decodeObject.insert(decodeObject.end(), intermediateResults.begin(), intermediateResults.end());
    }

    //mean-filter + otsu
    Mat otsu_im;
    threshold(filter_img, otsu_im, 26, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    intermediateResults = morphology_decode(otsu_im);
    if(intermediateResults.size() > 0){
        std::cout << "mean-filter + otsu" << std::endl;
        std::cout << intermediateResults.size() << std::endl;
        decodeObject.insert(decodeObject.end(), intermediateResults.begin(), intermediateResults.end());
    }

    //mean-filter + adaptive-filter
    Mat adaptive_im;
    adaptiveThreshold(filter_img, adaptive_im, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 41, 0);
    intermediateResults = morphology_decode(adaptive_im);
    if(intermediateResults.size() > 0){
        std::cout << "mean-filter + adaptive" << std::endl;
        std::cout << intermediateResults.size() << std::endl;
        decodeObject.insert(decodeObject.end(), intermediateResults.begin(), intermediateResults.end());
    }
    if(decodeObject.size() > 0){
        std::set<DecodeResult> s(decodeObject.begin(), decodeObject.end());
        decodeObject.assign(s.begin(), s.end());
        display(decodeObject);
        return decodeObject;
    }
    std::cout << "not found" << std::endl;
    return decodeObject;
}

extern "C"
{
DecodeResult* c_scanner(unsigned char *buf, int w, int h)
{
    TScanner tscanner(buf, w, h);
    DecodeResults decodeResults = tscanner.decode();
    //vector to pointer, vector are guaranteed to be stored contiguously
    // and we will not modify the vector subsequently
    DecodeResult *result = &decodeResults[0];
    return result;
}
}
