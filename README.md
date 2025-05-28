# SoC_HW2：ZYNQ LED 中斷控制設計  
本專案實作一個基於 ZYNQ 的 SoC 系統，透過 AXI GPIO 控制 LED 與接收 Switch 輸入，並使用中斷方式控制 LED 顯示行為。系統可部署於 ZedBoard，觀察按鍵觸發中斷與 LED 狀態變化。

 系統架構

## 元件：

- `axi_gpio_0`：連接 8-bit LED 輸出 (`leds_8bits`)
- `axi_gpio_1`：連接 8-bit 開關輸入 (`sws_8bits`) 並啟用中斷 (`ip2intc_irpt`)
- `processing_system7_0`：ZYNQ PS，處理 AXI 通訊與中斷
- `ps7_0_axi_periph`：AXI Interconnect，連接多個 AXI 裝置
- `rst_ps7_0_50M`：處理系統 Reset
- `design_1_wrapper.v`：HDL Wrapper（由 Block Design 自動生成）

## 功能說明

- LED 以 0.5 秒頻率自動遞增（模擬流水燈）
- 任何 Switch 變化都可觸發中斷
- 中斷發生時：
  - 停止流水燈
  - 將 LED 歸零
  - 延遲 2 秒
  - 繼續流水燈動作
- UART 輸出目前 LED 數值與中斷次數


## 專案結構  


```
SoC_HW2/
├── design_1.bd             # Block Design 原檔
├── design_1_wrapper.v      # 自動產生的 RTL Wrapper
├── helloworld.c            # 中斷處理與 GPIO 控制 C 程式
├── zedboard.xdc            # ZedBoard XDC 約束檔案
├── SOC_HW2.PNG             # Block Design 圖片
├── README.md               # 本文件
```
## 軟體 說明
- 使用 `XGpio` 控制 GPIO 輸入/輸出方向
- 使用 `XScuGic` 管理中斷控制器
- 定義 `Intr_Handler()` 中斷服務程序，包含：
  - 執行 debounce 延遲
  - 清除並重新啟用中斷
  - 將 LED 歸零
  - 中斷次數加一
- 主迴圈控制 LED 流水燈行為

