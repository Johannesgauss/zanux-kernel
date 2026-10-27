# Nuke built-in rules.
.SUFFIXES:

# Delete the target of a failed recipe.
.DELETE_ON_ERROR:

# Default user QEMU flags. These are appended to the QEMU command calls.
QEMUFLAGS := -m 2G

# Internal QEMU flags that should not be changed by the user.
override QEMU_MACHINE_FLAGS := \
    -M q35
override QEMU_UEFI_FLAGS := \
    -drive if=pflash,unit=0,format=raw,file=edk2-ovmf-bins/ovmf-code-x86_64.fd,readonly=on

override IMAGE_NAME := template
override OUTPUT := kernel

# User controllable size of the HDD image, in MiB.
HDD_SIZE := 64

# Internal HDD geometry that should not be changed by the user. Older mtools
# require one; 64 heads of 32 sectors make a cylinder exactly 1 MiB in size.
override HDD_HEADS := 64
override HDD_SECTORS_PER_TRACK := 32
override HDD_CYLINDER_SECTORS := $(shell echo $$(( $(HDD_HEADS) * $(HDD_SECTORS_PER_TRACK) )))

# Internal HDD partition layout that should not be changed by the user. sgdisk
# lays the partition out as GPT before converting the table to MBR, so the
# first and last cylinders are left to the GPT structures.
override HDD_PART_START := $(HDD_CYLINDER_SECTORS)
override HDD_PART_SECTORS := $(shell echo $$(( ($(HDD_SIZE) - 2) * $(HDD_CYLINDER_SECTORS) )))
override HDD_PART_END := $(shell echo $$(( $(HDD_PART_START) + $(HDD_PART_SECTORS) - 1 )))
override HDD_PART_OFFSET := $(shell echo $$(( $(HDD_PART_START) * 512 )))

# Toolchain for building the 'limine' executable for the host.
HOST_CC := cc
HOST_CFLAGS := -g -O2 -pipe
HOST_CPPFLAGS :=
HOST_LDFLAGS :=
HOST_LIBS :=

# User controllable toolchain and toolchain prefix for the kernel.
TOOLCHAIN :=
TOOLCHAIN_PREFIX :=
ifneq ($(TOOLCHAIN),)
    ifeq ($(TOOLCHAIN_PREFIX),)
        TOOLCHAIN_PREFIX := $(TOOLCHAIN)-
    endif
endif

# User controllable C compiler command.
ifneq ($(TOOLCHAIN_PREFIX),)
    CC := $(TOOLCHAIN_PREFIX)gcc
else
    CC := cc
endif

# Defaults overrides for variables if using "llvm" as toolchain.
ifeq ($(TOOLCHAIN),llvm)
    CC := clang
endif

# User controllable C flags.
ifneq ($(filter run-debug,$(MAKECMDGOALS)),)
    CFLAGS := -g -O0 -pipe
    RUSTFLAGS := -g -O
else
    CFLAGS := -g -O2 -pipe
    RUSTFLAGS := -O
endif

# User controllable C preprocessor flags. We set none by default.
CPPFLAGS :=

# User controllable nasm flags.
NASMFLAGS := -g

# User controllable linker flags. We set none by default.
LDFLAGS :=

.PHONY: all
all: $(IMAGE_NAME).iso

.PHONY: all-hdd
all-hdd: $(IMAGE_NAME).hdd

.PHONY: kernel
kernel: bin/$(OUTPUT)

.PHONY: run
run: $(IMAGE_NAME).iso
	qemu-system-x86_64 \
		$(QEMU_MACHINE_FLAGS) \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		$(QEMUFLAGS)

.PHONY: run-debug
run-debug: $(IMAGE_NAME).iso
	qemu-system-x86_64 \
		$(QEMU_MACHINE_FLAGS) \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		$(QEMUFLAGS) \
		-s -S -no-reboot -d int -D qemu.log

.PHONY: gdb
gdb:
	gdb -ex "target remote :1234" -ex "symbol-file bin/$(OUTPUT)" -ex "set disassembly-flavor att"

.PHONY: run-uefi
run-uefi: edk2-ovmf-bins $(IMAGE_NAME).iso
	qemu-system-x86_64 \
		$(QEMU_MACHINE_FLAGS) \
		$(QEMU_UEFI_FLAGS) \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		$(QEMUFLAGS)

.PHONY: run-hdd
run-hdd: $(IMAGE_NAME).hdd
	qemu-system-x86_64 \
		$(QEMU_MACHINE_FLAGS) \
		-hda $(IMAGE_NAME).hdd \
		$(QEMUFLAGS)

