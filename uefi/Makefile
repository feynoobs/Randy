ARCH         = $(shell uname -m | sed s,i[3456789]86,ia32,)

OBJS         = boot.o
TARGET       = BOOTX64.EFI

EFIINC       = /usr/include/efi
EFIINCS      = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
EFILIB       = /usr/lib
EFI_CRT_OBJS = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS      = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS       = $(EFIINCS) -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -DEFI_FUNCTION_WRAPPER
LDFLAGS      = -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS)

all: $(TARGET)
BOOTX64.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi
%.EFI: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-$(ARCH) $^ $@
clean:
	rm -f $(TARGET) BOOTX64.so boot.o