#include <stdbool.h>
#include <stdio.h>


#define NREGS 16
#define STACK_SIZE 1024
#define SP 13
#define LR 14
#define PC 15

int add(int a, int b);
int sub(int a, int b);
int mov(int a, int b);
int str(int a,int b);
int b(int a,int b);
struct arm_state {
    unsigned int regs[NREGS];
    unsigned int cpsr;
    unsigned char stack[STACK_SIZE];
};

void init_arm_state(struct arm_state *as, unsigned int *func,
                   unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    int i;

    /* zero out all arm state */
    for (i = 0; i < NREGS; i++) {
        as->regs[i] = 0;
    }

    as->cpsr = 0;

    for (i = 0; i < STACK_SIZE; i++) {
        as->stack[i] = 0;
    }

    as->regs[PC] = (unsigned int) func;
    as->regs[SP] = (unsigned int) &as->stack[STACK_SIZE];
    as->regs[LR] = 0;

    as->regs[0] = arg0;
    as->regs[1] = arg1;
    as->regs[2] = arg2;
    as->regs[3] = arg3;
}

bool is_add_inst(unsigned int iw)
{
    unsigned int op;
    unsigned int opcode;

    op = (iw >> 26) & 0b11;
    opcode = (iw >> 21) & 0b1111;

    return (op == 0) && (opcode == 0b0100);
}

void armemu_add(struct arm_state *state)
{
    unsigned int iw;
    unsigned int rd, rn, rm;

    iw = *((unsigned int *) state->regs[PC]);
    
    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;
    rm = iw & 0xF;

    state->regs[rd] = state->regs[rn] + state->regs[rm];
    if (rd != PC) {
        state->regs[PC] = state->regs[PC] + 4;
    }
}

bool is_str_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int lbit;

  op = (iw >> 26) & 0b11;
  lbit = (iw >> 20) & 0b1;

  return (op == 01) && (lbit == 0b0);
}

void armemu_str(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  rm = iw & 0xF;

  state->regs[rn] = state->regs[rd]; 
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_ldr_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int lbit;

  op = (iw >> 26) & 0b11;
  lbit = (iw >> 20) & 0b1;

  return (op == 01) && (lbit == 0b1);
}

void armemu_ldr(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  rm = iw & 0xF;

  state->regs[rd]=state->regs[rn];
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_sub_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int opcode;

  op = (iw >> 26) & 0b11;
  opcode = (iw >> 21) & 0b1111;

  return (op == 0) && (opcode == 0b0010);
}

void armemu_sub(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  rm = iw & 0xF;

  state->regs[rd] = state->regs[rn] - state->regs[rm];
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_mov_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int opcode;

  op = (iw >> 26) & 0b11;
  opcode = (iw >> 21) & 0b1111;

  return (op == 0) && (opcode == 0b1101);
}

void armemu_mov(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  rm = iw & 0xF;

  state->regs[rd] = state->regs[rm];
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_b_inst(unsigned int iw)
{
  unsigned int b_code;

  b_code = (iw >> 27) & 0b111;

  return (b_code==0b101);
}

void armemu_b(struct arm_state *state)
{
  printf("going here");
  unsigned int offset_check,offset_address,offset_check_bit;
  offset_check = *((unsigned int *) state->regs[PC]);
  offset_check = (offset_check>>24) & 0xFFFFFF;
  offset_check = offset_check<<2;
  offset_check_bit = (offset_check>>23) & 0b1;
    if(offset_check_bit==1){
      offset_address = 0xFF000000|offset_check;
    }
    else {
      offset_address = 0x00000000|offset_check;
    }
    printf("\n %d is the offset address ",offset_address);
    state->regs[PC]=state->regs[PC]+8+offset_address;  
}

bool is_bx_inst(unsigned int iw)
{
    unsigned int bx_code;

    bx_code = (iw >> 4) & 0x00FFFFFF;

    return (bx_code == 0b000100101111111111110001);
}

void armemu_bx(struct arm_state *state)
{
    unsigned int iw;
    unsigned int rn;

    iw = *((unsigned int *) state->regs[PC]);
    rn = iw & 0b1111;

    state->regs[PC] = state->regs[rn];
}

void armemu_one(struct arm_state *state)
{
    unsigned int iw;
    
    iw = *((unsigned int *) state->regs[PC]);

    if (is_bx_inst(iw)) {
        armemu_bx(state);
    } else if (is_add_inst(iw)) {
        armemu_add(state);
    }
    else if (is_sub_inst(iw)){
      armemu_sub(state);
    }
    else if (is_mov_inst(iw)){
      armemu_mov(state);
     }
    else if(is_str_inst(iw)){
      armemu_str(state);
    }
    else if(is_ldr_inst(iw)){
      armemu_ldr(state);
    }
    else if(is_b_inst(iw)){
      printf("\ngoing here");
      armemu_b(state);
    }
    else {
      printf("\n Instruction not found");
    }
}


unsigned int armemu(struct arm_state *state)
{

    while (state->regs[PC] != 0) {
        armemu_one(state);
    }

    return state->regs[0];
}
                  
    
int main(int argc, char **argv)
{
    struct arm_state state;
    unsigned int r;
    
    //init_arm_state(&state, (unsigned int *) add, 1, 2, 0, 0);
    //init_arm_state(&state, (unsigned int *) sub, 2, 1, 0, 0);
    //init_arm_state(&state, (unsigned int *) mov, 2, 1, 0, 0);
    //init_arm_state(&state, (unsigned int *) str, 1, 4, 0, 0);  
    init_arm_state(&state, (unsigned int *) b, 1, 1, 0, 0);  
    r = armemu(&state);
    printf("r = %d\n", r);
  
    return 0;
}
