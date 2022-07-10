#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>

class CircularBuffer {
public:
    inline explicit CircularBuffer(size_t capacity)
            : mCapacity(capacity),
              mData(std::make_unique<uint8_t[]>(capacity)) {}

    inline void Reset() {
        mBeginIndex = 0;
        mEndIndex = 0;
        mSize = 0;
    }

    size_t Write(const uint8_t* data, size_t bytes);

    size_t Read(uint8_t* data, size_t bytes);

    void PopFront(size_t count);

    inline uint8_t operator[](size_t index) const {
        return mData[(index + mBeginIndex) % mCapacity];
    }

    [[nodiscard]]
    inline int32_t GetBigEndian32(size_t index) const {
        return (operator[](index) << 8) + operator[](index + 1);
    }

    [[nodiscard]]
    inline size_t GetSize() const { return mSize;}

private:
    const size_t mCapacity;
    size_t mBeginIndex = 0;
    size_t mEndIndex = 0;
    size_t mSize = 0;
    std::unique_ptr<uint8_t[]> mData;
};
