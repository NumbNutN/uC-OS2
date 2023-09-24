#include "ucos_ii.h"
#include "os_cpu.h"

#include "main.h"


/* 
SystickInterupt计数器
在SystickIrq 陷入100次/s时，计数器可计数497天
*/
extern "C"{
INT32U _count_systick = 0;
}


/**
 * @brief Set a memory for BASEPRI Value Stored when Context Switch is performing
*/
OS_STK CPUSR_CTX;

/**
 * @brief Invoked in the process to create a new task
*/
OS_STK* OSTaskStkInit(void (*task)(void* pd),void *pdata,OS_STK* ptos,INT16U opt)
{
    /******************************************************************/
    /*      Notice Cortex-M4 behaviour when handling a exception     */
    /*      PC xPSR R0 R1 R2 R3 R12 LR will sequently push into stack*/
    /*****************************************************************/
    OS_STK *p_stk = ptos;
    p_stk = (OS_STK *)((uint32_t)p_stk & 0xFFFFFFF8);
    --p_stk;
    *p_stk = 0x01000000;        //xPSR
    *(--p_stk)=(OS_STK)task;      //PC
    *(--p_stk)=(OS_STK)OS_TaskReturn;      //LR
    *(--p_stk)=0x0;      //R12
    *(--p_stk)=0x0;      //R3
    *(--p_stk)=0x0;      //R2
    *(--p_stk)=0x0;      //R1
    *(--p_stk)=(OS_STK)pdata;      //R0
    /******************************************************************/
    /*      The registers store by software                          */
    /*      Probably should follow the sequence from high index to low ones*/
    /*      Cause an atom instruction stmfd can handle this with high efficency*/
    /*****************************************************************/
    *(--p_stk)=0xFFFFFFFD;      //EXC_RETURN
    *(--p_stk)=0x0;      //R11
    *(--p_stk)=0x0;      //R10
    *(--p_stk)=0x0;      //R9
    *(--p_stk)=0x0;      //R8
    *(--p_stk)=0x0;      //R7
    *(--p_stk)=0x0;      //R6
    *(--p_stk)=0x0;      //R5
    *(--p_stk)=0x0;      //R4
    
    
    return p_stk;
}

/**
 * @brief sysTick handler,we needn't write it in assembly language cause we handle all the registers preserve work in OSIntExit()
 *        As sysTick is a excecption with really high priority, we needn't create a critical area when OSIntEnter().
*/
void OSTickISR(void)
{
    OSIntEnter();
    ++_count_systick;
    OSTimeTick();
    OSIntExit();
}


void          OSInitHookBegin         (void)
{

}
void          OSInitHookEnd           (void)
{
    //配置 PENDSV优先级
    (SCB->SHP)[10] = 0xE0;

    //配置 BASEPRI初始值
    //set_basepri(0x0 << 4);
}

void          OSTaskCreateHook        (OS_TCB          *ptcb)
{

}
void          OSTaskDelHook           (OS_TCB          *ptcb)
{

}

void          OSTaskIdleHook          (void)
{

}

void          OSTaskReturnHook        (OS_TCB          *ptcb)
{

}

void          OSTaskStatHook          (void)
{

}

#if OS_TASK_SW_HOOK_EN > 0u
void          OSTaskSwHook            (void)
{

}
#endif

void          OSTCBInitHook           (OS_TCB          *ptcb)
{

}

#if OS_TIME_TICK_HOOK_EN > 0u
void          OSTimeTickHook          (void)
{
    
}
#endif