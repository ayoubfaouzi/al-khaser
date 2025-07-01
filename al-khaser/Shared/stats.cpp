#include "pch.h"

const char* category_names[CAT_COUNT] = { // New categories to be added here. Order must be same as Category enum
	"TLS",
	"DEBUG",
	"INJECTION",
	"GEN_SANDBOX",
	"VBOX",
	"VMWARE",
	"VPC",
	"QEMU",
	"KVM",
	"XEN",
	"WINE",
	"PARALLELS",
	"HYPERV",
	"TIMING_ATTACKS",
	"ANALYSIS_TOOLS",
	//"CODE_INJECTIONS",
	//"ANTI_DISASSM",
	//"DUMPING_CHECK"
};


static CategoryStat category_stats[CAT_COUNT];
static int total_passed = 0, total_failed = 0;

void stats_init(void) {
	memset(category_stats, 0, sizeof(category_stats));
	total_passed = 0;
	total_failed = 0;
}

void stats_record(Category cat, int result) {
	if (cat < 0 || cat >= CAT_COUNT)
		return;
	if (result == TRUE) {
		category_stats[cat].failed++;
		total_failed++;
	} else {
		category_stats[cat].passed++;
		total_passed++;
	}
}

void stats_print(void) {
	printf("\n==== Per-Category Statistics ====\n");
	for (int i = 0; i < CAT_COUNT; ++i) {
		printf("%-16s: Passed %3d | Failed %3d\n",
			category_names[i], category_stats[i].passed, category_stats[i].failed);
	}
	printf("\n==== Overall Statistics ====\n");
	printf("Total checks passed: %d\n", total_passed);
	printf("Total checks failed: %d\n", total_failed);
}