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
int sum_array(int a,int b);
int sum_array_real(int *a,int b);
int find_max(int *a,int b);
int find_str(char *s1,char *s2);
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
  unsigned int iw,iwimmediate;
    unsigned int rd, rn, rm;

    iw = *((unsigned int *) state->regs[PC]);
    iwimmediate = *((unsigned int *) state->regs[PC]);
    rd = (iw >> 12) & 0xF;
    rn = (iw >> 16) & 0xF;
    iwimmediate = (iwimmediate>>25) & 0b1;
    if(iwimmediate==0){
    rm = iw & 0xF;
    state->regs[rd] = state->regs[rn] + state->regs[rm];
    } else {
      rm = iw & 0xFF;
      state->regs[rd] = state->regs[rn]+rm;
    }
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

  state->regs[rn] = &state->regs[rd]; 
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
  unsigned int iw,iwimmediate;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);
  iwimmediate = *((unsigned int *) state->regs[PC]);
  iwimmediate = (iwimmediate>>22)&0b1;
  
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(iwimmediate==0) {
  rm = iw & 0xF;
   state->regs[rd]= *((unsigned int *) state->regs[rn]);
  }else {
    rm = iw & 0xFF;
    state->regs[rd] = *((char *)state->regs[rn]+rm);
  }
   //  printf("%u is r%d\n",state->regs[rn],rn);
  //state->regs[rd] = state->regs[rn];
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
  unsigned int iw,iwimmediate;
  unsigned int rd, rn, rm;

  iw = *((unsigned int *) state->regs[PC]);
  iwimmediate = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  iwimmediate = (iwimmediate>>25)&0b1;
  if(iwimmediate==0){
    rm = iw & 0xF;
    state->regs[rd] = state->regs[rn] - state->regs[rm];
  } else{
    rm = iw & 0xFF;
    state->regs[rd] = state->regs[rn] - rm;
  }

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
  unsigned int iw,iwimmediate;
  unsigned int rd, rn, rm;
  iw = *((unsigned int *) state->regs[PC]);
  iwimmediate = *((unsigned int *) state->regs[PC]);
  
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  iwimmediate = (iwimmediate>>25)&0b1;
  if(iwimmediate==0){
  rm = iw & 0xF;
  state->regs[rd] = state->regs[rm];
  } else {
    rm = iw & 0xFF;
    state->regs[rd] = rm;
  }
  
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_movle(struct arm_state *state)
{
  unsigned int iw,iw_nzcv;
  unsigned int rd, rn, rm;
  iw = *((unsigned int *) state->regs[PC]);
  iw_nzcv = state->cpsr;
  printf("\nvalue of iw_nzcv is %d\n",iw_nzcv);
  iw_nzcv = (iw_nzcv >> 31) & 0b1;
  printf("\nvalue of iw_nzcv after rotate is %d\n",iw_nzcv);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(iw_nzcv==1){
    printf("\nGoes inside nzcv=1 ov movle\n");
    armemu_mov(state);
  }
  else{
    printf("goes to else, inc PC\n");
    state->regs[PC] = state->regs[PC] + 4;
  }
}


