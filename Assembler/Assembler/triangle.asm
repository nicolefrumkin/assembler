# Define coordinates for points A, B, C
# A (xA, yA), B (xB, yB), C (xC, yC)
# Assume coordinates are stored in memory locations 0x100, 0x101, 0x102 respectively

# Load coordinates of points A, B, C
lw $t0, $imm, $zero, 0x100 # Load x coordinate of A into $t0
lw $t1, $imm, $zero, 0x101 # Load y coordinate of A into $t1
lw $t2, $imm, $zero, 0x102 # Load x coordinate of B into $t2
lw $t3, $imm, $zero, 0x103 # Load y coordinate of B into $t3
lw $t4, $imm, $zero, 0x104 # Load x coordinate of C into $t4
lw $t5, $imm, $zero, 0x105 # Load y coordinate of C into $t5

# Calculate slope m and intercept b for line AC
sub $t6, $t1, $t5         # $t6 = yA - yC
sub $t7, $t0, $t4         # $t7 = xA - xC
div $s0, $t6, $t7         # $s0 = m = (yA - yC) / (xA - xC)
mul $t8, $s0, $t0         # $t8 = m * xA
sub $s1, $t1, $t8         # $s1 = b = yA - m * xA

# Outer loop: Iterate over y from yA to yB
add $t9, $t1, $zero       # Initialize $t9 with yA
outer_loop:
    blt $t9, $t3, end_outer_loop # If y < yB, exit the outer loop

    # Inner loop: Iterate over x from xA to xC
    add $s2, $t0, $zero   # Initialize $s2 with xA
    inner_loop:
        beq $s2, $t4, end_inner_loop # If x > xC, exit the inner loop
        
        # Calculate y value on line AC at this x
        mul $t10, $s0, $s2 # $t10 = m * x
        add $t11, $t10, $s1 # $t11 = y_line = m * x + b
        
        # Check if current y is below or on the line AC
        bgt $t9, $t11, skip_pixel # If y > y_line, skip coloring this pixel
        
        # Set pixel color to white and draw it
        out $zero, $zero, $imm, $s2, $t9 # Select pixel at (x, y)
        out $zero, $zero, $imm, $imm, 21, 255 # Set pixel color to white
        out $zero, $zero, $imm, $zero, 20, 0 # Update address
        out $zero, $zero, $imm, $imm, 22, 1 # Draw pixel
        
        skip_pixel:
        addi $s2, $s2, 1 # Increment x by 1
        j inner_loop     # Jump to inner_loop for the next x value
        
    end_inner_loop:
    subi $t9, $t9, 1    # Decrement y by 1
    j outer_loop        # Jump to outer_loop for the next y value

end_outer_loop:
halt                    # Halt the program
