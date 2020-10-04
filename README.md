# yolt
Yolt, a exploratory microkernel implementation 

Yolt prototype was built during covid19 great lockdown (May-Jun 2020). The intention was to become up to date with x86-64 architecture on unikernel and microkernel developments. Initial focus has been on non-uniform and symmetric multiprocessing and paging organisation.

## Implementation

x86_64 - bootloader, x2apic, several MSRs, BIOS e820 smap, L4 paging, paging bitmaps, acpi, cpuid's

bootloader - ignored multiboot spec for now.

bump allocator next.

## Run time

KVM

## Building from source

*KVM needs to be installed.*

```
$ make image && make run
```
