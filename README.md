# yolt
Yolt, a exploratory unikernel implementation 

Yolt prototype was built during covid19 lockdown (jun-jul 2020). The intention was to become up to date with x86-64 architecture on unikernel and microkernel developments. Initial focus has been on non-uniform and symetric multiprocessing and paging organisation.

## Implementation

x86_64 - x2apic, several MSRs, BIOS e820 smap, L4 paging, paging bitmaps, acpi, cpuid's

## Run time

KVM

## Build

*KVM needs to be installed.*

```
$ make image && make run
```
