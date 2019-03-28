#ifndef __PCI_H
#define __PCI_H

#include "os.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define MAX_PCI_DEVICES 256

// bbbbbbbbdddddfffccccccccssssssss
typedef u32 pci_device_t;

u32 pci_read_config_word(u32 bus,u32 device,u32 function,u32 hword);
u32 pci_read_config_word_dev(pci_device_t dev,u32 hword);

void pci_dump();
void pci_detect_hardware();

//Returns device index
//-1 for NOTFOUND
pci_device_t pci_find_device(u32 class,u32 subclass);

#endif
