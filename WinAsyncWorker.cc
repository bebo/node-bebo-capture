#include "WinAsyncWorker.h"

typedef unsigned __int64 QWORD;

HRESULT RegOpen(REGSAM samDesired, HKEY *hkey)
{
	LSTATUS lstatus = 0;
	lstatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Bebo\\GameCapture", 0, samDesired, hkey);

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

HRESULT RegClose(HKEY hkey)
{
	LSTATUS lstatus = RegCloseKey(hkey);
	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}
	return NOERROR;
}

HRESULT RegGetBeboSZ(HKEY hkey, char *szValueName, char *data, LPDWORD datasize)
{

	LSTATUS lstatus = 0;
	WCHAR valueName[1024] = {0};
	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	if (data == NULL)
	{
		return E_INVALIDARG;
	}

	DWORD dwType = REG_SZ;

	// Check input parameters...

	WCHAR wdata[1024] = {0};
	// Get dword value from the registry...
	DWORD wSize = 1024;
	lstatus = RegQueryValueExW(hkey, valueName, 0, &dwType, (LPBYTE)wdata, &wSize);

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}
	else if (dwType != REG_SZ)
	{
		return DISP_E_TYPEMISMATCH;
	}

	*datasize = WideCharToMultiByte(CP_UTF8, 0, wdata, wSize, data, *datasize, NULL, NULL);

	return NOERROR;
}

HRESULT getSZ(HKEY hkey, char *key, std::string &value)
{
	DWORD size = 1024;
	char val[1024] = {0};
	HRESULT status = RegGetBeboSZ(hkey, key, val, &size);
	if (status == NOERROR && strlen(val) < 1024)
	{
		value.assign(val);
	}
	return status;
}

HRESULT RegGetDWord(HKEY hKey, char *szValueName, DWORD *lpdwResult)
{

	// Given a value name and an hKey returns a DWORD from the registry.
	// eg. RegGetDWord(hKey, TEXT("my dword"), &dwMyValue);

	LONG lResult;
	DWORD dwDataSize = sizeof(DWORD);
	DWORD dwType = REG_DWORD;
	WCHAR valueName[1024] = {0};

	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	// Check input parameters...
	if (hKey == NULL || lpdwResult == NULL)
		return E_INVALIDARG;

	// Get dword value from the registry...
	lResult = RegQueryValueExW(hKey, valueName, 0, &dwType, (LPBYTE)lpdwResult, &dwDataSize);

	// Check result and make sure the registry value is a DWORD(REG_DWORD)...
	if (lResult != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lResult);
	}
	else if (dwType != REG_DWORD)
	{
		return DISP_E_TYPEMISMATCH;
	}

	return NOERROR;
}

HRESULT getBool(HKEY hkey, char *key, bool *val)
{
	DWORD value = 0;
	HRESULT status = RegGetDWord(hkey, key, &value);
	if (status == NOERROR)
	{
		*val = value == 1;
	}
	return status;
}

HRESULT RegGetQWord(HKEY hKey, char *szValueName, QWORD *lpdwResult)
{

	LONG lResult;
	DWORD dwDataSize = sizeof(QWORD);
	DWORD dwType = REG_QWORD;
	WCHAR valueName[1024] = {0};

	if (!MultiByteToWideChar(CP_UTF8, 0, szValueName, strlen(szValueName), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	// Check input parameters...
	if (hKey == NULL || lpdwResult == NULL)
		return E_INVALIDARG;

	// Get dword value from the registry...
	lResult = RegQueryValueExW(hKey, valueName, 0, &dwType, (LPBYTE)lpdwResult, &dwDataSize);

	// Check result and make sure the registry value is a QWORD(REG_QWORD)...
	if (lResult != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lResult);
	}
	else if (dwType != REG_QWORD)
	{
		return DISP_E_TYPEMISMATCH;
	}

	return NOERROR;
}

HRESULT getQWord(HKEY hkey, char *key, uint64_t *val)
{
	QWORD value = 0;
	HRESULT status = RegGetQWord(hkey, key, &value);
	if (status == NOERROR)
	{
		*val = value;
	}
	return status;
}

HRESULT getDWord(HKEY hkey, char *key, uint32_t *val)
{
	DWORD value = 0;
	HRESULT status = RegGetDWord(hkey, key, &value);
	if (status == NOERROR)
	{
		*val = value;
	}
	return status;
}

HRESULT putSZ(HKEY hkey, char *key, std::string &value)
{

	WCHAR valueName[1024] = {0};
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	WCHAR wdata[2048] = {0};
	DWORD wSize = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), strlen(value.c_str()), wdata, 2048);
	if ((wSize == 0 && strlen(value.c_str()) != 0) || wSize == 0x0000FFFD0)
	{
		return E_INVALIDARG;
	}
	wSize = wSize * 2 + 2; // write two \0

	DWORD dwType = REG_SZ;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE)wdata, wSize);

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

HRESULT putQWord(HKEY hkey, char *key, uint64_t val)
{

	WCHAR valueName[1024] = {0};
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	QWORD value = val;
	DWORD dwType = REG_QWORD;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE)&value, sizeof(value));

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

HRESULT putDWord(HKEY hkey, char *key, uint32_t val)
{

	WCHAR valueName[1024] = {0};
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	DWORD value = val;
	DWORD dwType = REG_DWORD;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE)&value, sizeof(value));

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

HRESULT putBool(HKEY hkey, char *key, bool val)
{

	WCHAR valueName[1024] = {0};
	LSTATUS lstatus = 0;
	if (!MultiByteToWideChar(CP_UTF8, 0, key, strlen(key), valueName, 1024))
	{
		return E_INVALIDARG;
	}

	DWORD value = val;

	DWORD dwType = REG_DWORD;

	lstatus = RegSetValueExW(hkey, valueName, 0, dwType, (LPBYTE)&value, sizeof(value));

	if (lstatus != ERROR_SUCCESS)
	{
		return HRESULT_FROM_WIN32(lstatus);
	}

	return NOERROR;
}

const char *errno_to_text(HRESULT errorNumber)
{
	const wchar_t *errorMessage;
	DWORD nLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
							   NULL,
							   errorNumber,
							   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							   (LPTSTR)&errorMessage,
							   0,
							   NULL);
        if (nLen == 0) return "Unknown Error. ";

        char* buffer = NULL;
        size_t size;
        wcstombs_s(&size, buffer, 512, errorMessage, 512);

	return buffer;
}

bool WinAsyncWorker::chk(HRESULT hresult, std::string &msg)
{
	std::string mymsg(msg);
	if (hresult != NOERROR)
	{
          char buffer[512];
          sprintf_s(buffer, 512, "%s(0x%08x)", errno_to_text(hresult), hresult);
          mymsg.append(buffer);

          SetErrorMessage(mymsg.c_str());
          return false;
        }
	return true;
}

bool WinAsyncWorker::chk(HRESULT hresult, const char *msg)
{
	return chk(hresult, std::string(msg));
}
