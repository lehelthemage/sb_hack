#pragma once
#undef _UNICODE
#undef UNICODE
#include <windows.h>
#include "overlay.h"
#include "odbccore.h"
#include <map>
using namespace std;
using namespace overlay;

class DataPacket
{
public:
	char data[0x4000];
	int length;
	bool read;
	int index;
	map<int, int> highlights;
	DataPacket() {}
	DataPacket(const DataPacket& d) {
		memcpy(data, d.data, d.length);
		length = d.length;
		read = d.read;
		index = d.index;
		for(map<int,int>::const_iterator i = d.highlights.begin(); i != d.highlights.end(); i++)
			highlights.insert(*i);
	}
};



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

class CDataBox;

class CDataText : public overlay::CText
{

protected:
	//BYTE* pdata;
	int data_length;
	string text;
	std::string messagetype;
	//std::map<int, int> *highlights;
	DataPacket* current_packet;

	list<DataPacket*> cached_data;
	int GetTrueFirst(int start);
	int GetTrueWidth(int start, int width);
public:
	CDataText(float x, float y, float _width, float _height, const Color& bgcolor, const Font& _font)
		: CText(x, y,_width, _height, bgcolor, _font, "", CText::Left), data_length(0),  current_packet(0) {}
	virtual void Draw();
	void SetData(BYTE* data, int length, int _index);
	void SetText(const string& _text) { text = _text; }
	friend class CDataBox;
};

class CDataBox : public overlay::CTitleBox
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
	CDataText datatext;
	//BYTE data[0x4000];
	//int datalength;
	string text;
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
		: CTitleBox(x, y, _width, _height, titlefont, "", _bordercolor, _fillcolor, titlebg),
		datatext(x, y + titlefont.height + 2*CText::padding, _width, _height - (titlefont.height + 2*CText::padding), _fillcolor, textfont), datadir(dir), showcurrent(true), initialpause(true), index(0), format(RawBytes), formatdir(Increasing), indexdir(Increasing), scrolling(false), db(_db), db_cs(_dbcs), lasttime(0), formatchanged(true), F2Down(false), F3Down(false)
	{}
	void ShiftFormat(Direction d);
	void ShiftIndex(Direction d);
	void ShowCurrent();
	void SetData(BYTE* pdata, int length, int _index);
	virtual void Draw();
	virtual void OnTick(int ticker, const overlay::KeyStates& ks);
	bool ShowingCurrent() { return showcurrent;}
	void SetText(const string& _text) {datatext.SetText(_text);} 
};

class CDataChecker : public CTitleBox
{
protected:
	enum CheckFormat
	{
		UInt,
		NUInt,
		Int,
		NInt,
		Float,
		NFloat
	} format;

	bool arrowdown;
	CInputBox input;
	CText value;
public:
	CDataChecker(int x, int y, int _width, int _height)
		: CTitleBox(x, y, _width, _height, Font(Font::Helvetica12, Color(1, 1, 1)), "Data Check", Color(1,0,0), Color(0,0,1)),
		input(x + 10, y + _height/2 - 20, _width/2 - 20, 20, Color(1,1,1), Font(Font::Helvetica10, black)),
		value(x + _width/2 + 10, y + _height/2 - 20, _width/2 - 20, 20, Color(1,1,1), Font(Font::Helvetica10, black), ""), format(UInt) {}
	virtual void Draw();
	virtual void OnTick(int ticker, const KeyStates& ks);

};


extern std::list<DataPacket> readpackets, writepackets;

void PrunePackets();

extern CDataBox* pReadBox;
extern CDataBox* pWriteBox;
extern CODBCDatabase db;
extern CRITICAL_SECTION cs;
extern CDataChecker* pChecker;













