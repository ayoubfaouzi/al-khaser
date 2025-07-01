#pragma once

// Category enum and names
typedef enum { // New categories to be added here. Order is important.
	CAT_TLS,
	CAT_DEBUG,
	CAT_INJECTION,
	CAT_GEN_SANDBOX,
	CAT_VBOX,
	CAT_VMWARE,
	CAT_VPC,
	CAT_QEMU,
	CAT_KVM,
	CAT_XEN,
	CAT_WINE,
	CAT_PARALLELS,
	CAT_HYPERV,
	CAT_TIMING_ATTACKS,
	CAT_ANALYSIS_TOOLS,
	//CAT_CODE_INJECTIONS, // Disabled by default, not part of EnableDefaultChecks()
	//CAT_ANTI_DISASSM,
	//CAT_DUMPING_CHECK,
	CAT_COUNT // Always last -> number of categories
} Category; 

extern const char* category_names[CAT_COUNT];

typedef struct {
	int passed;
	int failed;
} CategoryStat;

void stats_init(void);
void stats_record(Category cat, int result); // result: 1 = pass, 0 = fail
void stats_print(void);
