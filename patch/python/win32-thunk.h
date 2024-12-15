#pragma once

#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <pathcch.h>
#include <strsafe.h>

static void Win32Thunk_GetSystemTimePreciseAsFileTime(
	_Out_ LPFILETIME lpSystemTimeAsFileTime
) {
	typedef __typeof__ (&GetSystemTimePreciseAsFileTime) type;
	static type real = 0;
	static int realInit = 0;

	if (!realInit) {
		real = (type)GetProcAddress(GetModuleHandleA("kernel32"), "GetSystemTimePreciseAsFileTime");
		realInit = 1;
	}
	if (real)
		return real(lpSystemTimeAsFileTime);

	return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

// from YY-Thunks
static bool __fastcall Win32Thunk_Internal_IsHexDigitWorker(wchar_t ch)
{
	return (ch >= L'0' && ch <= L'9') || (ch >= L'A' && ch <= L'F') || (ch >= L'a' && ch <= L'f');
}

// from YY-Thunks
static bool __fastcall Win32Thunk_Internal_StringIsGUIDWorker(LPCWSTR szStr)
{
	return szStr[0] == L'{'
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[1])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[2])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[3])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[4])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[5])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[6])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[7])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[8])
		&& szStr[9] == L'-'
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[10])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[11])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[12])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[13])
		&& szStr[14] == L'-'
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[15])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[16])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[17])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[18])
		&& szStr[19] == L'-'
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[20])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[21])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[22])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[23])
		&& szStr[24] == L'-'
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[25])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[26])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[27])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[28])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[29])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[30])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[31])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[32])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[33])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[34])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[35])
		&& Win32Thunk_Internal_IsHexDigitWorker(szStr[36])
		&& szStr[37] == L'}';
}

// from YY-Thunks
static bool Win32Thunk_Internal_PathIsVolumeGUIDWorker(LPCWSTR pszPath)
{
	return pszPath[0] == L'\\'
		&& pszPath[1] == L'\\'
		&& pszPath[2] == L'?'
		&& pszPath[3] == L'\\'
		&& (pszPath[4] == L'V' || pszPath[4] == L'v')
		&& (pszPath[5] == L'O' || pszPath[5] == L'o')
		&& (pszPath[6] == L'L' || pszPath[6] == L'l')
		&& (pszPath[7] == L'U' || pszPath[7] == L'u')
		&& (pszPath[8] == L'M' || pszPath[8] == L'm')
		&& (pszPath[9] == L'E' || pszPath[9] == L'e')
		&& Win32Thunk_Internal_StringIsGUIDWorker(pszPath + 10);
}

