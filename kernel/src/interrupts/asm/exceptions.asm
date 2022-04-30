bits 64

;; 2022 Ian Moffett <ian@kesscoin.com>

global div0_handler
global debug_exception
global overflow_exception
global bre_exception
global invalid_opcode_exception
global dev_not_avail_exception
global double_fault
global invalid_tss_exception
global snp_exception
global stackseg_fault
global gpf
global page_fault


extern log              ; void log(const char* format, STATUS status, ...);
extern hex2str
extern canvas
extern clearScreen
extern dump_stack

%define WARNING 0
%define INFO 1
%define CRITICAL 2
%define PANIC 3


%macro panic 1
    mov rdi, canvas
    mov rsi, 0x39254D
    call clearScreen

    mov rdi, panicmsg1
    mov rsi, PANIC
    call log

	mov rdi, panicmsg2
	mov rsi, -1
	call log

    mov rdi, %1
    call hex2str
    mov rdi, rax
    mov rsi, -1
    call log

	mov rdi, newline
	mov rsi, -1
	call log

	mov rdi, panicmsg3
	mov rsi, -1
	call log

	mov rdi, rsp
	call dump_stack

    cli
    hlt
%endmacro


div0_handler:
    panic 0x0

debug_exception:
    panic 0x1

overflow_exception:
    panic 0x4

bre_exception:
    panic 0x5

invalid_opcode_exception:
    panic 0x6

dev_not_avail_exception:
    panic 0x7

double_fault:
    panic 0x8

invalid_tss_exception:
    panic 0xA

snp_exception:
    panic 0xB

stackseg_fault:
    panic 0xC

gpf:
    panic 0xD

page_fault:
    panic 0xE


panicmsg1: db 0xA, "Whoops! I've encountered an issue. Below is some debug information to help understand what happened.", 0xA, 0x0
panicmsg2: db 0xA, "VECTOR FIRED: ", 0x0
panicmsg3: db 0xA, "<==== STACK DUMP BELOW ====>", 0xA, 0xA, 0x0
newline: db 0xA, 0x0
