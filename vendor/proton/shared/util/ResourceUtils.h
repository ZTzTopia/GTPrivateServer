//  ***************************************************************
//  ResourceUtils - Creation date: 04/13/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#pragma once
#include <cstdint>
#include <string>

uint8_t *brotliCompressToMemory(const uint8_t *pInput, std::size_t sizeBytes, std::size_t *pSizeCompressedOut, int compressionQuality = -1); // You must SAFE_DELETE_ARRAY what it returns
uint8_t *brotliDecompressToMemory(const uint8_t *pInput, std::size_t compressedSize, std::size_t decompressedSize); // You must SAFE_DELETE_ARRAY what it returns

uint8_t *zlibDeflateToMemory(uint8_t *pInput, int sizeBytes, int *pSizeCompressedOut); // You must SAFE_DELETE_ARRAY what it returns
uint8_t *zLibInflateToMemory(uint8_t *pInput, unsigned int compressedSize, unsigned int decompressedSize); // You must SAFE_DELETE_ARRAY what it returns
