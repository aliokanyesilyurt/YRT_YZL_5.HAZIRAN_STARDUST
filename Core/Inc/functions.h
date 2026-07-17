#ifndef NEWFUNCTIONS_H
#define NEWFUNCTIONS_H

#define DENIZ_BAS 101325.0f

#include "main.h"
#include "can.h"

#include <math.h>
#include <string.h>

#include "ms5611.h"
#include "bmi088.h"
#include "settings.h"

typedef enum {
    FAZ_RAMPA, // Rampa konumunda.
	FAZ_FIRLATMA, // İtkili atış kısmı.
    FAZ_TIRMANIS, // Apogee atamaz kilidi var.
	FAZ_ARAYIS, // Kilit kalktı ve apogee atabilir.
    FAZ_DUSUS, // Sürüklenme sonrası durum. Bu fazın sonunda ana paraşütü açacak.
    FAZ_INIS, // Ana paraşüt sonrası alçalma durumu.
	FAZ_BITIS // Yere tam olarak indiğini yahut hızının vs sıfırlandığını belirten durum.
} UcusFazlari;

// Conventerlar
typedef union {
    uint16_t u16;
    int16_t i16;
    uint8_t u8[2];
} u16_to_u8_2;

typedef union {
    float f32;
    uint32_t u32;
    uint8_t u8[4];
} float_to_u8_4;

// Externler
extern UcusFazlari ucusDurumu;
extern int16_t x_ivme, y_ivme, z_ivme;
extern float z_ortIvme, dikey_hiz, ivmeToplam;
extern float irtifaBagil, irtifaMax, irtifaFiltre, irtifaGuncel, irtifaBaslangic;
extern float basincFiltre, basincGuncel, basincBaslangic;
extern float sicaklikFiltre, sicaklikGuncel;
extern uint32_t gecmis_zaman;
extern uint32_t durma_zamani, dusus_sayaci;
extern uint8_t txAktif;
// Extern Fonksiyon
extern void CAN_sendData(uint32_t id, uint8_t *data, uint8_t length);
//Extern Değişik Şeyler
extern osMutexId_t veriMutexHandle;
extern I2C_HandleTypeDef hi2c2, hi2c3;

// Temel Fonksiyon Protipleri
void veriOkuma(void);

//void stateMachine(void);
//void firlatma(void);
//void tirmanma(void);
//void arama(void);
//void drogueAcma(void);
//void anaParasutAcma(void);
//void inisKontrol(void);
//void ledYakma(void);
void hizHesaplama(float z_ivme);
float ivmeHesapla(int16_t ax, int16_t ay, int16_t az);
float irtifaHesaplama(void);
float ortFiltreleme(float ortGuncel, float ortFiltre);
uint16_t YRT_CRCCalculator(uint8_t *data, uint16_t length);

void CAN_paketOlustur(void);
void CAN_telemetriGonder(void);

#endif