// modified from YY-Thunks
static BOOL Win32Thunk_PathIsUNCEx(
	_In_ LPCWSTR pszPath,
	_Outptr_opt_ PCWSTR* ppszServer
) {
	typedef __typeof__(&PathIsUNCEx) type;
	static type real;
	static int realInit = 0;

	if (!realInit) {
		HMODULE module = GetModuleHandleA("api_ms_win_core_path_l1_1_0");
		if (module)
			real = (type)GetProcAddress(module, "PathIsUNCEx");
		realInit = 1;
	}
	if (real)
		return real(pszPath, ppszServer);

	if (ppszServer)
		*ppszServer = NULL;

	if (pszPath[0] == L'\\' && pszPath[1] == L'\\')
	{
		if (pszPath[2] == L'?')
		{
			if (pszPath[3] == L'\\'
				&& (pszPath[4] == L'U' || pszPath[4] == L'u')
				&& (pszPath[5] == L'N' || pszPath[5] == L'n')
				&& (pszPath[6] == L'C' || pszPath[6] == L'c')
				&& pszPath[7] == L'\\')
			{
				pszPath += 8;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			//PathIsVolumeGUIDWorker 判断是多余的，因为 "\\\\?\\Volume{GUID}" 这样的形式走不到这个分支

			pszPath += 2;
		}

		if (ppszServer)
			*ppszServer = pszPath;

		return TRUE;
	}

	return FALSE;
}

// modified from YY-Thunks
static BOOL Win32Thunk_PathCchIsRoot(
	_In_opt_ PCWSTR pszPath
) {
	// NULL or ""
	if (pszPath == NULL || pszPath[0] == L'\0')
		return FALSE;


	// "C:\\" 形式
	if (((L'A' <= pszPath[0] && pszPath[0] <= L'Z') || (L'a' <= pszPath[0] && pszPath[0] <= L'z')) // iswalpha(pszPath[0])
		&& pszPath[1] == L':'
		&& pszPath[2] == L'\\'
		&& pszPath[3] == L'\0')
	{
		return TRUE;
	}

	// "\\" 形式
	if (pszPath[0] == L'\\' && pszPath[1] == L'\0')
	{
		return TRUE;
	}


	/*
	* "\\\path1\\path2"
	* "\\\\path1"
	* "\\\\?\\UNC\\path1\\path2"
	* "\\\\?\\UNC\\path1"
	*/
	PCWSTR pszServer;
	if (Win32Thunk_PathIsUNCEx(pszPath, &pszServer))
	{
		DWORD nSeparatorsCount = 0;

		for (; *pszServer;++pszServer)
		{
			if (*pszServer == L'\\')
			{
				++nSeparatorsCount;

				if (nSeparatorsCount > 1 || pszServer[1] == L'\0')
					return FALSE;
			}
		}

		return TRUE;
	}

	/*
	* "\\\\?\\C:\\"
	* "\\\\?\\Volume{guid}\\"
	*/

	if (pszPath[0] == L'\\'
		&& pszPath[1] == L'\\'
		&& pszPath[2] == L'?'
		&& pszPath[3] == L'\\')
	{
		// "\\\\?\\C:\\"
		if (((L'A' <= pszPath[4] && pszPath[4] <= L'Z') || (L'a' <= pszPath[4] && pszPath[4] <= L'z')) // iswalpha(pszPath[4])
			&& pszPath[5] == L':'
			&& pszPath[6] == L'\\'
			&& pszPath[7] == L'\0')
		{
			return TRUE;
		}

		// "\\\\?\\Volume{guid}\\"
		if (Win32Thunk_Internal_PathIsVolumeGUIDWorker(pszPath)
			&& pszPath[48] == L'\\'
			&& pszPath[49] == L'\0')
		{
			return TRUE;
		}

	}

	return FALSE;

}

// modified from YY-Thunks
static HRESULT Win32Thunk_PathCchSkipRoot(
	_In_ PCWSTR pszPath,
	_Outptr_ PCWSTR* ppszRootEnd
) {
	typedef __typeof__(&PathCchSkipRoot) type;
	static type real;
	static int realInit = 0;

	if (!realInit) {
		HMODULE module = GetModuleHandleA("api_ms_win_core_path_l1_1_0");
		if (module)
			real = (type)GetProcAddress(module, "PathCchSkipRoot");
		realInit = 1;
	}
	if (real)
		return real(pszPath, ppszRootEnd);

	if (pszPath == NULL || *pszPath == L'\0' || ppszRootEnd == NULL)
		return E_INVALIDARG;

	*ppszRootEnd = NULL;

	PCWSTR szRootEnd;
	if (Win32Thunk_PathIsUNCEx(pszPath, &szRootEnd))
	{
		//如果是UNC路径，则尝试跳过 2个 '\\'
		DWORD nSeparate = 0;

		for (; *szRootEnd && nSeparate < 2; ++szRootEnd)
		{
			if (*szRootEnd == L'\\')
			{
				++nSeparate;
			}
		}
	}
	else if (pszPath[0] == L'\\' && pszPath[1] != L'\\')
	{
		// "\\XXXXXX" 一个相对路径
		szRootEnd = pszPath + 1;
	}
	else
	{
		if (Win32Thunk_Internal_PathIsVolumeGUIDWorker(pszPath))
		{
			szRootEnd = pszPath + 48;
		}
		else if(pszPath[0] == L'\\'
			&& pszPath[1] == L'\\'
			&& pszPath[2] == L'?'
			&& pszPath[3] == L'\\'
			&& ((L'A' <= pszPath[4] && pszPath[4] <= L'Z') || (L'a' <= pszPath[4] && pszPath[4] <= L'z')) // iswalpha(pszPath[4])
			&& pszPath[5] == L':')
		{
			// "\\\\?\\C:\\"
			szRootEnd = pszPath + 6;
		}
		else if(((L'A' <= pszPath[0] && pszPath[0] <= L'Z') || (L'a' <= pszPath[0] && pszPath[0] <= L'z')) // iswalpha(pszPath[0])
			&& pszPath[1] == L':')
		{
			// "C:\\"
			szRootEnd = pszPath + 2;
		}
		else
		{
			return E_INVALIDARG;
		}

		if (*szRootEnd == L'\\')
			++szRootEnd;
	}

	*ppszRootEnd = szRootEnd;

	return S_OK;
}

// from YY-Thunks
static bool Win32Thunk_Interenal_IsExtendedLengthDosDevicePath(LPCWSTR pszPath)
{
	return pszPath[0] == L'\\'
		&& pszPath[1] == L'\\'
		&& pszPath[2] == L'?'
		&& pszPath[3] == L'\\';
}

// from YY-Thunks
static LPCWSTR Win32Thunk_Interenal_FindPreviousBackslashWorker(LPCWSTR szPathStart, LPCWSTR szPathEnd)
{
	for (; szPathStart < szPathEnd;)
	{
		--szPathEnd;

		if (*szPathEnd == L'\\')
			return szPathEnd;
	}

	return NULL;
}

// modified from YY-Thunks
static HRESULT Win32Thunk_PathCchCanonicalizeEx(
	_Out_writes_(cchPathOut) PWSTR pszPathOut,
	_In_ size_t cchPathOut,
	_In_ PCWSTR pszPathIn,
	_In_ ULONG dwFlags
) {
	typedef __typeof__(&PathCchCanonicalizeEx) type;
	static type real;
	static int realInit = 0;

	if (!realInit) {
		HMODULE module = GetModuleHandleA("api_ms_win_core_path_l1_1_0");
		if (module)
			real = (type)GetProcAddress(module, "PathCchCanonicalizeEx");
		realInit = 1;
	}
	if (real)
		return real(pszPathOut, cchPathOut, pszPathIn, dwFlags);


	if (pszPathOut && cchPathOut)
		*pszPathOut = L'\0';

	if (cchPathOut == 0 || cchPathOut > PATHCCH_MAX_CCH)
	{
		return E_INVALIDARG;
	}

	// MAX_PATH 长度的路径重新禁止长命名
	if (cchPathOut <= MAX_PATH)
		dwFlags &= ~PATHCCH_ALLOW_LONG_PATHS;
	else if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0)
		cchPathOut = MAX_PATH;

	DWORD ulReservedSize = 0;

	PWSTR pTempOut = pszPathOut;
	const PWSTR pszPathOutEnd = pszPathOut + cchPathOut;

	HRESULT hr = S_OK;

	do
	{
		PCWSTR pszServer;
		const bool bUNC = Win32Thunk_PathIsUNCEx(pszPathIn, &pszServer);
		if (bUNC)
		{
			if (dwFlags & PATHCCH_ALLOW_LONG_PATHS)
			{
				ulReservedSize = 6;

				if (pTempOut + 8 >= pszPathOutEnd)
				{
					hr = STRSAFE_E_INSUFFICIENT_BUFFER;
				}
				else
				{
					*pTempOut++ = L'\\';
					*pTempOut++ = L'\\';
					*pTempOut++ = L'?';
					*pTempOut++ = L'\\';
					*pTempOut++ = L'U';
					*pTempOut++ = L'N';
					*pTempOut++ = L'C';
					*pTempOut++ = L'\\';
					*pTempOut = L'\0';
				}
			}
			else
			{
				if (pTempOut + 2 >= pszPathOutEnd)
				{
					hr = STRSAFE_E_INSUFFICIENT_BUFFER;
				}
				else
				{
					*pTempOut++ = L'\\';
					*pTempOut++ = L'\\';
					*pTempOut = L'\0';
				}
			}
		}
		else
		{
			pszServer = pszPathIn;

			bool bSupportDosDevicePath = false;

			if (Win32Thunk_Interenal_IsExtendedLengthDosDevicePath(pszPathIn))
			{
				if ((L'A' <= pszPathIn[4] && pszPathIn[4] <= L'Z') || (L'a' <= pszPathIn[4] && pszPathIn[4] <= L'z')
					&& pszPathIn[5] == L':')
				{
					pszServer += 4;
					bSupportDosDevicePath = true;
				}
			}
			else if ((L'A' <= pszPathIn[0] && pszPathIn[0] <= L'Z') || (L'a' <= pszPathIn[0] && pszPathIn[0] <= L'z')
				&& pszPathIn[1] == L':')
			{
				bSupportDosDevicePath = true;
			}

			if (bSupportDosDevicePath && (dwFlags & PATHCCH_ALLOW_LONG_PATHS))
			{
				ulReservedSize = 4;

				if (pTempOut + 4 >= pszPathOutEnd)
				{
					hr = STRSAFE_E_INSUFFICIENT_BUFFER;
				}
				else
				{
					*pTempOut++ = L'\\';
					*pTempOut++ = L'\\';
					*pTempOut++ = L'?';
					*pTempOut++ = L'\\';
					*pTempOut = L'\0';
				}
			}
		}


		//LABEL_49
		//剩余部分是否 <= MAX_PATH，如果满足条件，那么应该重新开始。并且将路径限制在 MAX_PATH
		if ((dwFlags & PATHCCH_ALLOW_LONG_PATHS) && ulReservedSize && cchPathOut <= ulReservedSize + MAX_PATH)
		{
			if (cchPathOut > MAX_PATH)
				cchPathOut = MAX_PATH;

			ulReservedSize = 0;
			pszServer = pszPathIn;

			pTempOut = pszPathOut;

			//缓冲区必然 >= 1 ,所以我们可以直接设置。
			*pTempOut = L'\0';
			// 清除错误，方便我们重新开始。
			hr = S_OK;
		}


		if (FAILED(hr))
			break;

		for (;*pszServer;)
		{
			/* 获取下一个文件名，故意不用微软的方案，而且效率低……
			* 遇到 '\\' 则说明文件名结束
			* 遇到 '\0' 则说明路径也结束了
			*/
			unsigned cchFileName = 0;
			for (; pszServer[cchFileName] && pszServer[cchFileName] != L'\\'; ++cchFileName)
			{
				/* 安全检查
				* 1. 普通路径中，文件名最多 256个字符
				* 2. 路径最大处理长度为 PATHCCH_MAX_CCH
				*/

				if (((dwFlags & PATHCCH_ALLOW_LONG_PATHS) == 0 && cchFileName > 256)
					|| cchFileName >= PATHCCH_MAX_CCH)
				{
					hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
					goto OnFailedLabel;
				}
			}

			if (cchFileName == 1 && pszServer[0] == L'.')
			{
				//表示当前目录

				++pszServer;

				if (*pszServer)
				{
					//说明这里是 '\\'，继续跳过
					++pszServer;
				}
				else
				{
					//这里是 L'\0'

					if (pTempOut > pszPathOut && Win32Thunk_PathCchIsRoot(pszPathOut) == FALSE)
					{
						//不是跟目录，而且结束，那么需要删除最后的 '\\'
						--pTempOut;
						*pTempOut = L'\0';
					}
				}

				continue;
			}
			else if (cchFileName == 2
				&& pszServer[0] == L'.'
				&& pszServer[1] == L'.')
			{
				//表示上级目录

				pszServer += 2;

				if (pTempOut <= pszPathOut || Win32Thunk_PathCchIsRoot(pszPathOut))
				{
					//文件是根目录，或者开始位置时，遇到 ".." 则直接忽略即可。

					if (*pszServer == L'\\')
						++pszServer;
				}
				else
				{
					wchar_t* szPrevious = (wchar_t*)Win32Thunk_Interenal_FindPreviousBackslashWorker(pszPathOut, pTempOut - 1);

					pTempOut = szPrevious ? szPrevious : pszPathOut;

					*pTempOut = L'\0';
				}

				continue;
			}
			else
			{
				if (cchFileName == 0
					&& pszServer[0] == L'\\')
				{
					cchFileName = 1;
				}


				if (pTempOut + cchFileName >= pszPathOutEnd)
				{
					//空间不足，特殊判断一下，某些特定的形式是可以处理的。
					if (cchFileName == 1 && pszServer[0] == L'\\')
					{
						if (pszServer[1] == L'\0' || (pszServer[1] == L'.' && pszServer[2] == L'\0'))
						{
							//可以确认，路径已经结束，所以无需再添加 '\\'
							break;
						}
						else if (pTempOut + 1 == pszPathOutEnd
							&& pszServer[1] == L'.'
							&& pszServer[2] == L'.')
						{
							//下一个项目是返回上一个项目，所以可以伪造一下，直接设置为 L'\0' 然后往下移动
							++pszServer;
							*pTempOut++ = L'\0';
							continue;
						}
					}

					hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
					goto OnFailedLabel;
				}

				//复制文件名
				memcpy(pTempOut, pszServer, cchFileName * sizeof(pszServer[0]));
				pszServer += cchFileName;
				pTempOut += cchFileName;
				*pTempOut = L'\0';
			}
		}
			
		/* 删除 路径末尾的 .
		* "*." 则保留 
		*/
		if (pTempOut > pszPathOut)
		{
			PWSTR Str = pTempOut;
			--Str;

			if (*Str == L'.')
			{
				for (;;)
				{
					if (Str == pszPathOut)
					{
						*Str = L'\0';
						pTempOut = Str;
						break;
					}

					--Str;

					if (*Str != L'*')
					{
						pTempOut = Str + 1;
						*pTempOut = L'\0';
							
						if (*Str == L'.')
							continue;
					}

					//goto LABEL_29;
					break;
				}

					
			}
		}

		//LABEL_29:

		const int nCountOfData = _countof(L"::$DATA") - 1;

		if (pszPathOut + nCountOfData <= pTempOut
			&& pTempOut[-7] == L':'
			&& pTempOut[-6] == L':'
			&& pTempOut[-5] == L'$'
			&& (pTempOut[-4] == L'D' || pTempOut[-3] == L'd')
			&& (pTempOut[-3] == L'A' || pTempOut[-3] == L'a')
			&& (pTempOut[-2] == L'T' || pTempOut[-1] == L't')
			&& (pTempOut[-1] == L'A' || pTempOut[-1] == L'a'))
		{
			pTempOut -= nCountOfData;
			*pTempOut = L'\0';
		}


		//处于长命名模式中，如果规范化后的路径有效部分小于 MAX_PATH，那么删除长命名前缀
		if (ulReservedSize)
		{
			if ((size_t)(pTempOut - pszPathOut) <= ulReservedSize + MAX_PATH)
			{
				if (bUNC)
				{
					// "\\\\?\\UNC\\"
					memmove(pszPathOut + 2, pszPathOut + 8, (pTempOut - (pszPathOut + 8) + 1) * sizeof(pTempOut[0]));
				}
				else
				{
					memmove(pszPathOut, pszPathOut + 4, (pTempOut - (pszPathOut + 4) + 1) * sizeof(pTempOut[0]));
				}
			}
		}

			
		if (cchPathOut > 1 && *pszPathOut == L'\0')
		{
			//如果路径为 空，则转换为 "\\"
			pszPathOut[0] = L'\\';
			pszPathOut[1] = L'\0';
		}
		else if(cchPathOut > 3 && pszPathOut[1] == L':' && pszPathOut[2] == L'\0')
		{
			// 如果路径为 "X:"，则转换为 "X:\\"
			pszPathOut[2] = L'\\';
			pszPathOut[3] = L'\0';
		}

		return S_OK;

	} while (false);


	//LABEL_96 失败
OnFailedLabel:

	*pszPathOut = L'\0';

	if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
	{
		if (dwFlags & PATHCCH_ALLOW_LONG_PATHS)
		{
			if(cchPathOut == PATHCCH_MAX_CCH)
				hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
		}
		else
		{
			if (cchPathOut == MAX_PATH)
				hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
		}
	}

	return hr;
}

