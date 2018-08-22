#ifndef REGS_RCC_H
#define REGS_RCC_H

#include "register.h"

class RCC {
public:
  class CR : public Register32 {
  public:
    REGS_BOOL_FIELD_R(PLLI2SRDY, 27);
    REGS_BOOL_FIELD(PLLI2SON, 26);
    REGS_BOOL_FIELD(PLLRDY, 25);
    REGS_BOOL_FIELD(PLLON, 24);
    REGS_BOOL_FIELD(CSSON, 19);
    REGS_BOOL_FIELD(HSEBYP, 18);
    REGS_BOOL_FIELD_R(HSERDY, 17);
    REGS_BOOL_FIELD(HSEON, 16);
    REGS_BOOL_FIELD_R(HSIRDY, 1);
    REGS_BOOL_FIELD(HSION, 0);
  };

  class PLLCFGR : public Register32 {
  public:
    REGS_FIELD(PLLM, uint8_t, 5, 0);
    REGS_FIELD(PLLN, uint16_t, 14, 6);
    REGS_FIELD(PLLP, uint8_t, 17, 16);
    enum class PLLSRC {
      HSI = 0,
      HSE = 1
    };
    void setPLLSRC(PLLSRC s) volatile { setBitRange(22, 22, (uint8_t)s); }
    REGS_FIELD(PLLQ, uint8_t, 27, 24);
    REGS_FIELD(PLLR, uint8_t, 30, 28);
  };

  class CFGR : public Register32 {
  public:
    enum class SW {
      HSI = 0,
      HSE = 1,
      PLL = 2
    };
    void setSW(SW s) volatile { setBitRange(1, 0, (uint8_t)s); }
    SW getSWS() volatile { return (SW)getBitRange(3,2); }
    enum class AHBPrescaler {
      SysClk = 0,
      SysClkDividedBy2 = 8,
      SysClkDividedBy4 = 9,
      SysClkDividedBy8 = 10,
      SysClkDividedBy16 = 11,
      SysClkDividedBy64 = 12,
      SysClkDividedBy128 = 13,
      SysClkDividedBy256 = 14,
      SysClkDividedBy512 = 15
    };
    void setHPRE(AHBPrescaler p) volatile { setBitRange(7, 4, (uint32_t)p); }
    enum class APBPrescaler{
      AHB = 0,
      AHBDividedBy2 = 4,
      AHBDividedBy4 = 5,
      AHBDividedBy8 = 6,
      AHBDividedBy16 = 7
    };
    void setPPRE2(APBPrescaler r) volatile { setBitRange(15, 13, (uint32_t)r); }
    void setPPRE1(APBPrescaler r) volatile { setBitRange(12, 10, (uint32_t)r); }
    REGS_FIELD(RTCPRE, uint8_t, 4, 0);
  };

