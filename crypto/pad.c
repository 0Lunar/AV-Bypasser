#include <stdio.h>
#include <stdint.h>
#include <memory.h>


void unpad( const uint8_t *padded, size_t paddedLen, uint8_t *unpadBuf, uint8_t padSize, size_t *newMsgLen ) {
    size_t msgLen;
    uint8_t padch;

    padch = padded[paddedLen - 1];

    if (padch > padSize)
        return;

    msgLen = paddedLen - (size_t)padch;

    if (newMsgLen)
        *newMsgLen = msgLen;

    for ( size_t n = paddedLen - 2; n >= msgLen; n-- ) {
        if (padded[n] != padch)
            return;
    }

    memcpy(unpadBuf, padded, msgLen);
    return;
}