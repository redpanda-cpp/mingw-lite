// LLVM compiler-rt

#if defined(__x86_64__)

  .text
  .balign 4
  .globl ___chkstk_ms
___chkstk_ms:
  push %rcx
  push %rax
  cmp  $0x1000,%rax
  lea  24(%rsp),%rcx
  jb   1f
2:
  sub  $0x1000,%rcx
  test %rcx,(%rcx)
  sub  $0x1000,%rax
  cmp  $0x1000,%rax
  ja   2b
1:
  sub  %rax,%rcx
  test %rcx,(%rcx)
  pop  %rax
  pop  %rcx
  ret

#elif defined(__aarch64__)

  .text
  .balign 4
  .globl ___chkstk
___chkstk:
  lsl  x16, x15, #4
  mov  x17, sp
1:
  sub  x17, x17, #0x1000
  subs x16, x16, #0x1000
  ldr  xzr, [x17]
  b.gt 1b

  ret

#elif defined(__i386__)

  .text
  .balign 4
  .globl ___chkstk_ms
___chkstk_ms:
  push %ecx
  push %eax
  cmp  $0x1000,%eax
  lea  12(%esp),%ecx
  jb   1f
2:
  sub  $0x1000,%ecx
  test %ecx,(%ecx)
  sub  $0x1000,%eax
  cmp  $0x1000,%eax
  ja   2b
1:
  sub  %eax,%ecx
  test %ecx,(%ecx)
  pop  %eax
  pop  %ecx
  ret

#else

#error Unsupported architecture

#endif
