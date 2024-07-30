# Set the stack pointer to 1 << 11 = 2048
sll $sp, $imm1, $imm2, $zero, 1, 11      # Shift left logical to set stack pointer

# Load n and k from memory
lw $a0, $zero, $imm1, $zero, 0x100, 0    # Load n from memory address 0x100 into $a0
lw $a1, $zero, $imm1, $zero, 0x101, 0    # Load k from memory address 0x101 into $a1

# Call the binom function
jal $ra, $zero, $zero, $imm1, Binom , 0  # Jump to Binom function and link return address

# Store the result of binom(n,k) in memory at address 0x102
sw $zero, $zero, $imm1, $v0, 0x102, 0    # Store the result in memory address 0x102

# Halt the execution
halt $zero, $zero, $zero, $zero, 0, 0    # Halt the program

# Binomial function
Binom:
    # Adjust stack for 4 items
    add $sp, $sp, $imm2, $zero, 0, -4    # Adjust stack pointer to make space for 4 items
    
    # Save registers on the stack
    sw $zero, $sp, $imm1, $s0, 3, 0      # Save $s0 register on the stack
    sw $zero, $sp, $imm1, $ra, 2, 0      # Save return address ($ra) on the stack
    sw $zero, $sp, $imm1, $a0, 1, 0      # Save n ($a0) on the stack
    sw $zero, $sp, $imm1, $a1, 0, 0      # Save k ($a1) on the stack
    
    # Check for base cases
    beq $zero, $a0, $a1, $imm1, First_Loop, 0  # If n == k, go to First_Loop
    beq $zero, $zero, $a1, $imm1, First_Loop, 0 # If k == 0, go to First_Loop
    beq $zero, $zero, $zero, $imm1, Second_Loop, 0 # Else, go to Second_Loop
    
First_Loop:
    # Return 1 if n == k or k == 0
    add $v0, $imm1, $zero, $zero, 1, 0  # Set $v0 to 1 (base case result)
    beq $zero, $zero, $zero, $imm1, Third_Loop, 0 # Go to Third_Loop
    
Second_Loop:
    # Recursive calculation for binomial coefficient
    sub $a0, $a0, $imm1, $zero, 1, 0    # n = n - 1
    jal $ra, $zero, $zero, $imm1, Binom, 0 # Call Binom(n-1, k) recursively
    add $s0, $v0, $zero, $zero, 0, 0    # Store result in $s0
    sub $a1, $a1, $imm1, $zero, 1, 0    # k = k - 1
    jal $ra, $zero, $zero, $imm1, Binom, 0 # Call Binom(n-1, k-1) recursively
    add $v0, $v0, $s0, $zero, 0, 0      # Add results of both recursive calls
    
    # Restore registers from the stack
    lw $a1, $sp, $imm1, $zero, 0, 0     # Restore k ($a1) from the stack
    lw $a0, $sp, $imm1, $zero, 1, 0     # Restore n ($a0) from the stack
    lw $ra, $sp, $imm1, $zero, 2, 0     # Restore return address ($ra) from the stack
    lw $s0, $sp, $imm1, $zero, 3, 0     # Restore $s0 register from the stack

Third_Loop:
    # Pop 4 items from the stack
    add $sp, $sp, $imm1, $zero, 4, 0    # Adjust stack pointer back
    
    # Return to the caller
    beq $zero, $zero, $zero, $ra, 0, 0  # Return to the caller

