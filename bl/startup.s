/**
 * @file  startup.s
 * @brief STM32F303ZETx device vector table for GCC toolchain.
 */

.syntax unified
.global Default_Handler
.global Reset_Handler
.global Vector_Table

/* start address for the initialization values of the .data section. */
.word _sidata
/* start address for the .data section. */
.word _sdata
/* end address for the .data section. */
.word _edata
/* start address for the .bss section. */
.word _sbss
/* end address for the .bss section. */
.word _ebss

.section .text
.type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0 /* set stack pointer */

/* copy the data segment initializers from flash to SRAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

/* zero fill the bss segment. */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

/* Call the application's entry point.*/
    bl main

/* gets called when the processor receives an unexpected interrupt */
Default_Handler:
LoopForever:
    b LoopForever

/* the STM32F303ZETx vector table. */
.section .isr_vector,"a",%progbits
Vector_Table:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler               /* Window Watchdog interrupt */
    .word PVD_IRQHandler                /* PVD through EXTI line detection interrupt */
    .word TAMP_STAMP_IRQHandler         /* Tamper and TimeStamp interrupts */
    .word RTC_WKUP_IRQHandler           /* RTC Wakeup interrupt through the EXTI line */
    .word FLASH_IRQHandler              /* Flash global interrupt */
    .word RCC_IRQHandler                /* RCC global interrupt */
    .word EXTI0_IRQHandler              /* EXTI Line0 interrupt */
    .word EXTI1_IRQHandler              /* EXTI Line3 interrupt */
    .word EXTI2_TSC_IRQHandler          /* EXTI Line2 and Touch sensing interrupts */
    .word EXTI3_IRQHandler              /* EXTI Line3 interrupt */
    .word EXTI4_IRQHandler              /* EXTI Line4 interrupt */
    .word DMA1_CH1_IRQHandler           /* DMA1 channel 1 interrupt */
    .word DMA1_CH2_IRQHandler           /* DMA1 channel 2 interrupt */
    .word DMA1_CH3_IRQHandler           /* DMA1 channel 3 interrupt */
    .word DMA1_CH4_IRQHandler           /* DMA1 channel 4 interrupt */
    .word DMA1_CH5_IRQHandler           /* DMA1 channel 5 interrupt */
    .word DMA1_CH6_IRQHandler           /* DMA1 channel 6 interrupt */
    .word DMA1_CH7_IRQHandler           /* DMA1 channel 7interrupt */
    .word ADC1_2_IRQHandler             /* ADC1 and ADC2 global interrupt */
    .word USB_HP_CAN_TX_IRQHandler      /* USB High Priority/CAN_TX interrupts */
    .word USB_LP_CAN_RX0_IRQHandler     /* USB Low Priority/CAN_RX0 interrupts */
    .word CAN_RX1_IRQHandler            /* CAN_RX1 interrupt */
    .word CAN_SCE_IRQHandler            /* CAN_SCE interrupt */
    .word EXTI9_5_IRQHandler            /* EXTI Line5 to Line9 interrupts */
    .word TIM1_BRK_TIM15_IRQHandler     /* TIM1 Break/TIM15 global interruts */
    .word TIM1_UP_TIM16_IRQHandler      /* TIM1 Update/TIM16 global interrupts */
    .word TIM1_TRG_COM_TIM17_IRQHandler /* TIM1 trigger and commutation/TIM17 interrupts */
    .word TIM1_CC_IRQHandler            /* TIM1 capture compare interrupt */
    .word TIM2_IRQHandler               /* TIM2 global interrupt */
    .word TIM3_IRQHandler               /* TIM3 global interrupt */
    .word TIM4_IRQHandler               /* TIM4 global interrupt */
    .word I2C1_EV_EXTI23_IRQHandler     /* I2C1 event interrupt and EXTI Line23 interrupt */
    .word I2C1_ER_IRQHandler            /* I2C1 error interrupt */
    .word I2C2_EV_EXTI24_IRQHandler     /* I2C2 event interrupt & EXTI Line24 interrupt */
    .word I2C2_ER_IRQHandler            /* I2C2 error interrupt */
    .word SPI1_IRQHandler               /* SPI1 global interrupt */
    .word SPI2_IRQHandler               /* SPI2 global interrupt */
    .word USART1_EXTI25_IRQHandler      /* USART1 global interrupt and EXTI Line 25 interrupt */
    .word USART2_EXTI26_IRQHandler      /* USART2 global interrupt and EXTI Line 26 interrupt */
    .word USART3_EXTI28_IRQHandler      /* USART3 global interrupt and EXTI Line 28 interrupt */
    .word EXTI15_10_IRQHandler          /* EXTI Line15 to Line10 interrupts */
    .word RTCAlarm_IRQHandler           /* RTC alarm interrupt */
    .word USB_WKUP_IRQHandler           /* USB wakeup from Suspend */
    .word TIM8_BRK_IRQHandler           /* TIM8 break interrupt */
    .word TIM8_UP_IRQHandler            /* TIM8 update interrupt */
    .word TIM8_TRG_COM_IRQHandler       /* TIM8 Trigger and commutation interrupts */
    .word TIM8_CC_IRQHandler            /* TIM8 capture compare interrupt */
    .word ADC3_IRQHandler               /* ADC3 global interrupt */
    .word FMC_IRQHandler                /* FSMC global interrupt */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word SPI3_IRQHandler               /* SPI3 global interrupt */
    .word UART4_EXTI34_IRQHandler       /* UART4 global and EXTI Line 34 interrupts */
    .word UART5_EXTI35_IRQHandler       /* UART5 global and EXTI Line 35 interrupts */
    .word TIM6_DACUNDER_IRQHandler      /* TIM6 global and DAC12 underrun interrupts */
    .word TIM7_IRQHandler               /* TIM7 global interrupt */
    .word DMA2_CH1_IRQHandler           /* DMA2 channel1 global interrupt */
    .word DMA2_CH2_IRQHandler           /* DMA2 channel2 global interrupt */
    .word DMA2_CH3_IRQHandler           /* DMA2 channel3 global interrupt */
    .word DMA2_CH4_IRQHandler           /* DMA2 channel4 global interrupt */
    .word DMA2_CH5_IRQHandler           /* DMA2 channel5 global interrupt */
    .word ADC4_IRQHandler               /* ADC4 global interrupt */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word COMP123_IRQHandler            /* COMP1 & COMP2 & COMP3 interrupts combined with EXTI Lines 21, 22 and 29 interrupts */
    .word COMP456_IRQHandler            /* COMP4 & COMP5 & COMP6 interrupts combined with EXTI Lines 30, 31 and 32 interrupts */
    .word COMP7_IRQHandler              /* COMP7 interrupt combined with EXTI Line 33 interrupt */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word I2C3_EV_IRQHandler            /* I2C3 Event interrupt */
    .word I2C3_ER_IRQHandler            /* I2C3 Error interrupt */
    .word USB_HP_IRQHandler             /* USB High priority interrupt */
    .word USB_LP_IRQHandler             /* USB Low priority interrupt */
    .word USB_WKUP_EXTI_IRQHandler      /* USB wakeup from Suspend and EXTI Line 18 */
    .word TIM20_BRK_IRQHandler          /* TIM20 Break interrupt */
    .word TIM20_UP_IRQHandler           /* TIM20 Upgrade interrupt */
    .word TIM20_TRG_COM_IRQHandler      /* TIM20 Trigger and Commutation interrupt */
    .word TIM20_CC_IRQHandler           /* TIM20 Capture Compare interrupt */
    .word FPU_IRQHandler                /* Floating point interrupt */
    .word 0                             /* Reserved */
    .word 0                             /* Reserved */
    .word SPI4_IRQHandler               /* SPI4 Global interrupt */

