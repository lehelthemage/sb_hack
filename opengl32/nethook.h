#pragma once
#undef _UNICODE
#undef UNICODE
#include <windows.h>
#include "overlay.h"
#include "odbccore.h"


struct NETDATA
{
	char readdata[0x4000];
	int read_size;
	char writedata[0x4000];
	int write_size;
};

extern NETDATA netdata;

typedef void (*ReadCallback)(char* pdata, int *length);
extern ReadCallback readcallback;
typedef void (*WriteCallback)(char* pdata, int *length);
extern WriteCallback writecallback;

void SetCallbacks(ReadCallback r, WriteCallback w);
BOOL HackNetCode();


class CDataBox : public overlay::CTextBox
{
public:
	enum DataFormat
	{
		RawBytes = 0,
		ASCII1 = 1,
		ASCII2 = 2,
		Integer = 3,
		ShiftedInteger = 4,
		Float = 5,
		ShiftedFloat = 6,
		NInteger = 7,
		SNInteger = 8,
		NFloat = 9,
		NSFloat = 10,
		ASCIINInteger = 11,
		ASCIINFloat = 12,
		ASCIINSInteger = 13,
		ASCIINSFloat = 14
	};
	enum Direction
	{
		Increasing,
		Decreasing
	};
	enum DataDirection
	{
		Receive,
		Send
	};
protected:
	BYTE data[0x4000];
	int datalength;
	DataFormat format;
	int index;
	bool showcurrent;
	bool scrolling;
	int lasttime;
	bool initialpause;
	DataDirection datadir;
	Direction formatdir, indexdir;
	void FormatData();
	std::string GetMessageName();
	CODBCDatabase& db;
	CRITICAL_SECTION& db_cs;
	bool formatchanged;
	bool F2Down, F3Down;
	void LookupData(int indexref, Direction d); //look up next record from the index reference to left or right of that given record
public:
	CDataBox(CODBCDatabase& _db, CRITICAL_SECTION& _dbcs, float x, float y, float _width, float _height, const overlay::Font& titlefont, DataDirection dir, const overlay::Font& textfont, const overlay::Color& _bordercolor = overlay::nullcolor, const overlay::Color& _fillcolor = overlay::nullcolor, const overlay::Color& titlebg = overlay::black) 
		: CTextBox(x, y, _width, _height, titlefont, "", textfont, "", _bordercolor, _fillcolor, titlebg), datadir(dir), showcurrent(true), initialpause(true), index(0), format(RawBytes), formatdir(Increasing), indexdir(Increasing), scrolling(false), db(_db), db_cs(_dbcs), lasttime(0), formatchanged(true), F2Down(false), F3Down(false)
	{}
	void ShiftFormat(Direction d);
	void ShiftIndex(Direction d);
	void ShowCurrent();
	void SetData(BYTE* pdata, int length, int _index);
	virtual void Draw();
	virtual void OnTick(int ticker, const overlay::KeyStates& ks);
	bool ShowingCurrent() { return showcurrent;}
};

class DataPacket
{
public:
	char data[0x4000];
	int length;
	bool read;
	int index;
	DataPacket() {}
	DataPacket(const DataPacket& d) {
		memcpy(data, d.data, d.length);
		length = d.length;
		read = d.read;
		index = d.index;
	}
};

extern std::list<DataPacket> readpackets, writepackets;

void PrunePackets();

extern CDataBox* pReadBox;
extern CDataBox* pWriteBox;
extern CODBCDatabase db;
extern CRITICAL_SECTION cs;