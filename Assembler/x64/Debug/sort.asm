# This program sorts 16 numbers stored at memory locations 0x100 to 0x10F

# Registers:
# $t0 - index i
# $t1 - index j
# $t2 - temporary storage for swap
# $a0 - base address of the array (0x100)

start:
    add $a0, $zero, $zero, 256   # $a0 = 0x100 (base address of array)
    add $t0, $zero, $zero, 15    # $t0 = 15 (length of array - 1)

outer_loop:
    add $t1, $zero, $zero, 0     # $t1 = 0 (initialize inner loop index)
inner_loop:
    lw $v0, $a0, $t1             # load array[j] into $v0
    lw $t2, $a0, $t1, 1          # load array[j+1] into $t2
    ble $v0, $t2, skip_swap      # if array[j] <= array[j+1] skip the swap

    # Swap array[j] and array[j+1]
    sw $t2, $a0, $t1             # store array[j+1] into array[j]
    sw $v0, $a0, $t1, 1          # store array[j] into array[j+1]

skip_swap:
    add $t1, $t1, $imm, 1        # j++
    blt $t1, $t0, inner_loop     # if j < n - i - 1, repeat inner loop

    sub $t0, $t0, $imm, 1        # i--
    bge $t0, $zero, outer_loop   # if i >= 0, repeat outer loop

end:
    halt $zero, $zero, $zero, 0  # halt execution
