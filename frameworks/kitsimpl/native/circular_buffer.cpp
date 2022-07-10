#include <cstring>

#include "circular_buffer.h"

size_t CircularBuffer::Write(const uint8_t* data, size_t bytes) {
    if (bytes == 0) return 0;
    size_t bytesToWrite = std::min(bytes, mCapacity - mSize);

    /* Write in a single step */
    if (bytesToWrite <= mCapacity - mEndIndex) {
        memcpy(mData.get() + mEndIndex, data, bytesToWrite);
        mEndIndex += bytesToWrite;
        if (mEndIndex == mCapacity) mEndIndex = 0;
    } else /* Write in two steps */ {
        size_t size1 = mCapacity - mEndIndex;
        memcpy(mData.get() + mEndIndex, data, size1);
        size_t size2 = bytesToWrite - size1;
        memcpy(mData.get(), data + size1, size2);
        mEndIndex = size2;
    }

    mSize += bytesToWrite;
    return bytesToWrite;
}

size_t CircularBuffer::Read(uint8_t* data, size_t bytes) {
    if (bytes == 0) return 0;
    size_t bytesToRead = std::min(bytes, mSize);

    /* Read in a single step */
    if (bytesToRead <= mCapacity - mBeginIndex) {
        memcpy(data, mData.get() + mBeginIndex, bytesToRead);
        mBeginIndex += bytesToRead;
        if (mBeginIndex == mCapacity) mBeginIndex = 0;
    } else /* Read in two steps */ {
        size_t size1 = mCapacity - mBeginIndex;
        memcpy(data, mData.get() + mBeginIndex, size1);
        size_t size2 = bytesToRead - size1;
        memcpy(data + size1, mData.get(), size2);
        mBeginIndex = size2;
    }

    mSize -= bytesToRead;
    return bytesToRead;
}

void CircularBuffer::PopFront(size_t count) {
    size_t popBytes = std::min(count, mSize);

    /* pop in a single step */
    if (popBytes <= mCapacity - mBeginIndex) {
        mBeginIndex += popBytes;
        if (mBeginIndex == mCapacity) mBeginIndex = 0;
    } else /* pop in two steps */ {
        size_t size1 = mCapacity - mBeginIndex;
        size_t size2 = popBytes - size1;
        mBeginIndex = size2;
    }

    mSize -= popBytes;
}