/* provide weak aliases for each Exception handler to the Default_Handler. */
    .weak NMI_Handler
    .thumb_set NMI_Handler,Default_Handler

    .weak HardFault_Handler
    .thumb_set HardFault_Handler,Default_Handler

    .weak MemManage_Handler
    .thumb_set MemManage_Handler,Default_Handler

    .weak BusFault_Handler
    .thumb_set BusFault_Handler,Default_Handler

    .weak UsageFault_Handler
    .thumb_set UsageFault_Handler,Default_Handler

    .weak SVC_Handler
    .thumb_set SVC_Handler,Default_Handler

    .weak DebugMon_Handler
    .thumb_set DebugMon_Handler,Default_Handler

    .weak PendSV_Handler
    .thumb_set PendSV_Handler,Default_Handler

    .weak SysTick_Handler
    .thumb_set SysTick_Handler,Default_Handler

    .weak WWDG_IRQHandler
    .thumb_set WWDG_IRQHandler,Default_Handler

    .weak PVD_IRQHandler
    .thumb_set PVD_IRQHandler,Default_Handler

    .weak TAMP_STAMP_IRQHandler
    .thumb_set TAMP_STAMP_IRQHandler,Default_Handler

    .weak RTC_WKUP_IRQHandler
    .thumb_set RTC_WKUP_IRQHandler,Default_Handler

    .weak FLASH_IRQHandler
    .thumb_set FLASH_IRQHandler,Default_Handler

    .weak RCC_IRQHandler
    .thumb_set RCC_IRQHandler,Default_Handler

    .weak EXTI0_IRQHandler
    .thumb_set EXTI0_IRQHandler,Default_Handler

    .weak EXTI1_IRQHandler
    .thumb_set EXTI1_IRQHandler,Default_Handler

    .weak EXTI2_TSC_IRQHandler
    .thumb_set EXTI2_TSC_IRQHandler,Default_Handler

    .weak EXTI3_IRQHandler
    .thumb_set EXTI3_IRQHandler,Default_Handler

    .weak EXTI4_IRQHandler
    .thumb_set EXTI4_IRQHandler,Default_Handler

    .weak DMA1_CH1_IRQHandler
    .thumb_set DMA1_CH1_IRQHandler,Default_Handler

    .weak DMA1_CH2_IRQHandler
    .thumb_set DMA1_CH2_IRQHandler,Default_Handler

    .weak DMA1_CH3_IRQHandler
    .thumb_set DMA1_CH3_IRQHandler,Default_Handler

    .weak DMA1_CH4_IRQHandler
    .thumb_set DMA1_CH4_IRQHandler,Default_Handler

    .weak DMA1_CH5_IRQHandler
    .thumb_set DMA1_CH5_IRQHandler,Default_Handler

    .weak DMA1_CH6_IRQHandler
    .thumb_set DMA1_CH6_IRQHandler,Default_Handler

    .weak DMA1_CH7_IRQHandler
    .thumb_set DMA1_CH7_IRQHandler,Default_Handler

    .weak ADC1_2_IRQHandler
    .thumb_set ADC1_2_IRQHandler,Default_Handler

    .weak USB_HP_CAN_TX_IRQHandler
    .thumb_set USB_HP_CAN_TX_IRQHandler,Default_Handler

    .weak USB_LP_CAN_RX0_IRQHandler
    .thumb_set USB_LP_CAN_RX0_IRQHandler,Default_Handler

    .weak CAN_RX1_IRQHandler
    .thumb_set CAN_RX1_IRQHandler,Default_Handler

    .weak CAN_SCE_IRQHandler
    .thumb_set CAN_SCE_IRQHandler,Default_Handler

    .weak EXTI9_5_IRQHandler
    .thumb_set EXTI9_5_IRQHandler,Default_Handler

    .weak TIM1_BRK_TIM15_IRQHandler
    .thumb_set TIM1_BRK_TIM15_IRQHandler,Default_Handler

    .weak TIM1_UP_TIM16_IRQHandler
    .thumb_set TIM1_UP_TIM16_IRQHandler,Default_Handler

    .weak TIM1_TRG_COM_TIM17_IRQHandler
    .thumb_set TIM1_TRG_COM_TIM17_IRQHandler,Default_Handler

    .weak TIM1_CC_IRQHandler
    .thumb_set TIM1_CC_IRQHandler,Default_Handler

    .weak TIM2_IRQHandler
    .thumb_set TIM2_IRQHandler,Default_Handler

    .weak TIM3_IRQHandler
    .thumb_set TIM3_IRQHandler,Default_Handler

    .weak TIM4_IRQHandler
    .thumb_set TIM4_IRQHandler,Default_Handler

    .weak I2C1_EV_EXTI23_IRQHandler
    .thumb_set I2C1_EV_EXTI23_IRQHandler,Default_Handler

    .weak I2C1_ER_IRQHandler
    .thumb_set I2C1_ER_IRQHandler,Default_Handler

    .weak I2C2_EV_EXTI24_IRQHandler
    .thumb_set I2C2_EV_EXTI24_IRQHandler,Default_Handler

    .weak I2C2_ER_IRQHandler
    .thumb_set I2C2_ER_IRQHandler,Default_Handler

    .weak SPI1_IRQHandler
    .thumb_set SPI1_IRQHandler,Default_Handler

    .weak SPI2_IRQHandler
    .thumb_set SPI2_IRQHandler,Default_Handler

    .weak USART1_EXTI25_IRQHandler
    .thumb_set USART1_EXTI25_IRQHandler,Default_Handler

    .weak USART2_EXTI26_IRQHandler
    .thumb_set USART2_EXTI26_IRQHandler,Default_Handler

    .weak USART3_EXTI28_IRQHandler
    .thumb_set USART3_EXTI28_IRQHandler,Default_Handler

    .weak EXTI15_10_IRQHandler
    .thumb_set EXTI15_10_IRQHandler,Default_Handler

    .weak RTCAlarm_IRQHandler
    .thumb_set RTCAlarm_IRQHandler,Default_Handler

    .weak USB_WKUP_IRQHandler
    .thumb_set USB_WKUP_IRQHandler,Default_Handler

    .weak TIM8_BRK_IRQHandler
    .thumb_set TIM8_BRK_IRQHandler,Default_Handler

    .weak TIM8_UP_IRQHandler
    .thumb_set TIM8_UP_IRQHandler,Default_Handler

    .weak TIM8_TRG_COM_IRQHandler
    .thumb_set TIM8_TRG_COM_IRQHandler,Default_Handler

    .weak TIM8_CC_IRQHandler
    .thumb_set TIM8_CC_IRQHandler,Default_Handler

    .weak ADC3_IRQHandler
    .thumb_set ADC3_IRQHandler,Default_Handler

    .weak FMC_IRQHandler
    .thumb_set FMC_IRQHandler,Default_Handler

    .weak SPI3_IRQHandler
    .thumb_set SPI3_IRQHandler,Default_Handler

    .weak UART4_EXTI34_IRQHandler
    .thumb_set UART4_EXTI34_IRQHandler,Default_Handler

    .weak UART5_EXTI35_IRQHandler
    .thumb_set UART5_EXTI35_IRQHandler,Default_Handler

    .weak TIM6_DACUNDER_IRQHandler
    .thumb_set TIM6_DACUNDER_IRQHandler,Default_Handler

    .weak TIM7_IRQHandler
    .thumb_set TIM7_IRQHandler,Default_Handler

    .weak DMA2_CH1_IRQHandler
    .thumb_set DMA2_CH1_IRQHandler,Default_Handler

    .weak DMA2_CH2_IRQHandler
    .thumb_set DMA2_CH2_IRQHandler,Default_Handler

    .weak DMA2_CH3_IRQHandler
    .thumb_set DMA2_CH3_IRQHandler,Default_Handler

    .weak DMA2_CH4_IRQHandler
    .thumb_set DMA2_CH4_IRQHandler,Default_Handler

    .weak DMA2_CH5_IRQHandler
    .thumb_set DMA2_CH5_IRQHandler,Default_Handler

    .weak ADC4_IRQHandler
    .thumb_set ADC4_IRQHandler,Default_Handler

    .weak COMP123_IRQHandler
    .thumb_set COMP123_IRQHandler,Default_Handler

    .weak COMP456_IRQHandler
    .thumb_set COMP456_IRQHandler,Default_Handler

    .weak COMP7_IRQHandler
    .thumb_set COMP7_IRQHandler,Default_Handler

    .weak I2C3_EV_IRQHandler
    .thumb_set I2C3_EV_IRQHandler,Default_Handler

    .weak I2C3_ER_IRQHandler
    .thumb_set I2C3_ER_IRQHandler,Default_Handler

    .weak USB_HP_IRQHandler
    .thumb_set USB_HP_IRQHandler,Default_Handler

    .weak USB_LP_IRQHandler
    .thumb_set USB_LP_IRQHandler,Default_Handler

    .weak USB_WKUP_EXTI_IRQHandler
    .thumb_set USB_WKUP_EXTI_IRQHandler,Default_Handler

    .weak TIM20_BRK_IRQHandler
    .thumb_set TIM20_BRK_IRQHandler,Default_Handler

    .weak TIM20_UP_IRQHandler
    .thumb_set TIM20_UP_IRQHandler,Default_Handler

    .weak TIM20_TRG_COM_IRQHandler
    .thumb_set TIM20_TRG_COM_IRQHandler,Default_Handler

    .weak TIM20_CC_IRQHandler
    .thumb_set TIM20_CC_IRQHandler,Default_Handler

    .weak FPU_IRQHandler
    .thumb_set FPU_IRQHandler,Default_Handler

    .weak SPI4_IRQHandler
    .thumb_set SPI4_IRQHandler,Default_Handler