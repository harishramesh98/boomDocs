#include <stdio.h>
#include <stdint.h>
#include<math.h>
#include <stdlib.h>
#include "hpm-util.h"

//test case function declarations
void testCase1();
void testCase2();


int main(int argc, char **argv){
	if(argc>1){
		switch(atoi(argv[1])){
			case 1: testCase1();
				break;
			case 2: testCase2();
				break;
			default: printf("test case undefined\n");
				break;
		}
	}
	else{
		printf("Check arguments\n");
	}
return 0;
}

void testCase1(void){
	printf("Testing cycle and instr reg value changes\n");
	uint64_t start_cycles = HPM_READ(cycle);
	uint64_t start_instr = HPM_READ(instret);
	printf("cycle(%lu)\n", start_cycles);
    	printf("instr(%lu)\n", start_instr);
    	
    	int a, b;
    	float c;
    	printf("Enter side A:\n");
    	scanf("%d",&a);
    	printf("enter side B:\n");
    	scanf("%d",&b);
    	c=sqrt(a*a+b*b);
    	printf("Hypotenuse is %f units", c);
    	
    	uint64_t end_cycles = HPM_READ(cycle);
	uint64_t end_instr = HPM_READ(instret);
	printf("cycle(%lu)\n", end_cycles);
    	printf("instr(%lu)\n", end_instr);
    	
    	uint64_t diff_cycles = end_cycles-start_cycles;
    	uint64_t diff_instr = end_instr-start_cycles;
    	printf("difference in cycles = %lu\n",diff_cycles);
    	printf("difference in cycles = %lu\n",diff_instr);
return;
}

void testCase2(void){
    printf("Testing all, with removed scounteren stuff\n");
    
    // setup hpm events
    // can only use mhpmevent* here
    HPM_SETUP_EVENTS(mhpmevent3, 0, 1);
    HPM_SETUP_EVENTS(mhpmevent4, 0, 2);
    HPM_SETUP_EVENTS(mhpmevent5, 0, 4);

    printf("started!\n");

    // read hpms
    // can use either mhpm... or hpm... here depending on 
    // if you enable or user mode / supervisor mode reading
    printf("cycle(%lu)\n", HPM_READ(cycle));
    printf("instr(%lu)\n", HPM_READ(instret));

    // note: have to "zero" them out since they are not zero to begin with
    uint64_t start_hpm3 = HPM_READ(mhpmcounter3);
    uint64_t start_hpm4 = HPM_READ(mhpmcounter4);
    uint64_t start_hpm5 = HPM_READ(mhpmcounter5);
    printf("stalling\n");
    uint64_t end_hpm3 = HPM_READ(mhpmcounter3);
    uint64_t end_hpm4 = HPM_READ(mhpmcounter4);
    uint64_t end_hpm5 = HPM_READ(mhpmcounter5);
    printf("%lu = %lu - %lu\n", end_hpm3 - start_hpm3, end_hpm3, start_hpm3);
    printf("%lu = %lu - %lu\n", end_hpm4 - start_hpm4, end_hpm4, start_hpm4);
    printf("%lu = %lu - %lu\n", end_hpm5 - start_hpm5, end_hpm5, start_hpm5);
return;
}

