section .text
global main
main:
    mov ebp, esp; for correct debugging
    lea     ebx, root; load tree address
    mov     edx, 0x00000033
    call    find_key; find 0x33 results 0
    call    add_key; add key 0x33
    call    find_key; find 0x33 results not 0
    ret
  
;-----------------------------------------------------
;Finds key in tree
;Inputs: ebx - root address, edx - key to find
;Outputs: eax - key address, 0 if not found
;Stack: preserves ebx, esi
find_key:
    push    ebx; preserve registers
    push    esi
.loop:
    mov     eax, [ebx]; load current key
    cmp     edx, eax; determine direction, quit if found
    jz      .found
    ja      .above
.below:
    movzx   eax, byte [ebx + 4]; load offset of left child
    jmp     .next
.above:
    movzx   eax, byte [ebx + 5]; load offset of right child
.next:
    test    eax, eax
    jz      .not_found; quit, if child offset is 0
    mov     esi, eax; multiply child's offset by 6
    shl     eax, 2
    add     eax, esi
    add     eax, esi
    add     ebx, eax; shift ebx
    jmp     .loop; proceed to next key
.found:
    mov     eax, ebx; return node address
.not_found:
    pop     esi; restore registers
    pop     ebx
    ret
    
;-----------------------------------------------------
;Adds key to tree, does nothing if key exist
;Inputs: ebx - root address, edx - key to add
;Outputs: none
;Stack: saves ebx, esi, ecx, eax
add_key:
    push    ebx; restore registers
    push    esi
    push    ecx
    push    eax
    mov     ecx, [node_count]; ecx holds offset to empty space
.loop:
    mov     eax, [ebx]; load current key
    cmp     edx, eax; determine direction, quit if found
    jz      .quit
    ja      .above
.below:
    movzx   eax, byte [ebx + 4]; load offset of left child
    test    eax, eax; proceed, if child exist
    jnz     .next
    mov     byte [ebx + 4], cl; update current node's left child
    jmp     .add
.above:
    movzx   eax, byte [ebx + 5]; load offset of right child
    test    eax, eax; proceed, if child exist
    jnz     .next
    mov     byte [ebx + 5], cl; update current node's right child
    jmp     .add
.next:
    sub     ecx, eax; update offset of empty space
    mov     esi, eax; multiply child's offset by 6
    shl     eax, 2
    add     eax, esi
    add     eax, esi
    add     ebx, eax; shift ebx
    jmp     .loop
.add:
    mov     eax, ecx; eax := ecx * 6
    shl     eax, 2
    add     eax, ecx
    add     eax, ecx
    add     ebx, eax; ebx points to empty space
    mov     [ebx], edx; save key
    mov     word [ebx + 4], 0; nullify both children
    inc     dword [node_count]; update node counte
.quit:
    pop     eax; restore registers
    pop     ecx
    pop     esi
    pop     ebx
    ret
    
section .data
node_count:
    DD 9
root:
    DD 0x00000021
    DB 0x01, 0x02
    DD 0x00000011
    DB 0x02, 0x03
    DD 0x00000031
    DB 0x04, 0x06
    DD 0x00000005
    DB 0x00, 0x00
    DD 0x00000018
    DB 0x00, 0x01
    DD 0x00000019
    DB 0x00, 0x00
    DD 0x00000025
    DB 0x01, 0x00
    DD 0x00000023
    DB 0x00, 0x00
    DD 0x00000037
    DB 0x00, 0x00
    