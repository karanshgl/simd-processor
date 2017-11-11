.text
mov r0,7 @size (N)

CMP r0,0
BGT .do
B .ifail1

.do:
sub r11,r0,1 @N-1
%mov r1,0x100 @A[][]
mov r1, .input
%mov r2, 0x200 @B[][]
mov r2, .input2
mov r3, 0x300 @C[][]

mov r4,0 @i
mov r5,0 @j

.store :
mul r6,r4,r0 @r6=i*N
add r6,r6,r5 @r6=(i*N)+j
mul r6,r6,4 @r6 = ((i*N)+j)*4

add r7,r4,r5 @r7=i+j
mul r8,r4,2 @2i
mul r9,r5,3 @3j
add r8,r8,r9 @r8=2i+3j

add r9,r1,r6 @r9=0x100+offset
add r10,r2,r6 @r10=0x200+offset

st r7, 0[r9]
st r8,0[r10]

cmp r5,r11
BEQ .jfail
add r5,r5,1
B .store

.jfail : mov r5,0
cmp r4,R11
BEQ .ifail
add r4,r4,1
B .store

.ifail:
mov r4,0
mov r5,0

@Calculating C
.cal :
mul r6,r4,r0 @r6=i*N
add r6,r6,r5 @r6=(i*N)+j
mul r6,r6,4 @r6 = ((i*N)+j)*4

@r0=N, r1=A, R2=B, R3=C, r4=i, r5=j, r6=offset, r9=C+offset,
@r10=A[i][k], r12=B[k][j], r13=k , r8=product

add r9,r3,r6

mov r13,0 @k=0
mov r8,0

.calone:
mul r6,r4,r0 @r6=i*N
add r6,r6,r13 @r6=(i*N)+k
mul r6,r6,4 @r6 = ((i*N)+k)*4

add r7,r1,r6 @r7=A+offset
ld r10, 0[r7] @r10=A[i][]k

mul r6,r13,r0 @r6=k*N
add r6,r6,r5 @r6=(k*N)+j
mul r6,r6,4 @r6 = ((i*N)+j)*4

add r7,r2,r6 @r7= B+offset
ld r12, 0[r7] @r11=B[k][j]

mul r10,r10,r12 @r10=A[i][k]*B[k][j]
add r8,r8,r10

cmp r13,r11
BEQ .kfail1
add r13,r13,1
B .calone

.kfail1 :
.print r8
st r8, 0[r9]
cmp r5,r11
BEQ .jfail1
add r5,r5,1
B .cal

.jfail1 : mov r5,0
cmp r4,r11
BEQ .ifail1
add r4,r4,1
B .cal

.ifail1:

.end

.data

.input:
765
0x8675
86542
.input2:
875
0
-875
