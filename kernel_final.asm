#This is starter code, so that you know the basic format of this file.
#Use _ in your system labels to decrease the chance that labels in the "main"
#program will conflict

.data
.text
_syscallStart_:
    beq $v0, $0, _syscall0 #jump to syscall 0
    addi $k1, $0, 1
    beq $v0, $k1, _syscall1 #jump to syscall 1
    addi $k1, $0, 4

    # stars 
    beq $v0, $k1, _syscall4 #jump to syscall 4
    addi $k1, $0, 5

    beq $v0, $k1, _syscall5 #jump to syscall 5
    addi $k1, $0, 8

    #stars
    beq $v0, $k1, _syscall8 #jump to syscall 8
    addi $k1, $0, 9

    beq $v0, $k1, _syscall9 #jump to syscall 9
    addi $k1, $0, 10
    beq $v0, $k1, _syscall10 #jump to syscall 10
    addi $k1, $0, 11
    beq $v0, $k1, _syscall11 #jump to syscall 11
    addi $k1, $0, 12
    beq $v0, $k1, _syscall12 #jump to syscall 12
    addi $k1, $0, 60

    #stars
    # syscalls were randomly chosen
    beq $v0, $k1, _syscall60 #jump to syscall 60
    addi $k1, $0, 61

    #stars
    beq $v0, $k1, _syscall61 #jump to syscall 61

    addi $k1, $0, 62

    beq $v0, $k1, _syscall62 #jump to syscall 62

    addi $k1, $0, 63

    beq $v0, $k1, _syscall63 #jump to syscall 63

    #Error state - this should never happen - treat it like an end program
    j _syscall10

#Do init stuff
_syscall0:
    # Initialization goes here

    # initial value of stack pointer to -4096
    addi $sp, $0, -4096
    # initial heap pointer 
    la $k1, _END_OF_STATIC_MEMORY_
    sw $k1, 67104768($0) # 0x3FFF000 (memory reserved for the OS: between 0x3FFF000 and 0x3FFFFEFC)

    j _syscallEnd_

# Print Integer
_syscall1:

    

    addi $sp, $sp, -12
    addi $k1, $sp, 0 #Save the stack pointer in $k1 for later use
    sw $k0, 8($sp) #Save the return address ($k0) on the stack
    addi $k0, $a0, 0 #Move the integer to be printed from $a0 to $k0
    sw $t2, 4($sp)
    sw $t3, 0($sp)
    addi $t2, $0, 10
    slt $t2, $a0, $zero
    beq $t2, $zero, positive_num

    negative_num:
    addi $k0, $0, 45 #ASCII for '-'
    sw $k0, -256($0) #print minus sign
    sub $a0, $zero, $a0  # Make number positive

    positive_num:
    addi $k0, $a0, 0     # Move number to k0
    addi $t2, $0, 10     # Divisor = 10




    readInteger:
        div $k0, $t2
        mflo $k0 #save quotient in $k0 (for next iteration)
        mfhi $t3 #save remainder in $t3 (last digit)
        addi $sp, $sp, -4
        sw $t3, 0($sp) #Save the digit on the stack
        bne $k0, $0, readInteger #If the quotient is not zero, repeat the process 
        #if the quotient is zero, we etracted all the digits

    #pop the digits from the stack
    printOutLoop:
        lw $k0, 0($sp) #Load the next digit from stack
        addi $k0, $k0, 48 #Convert the digit to its ASCII 
        sw $k0, -256($0) #Print the character to the terminal
        addi $sp, $sp, 4
        bne $sp, $k1, printOutLoop #Repeat until all digits are printed

    #Restore the stack pointer and return address, and temp vars
    lw $t3, 0($sp)
    addi $sp, $sp, 4
    lw $t2, 0($sp)
    addi $sp, $sp, 4
    lw $k0, 0($sp)
    addi $sp, $sp, 4
    jr $k0


