# CustomOS 🖥️

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![OS: x86](https://img.shields.io/badge/Architecture-x86-blue.svg)](https://en.wikipedia.org/wiki/X86)
[![Status: Experimental](https://img.shields.io/badge/Status-Experimental-orange.svg)]()

CustomOS is a simple hobbyist operating system built from scratch in x86 assembly and C.
It features a protected-mode kernel, custom drivers, graphics support, and basic filesystem handling.  

---

## Features

### Core Architecture
- **Custom Bootloader** – Loads the kernel and initializes the system in x86 assembly.
- **Protected Mode** – Implements x86 protected mode with Global Descriptor Table (GDT) setup.
- **Paging** – 4KB paging for virtual memory management.
- **Interrupt Handling** – Configures the Interrupt Descriptor Table (IDT) for CPU and hardware interrupts.

### Graphics & UI
- **VESA Graphics Initialization** – Enables high-resolution graphics modes.
- **Linear Framebuffer Rendering** – Direct pixel manipulation for fast rendering.
- **BMP Image Loading** – Load images for UI elements and graphics.
- **Alpha-Blended Cursor** – Smooth, transparent mouse cursor.

### Storage & Filesystem
- **FAT16/32 Support** – Read files from FAT16 and FAT32 drives.
- **Directory Listing** – Browse files and directories.
- **File Reading** – Handles multi-cluster files.
- **Cluster Chain Traversal** – Traverses FAT table for sequential file access.

### Input & Hardware
- **PS/2 Keyboard Driver** – Full keyboard support.
- **PS/2 Mouse Driver** – Standard mouse input and movement.
- **PIC Controller Remapping** – Custom interrupt handling.
- **Serial Port Communication** – Debugging and data transfer.

---

## 🖼 Screenshots

- [ ] TODO; Add screenshots of the OS

---
