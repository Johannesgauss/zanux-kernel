unsafe extern "C" {
    #[link_name = "print_char"]
    pub fn c_print_char(c: u8);
    #[link_name = "kprint"]
    pub fn c_kprint(s: *const u8);
    #[link_name = "print_line"]
    pub fn c_print_line(s: *const u8);
}

pub fn print_char(char: u8)
{
    unsafe {
        c_print_char(char);
    }
}

pub fn kprint(line: *const u8)
{
    unsafe {
        c_kprint(line);
    }
}

pub fn print_line(line: *const u8)
{
    unsafe {
        c_print_line(line);
    }
}
