use crate::ffi::gfx::kprint;

#[no_mangle]
extern "C" fn panic(msg: *const u8) -> !
{
    kprint(b"-- KERNEL PANIC! --\n\0".as_ptr());
    kprint(msg);

    unsafe {
        core::arch::asm!("cli");
        loop {
            core::arch::asm!("hlt");
        }
    }
}
