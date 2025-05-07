unsigned __stdcall GetACP();
unsigned __stdcall GetConsoleCP();
unsigned __stdcall GetConsoleOutputCP();
int __stdcall SetConsoleCP(unsigned);
int __stdcall SetConsoleOutputCP(unsigned);

unsigned __mingw_lite_crt_saved_console_input_code_page;
unsigned __mingw_lite_crt_saved_console_output_code_page;

void __attribute__((constructor))
__mingw_lite_crt_set_console_code_page()
{
  __mingw_lite_crt_saved_console_input_code_page = GetConsoleCP();
  __mingw_lite_crt_saved_console_output_code_page = GetConsoleOutputCP();
  unsigned acp = GetACP();
  SetConsoleCP(acp);
  SetConsoleOutputCP(acp);
}

void __attribute__((destructor))
__mingw_lite_crt_restore_console_code_page()
{
  SetConsoleCP(__mingw_lite_crt_saved_console_input_code_page);
  SetConsoleOutputCP(__mingw_lite_crt_saved_console_output_code_page);
}
