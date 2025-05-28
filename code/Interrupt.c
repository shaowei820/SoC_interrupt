#include <stdio.h>
#include "platform.h"

#include "xparameters.h"  // 包含裝置ID定義
#include "xgpio.h"        // GPIO 控制
#include "xscugic.h"      // GIC 控制

#define Z700_INTC_ID      XPAR_PS7_SCUGIC_0_DEVICE_ID
#define BTN_ID            XPAR_AXI_GPIO_1_DEVICE_ID
#define LED_ID            XPAR_AXI_GPIO_0_DEVICE_ID
#define INTC_GPIO_ID      XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR
#define BTN_INT           XGPIO_IR_CH1_MASK
#define GIC_ID            XPAR_SCUGIC_SINGLE_DEVICE_ID

XGpio LED, BTN;
XScuGic INTCInst;

// 旗標與狀態變數
volatile int btn_flag = 0;
int LED_num = 0;
int Intr_CTN = 0;

// ============================
// 延遲函式（簡單 Busy-Wait）
void delay(int ms) {
    for (int i = 0; i < ms * 10000; i++) {
        asm("nop");
    }
}

// ============================
// 中斷服務函式（僅設定旗標）
void Intr_Handler() {
    XGpio_InterruptDisable(&BTN, BTN_INT);
    XGpio_InterruptClear(&BTN, BTN_INT);

    btn_flag = 1;
    Intr_CTN++;

    XGpio_InterruptEnable(&BTN, BTN_INT);
}

// ============================
// 中斷初始化設定
void Intr_Setup(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr)
{
    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(GIC_ID);
    XScuGic_CfgInitialize(GicInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                  (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                  GicInstancePtr);
    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

    XScuGic_Connect(GicInstancePtr,
                    INTC_GPIO_ID,
                    (Xil_ExceptionHandler)Intr_Handler,
                    (void *)GpioInstancePtr);
    XScuGic_Enable(GicInstancePtr, INTC_GPIO_ID);

    XGpio_InterruptGlobalEnable(GpioInstancePtr);
    XGpio_InterruptEnable(GpioInstancePtr, BTN_INT);
}

// ============================
// 主程式
int main()
{
    init_platform();  // 初始化 UART 與標準 I/O

    XGpio_Initialize(&LED, LED_ID);
    XGpio_SetDataDirection(&LED, 1, 0);  // CH1 為輸出

    XGpio_Initialize(&BTN, BTN_ID);
    XGpio_SetDataDirection(&BTN, 1, 1);  // CH1 為輸入

    Intr_Setup(&INTCInst, &BTN);  // 中斷初始化
    print("Init successful\n");

    while (1) {
        // LED 狀態更新
        XGpio_DiscreteWrite(&LED, 1, LED_num);
        printf("LED_num = %x\n", LED_num);
        LED_num++;
        delay(500);

        // 中斷事件處理
        if (btn_flag) {
            btn_flag = 0;
            printf("Interrupt : %d\n", Intr_CTN);
            LED_num = 0;
            delay(2000);
        }
    }

    return 0;
}
