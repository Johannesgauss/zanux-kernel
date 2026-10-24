// was_todo: verify these values
// Update: done
/*
According to AMD64 official manual (volume 2, section 4.8)
G: 1 (4kb)
D: 0 .
L: Long mode, so 1
AVL: I'm not using it by now
Seglimit: 4 bits, skipped
P: 1 (present)
DPL: 3 or 0 (rings)
11: Don't ask me why
C: Conforming bit. MUST BE ZERO FOR SECURITY!
R and A: whatever, let's go by 0
*/
//  G    D    L   AVL  skip     P   DPL   paper  C (!!) anything
//0b1/**/0/**/1/**/0/**/0000/**/1/**/00/**/11/**/0/**/00
#define GDT_KERNEL_CODE_FLAGS 0b1010000010011000
// Only DPL and W are used
//  G   D/W  ??   AVL  skip     P   DPL   paper  EWA
//0b1/**/0/**/0/**/0/**/0000/**/1/**/00/**/10/**/010
#define GDT_KERNEL_DATA_FLAGS 0b1000000010010010
//  G    D    L   AVL  skip     P   DPL paper  C (!!) anything
//0b1/**/0/**/1/**/0/**/0000/**/1/**/11/**/11/**/0/**/00
#define GDT_USER_CODE_FLAGS 0b1010000011111000
// Only DPL and W are used
//  G   D/W  ??   AVL  skip     P   DPL   paper  EWA
//0b1/**/0/**/0/**/0/**/0000/**/1/**/11/**/10/**/010
#define GDT_USER_DATA_FLAGS 0b1000000011110010

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_DATA  0x1b  // 0x18 + 3 
#define GDT_USER_CODE  0x23  // 0x20 + 3