#Read Integer
#ASCII 48-57
_syscall5:
    # Read Integer code goes here
    addi $sp, $sp, -12
    sw $t2, 0($sp)
    sw $t3, 4($sp) #incremented in each iteration of the loop to check against the ASCII values of the digits '0' to '9
    sw $k0, 8($sp) #Save the return address ($k0) on the stack
    addi $t2, $0, 10 #used for constructing the integer
    addi $k1, $0, 0  #Initialize $k1 to 0 (used for storing the integer being read)
    
    readLoop:
        addi $t3, $0, 48 #Set $t3 to ASCII value of '0'
        lw $k0, -240($0) #Load the keyboard ready status from address 0xFFFFFF10
        beq $k0, $0, end_syscall5 #if no keypress, jump to end
        lw $k0, -236($0) #0xFFFFFF14 = read keyboard character
        sw $0, -240($0)  # Reset keyboard ready flag
        beq $k0, $t3, ascii0 #If the character is '0', jump to ascii0
        addi $t3, $t3, 1
        beq $k0, $t3, ascii1
        addi $t3, $t3, 1
        beq $k0, $t3, ascii2
        addi $t3, $t3, 1
        beq $k0, $t3, ascii3
        addi $t3, $t3, 1
        beq $k0, $t3, ascii4
        addi $t3, $t3, 1
        beq $k0, $t3, ascii5
        addi $t3, $t3, 1
        beq $k0, $t3, ascii6
        addi $t3, $t3, 1
        beq $k0, $t3, ascii7
        addi $t3, $t3, 1
        beq $k0, $t3, ascii8
        addi $t3, $t3, 1
        beq $k0, $t3, ascii9
        addi $t3, $t3, 1
        add $k1, $0, $k0

        j end_syscall5  #Jump to end if not a digit

    # processing the digit explanation
    # When reading each digit of the integer, we need to construct the 
    # integer by placing each digit in its correct place value. This is done by: 
    # 1. Multiplying by 10: This shifts the current value in $k1 one place 
    #to the left in decimal (e.g., 12 becomes 120), making room for the next digit.
    # 2. Adding the New Digit: This adds the new digit to the least 
    # significant place of the current value in $k1.

    ascii0:
        mult $k1, $t2 #Multiply the current value by 10 
        mflo $k1 #store the result in $k1
        j end_syscall5
    ascii1:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 1  #Add 1 to k1 value of '1'
        j end_syscall5
    ascii2:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 2
        j end_syscall5
    ascii3:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 3
        j end_syscall5
    ascii4:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 4
        j end_syscall5
    ascii5:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 5
        j end_syscall5
    ascii6:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 6
        j end_syscall5
    ascii7:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 7
        j end_syscall5
    ascii8:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 8
        j end_syscall5
    ascii9:
        mult $k1, $t2
        mflo $k1
        addi $k1, $k1, 9
        j end_syscall5
    
    end_syscall5:
        add $v0, $k1, $0
        lw $t2, 0($sp)
        addi $sp $sp, 4
        lw $t3, 0($sp)
        addi $sp $sp, 4
        lw $k0, 0($sp)
        addi $sp, $sp, 4
        jr $k0

#Heap allocation
_syscall9:
    # Heap allocation code goes here

    

    addi $sp, $sp, -4 #Allocate space on the stack for 1 word (4 bytes)
    sw $k0, 0($sp) #Save the return address ($k0) on the stack
    lw $k0, 67104768($0) #Load the current heap pointer from address 0x03FFF000
    add $v0, $k0, $0  #Move the current heap pointer to $v0 (return value)
    add $k0, $k0, $a0 #Increment the heap pointer by the number of bytes requested in $a0
    sw $k0, 67104768($0) #Store the updated heap pointer back to address 0x03FFF000
    lw $k0, 0($sp) #Restore the return address ($k0)
    addi $sp, $sp, 4 #Adjust the stack pointer
    jr $k0                  

# "End" the program
_syscall10:
   
    #infinite loop
    j _syscall10

#print character
_syscall11:
    # print character code goes here

   
    # Store the character in $a0 to the terminal 
    # data register at address 0xFFFFFF00
    sw $a0, -256($0) 
    jr $k0        

#read character
_syscall12:
    # read character code goes here

   

    lw $k1, -240($0) #0xFFFFFF10 = keyboard ready
    beq $k1, $0, end_syscall12 #If no keypress, jump to end_syscall12
    
    #If keypress, save character in register $v0
    lw $k1, -236($0) #0xFFFFFF14 = read keyboard character
    sw $0, -240($0) #Set keyboard ready status to 0 to get the next character
    addi $v0, $k1, 0 #Save character value in $v0
    end_syscall12:
        jr $k0


