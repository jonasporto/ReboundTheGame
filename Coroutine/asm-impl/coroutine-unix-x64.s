.intel_syntax noprefix

#  
# This file implements context functionality for UNIX x64 systems that support
# the System V ABI
#

    .set context_rsp, 0
    .set context_rbp, context_rsp + 8
    .set context_rbx, context_rbp + 8
    .set context_r12, context_rbx + 8
    .set context_r13, context_r12 + 8
    .set context_r14, context_r13 + 8
    .set context_r15, context_r14 + 8
    .set context_num_saved_variables, context_r15 + 8

.section .text
    .extern malloc
    .extern free
  
#=======================================================================================
#
# coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
# - rdi: point
# - rsi: stack_size
# - rax: new coroutine_t*
#
#=======================================================================================

    .global coroutine_new
coroutine_new:
    # allocate memory for the context data plus the stack
    push rdi
    push rsi
    lea rdi, [rsi + context_num_saved_variables]  # allocate everything in a single chunk
    call malloc
    pop rsi
    pop rdi
    
    # If allocation fails, return early
    test rax, rax
    jz early_return
    
    # Initialize the stack for a stub entry point
    lea rsi, [rax + rsi + context_num_saved_variables-16]
    and rsi, -16
    sub rsi, 8
    
    mov [rsi+16], rdi                      # Bottom of stack: return pointer
    mov [rsi+8], rax                       # Middle of stack: context_t parameter
    lea rdi, entry_point                   # 
    mov [rsi], rdi                         # Top of stack: entry_point stub
    
    mov [rax + context_rsp], rsi           # Move the stack pointer to its place
    
early_return:
    ret

#=======================================================================================
# This implements the stub entry point for proper funcion entering
#=======================================================================================
entry_point:
    # assemble the function's parameters
    mov rdi, [rsp]
    mov rsi, rax
    call [rsp+8]
    
    # retrieve the parameters again
    mov rdi, [rsp]
    
    # invalidate the own method's stack - this is safe
    mov rsp, 0
    
    # yield a last time
    jmp cryield

#=======================================================================================
#
# void* coroutine_resume(coroutine_t coro, void* param);
# void* coroutine_yield(coroutine_t coro, void* param);
# - rdi: coro
# - rsi: param
# - rax: param returned from the entry point
#
#=======================================================================================
    .global coroutine_resume
coroutine_resume:
    .global coroutine_yield
coroutine_yield:
    test rdi, rdi # Careful with nullptr's
    jz return_nullptr
    
    # If the coroutine is ended, return
    cmp qword ptr [rdi + context_rsp], 0
    jz return_nullptr
    
    # Set the return value as the param passed
    mov rax, rsi
    
cryield:
    # Exchange callee-saved registers
    xchg [rdi + context_rsp], rsp
    xchg [rdi + context_rbp], rbp
    xchg [rdi + context_rbx], rbx
    xchg [rdi + context_r12], r12
    xchg [rdi + context_r13], r13
    xchg [rdi + context_r14], r14
    xchg [rdi + context_r15], r15
    ret
    
return_nullptr:
    mov rax, 0
    ret

#=======================================================================================
#
# int is_coroutine_ended(coroutine_t coro)
# - rdi: coro
# - rax: 1 if coroutine is ended, 0 if it is not
#
#=======================================================================================
    .global is_coroutine_ended
is_coroutine_ended:
    test rdi, rdi
    jz coroutine_is_ended

    cmp qword ptr [rdi + context_rsp], 0
    jz coroutine_is_ended
    mov rax, 0
    ret

coroutine_is_ended:
    mov rax, 1
    ret
    
#=======================================================================================
#
# void coroutine_destroy(coroutine_t coro)
# - rdi: coro
#
#=======================================================================================
    .global coroutine_destroy
coroutine_destroy:
    test rdi, rdi
    jz dont_delete_unended_coroutine

    cmp qword ptr [rdi + context_rsp], 0
    jnz dont_delete_unended_coroutine
    call free
dont_delete_unended_coroutine:
    ret
    
