#include <cassert>
#include <zlib.h>
#include <lz4.h>

#include "ResourceUtils.h"

// You must SAFE_DELETE_ARRAY what it returns
uint8_t *lz4CompressToMemory(uint8_t *pInput, int sizeBytes, int *pSizeCompressedOut) {
    assert(sizeBytes > 0);
    assert(pSizeCompressedOut != nullptr);

    int compressedSize = LZ4_compressBound(sizeBytes);
    auto *compressedData = new uint8_t[compressedSize];

    int result = LZ4_compress_default((const char *)pInput, (char *)compressedData, sizeBytes, compressedSize);
    if (result <= 0) {
        delete[] compressedData;
        return nullptr;
    }

    *pSizeCompressedOut = result;
    return compressedData;
}

// You must SAFE_DELETE_ARRAY what it returns
uint8_t *lz4DecompressToMemory(uint8_t *pInput, int compressedSize, int decompressedSize) {
    assert(compressedSize > 0);
    assert(decompressedSize > 0);

    auto *pDestBuff = new uint8_t[decompressedSize + 1]; // Room for extra null at the end;
	pDestBuff[decompressedSize] = 0; // Add the extra null, if we decompressed a text file this can be useful

    int result = LZ4_decompress_safe((const char *)pInput, (char*)pDestBuff, compressedSize, decompressedSize);
    if (result < 0 || result != decompressedSize) {
        delete[] pDestBuff;
        return nullptr;
    }

    return pDestBuff;
}

// You must SAFE_DELETE_ARRAY what it returns
uint8_t *zlibDeflateToMemory(uint8_t *pInput, int sizeBytes, int *pSizeCompressedOut) {
	z_stream strm;
	int ret;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if (ret != Z_OK) {
		return nullptr;
	}

#define ZLIB_PADDING_BYTES (5*1024)

	auto *pOut = new uint8_t[sizeBytes + ZLIB_PADDING_BYTES];  // Some extra padding in case the compressed version is larger than the decompressed version for some reason

	strm.avail_in = sizeBytes;
	strm.next_in = pInput;
	strm.avail_out = sizeBytes + ZLIB_PADDING_BYTES;
	strm.next_out = pOut;

	ret = deflate(&strm, Z_FINISH);
	assert(ret != Z_STREAM_ERROR); /* State not clobbered */

	// assert(ret == Z_STREAM_END);
	deflateEnd(&strm);
	*pSizeCompressedOut = (int)strm.total_out;
	return pOut;
}

// You must SAFE_DELETE_ARRAY what it returns
uint8_t *zLibInflateToMemory(uint8_t *pInput, unsigned int compressedSize, unsigned int decompressedSize) {
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) {
		return nullptr;
	}

    auto *pDestBuff = new uint8_t[decompressedSize + 1]; // Room for extra null at the end;
	pDestBuff[decompressedSize] = 0; // Add the extra null, if we decompressed a text file this can be useful

	strm.avail_in = compressedSize;
	strm.next_in = pInput;
	strm.avail_out = decompressedSize;
	strm.next_out = pDestBuff;

	ret = inflate(&strm, Z_NO_FLUSH);
	if (!(ret == Z_OK || ret == Z_STREAM_END)) {
		delete[] pDestBuff;
		return nullptr;
	}

	inflateEnd(&strm);
	return pDestBuff;
}
