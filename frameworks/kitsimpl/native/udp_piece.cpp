#include <cstdlib>
#include <cstring>

#include "hitbase.h"
#include "udp_piece.h"

void UdpPiece::Reset() {
    recvPieces = 0;
    totalSize = 0;
    totalPieces = 0;
    recvLen = 0;
}

int UdpPiece::Merge(uint8_t* data, size_t size) {
    // 先将接收到的数据buf拷贝到环形缓存中
    size_t bytesToWrite = buffer.Write(data, size);
    if (bytesToWrite != size) {
        LOGE("There is no enough space, only %{public}zu bytes, but need %{public}zu bytes", bytesToWrite, size);
        return -1;
    }

    /*
     * 重新收到的数据检测是否包含片头
     * 检测头部
     */
    while (buffer.GetSize() >= 2) {
        if (buffer[0] == 0xAF && buffer[1] == 0xAE) break;
        else buffer.PopFront(1);
    }

    int done = 0;
    while (buffer.GetSize() > HEAD_SIZE) {
        int32_t dataLen = buffer.GetBigEndian32(HEAD_POS_P_LENGTH);
        if (buffer.GetSize() >= HEAD_SIZE + dataLen) {
            int32_t pIndex = buffer.GetBigEndian32(HEAD_POS_P_INDEX);
            // 重置后第一次收到分片
            if (totalSize == 0) {
                /* 计算当前分片所属分片组数据的总大小 */
                totalSize = buffer.GetBigEndian32(HEAD_POS_TOTAL_SIZE);
                /* 计算当前分片所属分片组的总分片数量 */
                totalPieces = buffer.GetBigEndian32(HEAD_POS_TOTAL_PIECES);
                recvPieces = 0;
                recvLen = 0;
            }
            LOGD("buf size: %{public}zu, piece_data_len: %{public}d, p_index: %{public}d, recv_pieces: %{public}d, total_size: %{public}d, total_pieces: %{public}d",
                 buffer.GetSize(), dataLen, pIndex, recvPieces, totalSize, totalPieces);

            /* 计算当前分片所属分片组数据的总大小 */
            int32_t tmpTotalSize = buffer.GetBigEndian32(HEAD_POS_TOTAL_SIZE);
            /* 计算当前分片所属分片组的总分片数量 */
            int32_t tmpTotalPieces = buffer.GetBigEndian32(HEAD_POS_TOTAL_PIECES);

            if (tmpTotalSize != totalSize || tmpTotalPieces != totalPieces) {
                LOGI("Discard current frame, total_pieces:%{public}d, cur_total_pieces:%{public}d, recv_pieces:%{public}d, total_size:%{public}d, cur_total_size:%{public}d",
                     totalPieces, tmpTotalPieces, recvPieces, totalSize, tmpTotalSize);
                totalSize = tmpTotalSize;
                totalPieces = tmpTotalPieces;
                recvPieces = 1;
                recvLen = 0;
            } else {
                recvPieces++;
            }

            // 将帧头出队列
            buffer.PopFront(HEAD_SIZE);
            // 读取分片数据
            size_t bytesToRead = buffer.Read(recvBuf + PIECE_FIX_SIZE * pIndex, dataLen);
            if (bytesToRead != dataLen) {
                LOGE("There is no enough space, only %{public}zu bytes, but need %{public}zu bytes", bytesToRead, size);
                return -1;
            }

            LOGD("Remain size = %{public}zu", buffer.GetSize());

            recvLen += dataLen;
            if (recvPieces == totalPieces) {
                totalPieces = 0;
                recvPieces = 0;
                done = 1;
            }
        } else {
            LOGD("temp_size = %{public}zu, HEAD_SIZE + data_len = %{public}d", buffer.GetSize(), HEAD_SIZE + dataLen);
            break;
        }
    }

    return done;
}
