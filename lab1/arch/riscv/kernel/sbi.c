#include "types.h"
#include "sbi.h"


// struct sbiret sbi_ecall(int ext, int fid, uint64 arg0,
// 			            uint64 arg1, uint64 arg2,
// 			            uint64 arg3, uint64 arg4,
// 			            uint64 arg5) 
// {
//     // unimplemented    
// 	long error;
// 	long value;  
// 	__asm__ volatile (
// 		"mv a7, %[ext]\n"
// 		"mv a6, %[fid]\n"
// 		"mv a5, %[arg5]\n"
// 		"mv a4, %[arg4]\n"
// 		"mv a3, %[arg3]\n"
// 		"mv a2, %[arg2]\n"
// 		"mv a1, %[arg1]\n"
// 		"mv a0, %[arg0]\n"
// 		"ecall\n"
// 		"add %[value], x0, a1\n"
// 		"add %[error], x0, a0\n"
// 		: [error] "=r" (error), [value] "=r" (value)
// 		: [ext] "r" (ext), [fid] "r" (fid), [arg5] "r" (arg5), [arg4] "r" (arg4),
// 			[arg3] "r" (arg3), [arg2] "r" (arg2), [arg1] "r" (arg1), [arg0] "r" (arg0)
// 		: "memory"
// 	);     
// 	struct sbiret ret;
// 	ret.error = error;
// 	ret.value = value;
// 	return ret;
// }

struct sbiret sbi_ecall(int ext, int fid, uint64 arg0,
			            uint64 arg1, uint64 arg2,
			            uint64 arg3, uint64 arg4,
			            uint64 arg5) 
{
    // unimplemented   
	long error, value;
	__asm__ volatile (
		"add a7, x0, %[ext]\n"
		"add a6, x0, %[fid]\n"
		"add a0, x0, %[arg0]\n"
		"add a1, x0, %[arg1]\n"
		"add a2, x0, %[arg2]\n"
		"add a3, x0, %[arg3]\n"
		"add a4, x0, %[arg4]\n"
		"add a5, x0, %[arg5]\n"
		"ecall\n"
		"add %[value], x0, a1\n"
		"add %[error], x0, a0\n"

		:[error] "=r" (error), [value] "=r" (value)
		:[ext] "r" (ext), [fid] "r" (fid), 
		[arg0] "r" (arg0), [arg1] "r" (arg1), [arg2] "r" (arg2), 
		[arg3] "r" (arg3), [arg4] "r" (arg4), [arg5] "r" (arg5)
		:"memory"
	);

	struct sbiret _ret;
	_ret.error = error;
	_ret.value = value;

	return _ret;

}