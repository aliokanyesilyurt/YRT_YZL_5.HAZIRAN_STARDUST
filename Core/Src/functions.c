#include "functions.h"

UcusFazlari ucusDurumu = FAZ_RAMPA;
bmi088_t imu;
uint8_t txAktif = 1;

int16_t x_ivme = 0, y_ivme = 0, z_ivme = 0;
float z_ortIvme = 0, dikey_hiz = 0.0f, ivmeToplam = 0;
float irtifaBagil = 0, irtifaMax = 0, irtifaFiltre = 0, irtifaGuncel = 0,
		irtifaBaslangic = 0, basincFiltre = 0, basincGuncel = 0, basincBaslangic = 0,
		sicaklikFiltre = 0, sicaklikGuncel = 0;
uint32_t gecmis_zaman;
uint32_t durma_zamani = 0, dusus_sayaci = 0;

void veriOkuma(void) {

	MS5611_Measure(&hi2c3, &sicaklikGuncel, &basincGuncel);
	bmi088_read_accel(&imu, &x_ivme, &y_ivme, &z_ivme);

	if (basincGuncel < 30000.0f || basincGuncel > 110000.0f) return;

	osMutexAcquire(veriMutexHandle, osWaitForever);
    if(basincBaslangic == 0.0f && basincGuncel > 0.0f) {
        basincBaslangic = basincGuncel;
        basincFiltre = basincGuncel;
        sicaklikFiltre = sicaklikGuncel;

        irtifaGuncel = irtifaHesaplama();
        irtifaBaslangic = irtifaGuncel;
        irtifaFiltre = irtifaGuncel;
        irtifaMax = irtifaGuncel;
    }

    basincFiltre = ortFiltreleme(basincGuncel, basincFiltre);
    sicaklikFiltre = ortFiltreleme(sicaklikGuncel, sicaklikFiltre);
    z_ortIvme = ortFiltreleme((float)z_ivme , z_ortIvme);
    ivmeToplam = ivmeHesapla(x_ivme, y_ivme, z_ivme);

    if (basincBaslangic > 0) {
        irtifaGuncel = irtifaHesaplama();
    }

    irtifaFiltre = ortFiltreleme(irtifaGuncel, irtifaFiltre);
    irtifaBagil = irtifaFiltre - irtifaBaslangic;

    if (irtifaBagil < 0) irtifaBagil = 0;
    if (irtifaFiltre > irtifaMax) irtifaMax = irtifaFiltre;
    osMutexRelease(veriMutexHandle);
}

void hizHesaplama(float z_ivme) {

    uint32_t guncel_zaman = HAL_GetTick();
    float dt = (guncel_zaman - gecmis_zaman) / 1000.0f;
    gecmis_zaman = guncel_zaman;
    if (dt > 0.5f) return;

    dikey_hiz += (z_ivme * dt);
}

float irtifaHesaplama(void) {
	irtifaGuncel = 44330.0f * (1.0f - powf(basincFiltre / DENIZ_BAS, 0.1903f));
    return irtifaGuncel; // Basınç ve sıcaklıktan irtifa hesabı formülü.
}

float ivmeHesapla(int16_t ax, int16_t ay, int16_t az)
{
    const float olcek = (6.0f * 9.81f) / 32768.0f;   /* ham -> m/s^2 */

    float x = ax * olcek;
    float y = ay * olcek;
    float z = az * olcek;

    return sqrtf(x * x + y * y + z * z);
}

float ortFiltreleme(float ortGuncel, float ortFiltre) {
		ortFiltre = (ortGuncel * 0.1) + (ortFiltre * 0.9);
	return ortFiltre;
}

