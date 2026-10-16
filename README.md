<div align="center">

# 🛡️ AV Bypasser 
**An Educational Proof of Concept for Antivirus Evasion**

<!-- Badges per i linguaggi e lo stato del progetto -->
[![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://www.linux.org/)
[![Status](https://img.shields.io/badge/Status-Educational_PoC-red?style=for-the-badge)](#-disclaimer)

</div>

<br>

## ⚠️ Disclaimer

> **Strictly for Educational and Research Purposes Only.**
>
> This project was developed to demonstrate the fundamental limitations of modern Antivirus (AV) software and to illustrate how specific runtime manipulation techniques can evade standard defenses.
>
> **DO NOT** use this code for malicious purposes, unauthorized access, or on systems where you do not have explicit, written permission from the owner. The author assumes no liability and is not responsible for any misuse or damage caused by this program.

## 📖 Overview

A common misconception in cybersecurity is the blind trust placed in standard Antivirus solutions, often perceived as impenetrable shields. This project serves as a practical demonstration that AV engines are not flawless.

By employing a combination of cryptographic obfuscation and dynamic runtime memory manipulation, this Proof of Concept (PoC) successfully bypasses basic static and dynamic analysis checks. Currently, this iteration is designed specifically for POSIX-compliant systems (such as Linux).

This project underscores the necessity of a **Defense in Depth** strategy rather than relying on a single point of failure for endpoint security.

## ⚙️ Architecture & Mechanics

The project is divided into two primary components: a Python-based orchestrator/builder (`main.py`) and a C-based execution stub (`code.c`).

### 1. Payload Preparation & Encryption (`main.py`)

The Python script acts as the builder. It ingests raw shellcode (non-ELF formatted) and prepares the evasion chain:

* **🔑 Key Generation:** Generates a cryptographically secure 32-byte random key.
* **🔒 Cryptographic Obfuscation:** Encrypts the raw shellcode using the **AES** (Advanced Encryption Standard) algorithm.
* **💉 Stub Injection:** Dynamically injects the encrypted shellcode payload and the decryption key into predefined buffers within the C template (`code.c`).
* **🔨 Standalone Compilation (Anti-Analysis):** Compiles the resulting C code into an executable using `gcc`. It intentionally includes a custom, hand-written AES library to keep the binary independent from system cryptographic libraries.
  * **Aggressive Optimization (`-O3`):** This flag is used to maximize execution efficiency. However, as a byproduct of heavy optimization (like function inlining and loop unrolling), the resulting machine code structure is drastically altered, making static analysis and reverse engineering significantly more difficult.
  * **Symbol Stripping (`-s`):** This flag instructs the compiler to strip all debugging symbols and symbol tables from the final binary. Removing this metadata deprives reverse engineers and automated analysis tools of critical context (such as function and variable names), adding a strong layer of obfuscation.

### 2. Runtime Execution & Memory Protection (`code.c`)

The compiled C executable is engineered to evade both static signatures (via the AES encryption) and dynamic behavioral analysis:

* **🛑 Anti-Debugging:** Employs the `ptrace(PTRACE_TRACEME)` system call to detect if the process is being actively monitored or stepped through by a debugger. If attached, execution is immediately halted.
* **🔓 In-Memory Decryption:** The payload remains encrypted on disk and is only decrypted in memory at runtime, effectively bypassing static file scanning.
* **🧠 Secure Allocation:** Utilizes `mmap` to allocate a region of anonymous memory with specific execution privileges to host the decrypted shellcode.
* **🕵️‍♂️ Memory Forensics Evasion:**
  * **`mlock`:** Locks the memory pages containing the plaintext payload, preventing them from being swapped out to disk (which would leave forensic artifacts).
  * **`madvise (MADV_DONTDUMP)`:** Instructs the kernel to exclude the shellcode's memory region from core dumps in the event of a crash, thwarting post-mortem analysis.
* **🚀 Execution:** Finally, the instruction pointer is redirected to the newly decrypted and protected memory region to execute the payload.

## 💡 Conclusion

This PoC demonstrates that by masking the true intent of an executable until the exact moment of execution and employing targeted anti-analysis techniques, traditional AV controls can be subverted. True security requires comprehensive monitoring, behavioral heuristics, and a layered defensive architecture.