#extra challenge syscalls go here?

#print string
_syscall4:
    addi $sp, $sp, -4
    sw $k0, 0($sp)
    addi $k0, $a0, 0  #Copy the address of the string from $a0 to $k0

    print_string_loop:
        lw $k1, 0($k0) #Load the word at the address in $k0 into $k1
        beq $k1, $0, end_syscall4 #If the word is 0, jump to end
        sw $k1, -256($0) #Store the word in the terminal data register at address 0xFFFFFF00
        addi $k0, $k0, 4 #Increment the address in $k0 to point to the next word
        j print_string_loop

    end_syscall4:
        lw $k0, 0($sp) #Restore the return address ($k0)
        addi $sp, $sp, 4
        jr $k0

# read string
_syscall8:
    addi $sp, $sp, -8
    sw $k0, 4($sp) 
    sw $t0, 0($sp)                  
    addi $t0, $0, 1 #t0 is used for counting characters (and buffer overflow check)
    addi $k0, $a0, 0 #Copy the buffer address from $a0 to $k0
    slt $k1, $a1, $t0 #Check if $a1 (buffer size) is less than 1
    bne $k1, $0, end_syscall8 #end if size is invalid

    string_read_loop:
        beq $t0, $a1, end_syscall8 #If the buffer is full, end
        lw $k1, -240($0) #0xFFFFFF10 = keyboard ready
        beq $k1, $0, end_syscall8 #If no keypress, end
        
        # If keypress, save character in register $v0
        lw $k1, -236($0) #0xFFFFFF14 = read keyboard character
        
        beq $k1, 10, end_syscall8 #If the character is newline (ASCII 10), end
        # beq $k1, $0, end_syscall8 # If the character is 0, end
        
        sw $0, -240($0)  #Set keyboard ready status to 0 to get the next character
        sw $k1, 0($k0) #Store the character in the buffer at the address in $k0
        addi $k0, $k0, 4 #Increment the buffer address to point to the next word
        addi $t0, $t0, 1 #Increment the character count
        j string_read_loop        

    end_syscall8:
        sw $0, 0($k0) # terminate the string in the buffer
        lw $k0, 4($sp)
        lw $t0, 0($sp) 
        addi $sp, $sp, 8 
        jr $k0        


#read coord from joystick
#a0 and a1 represents x,y coord from joystick
# CHECK THE ADDRESSES
_syscall60:
    addi $sp, $sp -8
    sw $a0, 0($sp)
    sw $a1, 4($sp)
    lw $a0, -80($0) # for x cord at address 0x3FFFFB0
    lw $a1, -76($0) # for y cord at address 0x3FFFFB4
    addi $k1, $0, 8 # center coord(8,8)
    bne $k1, $a0, movement # If x coordinate is not 8, jump to movement
    bne $k1, $a1, movement
    j _syscall60 # no movement in joystick

    movement:
    # Use the loaded joystick coordinates in $a0 and $a1 as needed
    lw $a0, 0($sp)
    lw $a1, 4($sp)
    addi $sp, $sp, 8
    jr $k0

    end_syscall60:
    lw $a0, 0($sp)          
    lw $a1, 4($sp)          
    addi $sp, $sp, 8        
    jr $k0                  

# upload for sound control
# $a0, $a1, and $a2 represent the Volume, Frequency, and queue to play the sound
# use soundtest.asm for testing 
# CHECK THE ADDRESSES
_syscall61:
    addi $sp, $sp, -4       
    sw $a2, 0($sp)          
    sw $a0, -84($0) # Store the volume in the sound control register at address 0xFFFFFFAC
    sw $a1, -92($0) # Store the frequency in the sound control register at address 0xFFFFFFA4
    lw $t0, -88($0) # Load the queue value from the sound control register at address 0xFFFFFFA8
    # Use the queue value in $t0 if needed
    lw $a2, 0($sp)          
    addi $sp, $sp, 4       
    jr $k0                  

    # set LED to on or off
    # $a0 - LED state
_syscall62:
    sw $a0, -191($0) #Store the LED state in the LED control register at address 0xFFFFFF41
    jr $k0 


# HEX display
# $a0 - HEX value
_syscall63:
    sw $a0, -208($0) #Store the HEX value in the HEX display control register at address 0xFFFFFF30
    jr $k0


_syscallEnd_: