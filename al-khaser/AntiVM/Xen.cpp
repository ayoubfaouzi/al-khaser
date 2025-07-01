#include "pch.h"

#include "Xen.h"

/*
Check against Xen registry keys
*/
VOID xen_reg_keys()
{
	/* Array of strings of blacklisted registry keys */
	const TCHAR* szKeys[] = {
		_T("SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_5853*"),
	};

	WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

	/* Check one by one */
	for (int i = 0; i < dwlength; i++)
	{
		TCHAR msg[256] = _T("");
		_stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking reg key %s "), szKeys[i]);
		if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, szKeys[i])) {
			print_results(TRUE, msg);
			stats_record(CAT_XEN, TRUE);
		}
		else {
			print_results(FALSE, msg);
			stats_record(CAT_XEN, FALSE);
		}
	}
}

/*
Check for process list
*/

VOID xen_process()
{
	const TCHAR *szProcesses[] = {
		_T("xenservice.exe"),
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		TCHAR msg[256] = _T("");
		_stprintf_s(msg, sizeof(msg) / sizeof(TCHAR), _T("Checking Citrix Xen process %s"), szProcesses[i]);
		if (GetProcessIdFromName(szProcesses[i])) {
			print_results(TRUE, msg);
			stats_record(CAT_XEN, TRUE);
		}
		else {
			print_results(FALSE, msg);
			stats_record(CAT_XEN, FALSE);
		}
	}
}



/*
Check Xen NIC MAC address
*/
BOOL xen_check_mac()
{
	// Xensource, Inc. 
	return check_mac_addr(_T("\x00\x16\x3E"));
}
