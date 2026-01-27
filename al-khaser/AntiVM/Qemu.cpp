#include "pch.h"

#include "Qemu.h"

/*
Registry key values
*/

VOID qemu_reg_key_value()
{
    /* Array of strings of blacklisted registry key values */
    const TCHAR *szEntries[][3] = {
        {_T("HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0"), _T("Identifier"),
         _T("QEMU")},
        {_T("HARDWARE\\Description\\System"), _T("SystemBiosVersion"), _T("QEMU")},
    };

    WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);

    for (int i = 0; i < dwLength; i++)
    {
        TCHAR msg[256] = _T("");
        _stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking reg key %s "), szEntries[i][0]);
        if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, szEntries[i][0], szEntries[i][1], szEntries[i][2]))
            print_results(TRUE, msg);
        else
            print_results(FALSE, msg);
    }
}

/*
Check against QEMU registry keys
*/
VOID qemu_reg_keys()
{
    /* Array of strings of blacklisted registry keys */
    const TCHAR *szKeys[] = {
        _T("SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_1B36*"),
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

VOID qemu_processes()
{
    const TCHAR *szProcesses[] = {
        _T("qemu-ga.exe"),   // QEMU guest agent.
        _T("vdagent.exe"),   // SPICE guest tools.
        _T("vdservice.exe"), // SPICE guest tools.
    };

    WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
    for (int i = 0; i < iLength; i++)
    {
        TCHAR msg[256] = _T("");
        _stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking qemu processes %s "), szProcesses[i]);
        if (GetProcessIdFromName(szProcesses[i]))
            print_results(TRUE, msg);
        else
            print_results(FALSE, msg);
    }
}

/*
Check against blacklisted directories.
*/
VOID qemu_dir()
{
    TCHAR szProgramFile[MAX_PATH];
    TCHAR szPath[MAX_PATH] = _T("");

    const TCHAR *szDirectories[] = {
        _T("qemu-ga"),           // QEMU guest agent.
        _T("SPICE Guest Tools"), // SPICE guest tools.
    };

    WORD iLength = sizeof(szDirectories) / sizeof(szDirectories[0]);
    for (int i = 0; i < iLength; i++)
    {
        TCHAR msg[256] = _T("");

        if (IsWoW64())
            ExpandEnvironmentStrings(_T("%ProgramW6432%"), szProgramFile, ARRAYSIZE(szProgramFile));
        else
            SHGetSpecialFolderPath(NULL, szProgramFile, CSIDL_PROGRAM_FILES, FALSE);

        PathCombine(szPath, szProgramFile, szDirectories[i]);

        _stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking QEMU directory %s "), szPath);
        if (is_DirectoryExists(szPath))
            print_results(TRUE, msg);
        else
            print_results(FALSE, msg);
    }
}

/*
Check for SMBIOS firmware
*/
BOOL qemu_firmware_SMBIOS()
{
    BOOL result = FALSE;

    DWORD smbiosSize = 0;
    PBYTE smbios = get_system_firmware(static_cast<DWORD>('RSMB'), 0x0000, &smbiosSize);
    if (smbios != NULL)
    {
        PBYTE qemuString1 = (PBYTE) "qemu";
        size_t StringLen = 4;
        PBYTE qemuString2 = (PBYTE) "QEMU";

        if (find_str_in_data(qemuString1, StringLen, smbios, smbiosSize) ||
            find_str_in_data(qemuString2, StringLen, smbios, smbiosSize))
        {
            result = TRUE;
        }

        free(smbios);
    }

    return result;
}

/*
Check for ACPI firmware
*/
BOOL qemu_firmware_ACPI()
{
    BOOL result = FALSE;

    PDWORD tableNames = static_cast<PDWORD>(malloc(4096));

    if (tableNames)
    {
        SecureZeroMemory(tableNames, 4096);
        DWORD tableSize = enum_system_firmware_tables(static_cast<DWORD>('ACPI'), tableNames, 4096);

        // API not available
        if (tableSize == -1)
            return FALSE;

        DWORD tableCount = tableSize / 4;
        if (tableSize < 4 || tableCount == 0)
        {
            result = TRUE;
        }
        else
        {
            const char *strings[] = {
                "FWCF",     // fw_cfg name
                "QEMU0002", // fw_cfg HID/CID
                "BOCHS",    // OEM ID
                "BXPC"      // OEM Table ID
            };

            for (DWORD i = 0; i < tableCount; i++)
            {
                DWORD tableSize = 0;
                PBYTE table = get_system_firmware(static_cast<DWORD>('ACPI'), tableNames[i], &tableSize);

                if (table)
                {

                    for (DWORD j = 0; j < sizeof(strings) / sizeof(char *); j++)
                    {
                        if (find_str_in_data((PBYTE) strings[j], strlen(strings[j]), table, tableSize))
                        {
                            free(table);
                            result = TRUE;
                            goto out;
                        }
                    }

                    free(table);
                }
            }

            DWORD tableSize = 0;
            PBYTE table = get_system_firmware(static_cast<DWORD>('ACPI'), static_cast<DWORD>('PCAF'), &tableSize);

            if (table)
            {
                if (tableSize < 45)
                {
                    return FALSE; // Corrupted table
                }

                // Format: [HexOffset DecimalOffset ByteLength]  FieldName  : FieldValue (in hex)
                //		   [02Dh      0045          001h      ]	 PM Profile : 00 [Unspecified] - hardcoded in QEMU src
                if ((BYTE) table[45] == (BYTE) 0)
                {
                    result = TRUE;
                }

                free(table);
            }
        }
    out:
        free(tableNames);
    }
    return result;
}