void armemu_movgt(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rd, rn, rm;
  int iw_n,iw_z;
  iw = *((unsigned int *) state->regs[PC]);
  iw_n = state->cpsr;
  iw_z = state->cpsr;
  //printf("\nvalue of iw_nzcv in movgt function is %d\n",iw_nzcv);
  iw_n = (iw_n >> 31) & 0b1;
  iw_z = (iw_z >> 30) & 0b1;
  printf("\nvalue of iw_n=%d and iw_z = %d after rotate is \n",iw_n,iw_z);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(iw_n==0&&iw_z==0){
    printf("\nGoes inside nzcv=0of movgt\n");
    armemu_mov(state);
  }
  else{
    printf("goes to else movgt, inc PC\n");
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_cmp_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int opcode;

  op = (iw >> 26) & 0b11;
  opcode = (iw >> 21) & 0b1111;

  return (op == 0) && (opcode == 0b1010);
}

void armemu_cmp(struct arm_state *state)
{
  unsigned int iw,iwimmediate;
  unsigned int rd, rn, rm;
  int result;

  iw = *((unsigned int *) state->regs[PC]);
  iwimmediate = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;

  iwimmediate = (iwimmediate>>25)&0b1;
  
  if(iwimmediate==0){ 
  rm = iw & 0xF;
  result= state->regs[rn] - state->regs[rm];
  }else{
    rm = iw & 0xFF;
    result= state->regs[rn] - rm;
  }
  if(result==0){
    printf("\nresult is 0 == %d?",result);
    state->cpsr = state->cpsr|0x40000000;
  } if(result<0){
    printf("\nresult is <0 == %d?",result);
    state->cpsr = state->cpsr|0x80000000;
  }
  if(result>0) {
    printf("\nresult is >0 == %d?",result);
    state->cpsr = state->cpsr & 0x00000000;
  }
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

bool is_b_inst(unsigned int iw)
{
  unsigned int b_code,condition_code,temp;
  temp = iw;
  condition_code = (temp>>28) & 0b1111;
  //printf("%d is the condition code",condition_code);

  b_code = (iw >> 25) & 0b111;

  return (b_code==0b101);
}

bool is_beq_inst(unsigned int iw)
{
  //printf("\n goes inside this fuckall function");
  unsigned int beq_code;

  beq_code = (iw >> 28) & 0b1111;
  return (beq_code==0b0000);
}

void armemu_beq(struct arm_state *state)
{
  //printf("\ngoes inside the armemu_beq function");
  unsigned int iw,z_check_bit;
  unsigned int rd, rn, rm,result;

  iw = *((unsigned int *) state->regs[PC]);
  z_check_bit = state->cpsr;
  z_check_bit = (z_check_bit>>30) & 0b1;
  if(z_check_bit==1){
    armemu_b(state);
  }
  else {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_b(struct arm_state *state)
{
  //printf("\ngoing inside armemu_b function");
  unsigned int offset_check,offset_address,offset_check_bit;
  offset_check = *((unsigned int *) state->regs[PC]);
  offset_check = offset_check & 0x00FFFFFF;
  offset_check_bit = (offset_check>>23) & 0b1;
    if(offset_check_bit==1){
      offset_address = 0xFF000000|offset_check;
    }
    else {
      offset_address = 0x00000000|offset_check;
    }
    offset_address = (offset_address<<2);
    //printf("\n %x is the offset address ",offset_address);
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

bool is_le_inst(unsigned int iw)
{
  printf("\n Goes to is_le\n");
  unsigned int le_code;

  le_code = (iw >> 28) & 0b1111;
  return (le_code==0b1101);
}

bool is_gt_inst(unsigned int iw)
{
  printf("\n Goes to is_gt\n");
  unsigned int gt_code;

  gt_code = (iw >> 28) & 0b1111;
  return (gt_code==0b1100);
}

void armemu_one(struct arm_state *state)
{
  unsigned int iw,check_code,cc2,cc3,cc4;
    iw = *((unsigned int *) state->regs[PC]);
    check_code = *((unsigned int *) state->regs[PC]);
    cc2 = *((unsigned int *) state->regs[PC]);
    cc3 = *((unsigned int *) state->regs[PC]);
    if (is_bx_inst(iw)) {
        armemu_bx(state);
    } else if (is_add_inst(iw)) {
        armemu_add(state);
    }
    else if (is_sub_inst(iw)){
      armemu_sub(state);
    }
    else if (is_mov_inst(iw)){
      if(is_le_inst(cc2)){
	armemu_movle(state);
      }else if(is_gt_inst(cc3)){
	armemu_movgt(state);
      } else{
      armemu_mov(state);
      }
     }
    else if(is_str_inst(iw)){
      armemu_str(state);
    }
    else if(is_ldr_inst(iw)){
      armemu_ldr(state);
    }
    else if(is_cmp_inst(iw)){
      armemu_cmp(state);
    }
    else if(is_b_inst(iw)){
      //printf("\nis going in the arm_one statement");
      if(is_beq_inst(check_code)){
	//printf("\ngoes to arm_one beq");
	armemu_beq(state);
      }else{
	//printf("\ngoes to arm_one b");
      armemu_b(state);
      }
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
    int arr[] = {-5,-3,-4};
    //init_arm_state(&state, (unsigned int *) add, 1, 2, 0, 0);
    //init_arm_state(&state, (unsigned int *) sub, 2, 1, 0, 0);
    // init_arm_state(&state, (unsigned int *) str, 0, 5, 0, 0);
    //init_arm_state(&state, (unsigned int *) sum_array_real, arr, 5, 0, 0);  
    //init_arm_state(&state, (unsigned int *) b, 0, 3, 0, 0);


    /*
    init_arm_state(&state, (unsigned int *) find_max, arr, 3, 0, 0); 
    r = armemu(&state);
    printf("r = %d\n", r);
    */

    char s[]="abc";
    char ss[]="bc";

    char snf[]="fff";
    int n;
    n=find_str(s,snf);
    printf("\nAssembly");
    printf("\nfor string %s and the substring %s, the index found was %d",s,ss,n);
    
  
    return 0;
}