.PHONY: run-hdd-uefi
run-hdd-uefi: edk2-ovmf-bins $(IMAGE_NAME).hdd
	qemu-system-x86_64 \
		$(QEMU_MACHINE_FLAGS) \
		$(QEMU_UEFI_FLAGS) \
		-hda $(IMAGE_NAME).hdd \
		$(QEMUFLAGS)

.INTERMEDIATE: edk2-ovmf-bins.tar.gz
edk2-ovmf-bins.tar.gz:
	curl -fL -o $@ https://github.com/osdev0/edk2-ovmf-stable-bins/releases/latest/download/edk2-ovmf-bins.tar.gz

edk2-ovmf-bins: edk2-ovmf-bins.tar.gz
	rm -rf edk2-ovmf-bins
	gunzip < edk2-ovmf-bins.tar.gz | tar -xf -

.INTERMEDIATE: boot/limine-binary.tar.gz
boot/limine-binary.tar.gz:
	mkdir -p boot
	curl -fL -o $@ https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz

boot/limine-binary/limine: boot/limine-binary.tar.gz
	rm -rf boot/limine-binary
	gunzip < boot/limine-binary.tar.gz | tar -xf - -C boot
	$(MAKE) -C boot/limine-binary \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)"


# Stop on a problem the checks below find, unless only cleaning, which needs no toolchain.
ifneq ($(filter-out clean distclean,$(or $(MAKECMDGOALS),default)),)
    override TOOLCHAIN_ERROR = $(error $(1))
else
    override TOOLCHAIN_ERROR =
endif

# Check if CC is Clang.
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep -q '^Target: '; echo $$?)

# Macros to check if a flag is supported.
override FLAG_NAME = -$(subst -,-?,$(patsubst -%,%,$(firstword $(subst =, ,$(1)))))
override FLAG_IS_NAMED = grep -qE '(error|warning|note):.*[^[:alnum:]]($(call FLAG_NAME,$(1))$(if $(2),|$(call FLAG_NAME,$(2))))([^-[:alnum:]]|$$)'
override CC_HAS_COMPILE_FLAG = $(shell LC_ALL=C $(CC) $(CFLAGS) $(1) -c -x c /dev/null -o /dev/null 2>&1 >/dev/null | $(call FLAG_IS_NAMED,$(1),$(2)); echo $$?)
override CC_HAS_LINK_FLAG = $(shell LC_ALL=C $(CC) $(CFLAGS) $(LDFLAGS) $(1) -Wl,--version 2>&1 >/dev/null | $(call FLAG_IS_NAMED,$(1),$(2)); echo $$?)
override LD_HAS_FLAG = $(shell ! LC_ALL=C $(CC) $(CFLAGS) $(LDFLAGS) -Wl,--help 2>/dev/null | grep -qE '(^|[[:space:]])-?$(1)([[:space:],=[]|$$)'; echo $$?)

# Internal C flags.
override CFLAGS += \
    -Wall \
    -Wextra \
    -Wpadded \
    -std=gnu11 \
    -fgnu89-inline \
    -ffreestanding \
    -fno-common \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mno-mmx \
    -mno-sse \
    -mno-red-zone \
    -mcmodel=kernel

# Internal C preprocessor flags.
override CPPFLAGS := \
    -nostdinc \
    -I src \
    -I src/drivers/gfx \
    -isystem $(shell $(CC) -print-file-name=include) \
    $(CPPFLAGS) \
    -MMD \
    -MP

# Internal nasm flags.
override NASMFLAGS := \
    -f elf64 \
    $(patsubst -g,-g -F dwarf,$(NASMFLAGS)) \
    -i src/ \
    -Wall

override RUSTFLAGS := \
    $(RUSTFLAGS) \
    --target x86_64-unknown-none \
    -C code-model=kernel \
    -C no-redzone=y \
    -C relocation-model=static \
    --emit=obj

# Always ask for a build ID.
override LDFLAGS := \
    -Wl,--build-id=sha1 \
    $(LDFLAGS)

ifeq ($(CC_IS_CLANG),1)
    override CC += \
        -target x86_64-unknown-none-elf
    override LDFLAGS := \
        -fuse-ld=lld \
        $(LDFLAGS)
endif

override LDFLAGS += \
    -nostdlib \
    -static \
    -Wl,-z,max-page-size=0x1000 \
    -Wl,-z,noexecstack \
    -Wl,--gc-sections \
    -Wl,-T,boot/linker-scripts/x86_64.lds \
    -Wl,-m,elf_x86_64

