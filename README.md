# Zanux kernel
This is a kernel under develop.
## What works for now?
* A basic graphical text printing set of functions (src/gfx/print_text.c/h)
* AMD64's GDT and IDT
* Interruptions and IRQ's through Programmable Interval Time (PIT)
* A keyboard driver prototype
* Physical and Virtual Memory Management (PMM && VMM), including kmalloc/kmalloc and vmalloc/vfree.
* Ring 3 processes context switching (I'm using a dummy scheduler for now)
## How to compile?
Simple, just do guarantee you have the following dependencies: make, gcc, xorriso, qemu-system-x86_64, curl, tar and rustc and rustup with x86_64-unknown-none target.

### Installation
#### Ubuntu / Debian
```bash
sudo apt update
sudo apt install -y build-essential nasm xorriso qemu-system-x86 curl
rustup target add x86_64-unknown-none
```
#### Arch Linux
```bash
sudo pacman -S --needed base-devel nasm xorriso qemu-system-x86 curl rustup
rustup target add x86_64-unknown-none
```
#### Fedora
```bash
sudo dnf install -y @development-tools gcc nasm xorriso qemu-system-x86-core curl
rustup target add x86_64-unknown-none
```
Then, just do:
```bash
make run
```
