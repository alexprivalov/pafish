#define __USE_MINGW_ANSI_STDIO 1
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#include <cpuid.h>
#endif

#include "types.h"
#include "cpu.h"

static inline unsigned long long rdtsc_diff() {
    const unsigned long long ret = __rdtsc();
    const unsigned long long ret2 = __rdtsc();
    return ret2 - ret;
}

static /*inline*/ unsigned long long rdtsc_diff_vmexit() {
    const unsigned long long ret = __rdtsc();
    /* vm exit forced here. it uses: eax = 0; cpuid; */
#if defined(_MSC_VER)
//    int cpuInfo[4] = {0};
//    __cpuid(cpuInfo, 0);
    __asm { cpuid }
#else
    int cpuInfo[4] = {0};
    __cpuid(0, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif

    return __rdtsc() - ret;
}

static inline void cpuid_vendor_00(char * vendor) {
	int ebx = 0, ecx = 0, edx = 0;
#if defined(_MSC_VER)
//    An array of four integers that contains the information returned in EAX, EBX, ECX,
//    and EDX about supported features of the CPU.
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0);
    ebx = cpuInfo[1];
    ecx = cpuInfo[2];
    edx = cpuInfo[3];
#else
    __asm__ volatile("cpuid" \
            : "=b"(ebx), \
            "=c"(ecx), \
            "=d"(edx) \
            : "a"(0x00));
#endif
    sprintf(vendor  , "%c%c%c%c", ebx, (ebx >> 8), (ebx >> 16), (ebx >> 24));
    sprintf(vendor+4, "%c%c%c%c", edx, (edx >> 8), (edx >> 16), (edx >> 24));
    sprintf(vendor+8, "%c%c%c%c", ecx, (ecx >> 8), (ecx >> 16), (ecx >> 24));
	vendor[12] = 0x00;
}

static inline void cpuid_hv_vendor_00(char * vendor) {
	int ebx = 0, ecx = 0, edx = 0;

#if defined(_MSC_VER)
//    An array of four integers that contains the information returned in EAX, EBX, ECX,
//    and EDX about supported features of the CPU.
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0x40000000);
    ebx = cpuInfo[1];
    ecx = cpuInfo[2];
    edx = cpuInfo[3];
#else
    __asm__ volatile("cpuid" \
            : "=b"(ebx), \
            "=c"(ecx), \
            "=d"(edx) \
            : "a"(0x40000000));
#endif
	sprintf(vendor  , "%c%c%c%c", ebx, (ebx >> 8), (ebx >> 16), (ebx >> 24));
	sprintf(vendor+4, "%c%c%c%c", ecx, (ecx >> 8), (ecx >> 16), (ecx >> 24));
	sprintf(vendor+8, "%c%c%c%c", edx, (edx >> 8), (edx >> 16), (edx >> 24));
	vendor[12] = 0x00;
}

static inline void cpuid_brand(char * brand, uint32_t eax_value) {
	int eax = 0, ebx = 0, ecx = 0, edx = 0;
#if defined(_MSC_VER)
//    An array of four integers that contains the information returned in EAX, EBX, ECX,
//    and EDX about supported features of the CPU.
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, eax_value);
    eax = cpuInfo[0];
    ebx = cpuInfo[1];
    ecx = cpuInfo[2];
    edx = cpuInfo[3];
#else
    __asm__ volatile("cpuid" \
            : "=a"(eax), \
            "=b"(ebx), \
            "=c"(ecx), \
            "=d"(edx) \
            : "a"(eax_value));
#endif
	sprintf(brand   , "%c%c%c%c", eax, (eax >> 8), (eax >> 16), (eax >> 24));
	sprintf(brand+4 , "%c%c%c%c", ebx, (ebx >> 8), (ebx >> 16), (ebx >> 24));
	sprintf(brand+8 , "%c%c%c%c", ecx, (ecx >> 8), (ecx >> 16), (ecx >> 24));
	sprintf(brand+12, "%c%c%c%c", edx, (edx >> 8), (edx >> 16), (edx >> 24));
}

static inline int cpuid_hv_bit() {
    int cpuInfo[4] = {0};
#if defined(_MSC_VER)
    //    An array of four integers that contains the information returned in EAX, EBX, ECX,
    //    and EDX about supported features of the CPU.
        __cpuid(cpuInfo, 0x01);
#else
    __cpuid(0x01, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif
    int ecx = cpuInfo[2];
    return (ecx >> 31) & 0x1;
}

int cpu_rdtsc() {
	int i;
	unsigned long long avg = 0;
	for (i = 0; i < 10; i++) {
		avg = avg + rdtsc_diff();
		Sleep(500);
	}
	avg = avg / 10;
	return (avg < 750 && avg > 0) ? FALSE : TRUE;
}

int cpu_rdtsc_force_vmexit() {
	unsigned long long avg = 0;
    for (int i = 0; i < 10; i++) {
        avg = avg + rdtsc_diff_vmexit();
		Sleep(500);
	}
	avg = avg / 10;
	return (avg < 1000 && avg > 0) ? FALSE : TRUE;
}

int cpu_hv() {
	return cpuid_hv_bit() ? TRUE : FALSE;
}

void cpu_write_vendor(char * vendor) {
	cpuid_vendor_00(vendor);
}

void cpu_write_hv_vendor(char * vendor) {
	cpuid_hv_vendor_00(vendor);
}

void cpu_write_brand(char * brand) {
	int eax;
#if defined(_MSC_VER)
    // Calling __cpuid with 0x80000000 as the function_id argument
    // gets the number of the highest valid extended ID.
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 0x80000000);
    // Interpret CPU brand string if reported
    eax = cpuInfo[0] >= 0x80000004 ? 1 : 0;
#else
    /* Check if Processor Brand String is supported */
    __asm__ volatile(".intel_syntax noprefix;"
            "mov eax, 0x80000000;"
            "cpuid;"
            "cmp eax, 0x80000004;"
            "xor eax, eax;"
            "setge al;"
            ".att_syntax;"
            : "=a"(eax)
            );
#endif
    /* It's supported, so fill char * brand */
	if (eax) {
		cpuid_brand(brand,    0x80000002);
		cpuid_brand(brand+16, 0x80000003);
		cpuid_brand(brand+32, 0x80000004);
		brand[48] = 0x00;
	}
}

#define COUNT 6
int cpu_known_vm_vendors() {
	int i;
	char cpu_hv_vendor[13];
    string strs[COUNT];
	strs[0] = "KVMKVMKVM\0\0\0"; /* KVM */
	strs[1] = "Microsoft Hv"; /* Microsoft Hyper-V or Windows Virtual PC */
	strs[2] = "VMwareVMware"; /* VMware */
	strs[3] = "XenVMMXenVMM"; /* Xen */
	strs[4] = "prl hyperv  "; /* Parallels */
	strs[5] = "VBoxVBoxVBox"; /* VirtualBox */
	cpu_write_hv_vendor(cpu_hv_vendor);
    for (i = 0; i < COUNT; i++) {
		if (!memcmp(cpu_hv_vendor, strs[i], 12)) return TRUE;
	}
	return FALSE;
}