override CC_COMPILE_ERROR := $(shell LC_ALL=C $(CC) $(CFLAGS) -c -x c /dev/null -o /dev/null 2>&1 >/dev/null | grep -i 'error:' | head -n 1)
ifneq ($(CC_COMPILE_ERROR),)
    $(call TOOLCHAIN_ERROR,The compiler does not work with the given CFLAGS: $(CC_COMPILE_ERROR))
endif

ifneq ($(CC_IS_CLANG),1)
    override CC_EMITS_ELF := $(shell ! $(CC) $(CFLAGS) -dM -E -x c /dev/null 2>/dev/null | grep -q 'define __ELF__ 1'; echo $$?)
    ifneq ($(CC_EMITS_ELF),1)
        $(call TOOLCHAIN_ERROR,The compiler does not emit ELF objects)
    endif
endif

ifeq ($(call CC_HAS_COMPILE_FLAG,-fno-stack-clash-protection),1)
    override CFLAGS += -fno-stack-clash-protection
endif

ifeq ($(call CC_HAS_COMPILE_FLAG,-fcf-protection=none),1)
    override CFLAGS += -fcf-protection=none
endif

ifeq ($(call CC_HAS_COMPILE_FLAG,-mno-80387),1)
    override CFLAGS += -mno-80387
endif

ifeq ($(call CC_HAS_LINK_FLAG,-no-pie),1)
    override LDFLAGS += -no-pie
endif

ifeq ($(call LD_HAS_FLAG,-no-pie),1)
    override LDFLAGS += -Wl,-no-pie
endif

# Source files discovery in src/
override SRCFILES := $(shell find -L src -type f 2>/dev/null | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))
override RSFILES := $(filter %.rs,$(SRCFILES))
override RSOBJ := $(if $(wildcard src/lib.rs),obj/src/lib.rs.o,)
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o)) $(RSOBJ)
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d) $(NASMFILES:.asm=.asm.d))

# Link rules for the final kernel executable.
bin/$(OUTPUT): GNUmakefile boot/linker-scripts/x86_64.lds $(OBJ)
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

-include $(HEADER_DEPS)

obj/src/util/memory.c.o: override CFLAGS += -fno-lto

# Compilation rules for *.c files.
obj/%.c.o: %.c GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.S files.
obj/%.S.o: %.S GNUmakefile
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compilation rules for *.asm files.
obj/%.asm.o: %.asm GNUmakefile
	mkdir -p "$(dir $@)"
	nasm $(NASMFLAGS) -MD $(@:.o=.d) -MP $< -o $@

obj/src/lib.rs.o: src/lib.rs $(filter-out src/lib.rs,$(RSFILES)) GNUmakefile
	mkdir -p "$(dir $@)"
	rustc $(RUSTFLAGS) src/lib.rs -o $@

# ISO creation
$(IMAGE_NAME).iso: boot/limine-binary/limine bin/$(OUTPUT)
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp bin/$(OUTPUT) iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp boot/limine.conf boot/limine-binary/limine-bios.sys boot/limine-binary/limine-bios-cd.bin boot/limine-binary/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp boot/limine-binary/BOOTX64.EFI iso_root/EFI/BOOT/
	cp boot/limine-binary/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./boot/limine-binary/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

# HDD image creation
$(IMAGE_NAME).hdd: boot/limine-binary/limine bin/$(OUTPUT)
	rm -f $(IMAGE_NAME).hdd
	dd if=/dev/null bs=1024k seek=$(HDD_SIZE) of=$(IMAGE_NAME).hdd
	PATH=$$PATH:/usr/sbin:/sbin sgdisk $(IMAGE_NAME).hdd -n 1:$(HDD_PART_START):$(HDD_PART_END) -t 1:ef00 -m 1
	./boot/limine-binary/limine bios-install $(IMAGE_NAME).hdd
	mformat -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) -T $(HDD_PART_SECTORS) -h $(HDD_HEADS) -s $(HDD_SECTORS_PER_TRACK) ::
	mmd -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) bin/$(OUTPUT) ::/boot
	mcopy -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) boot/limine.conf boot/limine-binary/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) boot/limine-binary/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@$(HDD_PART_OFFSET) boot/limine-binary/BOOTIA32.EFI ::/EFI/BOOT

.PHONY: clean
clean:
	rm -rf bin obj boot/bin boot/obj iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd

.PHONY: distclean
distclean: clean
	rm -rf boot/limine-binary boot/limine-binary.tar.gz edk2-ovmf-bins edk2-ovmf-bins.tar.gz boot/.deps-obtained .cache compile_commands.json boot/freestanding-c-hdrs boot/cc-runtime boot/limine-protocol