uint16_t YRT_CRCCalculator(uint8_t  *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    char i = 0;

    while (len--)
    {
        crc ^= (*buf++);

        for (i = 0; i < 8; i++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}

typedef struct {
	int16_t altitude;
	int16_t maxAltitude;
	uint16_t pressure;
} altitude_info;

typedef struct {
	uint16_t absG;
} accel_info;

#pragma pack(push, 1)
typedef struct {
	uint8_t flightState;
	altitude_info alt;
	accel_info accel;
	uint16_t crc;
} system_info;
#pragma pack(pop)

system_info arifPaket;

void CAN_paketOlustur(void)
{
	osMutexAcquire(veriMutexHandle, osWaitForever);
	arifPaket.flightState = (uint8_t)ucusDurumu;
	arifPaket.alt.altitude = (int16_t)irtifaFiltre;
	arifPaket.alt.maxAltitude = (int16_t)irtifaMax;
	arifPaket.alt.pressure = (uint16_t)(basincFiltre / 10.0f);
	arifPaket.accel.absG   = (uint16_t)(ivmeToplam * 100.0f);
	osMutexRelease(veriMutexHandle);

	int16_t boyut = sizeof(system_info) - sizeof(uint16_t);
	arifPaket.crc = YRT_CRCCalculator((uint8_t*)&arifPaket, boyut);
}

void CAN_telemetriGonder(void)
{
    if (!txAktif) return;
    CAN_paketOlustur();

    uint8_t *TxBuffer = (uint8_t*)&arifPaket;

    CAN_sendData(0x101, &TxBuffer[0], 8);
    osDelay(2);
    CAN_sendData(0x102, &TxBuffer[8], sizeof(system_info) - 8);

}


//void stateMachine(void) {
//    switch(ucusDurumu) {
//        case FAZ_RAMPA:    firlatma(); break;
//        case FAZ_FIRLATMA: tirmanma(); break;
//        case FAZ_TIRMANIS: arama(); break;
//        case FAZ_ARAYIS:   drogueAcma(); break;
//        case FAZ_DUSUS:    anaParasutAcma();
//        case FAZ_INIS:     inisKontrol(); break;
//        case FAZ_BITIS:    ledYakma(); break;
//    }
//}
//void firlatma(void){
//	if(irtifaBaslangic == 0 && irtifaFiltre > 0){
//	        irtifaBaslangic = irtifaFiltre;
//	    }
//	if(ivmeToplam > 20.0f){
//		ucusDurumu = FAZ_FIRLATMA;
//	}
//}
//
//void tirmanma(void){
//	    if(z_ivme < 20.0f){
//	    	ucusDurumu = FAZ_TIRMANIS;
//	    }
//}
//
//void arama(void){
//	if(dikey_hiz > 30.0f || irtifaBagil > 100.0f){
//		ucusDurumu = FAZ_ARAYIS;
//	}
//}
//
//void drogueAcma(void) {
//    if(dikey_hiz < -10.0f) {
//        dusus_sayaci++;
//        if(dusus_sayaci > 5) {
//            HAL_GPIO_WritePin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin, 1);
//            HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, 1);
//            ucusDurumu = FAZ_DUSUS;
//        }
//     else {
//        dusus_sayaci = 0;
//    }
//}
//
//void anaParasutAcma(void){
//	if(irtifaFiltre < 600.0f){
//		HAL_GPIO_WritePin(ANA_PA8_GPIO_Port, ANA_PA8_Pin, 1);
//		HAL_GPIO_WritePin(LED_PA6_GPIO_Port, LED_PA6_Pin, 1);
//	}
//}
//
//void inisKontrol(void) {
//    if (dikey_hiz < -50.0f) {
//        HAL_GPIO_WritePin(YEDEK_PA7_GPIO_Port, YEDEK_PA7_Pin, 1);
//    }
//    if ((z_ivme < 10.5f && z_ivme > -9.5f) && (dikey_hiz < 1.0f && dikey_hiz > -1.0f)) {
//
//    	if(durma_zamani == 0){
//    		durma_zamani = HAL_GetTick();
//    	}
//    	if((HAL_GetTick() - durma_zamani) > 5000){
//        ucusDurumu = FAZ_BITIS;
//    	}// İnişin doğruluğunu kontrol edip yedek paraşüt açan fonksiyon.
//    }else{
//		durma_zamani = 0;
//	}
//}
//
//void ledYakma(void) {
//		HAL_GPIO_TogglePin(LED_PA6_GPIO_Port, LED_PA6_Pin); // İnince led yakma.
//}