  class AHB1ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(GPIOAEN, 0);
    REGS_BOOL_FIELD(GPIOBEN, 1);
    REGS_BOOL_FIELD(GPIOCEN, 2);
    REGS_BOOL_FIELD(GPIODEN, 3);
    REGS_BOOL_FIELD(GPIOEEN, 4);
    REGS_BOOL_FIELD(GPIOFEN, 5);
    REGS_BOOL_FIELD(GPIOGEN, 6);
    REGS_BOOL_FIELD(GPIOHEN, 7);
    REGS_BOOL_FIELD(CRCEN, 12);
    REGS_BOOL_FIELD(DMA1EN, 21);
    REGS_BOOL_FIELD(DMA2EN, 22);
  };

  class AHB2ENR : Register32 {
  public:
    REGS_BOOL_FIELD(RNGEN, 6);
    REGS_BOOL_FIELD(OTGFSEN, 7);
  };

  class AHB3ENR : Register32 {
  public:
    REGS_BOOL_FIELD(FSMCEN, 0);
    REGS_BOOL_FIELD(QSPIEN, 1);
  };

  class APB1ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM2EN, 0);
    REGS_BOOL_FIELD(TIM3EN, 1);
    REGS_BOOL_FIELD(TIM4EN, 2);
    REGS_BOOL_FIELD(TIM5EN, 3);
    REGS_BOOL_FIELD(TIM6EN, 4);
    REGS_BOOL_FIELD(TIM7EN, 5);
    REGS_BOOL_FIELD(TIM12EN, 6);
    REGS_BOOL_FIELD(TIM13EN, 7);
    REGS_BOOL_FIELD(TIM14EN, 8);
    REGS_BOOL_FIELD(RTCAPB, 10);
    REGS_BOOL_FIELD(SPI3EN, 15);
    REGS_BOOL_FIELD(USART3EN, 18);
    REGS_BOOL_FIELD(PWREN, 28);
  };

  class APB2ENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM1EN, 0);
    REGS_BOOL_FIELD(TIM8EN, 1);
    REGS_BOOL_FIELD(USART1EN, 4);
    REGS_BOOL_FIELD(ADC1EN, 8);
    REGS_BOOL_FIELD(SDIOEN, 11);
    REGS_BOOL_FIELD(SPI1EN, 12);
    REGS_BOOL_FIELD(SYSCFGEN, 14);
    REGS_BOOL_FIELD(TIM9EN, 16);
    REGS_BOOL_FIELD(TIM10EN, 17);
    REGS_BOOL_FIELD(TIM11EN, 18);
  };

  class APB1LPENR : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(TIM2LPEN, 0);
    REGS_BOOL_FIELD(TIM3LPEN, 1);
    REGS_BOOL_FIELD(TIM4LPEN, 2);
    REGS_BOOL_FIELD(TIM5LPEN, 3);
    REGS_BOOL_FIELD(TIM6LPEN, 4);
    REGS_BOOL_FIELD(TIM7LPEN, 5);
    REGS_BOOL_FIELD(TIM12LPEN, 6);
    REGS_BOOL_FIELD(TIM13LPEN, 7);
    REGS_BOOL_FIELD(TIM14LPEN, 8);
    REGS_BOOL_FIELD(RTCAPBEN, 10);
    REGS_BOOL_FIELD(SPI3LPEN, 15);
    REGS_BOOL_FIELD(USART3LPEN, 18);
    REGS_BOOL_FIELD(PWRLPEN, 28);
  };

  class BDCR : public Register32 {
  public:
    REGS_BOOL_FIELD(BDRST, 16);
    REGS_BOOL_FIELD(RTCEN, 15);
    enum class RTCSel {
      RTC_NoClock = 0,
      RTC_LSEClock = 1,
      RTC_LSIClock = 2,
      RTC_HSEClock = 3
    };
    void setRTCSel(RTCSel s) volatile { setBitRange(9, 8, (uint32_t)s); }
  };

  class CSR : public Register32 {
  public:
    REGS_BOOL_FIELD(LSION, 0);
    REGS_BOOL_FIELD_R(LSIRDY, 1);
  };

  class DCKCFGR2 : Register32 {
  public:
    REGS_BOOL_FIELD(CK48MSEL, 27);
    REGS_BOOL_FIELD(CKSDIOSEL, 28);
  };

  constexpr RCC() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(PLLCFGR, 0x04);
  REGS_REGISTER_AT(CFGR, 0x08);
  REGS_REGISTER_AT(AHB1ENR, 0x30);
  REGS_REGISTER_AT(AHB2ENR, 0x34);
  REGS_REGISTER_AT(AHB3ENR, 0x38);
  REGS_REGISTER_AT(APB1ENR, 0x40);
  REGS_REGISTER_AT(APB2ENR, 0x44);
  REGS_REGISTER_AT(APB1LPENR, 0x60);
  REGS_REGISTER_AT(BDCR, 0x70);
  REGS_REGISTER_AT(CSR, 0x74);
  REGS_REGISTER_AT(DCKCFGR2, 0x94);
private:
  constexpr uint32_t Base() const {
    return 0x40023800;
  }
};

constexpr RCC RCC;

#endif
