#include "pch.h"

#include "Parallels.h"

/*
Check against Parallels registry keys
*/
VOID parallels_reg_keys()
{
    /* Array of strings of blacklisted registry keys */
    const TCHAR *szKeys[] = {
        _T("SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_1AB8*"),
    };

    WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

    /* Check one by one */
    for (int i = 0; i < dwlength; i++)
    {
        TCHAR msg[256] = _T("");
        _stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking reg key %s "), szKeys[i]);

        if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, szKeys[i]))
            print_results(TRUE, msg);
        else
            print_results(FALSE, msg);
    }
}

/*
Check for process list
*/

VOID parallels_process()
{
    const TCHAR *szProcesses[] = {
        _T("prl_cc.exe"),
        _T("prl_tools.exe"),
    };

    WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
    for (int i = 0; i < iLength; i++)
    {
        TCHAR msg[256] = _T("");
        _stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking Parallels processes: %s"), szProcesses[i]);
        if (GetProcessIdFromName(szProcesses[i]))
            print_results(TRUE, msg);
        else
            print_results(FALSE, msg);
    }
}

/*
Check Parallels NIC MAC address
*/
BOOL parallels_check_mac()
{
    // Parallels, Inc.
    return check_mac_addr(_T("\x00\x1C\x42"));
}
