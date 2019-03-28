#include "pci.h"
#include "vga_terminal.h"
#include "stdlib.h"
#include "io.h"

typedef struct {
	u32 device_count;
	pci_device_t devices[MAX_PCI_DEVICES];
} pci_hardware_t;

static pci_hardware_t s_pci;

struct pci_class_s {
	u32 len;
	const char* name;
};

static struct pci_class_s s_classes[] = {
	{2,"Unclassified"},
	{9,"Mass Storage Controller"},
	{9,"Network Controller"},
	{3,"Display Controller"},
	{4,"Multimedia Controller"},
	{2,"Memory controller"},
	{11,"Bridge Device"},
	{6,"Simple Communication Controller"},
	{7,"Base System Peripheral"},
	{5,"Input Device Controller"},
	{0,"Docking Station"},
	{0,"Processor"},
	{10,"Serial Bus Controller"},
	{0,"Wireless Controller"},
	{0,"Intelligent Controller"},
	{0,"Satellite Communication Controller"},
	{0,"Encryption Controller"},
	{0,"Signal Processing Controller"},
	{0,"Processing Accelerator"},
	{0,"Non-Essential Instrumentation"}
};

static char* s_subclasses[] = {
	"Non-VGA-Compatible devices",
	"VGA-Compatible Device",
	
	"SCSI Bus Controller",
	"IDE Controller",
	"Floppy Disk Controller",
	"IPI Bus Controller",
	"RAID Controller",
	"ATA Controller",
	"Serial ATA",
	"Serial Attached SCSI",
	"Non-Volatile Memory Controller",
	
	"Ethernet Controller",
	"Token Ring Controller",
	"FDDI Controller",
	"ATM Controller",
	"ISDN Controller",
	"WorldFip Controller",
	"PICMG 2.14 Multi Computing",
	"Infiniband Controller",
	"Fabric Controller",
	
	"VGA Compatible Controller",
	"XGA Controller",
	"3D Controller (Not VGA-Compatible)",
	
	"Multimedia Video Controller",
	"Multimedia Audio Controller",
	"Computer Telephony Device",
	"Audio Device",
	
	"RAM Controller",
	"Flash Controller",
	
	"Host Bridge",
	"ISA Bridge",
	"EISA Bridge",
	"MCA Bridge",
	"PCI-to-PCI Bridge",
	"PCMCIA Bridge",
	"NuBus Bridge",
	"CardBus Bridge",
	"RACEway Bridge",
	"PCI-to-PCI Bridge",
	"InfiniBand-to-PCI Host Bridge",
	
	"Serial Controller",
	"Parallel Controller",
	"Multiport Serial Controller",
	"Modem",
	"IEEE 488.1/2 (GPIB) Controller",
	"Smart Card",
	
	"PIC",
	"DMA Controller",
	"Timer",
	"RTC Controller",
	"PCI Hot-Plug Controller",
	"SD Host controller",
	"IOMMU",
	
	"Keyboard Controller",
	"Digitizer Pen",
	"Mouse Controller",
	"Scanner Controller",
	"Gameport Controller",
	
	"FireWire (IEEE 1394) Controller",
	"ACCESS Bus",
	"SSA",
	"USB Controller",
	"Fibre Channel",
	"SMBus",
	"InfiniBand",
	"IPMI Interface",
	"SERCOS Interface (IEC 61491)",
	"CANbus"
	
};

u32 pci_read_config_word(u32 bus,u32 device,u32 function,u32 offset)
{
	u32 address;
	
	bus 		&= 0xFF;	// 8 bit
	device 		&= 0x1F;	// 5 bit
	function 	&= 0x07;	// 3 bit
	offset 		&= 0xFC;	// 6 bit
	
	address = 0x80000000UL|(bus<<16)|(device<<11)|(function<<8)|(offset);
	
	outl(PCI_CONFIG_ADDRESS,address);
	return inl(PCI_CONFIG_DATA);
}

u32 pci_read_config_word_dev(pci_device_t dev,u32 offset)
{
	u32 address;
	
	offset 		&= 0xFC;	// 6 bit
	
	address = 0x80000000|((dev&0xFFFF0000)>>8)|(offset);
	
	outl(PCI_CONFIG_ADDRESS,address);
	return inl(PCI_CONFIG_DATA);
}

const char* pci_get_class(u32 class)
{
	if(class >= sizeof(s_classes)/sizeof(struct pci_class_s))
		return "Unknown";
	return s_classes[class].name;
}

const char* pci_get_subclass(u32 class,u32 subclass)
{
	u32 i,index;
	
	if(class >= sizeof(s_classes)/sizeof(struct pci_class_s))
		return "Unknown";
	if(subclass >= sizeof(s_subclasses)/sizeof(const char*))
		return "Unknown";
	else if(subclass == 0x80) return "Other";
	
	index = 0;
	for(i = 0; i < class; i++)
		index += s_classes[i].len;
	if(i == class && subclass >= s_classes[i].len) return "Unknown";
	
	return s_subclasses[index+subclass];
}

void pci_dump()
{
	for(u32 i = 0; i < s_pci.device_count; i++)
	{
		u32 class,subclass;
		
		class = (s_pci.devices[i]>>8)&0xFF;
		subclass = s_pci.devices[i] & 0xFF;
		kprintf("%x\t%s\t%s\n",
			s_pci.devices[i],
			pci_get_class(class),
			pci_get_subclass(class,subclass));
	}
}

static void pci_add_device(u32 bus,u32 device,u32 func,
	u32 class,u32 subclass)
{
	pci_device_t dev;
	
	dev = (bus<<24)|(device<<19)|(func<<16)|(class<<8)|(subclass);
	if(s_pci.device_count < MAX_PCI_DEVICES)
		s_pci.devices[s_pci.device_count++] = dev;
}

static void pci_scan_bus(u32 bus);

static void pci_scan_device(u32 bus,u32 dev,u32 func)
{
	u32 class = pci_read_config_word(bus,dev,func,0x08) >> 16;
	u32 hdr = (pci_read_config_word(bus,dev,func,0x0C) >> 16) & 0xFF;
	
	if(class == 0x0604) //PCI-to-PCI bridge
	{
		pci_scan_bus((pci_read_config_word(
			bus,dev,func,0x18) >> 8) & 0xFF);
	}
	if(hdr & 0x80 && func == 0) //Multifunction
	{
		for(u32 i = 1; i < 8; i++)
		{
			u32 vendor = pci_read_config_word(bus,dev,i,0x00) & 0xFFFF;
			if(vendor != 0xFFFF)
				pci_scan_device(bus,dev,i);
		}
	}
	
	pci_add_device(bus,dev,func,class>>8,class&0xFF);
}

static void pci_scan_bus(u32 bus)
{
	for(u32 i = 0; i < 32; i++)
	{
		u32 vendor = pci_read_config_word(bus,i,0,0x00) & 0xFFFF;
		if(vendor != 0xFFFF)
			pci_scan_device(bus,i,0);
	}
}

void pci_detect_hardware()
{
	s_pci.device_count = 0;
	for(u32 i = 0; i < 256; i++)
		pci_scan_bus(i);
}

pci_device_t pci_find_device(u32 class,u32 subclass)
{
	u32 data = class<<8|subclass;
	for(u32 i = 0; i < s_pci.device_count; i++)
	{
		if((s_pci.devices[i] & 0xFFFF) == data)
			return s_pci.devices[i];
	}
	return 0;
}
