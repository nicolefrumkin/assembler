# This program sums the contents of sectors 0 to 7 and stores the result in sector 8

# Registers:
# $t0 - sector index
# $t1 - word index
# $t2 - temporary storage for sum
# $t3 - base address for disk buffer
# $a0 - disk command register
# $a1 - disk sector register
# $a2 - disk buffer register
# $v0 - result register

start:
    add $t0, $zero, $zero, 0        # $t0 = 0 (sector index)
    add $t1, $zero, $zero, 0        # $t1 = 0 (word index)
    add $t2, $zero, $zero, 0        # $t2 = 0 (sum)
    add $t3, $zero, $zero, 512      # $t3 = 0x200 (base address of disk buffer)

# Read sectors 0 to 7
read_sectors:
    add $a0, $zero, $zero, 1        # $a0 = 1 (read sector command)
    sw $t0, $zero, 15               # disksector = $t0 (sector index)
    sw $t3, $zero, 16               # diskbuffer = $t3 (base address of disk buffer)
    sw $a0, $zero, 14               # diskcmd = $a0 (read command)

wait_read:
    lw $v0, $zero, 17               # $v0 = diskstatus
    beq $v0, $zero, wait_read       # wait until diskstatus is 0

# Sum contents of current sector
sum_sector:
    lw $v0, $t3, $t1                # load word from disk buffer
    add $t2, $t2, $v0, 0            # sum += word
    add $t1, $t1, $imm, 1           # $t1++
    blt $t1, $imm, 128, sum_sector  # if word index < 128, repeat

    add $t0, $t0, $imm, 1           # $t0++ (next sector)
    ble $t0, $imm, 7, read_sectors  # if sector index <= 7, repeat

# Write sum to sector 8
write_sector:
    add $a0, $zero, $zero, 2        # $a0 = 2 (write sector command)
    sw $t2, $t3, $zero              # store sum in disk buffer
    sw $t0, $zero, 15               # disksector = 8 (sector index)
    sw $t3, $zero, 16               # diskbuffer = $t3 (base address of disk buffer)
    sw $a0, $zero, 14               # diskcmd = $a0 (write command)

wait_write:
    lw $v0, $zero, 17               # $v0 = diskstatus
    beq $v0, $zero, wait_write      # wait until diskstatus is 0

end:
    halt $zero, $zero, $zero, 0     # halt execution
