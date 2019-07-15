.data
.text
move $fp $sp
j main
add:
lw $s7 0($sp)
subi $sp $sp 8
sw $s7 0($sp)
addi $sp $sp 12
move $s7 $fp
move $fp $sp
subi $fp $fp 4
sw $s7 0($fp)
sw $ra -4($fp)
subi $sp $sp 20
lw $t0 -8($fp)
li $t1 5
add $t0 $t0 $t1
sw $t0 -16($fp)
lw $t0 -16($fp)
sw $t0 -12($fp)
lw $t0 -12($fp)
move $v0 $t0
lw $ra -4($fp)
addi $sp $fp 4
lw $fp 0($fp)
jr $ra
main:
move $s7 $fp
move $fp $sp
subi $fp $fp 4
sw $s7 0($fp)
sw $ra -4($fp)
subi $sp $sp 20
li $t0 4
li $t1 5
add $t0 $t0 $t1
sw $t0 -12($fp)
lw $t0 -12($fp)
subi $sp $sp 4
sw $t0 0($sp)
jal add
move $t0 $v0
sw $t0 -16($fp)
lw $t0 -16($fp)
sw $t0 -8($fp)
lw $t0 -8($fp)
move $a0 $t0
li $v0 1
syscall
li $v0 10
syscall
