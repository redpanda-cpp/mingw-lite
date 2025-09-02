#pragma once

#include <processthreadsapi.h>

// YY-Thunks 1.1.6
// This structure stores the value for each attribute
typedef struct _PROC_THREAD_ATTRIBUTE_ENTRY
{
  DWORD_PTR Attribute; // PROC_THREAD_ATTRIBUTE_xxx
  SIZE_T cbSize;
  PVOID lpValue;
} PROC_THREAD_ATTRIBUTE_ENTRY, *LPPROC_THREAD_ATTRIBUTE_ENTRY;

// YY-Thunks 1.1.6
// This structure contains a list of attributes that have been added using
// UpdateProcThreadAttribute
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
  DWORD dwFlags;
  ULONG Size;
  ULONG Count;
  ULONG Reserved;
  LPPROC_THREAD_ATTRIBUTE_ENTRY lpExtendedFlags;
  PROC_THREAD_ATTRIBUTE_ENTRY Entries[ANYSIZE_ARRAY];
} PROC_THREAD_ATTRIBUTE_LIST, *LPPROC_THREAD_ATTRIBUTE_LIST;
