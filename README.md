<div align="center">
  <h1>💻 C Exercises 💻</h1>
  <p>Programming exercises in <strong>C</strong></p>

![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=00599C&labelColor=fff&color=00599C)
![Arch](https://img.shields.io/badge/Arch-1793D1?logo=archlinux&logoColor=1793D1&labelColor=fff&color=1793D1)

</div>

## 🌟 Description

This repository contains progressive programming exercises in C, designed to provide a comprehensive learning path from the fundamentals of the language to specialization in low-level systems.

Explore the true power of C, direct interaction with the machine. Start with logic, data structures, algorithms, files and culminate in direct manipulation of processes, memory, threads, and networks directly via the operating system.

Practical examples include:

- **Basic logic**: loops, arrays, and recursion.
- **Data structures**: implementation of lists, trees, and hash tables.
- **Algorithms**: sorting, searching, and complexity analysis.
- **System calls**: `fork`, `exec`, `pipe`, `signals`, and process control.
- **Networking & concurrency**: TCP sockets and multithreading with `pthreads`.

## 📂 Exercises

| Category            | Count | Level | Description                                                           |
| ------------------- | ----- | ----- | --------------------------------------------------------------------- |
| **Fundamentals**    | 8     | 🟢    | Core C syntax, control structures, and recursive logic.               |
| **Data Structures** | 12    | 🟡    | Implementation of dynamic structures (Lists, Stacks, Queues, Trees).  |
| **Algorithms**      | 10    | 🟡    | Implementation of sorting and search algorithms, graph traversal.     |
| **File Handling**   | 4     | 🟡    | Handling text and binary files.                                       |
| **Specialization**  | 10    | 🔴    | Implementation of process management, IPC, Thread Pools, and Sockets. |

## 📋 Installation Requirements

- **Operating System**: Arch Linux or WSL2 on Windows.
- **Architecture**: x86-64 (64-bit).
- **C Compiler GCC**: version 15.2.1 or higher.
- **C Debugger GDB**: version 17.1 or higher.
- **Just command runner**: version 1.46.0 or higher.

## 🚀 Execution Instructions

### 🖥️ Arch Linux/WSL (Recommended)

1. **Clone the repository**:
   ```bash
   git clone https://github.com/edavnix/c-systems-fundamentals.git
   ```
   > **Note**: When using Arch Linux natively, proceed to **Step 4**. For Windows environments, complete all steps to install and configure WSL Arch.
2. **Download and install WSL Arch** (PowerShell):
   ```bash
   wsl --install -d archlinux
   ```
3. **Restart the system** and access Arch Linux.
4. **Install GDB, compilation tools and Just**:
   ```bash
   pacman -Syu
   pacman -S gdb base-devel just
   ```
5. **Verify installation**:
   ```bash
   gcc --version    # e.g. gcc (GCC) 15.2.1 or higher
   gdb --version    # e.g. GNU gdb (GDB) 17.1 or higher
   just --version   # e.g. just 1.46.0 or higher
   uname -m         # e.g. x86_64
   ```
6. **Navigate to the directory**:
   ```bash
   cd c-systems-fundamentals
   ```
7. **Execute exercises** use `just run` followed by the file name or path:
   ```bash
   just                                                 # view available commands
   just list                                            # display exercises by modules
   just run 01_multi_calculator.c                       # practical example by file name
   just run src/01_fundamentals/01_multi_calculator.c   # practical example by full path
   just check                                           # verify that all exercises compile
   ```

### 🌐 OneCompiler (No Installation Required)

Code execution without local installation:

1. Access **[OneCompiler - C](https://onecompiler.com/c)**.
2. Copy and paste the exercise code.
3. For exercises requiring input:
   - Navigate to the **STDIN** panel.
   - Enter values separated by spaces.
4. Click **Run**.

> **Note**: Online compilers will **not work** for the **Specialization** module (System Calls, Threads, Sockets) as they require a real OS environment.

## 📖 Official Documentation

Additional resources:

- **[C Reference](https://en.cppreference.com/w/c)** - Complete C language reference.
- **[Linux Man Pages](https://linux.die.net/man/)** - Reference for system calls.
- **[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)** - Excellent guide for sockets.

<div align="center">
  <br>
  <img
    src="https://img.shields.io/badge/Made%20with-C%20%26%20Dedication-00599C?style=for-the-badge"
    alt="Made with C"
  />
  <br><br>
  <p>⭐ <strong>Star this repository to show support</strong> ⭐</p>
</div>
