# ; syscall:
# ;     beq $v0 $0 main #if $vo is 0, jump to main addi $k1, $0, 12
# ;     beq $v0 $k1 syscall12
# ;     addi $k1 $0 9
# ;     beq $v0 $k1 syscall9
# ;     addi $k1 $0 10
# ;     beq $v0 $k1 syscall10
# ; sycall9:
# ;     # your assembly goes here
# ; syscall10:
# ;     j syscall10
# ; syscall12:
.globl main
syscall:
    beq $v0, $0, main
    addi $k1, $0, 10
    beq $v0, $k1, syscall10
    addi $k1, $0, 12
    beq $v0, $k1, syscall12
    addi $k1, $0, 80
    beq $v0, $k1, syscall80
    addi $k1, $0, 5
    beq $v0, $k1, syscall5
    addi $k1, $0, 77
    beq $v0, $k1, syscall77
    addi $k1, $0, 65
    beq $v0, $k1, syscall65



# END PROGRAM
syscall10:
    j syscall10
    jr $k0


# PRINT CHAR
syscall12:
    lw $k1, 16($0)            #we is 0, access to 16
    beq $k1, $0, do_nothing   #
    lw $v0, 20($0)
    sw $0, 16($0)
    j finish
    do_nothing:
    addi $v0, $0, 0
    finish:
    jr $k0



# READ INTEGER
syscall5:

    addi $sp, $sp, -4
    sw $t0, 0($sp)

    addi $v1, $0, 0
    read:

    addi $sp, $sp, -4
    sw $k0, 0($sp)
    addi $v0, $0, 12
    syscall
    lw $k0, 0($sp)
    addi $sp, $sp, 4

    beq $v0, $0, read
    #lw $k1, 16($0)
    #beq $k1, $0, read

    addi $t0, $0, 32
    beq $v0, $t0, stop

    beq $v1, $0, miss_mult
    addi $k1, $0, 10
    mult $v1, $k1
    mflo $v1

    miss_mult:
    #sw $0, 16($0)

    #lw $v0, 20($0)


    addi $k1, $0, 48
    bne $v0, $k1, not0
    addi $v1, $v1, 0
    j found_int
    not0:
    addi $k1, $k1, 1
    bne $v0, $k1, not1
    addi $v1, $v1, 1
    j found_int
    not1:

    addi $k1, $k1, 1
    bne $v0, $k1, not2
    addi $v1, $v1, 2
    j found_int
    not2:

    addi $k1, $k1, 1
    bne $v0, $k1, not3
    addi $v1, $v1, 3
    j found_int
    not3:

    addi $k1, $k1, 1
    bne $v0, $k1, not4
    addi $v1, $v1, 4
    j found_int
    not4:

    addi $k1, $k1, 1
    bne $v0, $k1, not5
    addi $v1, $v1, 5
    j found_int
    not5:

    addi $k1, $k1, 1
    bne $v0, $k1, not6
    addi $v1, $v1, 6
    j found_int
    not6:

    addi $k1, $k1, 1
    bne $v0, $k1, not7
    addi $v1, $v1, 7
    j found_int
    not7:

    addi $k1, $k1, 1
    bne $v0, $k1, not8
    addi $v1, $v1, 8
    j found_int
    not8:

    addi $k1, $k1, 1
    bne $v0, $k1, stop
    addi $v1, $v1, 9

    found_int:

    j read

    stop:

    addi $v0, $v1, 0

    lw $t0, 0($sp)
    addi $sp, $sp, 4

    jr $k0



# Heap Allocation
syscall9:
    lw $k1, 0($a0) # load current heap pointer  
    add $k1, $k1, $a1 #increase heap pointer value by set amount
    sw $k1, 0($a0) #store new pointer value
    jr $k0  

# syscall80:
#     sw $a0, 32($0)
#     sw $a1, 36($0)
#     sw $a2, 40($0)
#     sw $0, 44($0)
#     #addi $v0, $0, 18
#     jr $k0

# syscall77:
#     sw $a0, 112($0)
#     jr $k0

# syscall65:
#     addi $sp, $sp, -8
#     sw $t0, 0($sp)
#     sw $t1, 4($sp)
#     lw $t0, 48($0)
#     lw $t1, 52($0)
#     addi $v0, $t0, 0
#     addi $v1, $t1, 0
#     lw $t0, 0($sp)
#     lw $t1, 4($sp)
#     addi $sp, $sp, 8
#     jr $k0
