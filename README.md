<div align="center">

# 🛡️ AV Bypasser 
**An Educational Proof of Concept for Antivirus Evasion**

<!-- Badges per i linguaggi e lo stato del progetto -->
[![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://www.linux.org/)
[![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://www.microsoft.com/windows)
[![Status](https://img.shields.io/badge/Status-Educational_PoC-red?style=for-the-badge)](#-disclaimer)

</div>

<br>

## ⚠️ Disclaimer

> **Strictly for Educational and Research Purposes Only.**
>
> This project was developed to demonstrate the fundamental limitations of modern Antivirus (AV) software and Endpoint Detection and Response (EDR) solutions, illustrating how specific runtime manipulation techniques can evade standard defenses.
>
> **DO NOT** use this code for malicious purposes, unauthorized access, or on systems where you do not have explicit, written permission from the owner. The author assumes no liability and is not responsible for any misuse or damage caused by this program.

## 📖 Overview

A common misconception in cybersecurity is the blind trust placed in standard Antivirus solutions, often perceived as impenetrable shields. This project serves as a practical demonstration that AV engines are not flawless.

By employing a combination of cryptographic obfuscation and dynamic runtime memory manipulation, this Proof of Concept (PoC) successfully bypasses basic static and dynamic analysis checks. It features cross-platform support, compiling execution stubs tailored for both **POSIX-compliant systems (like Linux)** and **Windows**.

This project underscores the necessity of a **Defense in Depth** strategy rather than relying on a single point of failure for endpoint security.

## ⚙️ Architecture & Mechanics

The project is divided into two primary components: a cross-platform Python-based orchestrator/builder (`main_2.py`) and a C-based execution stub (`code_2.c`).

### 1. Payload Preparation & Encryption (`main_2.py`)

The Python script acts as the builder. It ingests raw shellcode (non-ELF/non-PE formatted) and prepares the evasion chain:

* **🔑 Key Generation:** Generates a cryptographically secure 32-byte random key.
* **🔒 Cryptographic Obfuscation:** Encrypts the raw shellcode using the **AES** (Advanced Encryption Standard) algorithm in ECB mode with PKCS7 padding.
* **💉 Stub Injection:** Dynamically injects the encrypted shellcode payload and the decryption key into predefined buffers within the C template (`code_2.c`).
* **🔨 Standalone Compilation (Anti-Analysis):** Compiles the resulting C code into an executable using `gcc` (supported on both Linux and Windows via MinGW/MSYS2). It intentionally includes a custom, hand-written AES library to keep the binary independent from system cryptographic libraries.
  * **Aggressive Optimization (`-O3`):** This flag is used to maximize execution efficiency. However, as a byproduct of heavy optimization (like function inlining and loop unrolling), the resulting machine code structure is drastically altered, making static analysis and reverse engineering significantly more difficult.
  * **Symbol Stripping (`-s`):** This flag instructs the compiler to strip all debugging symbols and symbol tables from the final binary. Removing this metadata deprives reverse engineers and automated analysis tools of critical context (such as function and variable names), adding a strong layer of obfuscation.

### 2. Runtime Execution & Memory Protection (`code_2.c`)

The compiled C executable is engineered to evade both static signatures (via the AES encryption) and dynamic behavioral analysis by utilizing OS-specific APIs:

* **🛑 Anti-Debugging:** 
  * **Linux:** Employs the `ptrace(PTRACE_TRACEME)` system call to detect if the process is being actively monitored or stepped through by a debugger.
  * **Windows:** Utilizes `IsDebuggerPresent()` and `CheckRemoteDebuggerPresent()` to detect attached debuggers. If a debugger is found, execution is immediately halted.
* **🔓 In-Memory Decryption:** The payload remains encrypted on disk and is only decrypted in memory at runtime, effectively bypassing static file scanning.
* **🧠 Secure Allocation:** 
  * **Linux:** Utilizes `mmap` to allocate a region of anonymous memory.
  * **Windows:** Uses `VirtualAlloc` (`MEM_COMMIT | MEM_RESERVE`) with `PAGE_READWRITE` permissions to securely host the decrypted shellcode.
* **🕵️‍♂️ Memory Forensics Evasion:**
  * **Linux:** Locks the memory pages (`mlock`) to prevent swapping and excludes the region from core dumps (`madvise` with `MADV_DONTDUMP`).
  * **Windows:** Uses `VirtualLock` to lock the memory pages into the working set, preventing them from being paged out to the disk and leaving forensic artifacts.
* **🛡️ Privilege Escalation & Execution:**
  * **Linux:** Modifies memory permissions using `mprotect` to `PROT_EXEC | PROT_READ`.
  * **Windows:** Uses `VirtualProtect` to transition the memory region to `PAGE_EXECUTE`.
  * Finally, the instruction pointer is redirected to the newly decrypted and protected memory region to execute the payload.

## 🚀 Usage

**Prerequisites:**
- Python 3.x
- `cryptography` and `pwntools` Python libraries
- `gcc` compiler installed and accessible in your system's PATH (For Windows, use MinGW-w64)

**Command Line:**
```bash
python main.py -f <path_to_raw_shellcode> -o <output_binary_name>
```

## 🕵️ Limitations & EDR Detection

While this Proof of Concept effectively bypasses traditional signature-based Antivirus solutions and static analysis, it is important to understand its limitations against modern Endpoint Detection and Response (EDR) systems. Advanced EDRs employ behavioral monitoring and heuristics that can still flag this execution flow:

* **Memory Permission Anomalies:** Dynamically allocating memory with Execute permissions (or transitioning memory from Read-Write to Read-Execute) is highly scrutinized. Legitimate applications typically execute code exclusively from their predefined `.text` sections. EDRs actively monitor for anomalous RX or RWX memory regions and will often flag this behavior as inherently suspicious.
* **Dynamic Analysis and API Hooking:** Modern security solutions hook critical system calls and user-mode APIs (such as `VirtualAlloc`, `VirtualProtect`, `mmap`, and `mprotect`). At the exact moment the payload is fully decrypted and execution is requested, the EDR can inspect the memory buffer in its plaintext form. If the in-memory payload matches known shellcode signatures or exhibits malicious behavior, the process will be terminated instantly.
* **Scope of Effectiveness:** This tool is primarily effective against static scanning, on-disk file-based hash comparisons, and basic sandboxing. It serves as a fundamental baseline to understand evasion mechanics, but advanced runtime monitoring requires far more complex techniques (such as unhooking, direct syscalls, or module stomping) to be successfully bypassed.