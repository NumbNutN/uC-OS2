/*
**********************************************************************************
*                                  DATA TYPES
*                             (Compiler Specific)
**********************************************************************************
*/
  
typedef unsigned char  BOOLEAN;                                               
typedef unsigned char  INT8U;       /* Unsigned  8 bit quantity           */
typedef signed   char  INT8S;       /* Signed    8 bit quantity           */
typedef unsigned short   INT16U;      /* Unsigned 16 bit quantity           */
typedef signed   short   INT16S;      /* Signed   16 bit quantity           */
typedef unsigned long  INT32U;      /* Unsigned 32 bit quantity           */
typedef signed   long  INT32S;      /* Signed   32 bit quantity           */
typedef float          FP32;        /* Single precision floating point    */  
typedef double         FP64;        /* Double precision floating point    */
  
typedef INT32U   OS_STK;      /* Each stack entry is 16-bit wide    */  
typedef INT32U OS_CPU_SR;   /* Define size of CPU status register */  

/*
*********************************************************************************
*                                 LGD Added
*********************************************************************************
*/

#define CRITICAL_INT_MASK_PRIO 1
#define INLINE_INT_MASK_PRIO "#1"

extern INT32U _count_systick;

INT32U get_basepri();

void set_basepri(INT32U cpu_sr);

INT32U CPU_SR_STORE(INT32U cpu_sr);

void OSTickISR(void);

/*
*********************************************************************************
*                             Processor Specifics
*********************************************************************************
*/
#define  OS_CRITICAL_METHOD   3                                             
  
#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()  \
        asm("MOV R0,"INLINE_INT_MASK_PRIO"\n\t"\
            "LSL R0, R0,#4\n\t"\
            "MSR BASEPRI, R0")

#define  OS_EXIT_CRITICAL()   \
        asm("MOV R0,#0xF0\n\t"\
            "MSR BASEPRI,R0")
#endif
  
#if      OS_CRITICAL_METHOD == 2
#define  OS_ENTER_CRITICAL()  ????                                            
#define  OS_EXIT_CRITICAL()   ????
#endif
  
#if      OS_CRITICAL_METHOD == 3 
#define  OS_ENTER_CRITICAL()  \
            do{                 \
                cpu_sr = CPU_SR_STORE(CRITICAL_INT_MASK_PRIO<<4);\
            }while(0)

#define  OS_EXIT_CRITICAL()   \
            do{CPU_SR_STORE(cpu_sr);}while(0)
#endif
  
  
#define  OS_STK_GROWTH        1          /* Stack growth (0=Up, 1=Down) */    
  
#define  OS_TASK_SW()   OSCtxSw()