// from YY-Thunks
static bool Win32Thunk_Internal_PathIsRelativeWorker(LPCWSTR szPath)
{
	return szPath[0] != L'\\'
		&& (((L'A' <= szPath[0] && szPath[0] <= L'Z') || (L'a' <= szPath[0] && szPath[0] <= L'z')) && szPath[1] == L':') == false;
}

// modified from YY-Thunks
static HRESULT Win32Thunk_PathCchCombineEx(
	_Out_writes_(cchPathOut) PWSTR pszPathOut,
	_In_ size_t cchPathOut,
	_In_opt_ PCWSTR pszPathIn,
	_In_opt_ PCWSTR pszMore,
	_In_ ULONG dwFlags
) {
	typedef __typeof__(&PathCchCombineEx) type;
	static type real;
	static int realInit = 0;

	if (!realInit) {
		HMODULE module = GetModuleHandleA("api_ms_win_core_path_l1_1_0");
		if (module)
			real = (type)GetProcAddress(module, "PathCchCombineEx");
		realInit = 1;
	}
	if (real)
		return real(pszPathOut, cchPathOut, pszPathIn, pszMore, dwFlags);

	if (pszPathOut == NULL
		|| cchPathOut == 0
		|| cchPathOut > PATHCCH_MAX_CCH)
	{
		return E_INVALIDARG;
	}


	wchar_t PathOutStaticBuffer[MAX_PATH];
	PWSTR pszPathOutTmp = NULL;
	size_t cchPathOutTmp = 0;

	HRESULT hr = S_OK;

	do
	{
		if (pszPathIn == NULL && pszMore == NULL)
		{
			hr = E_INVALIDARG;
			break;
		}

		size_t cchPathIn = pszPathIn ? wcslen(pszPathIn) : 0;
		if (cchPathIn >= PATHCCH_MAX_CCH)
		{
			hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
			break;
		}

		size_t cchMore = pszMore ? wcslen(pszMore) : 0;
		if (cchMore >= PATHCCH_MAX_CCH)
		{
			hr = __HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
			break;
		}

		//极端情况可能需要额外添加 '\\' 连接，简化处理，直接 + 1。改逻辑与微软不同。
		//字符串末尾需要 '\0' 结束
		const size_t cchPathInNeed = cchPathIn + cchMore + 1 + 1;


		if (cchPathInNeed < _countof(PathOutStaticBuffer))
		{
			//静态缓冲区可以容纳，我们直接使用静态缓冲区
			pszPathOutTmp = PathOutStaticBuffer;
		}
		else
		{
			//好吧，我们真的需要开辟临时缓冲区
			pszPathOutTmp = (PWSTR)LocalAlloc(LMEM_ZEROINIT, cchPathInNeed * sizeof(wchar_t));
			if (!pszPathOutTmp)
			{
				hr = E_OUTOFMEMORY;
				break;
			}
		}

		if (cchPathIn == 0)
		{
			memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
			cchPathOutTmp = cchMore;
		}
		else if (cchMore == 0)
		{
			memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
			cchPathOutTmp = cchPathIn;
		}
		else
		{
			if (Win32Thunk_Internal_PathIsRelativeWorker(pszMore))
			{
				memcpy(pszPathOutTmp, pszPathIn, cchPathIn * sizeof(wchar_t));
				cchPathOutTmp = cchPathIn;
			}
			else
			{
				if (pszMore[0] == L'\\' && pszMore[1] != L'\\')
				{
					//路径是 '\\' 开头，那么 它与 pszPathIn 的根目录进行合并
					++pszMore;
					--cchMore;


					PCWSTR pszRootEnd;
					hr = Win32Thunk_PathCchSkipRoot(pszPathIn, &pszRootEnd);
					if (FAILED(hr))
					{
						break;
					}

					cchPathOutTmp = pszRootEnd - pszPathIn;
					memcpy(pszPathOutTmp, pszPathIn, cchPathOutTmp * sizeof(wchar_t));
				}
				else
				{
					//这是一个绝对的路径，我们只需要把 pszMore 复制到 pszPathOutTmp
					memcpy(pszPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
					cchPathOutTmp = cchMore;
					break;
				}
			}

			/*
			* 不想调用它……只是追加一个 '\' 而已
			PWSTR pszEnd;
			hr = PathCchAddBackslashEx(pszPathOutTmp, cchPathOutTmp, &pszEnd, &cchPathOutTmp);
			if (FAILED(hr))
			{
				break;
			}
			*/
			if (cchPathOutTmp && pszPathOutTmp[cchPathOutTmp - 1] != L'\\')
			{
				pszPathOutTmp[cchPathOutTmp] = L'\\';
				++cchPathOutTmp;
			}

			memcpy(pszPathOutTmp + cchPathOutTmp, pszMore, cchMore * sizeof(wchar_t));
			cchPathOutTmp += cchMore;
		}

	} while (false);

	if (FAILED(hr))
	{
		*pszPathOut = L'\0';
	}
	else
	{
		//保证字符串 '\0' 截断
		pszPathOutTmp[cchPathOutTmp] = L'\0';
		hr = Win32Thunk_PathCchCanonicalizeEx(pszPathOut, cchPathOut, pszPathOutTmp, dwFlags);
	}

	if (pszPathOutTmp && pszPathOutTmp != PathOutStaticBuffer)
		LocalFree(pszPathOutTmp);

	return hr;
}

#define GetSystemTimePreciseAsFileTime Win32Thunk_GetSystemTimePreciseAsFileTime
#define PathCchSkipRoot Win32Thunk_PathCchSkipRoot
#define PathCchCombineEx Win32Thunk_PathCchCombineEx
