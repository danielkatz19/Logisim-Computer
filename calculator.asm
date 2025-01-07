.data
.text
.globl main

main:
    addi $s0, $0, 1    #s0 for 10*10*10...
    addi $s5, $0, 0
    addi $t5, $0, 0

    readIntLoop:
        addi $s1, $0, 10    #s1 = 10
        

        addi $v0, $0, 5 
        syscall
        addi $s2, $v0, 0    #s2 = int 0-9

        slt $s3, $s2, $s1   # If 0-9
        beq $s3, $0, reverse_loop #Else, is a char
        
        mult $s2, $s0   #s2 = s2*s0 (1,10,100...)
        mflo $s2    

        mult $s0, $s1   #s0 = (1,10,100...)
        mflo $s0

        add $s5, $s5, $s2   #s5 = s5+s2
        j readIntLoop

    reverse_loop:
    beq $s5, $zero, EndReadIntLoop  # If $s5 is 0, finish the loop
    addi $s1, $0, 10    #s1 = 10
    div $s5, $s1            # Divide $t1 by 10; quotient in $lo, remainder in $hi
    mflo $s5               # Move quotient back to $t1 for next iteration
    mfhi $t3                # Move remainder to $t3
    
    mult $t5, $s1       # Multiply current reversed number by 10
    mflo $t5
    add $t5, $t5, $t3       # Add the last digit to the reversed number

    j reverse_loop          # Jump back to the start of the loop

    EndReadIntLoop:
    add $s5, $t5, $0

    addi $s4, $0, 95
    beq $s6, $s4, previousAnswer
    beq $s2, $s4, previousAnswer

    addi $s4, $0, 43
    beq $s6, $s4, additionLoop
    beq $s2, $s4, additionLoop
    

    addi $s4, $0, 45
    beq $s6, $s4, subtractionLoop
    beq $s2, $s4, subtractionLoop
    

    addi $s4, $0, 42
    beq $s6, $s4, multiplicationLoop
    beq $s2, $s4, multiplicationLoop

    addi $s4, $0, 47
    beq $s6, $s4, divisionLoop
    beq $s2, $s4, divisionLoop


    j main

previousAnswer:
    addi $t6, $0, 43
    beq $s6, $t6, afterAdditionCheck
    beq $s2, $t6, afterAdditionCheck

    addi $t6, $0, 45
    beq $s6, $t6, afterSubtractionCheck
    beq $s2, $t6, afterSubtractionCheck


    addi $t6, $0, 42
    beq $s6, $t6, afterMultiplicationCheck
    beq $s2, $t6, afterMultiplicationCheck

    addi $t6, $0, 47
    beq $s6, $t6, afterDivisionCheck
    beq $s2, $t6, afterDivisionCheck

    j main

additionLoop:
    addi $s7, $s5, 0
    jal Read2ndInt
    addi $s8, $t5, 0
    add $t7, $s8, $s7 #t7 holds original value
    add $a0, $s8, $s7
    addi $v0, $0, 1
    syscall
    j main

afterAdditionCheck:
    jal Read2ndInt
    addi $s8, $t5, 0
    add $a0, $t7, $s8
    addi $v0, $0, 1
    syscall
    add $t7, $t7, $s8 #t7 holds original value
    j main

subtractionLoop:
    addi $s7, $s5, 0
    jal Read2ndInt
    addi $s8, $t5, 0
    sub $t7, $s7, $s8 #t7 holds original value
    sub $a0, $s7, $s8
    addi $v0, $0, 1
    syscall
    j main

afterSubtractionCheck:
    jal Read2ndInt
    addi $s8, $t5, 0
    sub $a0, $t7, $s8
    addi $v0, $0, 1
    syscall
    sub $t7, $t7, $s8 #t7 holds original value
    j main

multiplicationLoop:
    addi $s7, $s5, 0
    jal Read2ndInt
    addi $s8, $t5, 0
    mult $s7, $s8
    mflo $t7    #t7 holds original value
    add $a0, $0, $t7
    addi $v0, $0, 1
    syscall
    j main

afterMultiplicationCheck:
    
    jal Read2ndInt
    addi $s8, $t5, 0

    mult $t7, $s8
    mflo $a0    
    addi $v0, $0, 1
    syscall
    add $t7, $0, $a0 #t7 holds original value
    j main

divisionLoop:
    addi $s7, $s5, 0
    jal Read2ndInt
    addi $s8, $t5, 0
    div $s7, $s8
    mflo $t7    #t7 holds original value
    add $a0, $0, $t7
    addi $v0, $0, 1
    syscall
    j main

afterDivisionCheck:
    jal Read2ndInt
    addi $s8, $t5, 0

    div $t7, $s8
    mflo $a0    
    addi $v0, $0, 1
    syscall
    add $t7, $0, $a0 #t7 holds original value
    j main


Read2ndInt:
addi $s0, $0, 1    #s0 for 10*10*10...
addi $s5, $0, 0
addi $t5, $0, 0

read2ndIntLoop:
    addi $s1, $0, 10    #s1 = 10

    addi $v0, $0, 5 
    syscall
    addi $s2, $v0, 0    #s2 = int 0-9

    slt $s3, $s2, $s1   # If 0-9
    beq $s3, $0, reverse2nd_loop #Else, is a char
    
    mult $s2, $s0   #s2 = s2*s0 (1,10,100...)
    mflo $s2    

    mult $s0, $s1   #s0 = (1,10,100...)
    mflo $s0

    add $s5, $s5, $s2   #s5 = s5+s2
    j read2ndIntLoop

reverse2nd_loop:
beq $s5, $zero, EndRead2ndIntLoop  # If $s5 is 0, finish the loop
addi $s1, $0, 10    #s1 = 10
div $s5, $s1            # Divide $t1 by 10; quotient in $lo, remainder in $hi
mflo $s5               # Move quotient back to $t1 for next iteration
mfhi $t3                # Move remainder to $t3

mult $t5, $s1       # Multiply current reversed number by 10
mflo $t5
add $t5, $t5, $t3       # Add the last digit to the reversed number

j reverse2nd_loop          # Jump back to the start of the loop

EndRead2ndIntLoop:
    addi $s6, $s2, 0
    jr $ra