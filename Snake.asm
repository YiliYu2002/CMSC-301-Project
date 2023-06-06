.data
.text
.globl main

main:
    #uncomment the below if you haven't added the stack
    #pointer initialization to your assembler yet
    addi $sp, $sp, -48
    #Store RGB values in stack allocated array
    #Mixing palette
    addi $t0, $0, 255 #blue
    sll $t1, $t0, 8  #green
    sll $t2, $t0, 16 #red
    addi $t3, $0, 127 #less blue
    sll $t4, $t3, 8 #less green
    sll $t5, $t3, 16 #less red

    #red - 0
    sw $t2, 0($sp)
    #red-orange - 1
    add $t8, $t2, $t4
    sw $t8, 4($sp)
    #yellow - 2
    add $t8, $t2, $t1
    sw $t8, 8($sp)
    #green-yellow - 3
    add $t8, $t5, $t1
    sw $t8, 12($sp)
    #green - 4
    sw $t1, 16($sp)
    #cyan-green - 5
    add $t8, $t3, $t1
    sw $t8, 20($sp)
    #cyan-6
    add $t8, $t0, $t1
    sw $t8, 24($sp)
    #cyan-blue
    add $t8, $t0, $t4
    sw $t8, 28($sp)
    #blue - 8
    sw $t0, 32($sp)
    #blue-magenta - 9
    add $t8, $t0, $t5
    sw $t8, 36($sp)
    #magenta - 10
    add $t8, $t0, $t2
    sw $t8, 40($sp)
    #red-magenta - 11
    add $t8, $t3, $t2
    sw $t8, 44($sp)

    add $t9, $0, $sp

    # Array Allocation
    addi $sp, $sp, -16128
    add $s3, $sp, $0
    # Heap Pointer 
    addi $t1, $0, 0
    sw $k1, 0($a0) # !!!probably an issue now!!!


    addi $s0, $0, 64 #x coordinate
    addi $s1, $0, 63 #y coordinate
    addi $s2, $0, 0 #color
    addi $t0, $0, 100
 
loop:
    #check for keypress
    addi $v0 $0 12
    syscall 
    beq $v0, $0, noNew #if no keypress, keep on keeping on 
        add $t0, $v0, $0
        addi $s4, $0, 0

        add $a0, $0, $v0
        addi $v0, $0, 66
        syscall
    keepon:
    #if keypress, update x,y coordinate
    #lw $t0, 65412($0) #0xFF84 = read keyboard character
    addi $t1, $0, 119 #ascii w
    bne $t0, $t1, notw
    addi $s1, $s1, -1
    j update_positions
notw:
    addi $t1, $0, 97 #ascii a
    bne $t0, $t1, nota
    addi $s0, $s0, -1
    j update_positions
nota:
    addi $t1, $0, 115 #ascii s
    bne $t0, $t1, nots
    addi $s1, $s1, 1
    j update_positions
nots:
    addi $t1, $0, 100 #ascii d
    bne $t0, $t1, endkeypress # invalid keypress
    addi $s0, $s0, 1
    j update_positions

# delete endkeypress
endkeypress:
    sw $0, 65408($0) #set keyboard ready to 0 to get next character
colorchange:
    #change color
    addi $s2, $s2, 1
    addi $t6, $0, 12
    div $s2, $t6
    mfhi $s2 #s2 = (s2 + 1) % 12
    sll $t6, $s2, 2
    add $t6, $t6, $t9 #get address of color
    lw $t6, 0($t6) #t6 = load rgb color from memory
    #display pixel
    add $a0, $s0, $0 #0xFF90 = monitor x coordinate
    add $a1, $s1, $0 #0xFF94 = monitor y coordinate
    add $a2, $t6, $0 #0xFF98 = monitor color
    add $a3, $0, $0
    addi $v0, $0, 80 
    syscall
    j loop



update_positions:
        
        # snakex < 0 || 
        addi    $t7, $0, 64
        slt     $t3, $s0, $0
        bne     $t3, $zero, break

        # snakey < 0 || 
        addi    $t7, $0, 63
        slt     $t3, $s1, $zero
        bne     $t3, $zero, break
        
        # snakex >= screenw
        addi    $t7, $0, 126
        slt     $t3, $t7, $s0
        bne     $t3, $zero, break
        
        # snakey >= screenh
        addi    $t7, $0, 128
        slt     $t4, $t7, $s1
        bne     $t4, $zero, break

        # # if (board[snakey][snakex] == 1 ||
        addi    $t6, $0, 4
        mult    $t6, $s0 # x * 4
        mflo    $t1
        addi    $t6, $0, 504
        mult    $t6, $s1 # y * 504
        mflo    $t6
        add     $t6, $t6, $t1
        add     $t6, $t6, $s3 #s3 is beginning of array in mem
        lw      $t1, 0($t6)
        bne     $t1, $0, break

        addi    $t1, $0, 1
        sw      $t1, 0($t6)

        # Continue execution
        bne $s4, $0, colorchange
        j       endkeypress

break:
    # sound stuff, but couldn't make work
    # addi $a0, $0, 440 
    # addi $a1, $0, 1 
    # addi $a2, $0, 50
    # addi $v0, $0, 65
    # syscall
    addi $v0, $0, 10
    syscall
        # Handle break case here

noNew:
    addi $s4, $0, 1
    j keepon