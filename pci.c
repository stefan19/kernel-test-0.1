#include "Include/types.h"
#include "Include/system.h"

u8int deviceFound = 0;
void checkDevice(u8int bus, u8int device);
u8int targetBus;
u8int targetDevice;
u8int targetFunc;
extern u32int inl(u16int port);
extern void outl(u16int port,u32int value);
u8int targetClass;
u8int targetSubClass;

u16int pciConfigReadWord (u8int bus, u8int slot, u8int func, u8int offset){
	u32int address;
	u32int lbus = (u32int)bus;
	u32int lslot = (u32int)slot;
	u32int lfunc = (u32int)func;
	u16int tmp = 0;

	address = (u32int)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((u32int)0x80000000 ));
	outl(0xCF8, address);

	tmp = (u16int)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return(tmp);
}

u16int getVendorID(u8int bus, u8int slot,u8int func){
	u16int vendor;

	vendor = pciConfigReadWord(bus,slot,func,0);
	return (vendor);
}

u8int pciGetDeviceClass(u8int bus,u8int slot,u8int func){
	u8int deviceClass;

	deviceClass = (pciConfigReadWord(bus,slot,func,0x0A) >> 8) & 0xFF;
	return (deviceClass);
}

u8int pciGetSubClass(u8int bus,u8int slot,u8int func){
	u8int subClass;

	subClass = pciConfigReadWord(bus,slot,func,0x0A) & 0xFF;
	return (subClass);
}

u8int pciGetProgIF(u8int bus,u8int slot,u8int func){
	u8int progIF;
	
	progIF = ((pciConfigReadWord(bus,slot,func,0x08)) >> 8) & 0xFF;
	return (progIF);
}

u8int pciGetHeaderType(u8int bus,u8int slot,u8int func){
	u8int headerType;
	
	headerType = pciConfigReadWord(bus,slot,func,0x0E) & 0xFF;
	return (headerType);
}

u8int getSecondaryBus(u8int bus,u8int device,u8int func){
	u8int secondaryBus;

	secondaryBus = (pciConfigReadWord(bus,device,func,0x18) >> 8) & 0xFF;
	return (secondaryBus);
}

u32int getBAR4(u8int bus,u8int device,u8int func){
	u32int BAR4;

	BAR4 = pciConfigReadWord(bus,device,func,0x22) * 0x10000 + pciConfigReadWord(bus,device,func,0x20);
	return (BAR4);
}

u8int getInterruptLine(u8int bus,u8int device,u8int func){
	u8int InterruptLine;
	
	InterruptLine = pciConfigReadWord(bus,device,func,0x3C) & 0xFF;
	return (InterruptLine);
}

void checkBus(u8int bus){
	u8int device;

	for(device = 0;device<32;device++){
		checkDevice(bus,device);
	}
}

void checkFunction(u8int bus,u8int device,u8int function){
	u8int baseClass;
	u8int subClass;
	u8int secondaryBus;

	baseClass = pciGetDeviceClass(bus,device,function);
	subClass = pciGetSubClass(bus,device,function);
	if( (baseClass == 0x06) && (subClass == 0x04) ){
		secondaryBus = getSecondaryBus(bus,device,function);
		checkBus(secondaryBus);
	}
}

void checkDevice(u8int bus, u8int device){
	u8int function = 0;

	u16int vendorID = getVendorID(bus,device,function);
	if(vendorID == 0xFFFF) return;
	checkFunction(bus,device,function);	

	u8int deviceClass = pciGetDeviceClass(bus,device,function);
	u8int subClass = pciGetSubClass(bus,device,function);
	u8int progIF = pciGetProgIF(bus,device,function);

	if( (deviceClass == targetClass) && (subClass == targetSubClass) ){
		deviceFound = 1;
		targetBus = bus;
		targetDevice = device;
		targetFunc = function;
	}

	u8int headerType = pciGetHeaderType(bus,device,function);
	if( (headerType & 0x80) != 0 ){
		for(function = 1;function < 8;function++){
			checkFunction(bus,device,function);
			deviceClass = pciGetDeviceClass(bus,device,function);
			subClass = pciGetSubClass(bus,device,function);
			progIF = pciGetProgIF(bus,device,function);
			
			if( (deviceClass == targetClass) && (subClass == targetSubClass) ){
				deviceFound = 1;
				targetBus = bus;
				targetDevice = device;
				targetFunc = function;
			}
		}
	}
}

u32int checkAllBuses(u8int deviceClass,u8int subClass) {
	u8int function;
	u8int bus;

	targetClass = deviceClass;
	targetSubClass = subClass;
	u8int headerType = pciGetHeaderType(0,0,0);
	if( (headerType & 0x80) == 0){
		checkBus(0);
	} else {
		for(function = 0;function < 8;function++){
			if(getVendorID(0,0,function) == 0xFFFF) break;
			bus = function;
			checkBus(bus);
		}
	}
	u32int result;
	if(deviceFound = 1){
	result = (targetBus << 16) | (targetDevice << 8) | (targetFunc & 0xFF);
	}
	else if (deviceFound = 0){
	result = 0xFFFFFF;
	}
	deviceFound = 0;
	return result;
 }
