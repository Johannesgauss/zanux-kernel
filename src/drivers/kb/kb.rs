use crate::port::inb;

const BUFFER_SIZE: usize = 256;
static mut KB_BUFFER: [u8; BUFFER_SIZE] = [0; BUFFER_SIZE];
static mut HEAD: usize = 0;
static mut TAIL: usize = 0;

static mut SHIFT: bool = false;
static mut CAPS: bool = false;

static SCAN_NORMAL: [u8; 128] = [
    0,   27,  b'1', b'2', b'3', b'4', b'5', b'6', b'7', b'8', b'9', b'0', b'-', b'=', 8,
    b'\t', b'q', b'w', b'e', b'r', b't', b'y', b'u', b'i', b'o', b'p', b'[', b']', b'\n',
    0,   b'a', b's', b'd', b'f', b'g', b'h', b'j', b'k', b'l', b';', b'\'', b'`',
    0,   b'\\', b'z', b'x', b'c', b'v', b'b', b'n', b'm', b',', b'.', b'/', 0,
    b'*', 0,   b' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
];

static SCAN_SHIFT: [u8; 128] = [
    0,   27,  b'!', b'@', b'#', b'$', b'%', b'^', b'&', b'*', b'(', b')', b'_', b'+', 8,
    b'\t', b'Q', b'W', b'E', b'R', b'T', b'Y', b'U', b'I', b'O', b'P', b'{', b'}', b'\n',
    0,   b'A', b'S', b'D', b'F', b'G', b'H', b'J', b'K', b'L', b':', b'"', b'~',
    0,   b'|', b'Z', b'X', b'C', b'V', b'B', b'N', b'M', b'<', b'>', b'?', 0,
    b'*', 0,   b' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
];

fn push_char(c: u8) {
    unsafe {
        let next = (HEAD + 1) % BUFFER_SIZE;
        if next != TAIL {
            if let Some(slot) = KB_BUFFER.get_mut(HEAD) {
                *slot = c;
            }
            HEAD = next;
        }
    }
}

#[no_mangle]
pub extern "C" fn kb_getchar() -> i32 {
    unsafe {
        if TAIL == HEAD {
            -1
        } else {
            let c = match KB_BUFFER.get(TAIL) {
                Some(&byte) => byte as i32,
                None => -1,
            };
            TAIL = (TAIL + 1) % BUFFER_SIZE;
            c
        }
    }
}

#[no_mangle]
pub extern "C" fn kb_has_char() -> bool {
    unsafe { TAIL != HEAD }
}

#[no_mangle]
pub extern "C" fn kb_handler() {
    let scancode = inb(0x60);

    if scancode == 0xAA || scancode == 0xB6 {
        unsafe { SHIFT = false; }
        return;
    }
    if scancode & 0x80 != 0 {
        return;
    }

    if scancode == 0x2A || scancode == 0x36 {
        unsafe { SHIFT = true; }
        return;
    }
    if scancode == 0x3A {
        unsafe { CAPS = !CAPS; }
        return;
    }

    let code = (scancode & 0x7F) as usize;

    let table = unsafe {
        if SHIFT { &SCAN_SHIFT } else { &SCAN_NORMAL }
    };

    let mut ch = match table.get(code) {
        Some(&c) if c != 0 => c,
        _ => return,
    };

    unsafe {
        if CAPS {
            if ch >= b'a' && ch <= b'z' { ch -= 32; }
            else if ch >= b'A' && ch <= b'Z' { ch += 32; }
        }
    }

    push_char(ch);
}
