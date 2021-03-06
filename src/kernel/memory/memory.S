/* The functions in this file are for the most part self-explanatory.
 *
 * They are basic helpers to get and set the various paging registers.
 */
.global read_cr0
read_cr0:
	mov %cr0, %eax
	ret

.global write_cr0
write_cr0:
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %eax
	mov %eax, %cr0
	pop %ebp
	ret

.global read_cr3
read_cr3:
	mov %cr3, %eax
	ret

.global read_cr2
read_cr2:
	mov %cr2, %eax
	ret

.global write_cr3
write_cr3:
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %eax
	mov %eax, %cr3
	pop %ebp
	ret
