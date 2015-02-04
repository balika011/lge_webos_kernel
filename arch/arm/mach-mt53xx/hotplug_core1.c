void mt53xx_Core1StubSimple(void)
{
	
#ifdef CC_TRUSTZONE_SUPPORT
			 unsigned int  reg;
#endif //CC_TRUSTZONE_SUPPORT
#if defined(CONFIG_ARCH_MT5890)||defined(CONFIG_ARCH_MT5882)
		unsigned int  addr = 0xf0008068,  magic = 0xffffb007;
	
		__asm__ ("isb");
	
		__asm__ ("LOOP:");
		__asm__ ("wfe");
		__asm__ ("MOV	  r0, %0" : : "r" (addr));
		__asm__ ("MOV	  r1, %0" : : "r" (magic));
		__asm__ ("ldr	  r2, [r0]");
		__asm__ ("cmp	  r2, r1");
		__asm__ ("bne	  LOOP");
	
#ifdef CC_TRUSTZONE_SUPPORT
			//reg = (1024 * 1024 * 1024) - (18 * 1024 * 1024) + (0x5000);
			#if 1
			//__asm__ ("LOOP1:");
			//__asm__ ("bl	  LOOP1");
			reg = 0xf0008060;
			__asm__ ("MOV	  r0, %0" : : "r" (reg));
			//__asm__ ("LDR     r0, =0xf0008060");
			__asm__ ("ldr	  r1, [r0]");
			//__asm__ ("MOV	  r1, %0" : : "r" (reg));
			__asm__ ("MOV	  r0,#0x0");  // core1 always run normal bootup flow
			__asm__ ("mov	  r3, pc");
			__asm__ ("blx	  r1");
			__asm__ ("bl	  inv_cache");
			#else
		__asm__ ("MOV	  r1, %0" : : "r" (0x3ee05000));
		__asm__ ("MOV	  r0,#0x0");  // core1 always run normal bootup flow
		__asm__ ("mov	  r3, pc");
		__asm__ ("blx	  r1");
		__asm__ ("bl	  inv_cache");
			#endif
#endif // CC_TRUSTZONE_SUPPORT
	
		__asm__ ("LDR     r3, =0xf0008188");
		__asm__ ("MOV	  r0, r3");
		__asm__ ("ldr	  r1, [r0]");
		__asm__ ("blx	  r1");
		__asm__ volatile (
					"inv_cache: 							 \n"
		
					"		mcr  p15, 0, r0, c8, c7, 0		 \n"
					"		dmb 							 \n"
					"		mov  r0, #255		@ Sets to go \n"
					"1: 									 \n"
					"		mov  r1, #3 		@ 4 Ways	 \n"
					"2: 									 \n"
					"		mov  r2, r1, lsl #30			 \n"
					"		orr  r2, r2, r0, lsl #5 		 \n"
					"		mcr  p15, 0, r2, c7, c6, 2	@ Invalidate\n"
					"		subs r1, r1, #1 				 \n"
					"		bge  2b 						 \n"
					"		subs r0, r0, #1 				 \n"
					"		bge  1b 						 \n"
					"		dsb 							 \n"
					"		mov  pc, lr 					 \n");
	
#endif // defined(CONFIG_ARCH_MT5890)
	}