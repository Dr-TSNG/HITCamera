#pragma once

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <memory>

#include "circular_buffer.h"

#define SBUFFER 1000000
#define HEAD_POS_SYNC_WORD 0                  // 同步字
#define HEAD_POS_TOTAL_SIZE 2                 // 所有分片数据的大小（不包括HEAD） //total data length of all pieces
#define HEAD_POS_TOTAL_PIECES 4               // 所有分片的数量
#define HEAD_POS_P_INDEX 6                    // 分片序号，从0开始
#define HEAD_POS_P_LENGTH 8                   // 当前分片数据的大小
#define HEAD_SIZE 12                          // 头部大小
#define PIECE_FIX_SIZE (576 - 8 - 20 - 12)  //每个分片数据最大字节数
//此处我们定义为internat MTU - UDP_head - IP_head - 分片头，当然MTU的值也不是固定的

class UdpPiece {
public:
    inline explicit UdpPiece(size_t capacity)
            : buffer(capacity),
              recvBuf(std::make_unique<uint8_t[]>(SBUFFER)) {}

    /**
     * @brief 重置，这里不会重新分配资源，只是将部分参数重置到初始化状态
     */
    void Reset();

    /**
     * @brief 重组分片，使用环形缓存
     * @param data 分片数据的指针
     * @param size 分片数据的长度
     * @return  返回-1则重组失败，返回0则正在重组中，返回1则重组成功
     */
    int Merge(uint8_t* data, size_t size);

    [[nodiscard]]
    inline int GetRecvLen() const { return recvLen; }

    [[nodiscard]]
    inline const uint8_t* GetRecvBuf() const { return recvBuf.get(); }

private:
    int recvPieces = 0;                  // 当前已经接收的分片数量
    int totalSize = 0;                   // 总数据大小
    int totalPieces = 0;                 // 分片总数量
    int recvLen = 0;                     // 接收数据的长度
    CircularBuffer buffer;               // 缓存
    std::unique_ptr<uint8_t[]> recvBuf;  // 保存接收数据
};
