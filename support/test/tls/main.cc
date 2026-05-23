#include <semaphore>
#include <thread>

#include <stdio.h>

#include <libloaderapi.h>

std::binary_semaphore s_thread(0), s_loader(0);
void (*check_tls_fn)(void) = nullptr;

void f1() {
  printf("[t1; start]\n");
  s_thread.release();
  s_loader.acquire();
  printf("[t1; ");
  check_tls_fn();
  printf("[t1; end]\n");
}

void f2() {
  printf("[t2; start]\n");
  printf("[t2; ");
  check_tls_fn();
  printf("[t2; end]\n");
}

int main() {
  std::thread t1(f1);
  s_thread.acquire();

  HMODULE h = LoadLibraryW(L"lib.dll");
  check_tls_fn = (void (*)())GetProcAddress(h, "check_tls");

  printf("[main; ");
  check_tls_fn();

  s_loader.release();
  t1.join();

  std::thread t2(f2);
  t2.join();

  return 0;
}
