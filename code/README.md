# ZYNQ GPIO 中斷控制程式（使用中斷旗標）

本程式實作 ZYNQ SoC 平台上的中斷驅動 LED 控制，透過 AXI GPIO 接收外部開關輸入（Switch），並透過 GIC 中斷控制器觸發中斷事件，由 Processing System 控制 LED 顯示。


## 功能說明

- LED 流水燈效果：
  - 每 0.5 秒遞增一次 LED 顯示數值 (`LED_num`)
- 中斷觸發機制：
  - 使用 AXI GPIO 連接 Switch (`sws_8bits`)
  - 任一 Switch 變化即觸發中斷
- 中斷事件處理邏輯：
  - 由中斷服務程式設定 `btn_flag`
  - 主程式檢查 `btn_flag` 為 1 時：
    - 歸零 LED 顯示
    - 延遲 2 秒
    - 繼續執行 LED 流水燈
    - 顯示中斷次數 `Intr_CTN` 至 UART



## 程式架構

### main()
- 初始化 GPIO LED 與 Switch
- 設定中斷控制器
- 主迴圈：
  - 持續顯示遞增的 LED 數值
  - 檢查中斷旗標並處理事件

### Intr_Handler()
- 禁用與清除中斷
- 設定 `btn_flag = 1`
- 中斷次數加一
- 重新啟用中斷

### Intr_Setup()
- 註冊並啟用 GIC 中斷控制器
- 設定中斷對應的處理函式（ISR）

### delay(ms)
- 使用 `nop` 實作簡易延遲（非精確）


## 測試方式

1. 於 Vivado 建立 Block Design 並匯出硬體與 bitstream
2. 於 Vitis 載入 `Interrupt.c` 並 Build 應用程式
3. 將應用程式燒錄至 ZedBoard
4. 觀察執行行為：

   | 動作 | 結果 |
   |------|------|
   | 每 0.5 秒 | LED 依序遞增 |
   | 按下任一 Switch | LED 歸零並暫停 2 秒，UART 顯示中斷次數 |


## 使用元件與 API

| 模組 | 說明 |
|------|------|
| `XGpio` | 控制 AXI GPIO 輸入/輸出方向與資料傳輸 |
| `XScuGic` | 控制 ZYNQ 的 GIC 中斷控制器 |
| `xparameters.h` | 自動產生的裝置 ID 定義 |
| `platform.h` | 初始化平台（包含 UART 輸出） |



