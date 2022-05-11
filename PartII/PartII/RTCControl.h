#ifndef RTCONTROL_H_
#define RTCONTROL_H_

#define RTC_SLAVE_ADDR 0x68u

#define SEC_ADDR 0x00u
#define MIN_ADDR 0x01u
#define HRS_ADDR 0x02u
#define DAY_ADDR 0x03u
#define DATE_ADDR 0x04u
#define MON_ADDR 0x05u
#define YEAR_ADDR 0x06u

typedef union {
	struct  {
		uint8_t uni_seconds :4,
		dec_seconds :3,
		CH          :1;
	} field;
	uint8_t reg;
} seconds_type;

typedef union {
	struct  {
		uint8_t uni_minutes :4,
		dec_minutes :3,
		RESERVED    :1;
	} field;
	uint8_t reg;
} minutes_type;

typedef union {
	struct  {
		uint8_t uni_hours :4,
		dec_hours :2,
		h12       :1,
		RESERVED  :1;
	} field_24hrs;
	struct  {
		uint8_t uni_hours :4,
		dec_hours :1,
		pm        :1,
		h12       :1,
		RESERVED  :1;
	} field_12hrs;
	uint8_t reg;
} hours_type;

typedef union {
	struct  {
		uint8_t uni_day  :3,
		RESERVED :5;
	} field;
	uint8_t reg;
} day_type;

typedef union {
	struct  {
		uint8_t uni_date :4,
		dec_date :2,
		RESERVED :2;
	} field;
	uint8_t reg;
} date_type;

typedef union {
	struct  {
		uint8_t uni_month :4,
		dec_month :1,
		RESERVED  :3;
	} field;
	uint8_t reg;
} month_type;

typedef union {
	struct  {
		uint8_t uni_year :4,
		dec_year :4;
	} field;
	uint8_t reg;
} year_type;

typedef struct {
	uint8_t reg;
} ram_type;

typedef struct {
	seconds_type sec;
	minutes_type min;
	hours_type   hrs;
	day_type     day;
	date_type    date;
	month_type   mon;
	year_type    year;
	ram_type     ram[55];
} rtc_type;

void RTCInit();
void sendI2CDataArray(uint8_t SlAddr, uint8_t ptr, uint8_t *ptrData, uint32_t Size);
void SendI2CData(uint8_t Data);
void StopCond(void);
void receiveI2CDataArray(uint8_t SlAddr, uint8_t ptr, uint8_t *ptrData, uint32_t Size);
void ReadI2CData(uint8_t *Data);
void TurnOffI2C(void);

#endif /* RTCONTROL_H_ */