[GLOBAL return_from_scheduler]

return_from_scheduler:
    mov esp, [esp+4]

    pop eax
    mov ds, ax

    pop eax
    mov es, ax

    pop eax
    mov fs, ax

    pop eax
    mov gs, ax

    popa
    add esp, 8 ; clears error code and isr number
    sti
    iret ; CS, EIP, EFLAGS, SS, and ESP