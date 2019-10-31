.mult64 {
  UMULL r2, r3, r0, r1
  MOV r0, r3 LSR #28
  ORR r0, r0, r2 LSL #4
}
