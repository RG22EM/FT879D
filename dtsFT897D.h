// �� ������ ������� ������ DetSimen �������� 2020 
//
#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>


const bool ON = true;
const bool OFF = false;


#pragma pack(push,1)

struct FT897DCommand {    // ����� ������
	union {
		struct {
			uint8_t Byte0;
			uint8_t Byte1;
			uint8_t Byte2;
			uint8_t Byte3;
		};
		uint8_t AsBytes[4];
	};
	uint8_t Command;
};

const uint8_t COMMAND_SIZE = sizeof(FT897DCommand); // ����� ������ �������


enum class TOperatingMode : uint8_t {  // ������ ������
	LSB = 0x00,
	USB = 0x01,
	CW  = 0x02,
	CWR = 0x03,
	AM  = 0x04,
	FM  = 0x08,
	DIG = 0x0A,
	PKT = 0x0C,
	FMN = 0x88
};

enum class TRepeaterOffset : uint8_t {
	Down = 0x09,
	Up = 0x49,
	Simplex = 0x89
};

enum class TDCS_Mode : uint8_t {
	DCS_ON = 0x0A,
	DCS_DECODER_ON = 0x0B,
	DCS_ENCODER_ON = 0x0C,
	CTCSS_ON = 0x2A,
	CTCSS_DECODER_ON = 0x3A,
	CTCSS_ENCODER_ON = 0x4A,
	OFF = 0x8A	
};

struct TRX_Status {
	uint8_t SMValue	: 4;
	bool	unused	: 1;
	bool	Discr	: 1;
	bool	Code	: 1;
	bool	NR		: 1;
};

struct TTX_Status {
	uint8_t PowerValue	: 4;
	bool    unused		: 1;
	bool	Split		: 1;
	bool	KSV			: 1;
	bool	Tangenta	: 1;
};

const uint8_t CMD_LOCK_ON	= 0x00;		// ���������� ���/����
const uint8_t CMD_LOCK_OFF	= 0x80;

const uint8_t CMD_PTT_ON	= 0x08;		// �������� ���/����
const uint8_t CMD_PTT_OFF	= 0x88;

const uint8_t CMD_SET_OPERATING_MODE = 0x07;

const uint8_t CMD_CLAR_ON	= 0x05;  // ���������� ������� (ON | OFF)
const uint8_t CMD_CLAR_OFF	= 0x85;
const uint8_t CMD_CLAR_SETFREQ = 0xF5; // ���������� �������

const uint8_t CMD_SET_MAIN_FREQ = 0x01; // ��������� ������� PLL

const uint8_t CMD_SWITCH_VFO = 0x81;  // VFO-A/B

const uint8_t CMD_SPLIT_ON = 0x02;  // ������ (split) (ON | OFF)
const uint8_t CMD_SPLIT_OFF = 0x82;

const uint8_t CMD_SET_REPEATER_OFFSET = 0x09;
const uint8_t CMD_SET_REPEATER_OFFSET_FREQ = 0xF9;

const uint8_t CMD_SET_DCS_MODE = 0x0A;

const uint8_t CMD_SET_CTCSS_TONE_FREQ = 0x0B;

const uint8_t CMD_SET_DCS_CODE = 0x0C;

const uint8_t CMD_READ_RX_STATUS = 0xE7;
const uint8_t CMD_READ_TX_STATUS = 0xF7;




class dtsFT897D {
protected:
	SoftwareSerial& FPort;
	FT897DCommand   FCommand;

	dtsFT897D() = delete;
	dtsFT897D(dtsFT897D& rvalue) = delete;

	void ClearCmdBuffer(void);   // �������(��������) ����� �������. ��� ����������� �������������.

	void SendCommand(void) const; // �������� 5 ���� ������ � ������ � ����������

	// 
	// ����������� ������� � ������ � ������������� �����, ������� � ����� �������� '0' �� 
	// ����� ALength
	//
	// AFreq      - ������� � ���� ����� � ����. ������
	// AIntDigits - ������������ ����� ����� ����� �����, 1, 2 ��� 3
	// ALength    - ����� ����� ����� ���������� ������������� �����
	//
	// �������� � ������� 114.32145 ����� ����� ���� 3 (114), � ����� ����� ���� ���� - 8
	//
	// ���������� �����, ������� �� �����
	//
	const char* Freq2String(const float AFreq, const uint8_t AIntDigits = 3, const uint8_t ALength = 8);

	void StringToBCD(const char* AFrom, uint8_t* ATo, const uint8_t ALen);

	uint8_t ReadByteFromPort(const uint16_t ATimeoutMS);

	void   FlushPort(void) const;

public:

	// �����������. ��������� ����� ��������� SoftwareSerial
	dtsFT897D(SoftwareSerial& ASerialPort); 
    
	// ������������� ������
	void Init(const uint32_t ABaudRate = 9600); 

	// AValue == (ON | OFF)  ���������� ���/����
	void SetLock(const bool AValue);  

	// AValue == (ON | OFF)  �������� ���/����
	void SetPTT(const bool AValue);   

	// �� ��������� ������� ����� - ����
	void SetMode(const TOperatingMode AMode = TOperatingMode::CW); 

	// AValue == (ON | OFF)  ���������� ������� ���/����
	void SetCLAR(const bool AValue); 

	// ��������� ������� ���������� AClarFreq = -99.99...+99.99
	void SetCLARFreq(float AClarFreq);

	// ������ ������ (split) ���/����
	void SetSplit(const bool AValue);

	// ��������� ����������� �������� ��� ������� 
	void SetRepeaterOffsetDir(const TRepeaterOffset AValue);

	// ��������� ������� �������� ��� ������� ARepFreq = �������, �������� 12.34
	void SetRepeaterOffsetFreq(float ARepFreq);

	// ��������� ������� � �������� ������, �������� 14.1234 MHz
	void SetMainFreq(float AMainFreq); 
	
	// ����������� VFO A/B
	void ToggleVFO(void); 

	// ���������� ����� DCS | CTCSS
	void SetDCSMode(const TDCS_Mode AMode);

	// ���������� ������� ������� ������ CTCSS ��� ������ � �������� 
	// 0..999�� � �� � ������
	void SetCTCSSToneFreq(float ATXFreq, float ARXFreq);

	// ���������� ��� ��� DCS ��� �������� � ������
	// �������� 0..9999 � ��� ���� � ��� �������
	void SetDCSCode(const uint16_t ATXCode, const uint16_t ARXCode);

	// ��������� ������ ������
	//
	TRX_Status ReadRXStatus();

	// ��������� ������ ��������
	//
	TTX_Status ReadTXStatus();
};

#pragma pack(pop)