#include <stdbool.h>
#include <stdio.h>
#include<sys/times.h>
#include<time.h>
#include<unistd.h>

#define NREGS 16
#define STACK_SIZE 1024
#define SP 13
#define LR 14
#define PC 15
#define ITERS 1000
/*
int instruction_count = 0;
int data_processing_instruction_count = 0;
int branch_instruction_count = 0;
int memory_instruction_count = 0;
int branch_taken = 0;
int branch_not_taken = 0;
*/	     
int add(int a, int b);
int sub(int a, int b);
int mov(int a, int b);
int str(int a,int b);
int b(int a,int b);
int sum_array(int a,int b);
int sum_array_real(int *a,int b);
int find_max(int *a,int b);
int find_str(char *s1,char *s2);
int fib_recur(int a);
int test1(int a);
fib_iter(int a);
struct arm_state {
    unsigned int regs[NREGS];
    unsigned int cpsr;
    unsigned char stack[STACK_SIZE];
  int instruction_count;
  int data_processing_instruction_count;
  int branch_instruction_count;
  int memory_instruction_count;
  int branch_taken;
  int branch_not_taken;
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

    as->instruction_count = 0;
    as->data_processing_instruction_count = 0;
    as->branch_instruction_count = 0;
    as->memory_instruction_count = 0;
    as->branch_taken = 0;
    as->branch_not_taken = 0;
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
 state-> data_processing_instruction_count++;
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

void armemu_str_second(struct arm_state *state)
{
  state->memory_instruction_count++;
  unsigned int iw,iwimmediate,immediate,addr;
  unsigned int rd, rn, rm;
 int *ptr;
  iw = *((unsigned int *) state->regs[PC]);
  immediate = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  
  //  state->regs[rn] = &state->regs[rd];

  *((unsigned int *)state->regs[rn]) = state->regs[rd];
  //  state->regs[rn]= &state->regs[rd];
  
  if (rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}



void armemu_str(struct arm_state *state)
{
  unsigned int iw,iwimmediate,*addr,immediate,offset_addr;
  unsigned int rd, rn, rm;
  //unsigned int addr = state->regs[rn]+rm
  //unsigned int *ptr = (unsigned int*)addr;
 //*ptr = state->regs[rd]
  unsigned int *ptr;
  iw = *((unsigned int *) state->regs[PC]);
  immediate = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  //check if I bit is 0
  iwimmediate = *((unsigned int *) state->regs[PC]);
  iwimmediate = (iwimmediate>>25)&0b1;
  if(iwimmediate==0){
    //addr = (unsigned int *)state->regs[rn];
    immediate = iw & 0xFFF; //get the offset
    /*
    offset_addr = (unsigned int)addr+immediate;
    *((unsigned int *)offset_addr)=state->regs[rd];
*/
    *((unsigned int *)state->regs[rn]+immediate) = state->regs[rd];
  }else{
    //printf("\n goes to else of STR!!!!!!!!!!");
    rm = iw & 0xF;    
    //addr = (unsigned int *)state->regs[rn];
    //offset_addr= (unsigned int)addr;
    //*((unsigned int *)offset_addr)=state->regs[rd];
    //*addr = state->regs[rd];
      //*((unsigned int *)state->regs[rn]) = state->regs[rd];
    state->regs[rn] = (unsigned int) &state->regs[rd];
  }
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
  state->memory_instruction_count++;
  unsigned int iw,iwimmediate,b_or_word,immediate,offset_addr;
  unsigned int rd, rn, rm;
  //unsigned int *addr;

  iw = *((unsigned int *) state->regs[PC]);

  b_or_word = *((unsigned int *) state->regs[PC]);
  b_or_word = (b_or_word>>22)&0b1;
  
  iwimmediate = *((unsigned int *) state->regs[PC]);
  iwimmediate = (iwimmediate>>25)&0b1;
  immediate = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(b_or_word==0) {
    state->regs[rd]= *((unsigned int *) state->regs[rn]);
    // printf("%u is r%d\n",state->regs[rd],rd);  
    //state->regs[rd] = state->regs[rn];
  }else {
    rm = iw & 0xFF;
    state->regs[rd] = *((char *)state->regs[rn]+rm);
  }
  //     printf("%u is r%d\n",state->regs[rn],rn);
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
  state->data_processing_instruction_count++;
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
  state->data_processing_instruction_count++;
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
  state->data_processing_instruction_count++;
  unsigned int iw,iw_nzcv;
  unsigned int rd, rn, rm;
  iw = *((unsigned int *) state->regs[PC]);
  iw_nzcv = state->cpsr;
  iw_nzcv = (iw_nzcv >> 31) & 0b1;
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(iw_nzcv==1){
    armemu_mov(state);
  }
  else{
    state->regs[PC] = state->regs[PC] + 4;
  }
}


void armemu_movgt(struct arm_state *state)
{
  state->data_processing_instruction_count++;
  unsigned int iw;
  unsigned int rd, rn, rm;
  int iw_n,iw_z;
  iw = *((unsigned int *) state->regs[PC]);
  iw_n = state->cpsr;
  iw_z = state->cpsr;
  iw_n = (iw_n >> 31) & 0b1;
  iw_z = (iw_z >> 30) & 0b1;
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  if(iw_n==0&&iw_z==0){
    armemu_mov(state);
  }
  else{
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
  state->data_processing_instruction_count++;
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
    state->cpsr = state->cpsr|0x40000000;
  } if(result<0){
    state->cpsr = state->cpsr|0x80000000;
  }
  if(result>0) {
    state->cpsr = state->cpsr | 0x00000000;
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
  state->branch_instruction_count++;
  //printf("\ngoes inside the armemu_beq function");
  unsigned int iw,z_check_bit;
  unsigned int rd, rn, rm,result;

  iw = *((unsigned int *) state->regs[PC]);
  z_check_bit = state->cpsr;
  z_check_bit = (z_check_bit>>30) & 0b1;
  if(z_check_bit==1){
    state->branch_taken++;
    armemu_b(state);
  }
  else {
    state->branch_not_taken++;
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_blt(struct arm_state *state)
{
  state->branch_instruction_count++;
  //printf("\ngoes inside the armemu_beq function");
  unsigned int iw,n_check_bit,v_check_bit;
  unsigned int rd, rn, rm,result;

  iw = *((unsigned int *) state->regs[PC]);
  n_check_bit = state->cpsr;
  v_check_bit = state->cpsr;
  n_check_bit = (n_check_bit>>31) & 0b1;
  v_check_bit = (n_check_bit>>28) & 0b1;
  if(n_check_bit!=v_check_bit){
    state->branch_taken++;
    armemu_b(state);
  }
  else {
    state->branch_not_taken++;
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_b(struct arm_state *state)
{
  //printf("\ngoing inside armemu_b function");
  unsigned int offset_check,offset_address,offset_check_bit,link_bit;
  link_bit = *((unsigned int *) state->regs[PC]);
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
    link_bit = (link_bit>>24) & 0b1;
    if(link_bit==1){
      state->regs[LR] = state->regs[PC]+4;
    }
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
  state->branch_taken++;
    unsigned int iw;
    unsigned int rn;

    iw = *((unsigned int *) state->regs[PC]);
    rn = iw & 0b1111;

    state->regs[PC] = state->regs[rn];
}

bool is_le_inst(unsigned int iw)
{
  unsigned int le_code;

  le_code = (iw >> 28) & 0b1111;
  return (le_code==0b1101);
}

bool is_gt_inst(unsigned int iw)
{
  unsigned int gt_code;

  gt_code = (iw >> 28) & 0b1111;
  return (gt_code==0b1100);
}

bool is_lt_inst(unsigned int iw)
{
  unsigned int lt_code;

  lt_code = (iw >> 28) & 0b1111;
  return (lt_code==0b1011);
}

void armemu_one(struct arm_state *state)
{
  unsigned int iw,check_code,cc2,cc3,cc4;
    iw = *((unsigned int *) state->regs[PC]);
    check_code = *((unsigned int *) state->regs[PC]);
    cc2 = *((unsigned int *) state->regs[PC]);
    cc3 = *((unsigned int *) state->regs[PC]);
    cc4 = *((unsigned int *) state->regs[PC]);
    if (is_bx_inst(iw)) {
        armemu_bx(state);
	state->instruction_count++;
	state->branch_instruction_count++;
    } else if (is_add_inst(iw)) {
        armemu_add(state);
	state->instruction_count++;
    }
    else if (is_sub_inst(iw)){
      armemu_sub(state);
      state->instruction_count++;
    }
    else if (is_mov_inst(iw)){
      if(is_le_inst(cc2)){
	armemu_movle(state);
	state->instruction_count++;
      }else if(is_gt_inst(cc3)){
	armemu_movgt(state);
	state->instruction_count++;
      } else{
      armemu_mov(state);
      state->instruction_count++;
      }
     }
    else if(is_str_inst(iw)){
      armemu_str_second(state);
      state->instruction_count++;
    }
    else if(is_ldr_inst(iw)){
      armemu_ldr(state);
      state->instruction_count++;
    }
    else if(is_cmp_inst(iw)){
      state->cpsr = 0;
      armemu_cmp(state);
      state->instruction_count++;
    }
    else if(is_b_inst(iw)){
      //printf("\nis going in the arm_one statement");
      if(is_beq_inst(check_code)){
	//printf("\ngoes to arm_one beq");
	armemu_beq(state);
	state->instruction_count++;
      } else if(is_lt_inst(check_code)){
	armemu_blt(state);
	state->instruction_count++;
      } else{
	//printf("\ngoes to arm_one b");
      armemu_b(state);
      state->instruction_count++;
      state->branch_instruction_count++;
      state->branch_taken++;
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

double get_time(struct tms *t1, struct tms *t2)
{
  int ticks_per_second;
  double total_secs = 0.0;
  ticks_per_second = sysconf(_SC_CLK_TCK);
  total_secs = ((double) (t2->tms_utime - t1->tms_utime)) / ((double) ticks_per_second);
  return total_secs;
}

void print_report(struct arm_state *state)
{
  printf("Total instructions executed = %d\n",state->instruction_count);
  printf("Data Processing instructions executed = %d\n",state->data_processing_instruction_count);
  printf("Memory instructions executed = %d\n",state->memory_instruction_count );
  printf("Branch instructions executed = %d\n",state->branch_instruction_count);
  printf("branch not taken = %d\n",state->branch_not_taken);
  printf("branch taken = %d\n",state->branch_taken);
}
                  
    
int main(int argc, char **argv)
{
    struct arm_state state;
    unsigned int r;
    struct tms t1,t2;
    int i;
    double total_secs = 0.0;
    int arr[] = {-5,-3,-4};
    int arr2[] = {1,1,1};
    int a0[5]={0,0,0,0,0};
    int mixed[3] = {-5,4,5};
    int arrthousand[1000];
    int len = 1000;
    for(i=0;i<len;i++){
      arrthousand[i]=1;
    }
    //init_arm_state(&state, (unsigned int *) add, 1, 2, 0, 0);
    //init_arm_state(&state, (unsigned int *) sub, 2, 1, 0, 0);
    // init_arm_state(&state, (unsigned int *) str, 0, 5, 0, 0);
    //init_arm_state(&state, (unsigned int *) sum_array_real, arr2, 3, 0, 0);  
    //init_arm_state(&state, (unsigned int *) b, 0, 3, 0, 0);
    int num = 4;
    // init_arm_state(&state, (unsigned int *) fib_recur, 8, 0, 0, 0);   
     //init_arm_state(&state, (unsigned int *) find_max, arr, 3, 0, 0);

    //array with 3 elements
    times(&t1);
    for(i = 0;i<ITERS;i++){
      r = sum_array_real(arr2, 3);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<ITERS;i++){
    init_arm_state(&state, (unsigned int *) sum_array_real, arr2, 3, 0, 0);
    r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu = %lf\n", total_secs);
    print_report(&state);

    //array with 0 elements
    times(&t1);
    for(i = 0;i<ITERS;i++){
      r = sum_array_real(a0, 5);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<ITERS;i++){
      init_arm_state(&state, (unsigned int *) sum_array_real, a0, 5, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu = %lf\n", total_secs);
    print_report(&state);

    //array with mixed elements
    times(&t1);
    for(i = 0;i<ITERS;i++){
      r = sum_array_real(arr2, 3);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<ITERS;i++){
      init_arm_state(&state, (unsigned int *) sum_array_real, mixed, 3, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu = %lf\n", total_secs);
    print_report(&state);

    //array with thousand elements
    times(&t1);
    for(i = 0;i<10;i++){
      r = sum_array_real(arrthousand, 1000);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<10;i++){
     init_arm_state(&state, (unsigned int *) sum_array_real, arrthousand, 1000, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu = %lf\n", total_secs);
    print_report(&state);


    //FIB_RECUR REPORT

    times(&t1);
    for(i = 0;i<100;i++){
      r = fib_recur(10);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native fib_recur = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<100;i++){
      init_arm_state(&state, (unsigned int *) fib_recur, 10, 0, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu fib_recur = %lf\n", total_secs);
    print_report(&state);

    times(&t1);
    for(i = 0;i<100;i++){
      r = fib_recur(19);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native fib_recur = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<100;i++){
      init_arm_state(&state, (unsigned int *) fib_recur, 19, 0, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu fib_recur = %lf\n", total_secs);
    print_report(&state);

    // FIB ITER
    times(&t1);
    for(i = 0;i<100;i++){
      r = fib_iter(11);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native fib_recur = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<100;i++){
      init_arm_state(&state, (unsigned int *) fib_iter, 11, 0, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu fib_recur = %lf\n", total_secs);
    print_report(&state);

    // fib iter 20
    times(&t1);
    for(i = 0;i<100;i++){
      r = fib_recur(20);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native fib_iter = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<100;i++){
      init_arm_state(&state, (unsigned int *) fib_iter, 20, 0, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu fib_iter = %lf\n", total_secs);
    print_report(&state);

    // FIND_MAX

    times(&t1);
    for(i = 0;i<ITERS;i++){
      r = find_max(arr, 3);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("total_secs for native = %lf\n", total_secs);
    total_secs = 0.0;
    times(&t1);
    for(i = 0;i<ITERS;i++){
      init_arm_state(&state, (unsigned int *) find_max, arr, 3, 0, 0);
      r = armemu(&state);
    }
    times(&t2);
    total_secs = get_time(&t1,&t2);
    printf("r = %d\n", r);
    printf("total_secs for armemu = %lf\n", total_secs);
    print_report(&state);

    //find Max for thousand elements
    
    
    /*
    init_arm_state(&state, (unsigned int *) find_max, arr, 3, 0, 0);
    r = armemu(&state);
    printf("r = %d\n", r);
    */
   
    
    /*    
    char s[]="abc";
    char ss[]="bc";

    char snf[]="fff";
    int n;
    n=find_str(s,ss);
    printf("\nAssembly");
    printf("\nfor string %s and the substring %s, the index found was %d",s,ss,n);
    */
    return 0;
}
