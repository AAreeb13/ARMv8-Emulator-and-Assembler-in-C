// to align buffer
nop
nop
nop

// address of buffer
movz x0 , #144
add x0, x0, #0x80, lsl #12 

// addresss of mailbox read reg
movz x1, #0x3f00, lsl #16
movk x1, #0xb880

// address of mailbox read status reg
add x10, x1, #0x18

// address of mailbox write reg
add x11, x1, #0x20

// address of mailbox write status reg
add x12, x1, #0x38

while:
  // on/off?
  ldr w7, [x0, #0x18]
  
  // F flag clear?
  ldr w2, [x12]
  cmp wzr, w2, lsr #31
  b.ne while 
  
  // create and send request
  sub x3, x3, x3 
  add w3, w3, w0, lsl #4  
  add w3, w3, #8 
  str w3, [x11]
  
  // E flag clear?
  E_flag:
    ldr w4, [x10]
    add w4, wzr, w4, lsl #1
    cmp wzr, w4, lsr #31
    b.ne E_flag
  
  // read response
  ldr w5, [x1] 
  
  delay:
    sub x20, x20, x20
    add x20, x20, #100
    subs x20, x20, #1
    b.ne delay
    
  // restore 130 to offset 0x14 from buffer
  add x25, xzr, #130
  str w25, [x0, #0x14]
  
  // check on/off
  cmp w7, wzr
  b.eq set_on
  
  // set off
  str wzr, [x0, #0x18]
  b while
  
  set_on:
  add w30, wzr, #0x1
  str w30, [x0, #0x18]
  b while

request_buffer:
  .int 32
  .int 0
  .int 0x00038041
  .int 8
  .int 0x0
  .int 130
  .int 0x1
  .int 0x0
