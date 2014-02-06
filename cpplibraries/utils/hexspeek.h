#if !defined(__HEXSPEEK_H__350A3C08_505E_444F_8BDB_7B4E5D785363__INCLUDED) 
#define __HEXSPEEK_H__350A3C08_505E_444F_8BDB_7B4E5D785363__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

static const unsigned long AteBadFood =		0x8BADF00D;	// is used by Apple in 
	// iOS crash reports, when an application takes too long to launch, terminate
	// , or respond to system events.

static const unsigned long ABadBabe =		0xABADBABE;	// is used by Apple as 
	// the "Boot Zero Block" magic number.

static const unsigned long IBadBoot =		0x1BADB002;	// Multiboot header magic number.

static const unsigned long BadFood =		0xBAADF00D;	// is used by Microsoft's 
	// LocalAlloc(LMEM_FIXED) to indicate uninitialised allocated heap memory 
	// when the debug heap is used.

static const unsigned long BadCable =		0xBADCAB1E;	// Error Code returned 
	// to the Microsoft eVC debugger when connection is severed to the debugger. 

static const unsigned long BadCafe =		0xBADDCAFE;	// is used by 'watchmalloc' 
	// in OpenSolaris to mark allocated but uninitialized memory. 

static const unsigned long BeadFace =		0xBEADFACE;	// Is the pattern that 
	// fills all unused memory locations in the Motorola 68HCS12DP256 
	// micro-controller simulator, SimHC12.

static const unsigned long FaceBead =		0xBEADFACE;	// Is the pattern that 
	// fills all unused memory locations in the Motorola 68HCS12DP256 
	// micro-controller simulator, SimHC12.

static const unsigned long CafeBabe =		0xCAFEBABE;	// is used by Mach-O to 
	// identify Universal object files, and by the Java programming language to 
	// identify Java bytecode class files.

static const unsigned long CafeDude	=		0xCAFED00D;	// is used by Java as a 
	// magic number for their pack200 compression.

static const unsigned long Disease =		0xD15EA5E;	// is a flag that indicates 
	// regular boot on the Nintendo GameCube and Wii consoles.

static const unsigned long DeadBabe =		0xDEADBABE;	// is used by IBM Jikes 
	// RVM as a sanity check of the stack of the primary thread

static const unsigned long DeadBeaf =		0xDEADBEEF;	// s used by IBM RS/6000 
	// systems, Mac OS on 32-bit PowerPC processors and the Commodore Amiga as a 
	// magic debug value. On Sun Microsystems' Solaris, it marks freed kernel memory. 
	// On OpenVMS  running on Alpha processors, DEAD_BEEF can be seen by pressing 
	// CTRL-T. The DEC Alpha SRM console has a background process that traps 
	// memory errors, identified by PS as "BeefEater waiting on 0xdeadbeef". 

static const unsigned long DeadDead =		0xDEADDEAD;	//is the bug check (STOP) 
	// code displayed when invoking a Blue Screen of Death either by telling the 
	// kernel via the attached debugger, or by using a special keystroke 
	// combination[7]. This is usually seen by driver developers, as it is used 
	// to get a memory dump on Windows NT based systems. An alternative to 
	// 0xDEADDEAD is the bug check code 0x000000E2[8], as they are both called 
	// MANUALLY_INITIATED_CRASH as seen on the Microsoft Developer Network.

static const unsigned long DeadFall =		0xDEADFA11; // is used by Apple in 
	// iOS crash reports, when user force quit the application.

static const unsigned long Defecated =		0xDEFEC8ED;	// is the magic number for 
	// OpenSolaris core dumps.

static const unsigned long FaceFeed =		0xFACEFEED;	// is used by Alpha 
	// servers running Windows NT. The Alpha Hardware Abstraction Layer (HAL) 
	// generates this error when it encounters a hardware failure.

static const unsigned long FeelDead =		0xFEE1DEAD;	// is used as a magic 
	// number in the Linux reboot system call.

static const unsigned long FeedFace =		0xFEEDFACE;	// is used as a header 
	// for Mach-O binaries, and as an invalid pointer value for 'watchmalloc' 
	// in OpenSolaris.

static const unsigned long Coffee =			0xC0FFEE;	// is used in 24-bit 
	// byte DSP like Motorola 56303.


static const unsigned long FeeeFeee =		0xFEEEFEEE;	// Used by Microsoft's 
	// HeapFree() to mark freed heap memory


#endif //#if !defined(__HEXSPEEK_H__350A3C08_505E_444F_8BDB_7B4E5D785363__INCLUDED) 
