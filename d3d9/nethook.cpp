#include "nethook.h"
#include <Tlhelp32.h>
//#include "odbccore.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "opengl32.h"

using namespace std;

CDataChecker* pChecker = NULL;
ReadCallback readcallback = NULL;
WriteCallback writecallback = NULL;
CODBCDatabase db;
NETDATA netdata;
CRITICAL_SECTION cs;
bool cs_init = false;
CDataBox* pReadBox = NULL;
CDataBox* pWriteBox = NULL;
int read_data_index = 0;
int write_data_index = 0;
int current_data_index = 0;
DataPacket dp;

std::list<std::string> commands;
bool running = true;

std::list<DataPacket> readpackets, writepackets;

void PrunePackets()
{
	if(readpackets.size() > 200)
	{
		for(int i = 0; i < 100; i++)
			readpackets.pop_front();
	}

	if(writepackets.size() > 200)
	{
		for(int i = 0; i < 100; i++)
			writepackets.pop_front();
	}
}

unsigned char Hex2Dec(std::string s);

unsigned char Hex2Dec(std::string s)
{
    std::stringstream ss(s);
    unsigned char i;
    ss >> std::hex >> i;
    return i;
}

unsigned char Str2Hex(const std::string& s)
{
	char i = 0;
	if(s[0] >= 0x30 && s[0] <= 0x39)
		i = s[0] - 0x30;
	else if(s[0] >= 0x41 && s[0] <= 0x46)
		i = s[0] - 0x37;
	i <<= 4;
	if(s[1] >= 0x30 && s[1] <= 0x39)
		i += s[1] - 0x30;
	else if(s[1] >= 0x41 && s[1] <= 0x46)
		i += s[1] - 0x37;

	return i;
}

void StartUpDb()
{
	if(!db.IsConnected())
		db.Open("sb");
	else
		return;
	
	db.Execute("DELETE FROM net WHERE index < (SELECT MAX(index) from net ) - 100");
	CODBCRecordset rs(&db);
	
	rs.Open("SELECT max(index) FROM net ORDER BY index DESC");
	if(!rs.IsEof())
	{
		rs.GetFieldValue(0, (long*)&read_data_index);
	}

	rs.Close();

}

void _cdecl DatabaseWriteThread(void* parg)
{
	/*
	char buffer[0x20000];
	ZeroMemory(buffer, 0x20000);
	char buffer2[10];
	int len = 0;
	
	
	memcpy(netdata.readdata, pdata, length);
	netdata.read_size = length;
	
	
	if(!db.IsConnected())
		StartUpDb();

	::EnterCriticalSection(&cs);
	sprintf(buffer, "INSERT INTO net (index, time1, read, data_size, message) VALUES (%d, NOW(), 1, %d, 0x", read_data_index + 1, length);
	for(int i = 0; i < length; i++)
	{
		sprintf(buffer2, "%2.2X", *(pdata + i));
		strcat(buffer, buffer2);
		len = strlen(buffer);
	}
	strcat(buffer, ")");
	db.Execute(buffer);
	read_data_index++;
	::LeaveCriticalSection(&cs);	

	DataPacket dp;
	memcpy(dp.data, pdata, length);
	dp.length = length;
	dp.read = true;
	dp.index = read_data_index;
	readpackets.push_back(dp);
	if(pWriteBox->ShowingCurrent())
		pWriteBox->SetData((BYTE*)pdata, length, read_data_index);
*/
}

#pragma warning (disable : 4996)

void MyReadCallback(char* pdata, int* length)
{
	char buffer[0x20000];
	ZeroMemory(buffer, 0x20000);
	int len = 0;
	
	
	memcpy(netdata.readdata, pdata, *length);
	netdata.read_size = *length;
	
/*	
	if(!db.IsConnected())
		StartUpDb();
*/
	::EnterCriticalSection(&cs);
/*
	sprintf(buffer, "INSERT INTO net (index, time1, read, data_size, message) VALUES (%d, NOW(), 1, %d, 0x", read_data_index + 1, *length);
	for(int i = 0; i < *length; i++)
	{
		sprintf(buffer2, "%2.2X", *(pdata + i));
		strcat(buffer, buffer2);
		len = strlen(buffer);
	}
	strcat(buffer, ")");
	try
	{
		db.Execute(buffer);
		
	}
	catch(...)
	{
	}
	
*/
	read_data_index++;
	::LeaveCriticalSection(&cs);	

	DataPacket dp;
	memcpy(dp.data, pdata, *length);
	dp.length = *length;
	dp.read = true;
	dp.index = read_data_index;
	readpackets.push_back(dp);
	if(pReadBox->ShowingCurrent())
		pReadBox->SetData((BYTE*)pdata, *length, read_data_index);
}

void MyWriteCallback(char* pdata, int* length)
{
	__asm
	{
		push EAX
		push EBX
		push ECX
		push EDX
		push ESI
		push EDI
		
	}

	char buffer[0x20000];
	ZeroMemory(buffer, 0x20000);
	int len = 0;
	
	
//	memcpy(netdata.writedata, pdata, *length);
//	netdata.write_size = *length;
//	
///*	
//	if(!db.IsConnected())
//		StartUpDb();
//*/
::EnterCriticalSection(&cs);
///*
//	sprintf(buffer, "INSERT INTO net (index, time1, read, data_size, message) VALUES (%d, NOW(), 0, %d, 0x", read_data_index + 1, *length);
//	for(int i = 0; i < *length; i++)
//	{
//		sprintf(buffer2, "%2.2X", *(pdata + i));
//		strcat(buffer, buffer2);
//		len = strlen(buffer);
//	}
//	strcat(buffer, ")");
//	try
//	{
//		db.Execute(buffer);
//		
//	}
//	catch(...)
//	{
//	}
//*/
	read_data_index++;
	::LeaveCriticalSection(&cs);	
//
	
	memcpy(dp.data, pdata, *length);
	dp.length = *length;
	dp.read = false;
	dp.index = read_data_index;
	writepackets.push_back(dp);
	if(pWriteBox->ShowingCurrent())
		pWriteBox->SetData((byte*)pdata, *length, read_data_index);

	__asm {
		pop EDI
		pop ESI
		pop EDX
		pop ECX
		pop EBX
		pop EAX
	}
}


std::string CDataBox::GetMessageName()
{
	char buffer[100];
	char buffer2[20];

	char* pdata = NULL;
	if(datatext.current_packet != NULL)
		pdata = datatext.current_packet->data;

	strcpy(buffer, "SELECT label FROM message WHERE message = 0x");
	for(int i = 0; i < 4; i++)
		sprintf(buffer2, "%2.2X ", pdata[i]);
	strcat(buffer, buffer2);

	//::EnterCriticalSection(&db_cs);

/*	CODBCRecordset rs(&db);
	rs.Open(buffer2);
	
	strcpy(buffer, "");

	if(!rs.IsEof())
	{
		rs.GetFieldValue(0, buffer);
	}

	rs.Close();
	//::LeaveCriticalSection(&db_cs);
*/
	return std::string(buffer);
}

void CDataBox::Draw()
{
	std::string s;
	if(datadir == Receive)
		s += "Recv: ";
	else
		s += "Send: ";

	char buffer[20];
	sprintf(buffer, "index:%d ", index);
	s += buffer;

	s += "Format: ";

	switch(format)
	{
	default:
	case CDataBox::RawBytes:
		s += "Raw Bytes";
		break;
	case ASCII1:
		s += "ASCII";
		break;
	case ASCII2:
		s += "ASCII and bytes";
		break;
	case Integer:
		s += "Integer";
		break;
	case ShiftedInteger:
		s += "Integer + 2";
		break;
	case Float:
		s += "Float";
		break;
	case ShiftedFloat:
		s += "Float + 2";
		break;
	case NInteger:
		s += "NInt";
		break;
	case SNInteger:
		s += "SNInt";
		break;
	case NFloat:
		s += "NFloat";
		break;
	case NSFloat:
		s += "NSFloat";
		break;
	case ASCIINInteger:
		s += "ASCIINInt";
		break;
	case ASCIINFloat:
		s += "ASCIINFloat";
		break;
	case ASCIINSInteger:
		s += "ASCIINSInt";
		break;
	case ASCIINSFloat:
		s += "ASCIINSFloat";
		break;
	}

	
	SetTitleText(s);

	FormatData();

	CTitleBox::Draw();
	datatext.Draw();
}

void CDataBox::FormatData()
{
	std::string _text = "";
	char buffer[0x1000];
	std::string name;
	int a;
	bool isstring = false;
	bool alltext = false;

	int line_length1 = 16;

	char* data = (char*)datatext.current_packet->data;
	int datalength = datatext.data_length;

	if(data == NULL)
		return;

	if(!formatchanged)
		return;

	formatchanged = false;

	for(int i = 0; i < 2; i++)
		{
			sprintf(buffer, "%2.2X ", data[i]);
			_text += buffer;
		}

	for(int i = 2; i < datalength; i++)
	{
		if(format == RawBytes)
		{
			sprintf(buffer, "%2.2X", data[i]);
			_text += buffer;
			if((i - 2) % line_length1 == 3 || (i - 2)  % line_length1 == 11)
				_text += " ";
			else if((i - 2)  % line_length1 == 7)
				_text += "  ";
		}
		else if(format == ASCII1)
		{
			if(data[i] >= 32 && data[i] <= 126)
				_text +=(char)data[i];
			else
			{
				_text += ".";
			}
			_text += " ";
		}
		else if(format == ASCII2)
		{
			if(data[i] >= 32 && data[i] <= 126)
				_text +=(char)data[i];
			else
			{
				sprintf(buffer, "%2.2X ", data[i]);
				_text += buffer;
			}
			_text += " ";
		}
		else if(format == Integer)
		{
			sprintf(buffer, "%d ", *(int*)(data + i));
			_text += buffer;
			i += 3;
		}
		else if(format == ShiftedInteger)
		{ 
			if(i == 4)
				i = 6;

			sprintf(buffer, "%d ", *(int*)(data + i));
			_text += buffer;
			i += 3;
		}
		else if(format == Float)
		{
			sprintf(buffer, "%g ", *(float*)(data + i));
			_text += buffer;
			i += 3;
		}
		else if(format == ShiftedFloat)
		{
			if(i == 4)
				i = 6;

			sprintf(buffer, "%g ", *(float*)(data + i));
			_text += buffer;
			i += 3;
		}
		else if(format == NInteger)
		{
			sprintf(buffer, "%d ", ntohl(*(int*)(data + i)));
			_text += buffer;
			i += 3;
		}
		else if(format == SNInteger)
		{
			if(i == 4)
				i = 6;

			sprintf(buffer, "%d ", ntohl(*(int*)(data + i)));
			_text += buffer;
			i += 3;
		}
		else if(format == NFloat)
		{
			sprintf(buffer, "%g ", (float)ntohl(*(long*)(data + i)));
			_text += buffer;
			i += 3;

		}
		else if(format == NSFloat)
		{
			if(i == 4)
				i = 6;

			sprintf(buffer, "%g ", (float)ntohl(*(long*)(data + i)));
			_text += buffer;
			i += 3;
		}
		else if(format == ASCIINInteger)
		{
			if(i == 4)
			{
				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
			}
			else 
			if((data[i] >= 32 && data[i] < 126) || data[i] == 10 || data[i] == 13 || data[i] == 9)
			{

				for(a = i; a < datalength; a++)
				{
					if(data[a] == 0 || a == datalength - 1)
					{
						for(; i <= a; i++)
							_text += data[i];

						break;
					}
					else if((data[a] >= 32 && data[a] < 126) || data[a] == 10 || data[a] == 13|| data[a] == 9)
						continue;
					
				}
			}
			else
			{
				sprintf(buffer, "%d ", ntohl(*(int*)(data + i)));
				_text += buffer;
				i += 3;
			}
		}
		else if(format == ASCIINFloat)
		{
			if(i == 4)
			{
				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
			}
			else if((data[i] >= 32 && data[i] < 126) || data[i] == 10 || data[i] == 13 || data[i] == 9)
			{
				for(a = i; a < datalength; a++)
				{
					if(data[a] == 0 || a == datalength - 1)
					{
						for(; i <= a; i++)
							_text += data[i];

						break;
					}
					else
					if((data[a] >= 32 && data[a] < 126)|| data[a] == 10 || data[a] == 13 || data[a] == 9) 
						continue;
				}
			}
			else
			{
				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
				i += 3;
			}
		}
		else if(format == ASCIINSInteger)
		{
			if(i == 4)
			{
				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
			}
			else if((data[i] >= 32 && data[i] < 126) || data[i] == 10 || data[i] == 13 || data[i] == 9)
			{
				for(a = i; a < datalength; a++)
				{
					if(data[a] == 0 || a == datalength - 1)
					{
						for(; i <= a; i++)
							_text += data[i];

						break;
					}
					else
					if((data[a] >= 32 && data[a] < 126) || data[a] == 10 || data[a] == 13 || data[a] == 9)
						continue;
				}
			}
			else 
			{
				if(data[i - 1] == 0 && (data[i - 2] >= 32 && data[i - 2] < 126))
				{
					sprintf(buffer, "%d ", ntohs(*(short*)(data + i)));
					_text += buffer;
					i += 2;
				}
				if(i > datalength - 3)
					break;

				sprintf(buffer, "%d ", ntohl(*(int*)(data + i)));
				_text += buffer;
				i += 3;
			}
		}
		else if(format == ASCIINSFloat)
		{
			if(i == 4)
			{
				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
			}
			else 
				if((data[i] >= 32 && data[i] < 126) || data[i] == 10 || data[i] == 13 || data[i] == 9)
			{
				for(a = i; a < datalength; a++)
				{
					if(data[a] == 0 || a == datalength - 1)
					{
						for(; i <= a; i++)
							_text += data[i];

						break;
					}
					else
					if((data[a] >= 32 && data[a] < 126) || data[a] == 10 || data[a] == 13 || data[a] == 9)
						continue;
				}
			}
			else 
			{
				if(data[i - 1] == 0 && (data[i - 2] >= 32 && data[i - 2] < 126))
				{
					sprintf(buffer, "%d ", ntohs(*(short*)(data + i)));
					_text += buffer;
					i += 2;
				}
				if(i > datalength - 3)
					break;

				sprintf(buffer, "%d ", (float)ntohl(*(long*)(data + i)));
				_text += buffer;
				i += 3;
			}
		}

			
		SetText(_text);
		
	}
}

void CDataBox::LookupData(int indexref, Direction d)
{
	if(datatext.current_packet == NULL)
		return;
	char* data = (char*)datatext.current_packet->data;
	int datalength = datatext.current_packet->length;

	std::list<DataPacket*>::iterator it, it2;
	for(it = datatext.cached_data.begin(); it != datatext.cached_data.end(); it++)
	{
		if((*it)->index == index)
		{
			it2 = it;
			if(d == Increasing && ++it2 != datatext.cached_data.end())
			{
				memcpy(data, (*it2)->data, (*it2)->length);
				datalength = (*it2)->length;
				index = (*it2)->index;
				break;
			}
			else if(d == Decreasing && it != datatext.cached_data.begin())
			{
				it2--;
				memcpy(data, (*it2)->data, (*it2)->length);
				datalength = (*it2)->length;
				index = (*it2)->index;
				break;
			}
		}
	}

	formatchanged = true;
	this->showcurrent = false;

}

void CDataBox::ShiftFormat(Direction d)
{
	formatchanged = true;

	if(d == Increasing && format == ASCIINSFloat)
		format = RawBytes;
	else if(d == Decreasing && format == RawBytes)
		format = ASCIINSFloat;
	else if(d == Increasing)
		format = (DataFormat)((int)format + 1);
	else
		format = (DataFormat)((int)format - 1);
}

void CDataBox::ShiftIndex(Direction d)
{
	LookupData(index, d);
}

void CDataBox::ShowCurrent()
{
	showcurrent = true;
}
void CDataBox::SetData(BYTE* pdata, int length, int _index)
{
	if(!showcurrent)
		return;
	formatchanged = true;
	
	datatext.SetData(pdata, length, _index);
	index = _index;
}

void CDataBox::OnTick(int ticker, const overlay::KeyStates& ks)
{
	if(ks.larrowdown || ks.rarrowdown || ks.uarrowdown || ks.darrowdown)
	{
		if(lasttime == 0)
		{
			initialpause = true;
			lasttime = ticker;
			goto CheckKey;
		}

		if(initialpause)
		{
			if(ticker - lasttime < 10)
			{
				return;
			}
			else
				initialpause = false;
		}
		else
		{
			if(ticker - lasttime < 3)
			{
				return;
			}
		}
	}
CheckKey:
	if(ks.larrowdown)
	{
		ShiftIndex(Decreasing);
	}
	else if(ks.rarrowdown)
	{
		ShiftIndex(Increasing);
	}
	else if(ks.uarrowdown)
	{
		ShiftFormat(Increasing);
	}
	else if(ks.darrowdown)
	{
		ShiftFormat(Decreasing);
	}

	if(!ks.larrowdown && !ks.rarrowdown && !ks.uarrowdown && !ks.darrowdown)
	{
		lasttime = 0;
	}

	if(ks.F2down && datadir == Receive)
	{
		F2Down = true;
	}
	if(ks.F3down && datadir == Send)
		F3Down = true;

	if(ks.F2down == false && F2Down)
	{
		F2Down = false;
		Visible = !Visible;
		showcurrent = true;
	}
	if(ks.F3down == false && F3Down)
	{
		F3Down = false;
		Visible = !Visible;
		showcurrent = true;
	}
}


DWORD GetModuleBaseAddress(int tpid, char* module)
{
	MODULEENTRY32 me32;
	HANDLE hModules;
	DWORD addr = 0;

	hModules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, tpid);
	me32.dwSize = sizeof(me32);
	BOOL ret = Module32First(hModules, &me32);

	while(ret)
	{
		if (!_strcmpi(me32.szModule, module)){
			addr = (int)me32.modBaseAddr;
			break;
		}

		ret = Module32Next(hModules, &me32);
	}
	CloseHandle(hModules);

	return addr;
}

void SetNetCallbacks(ReadCallback r, WriteCallback w)
{
	
}

BOOL HackNetCode()
{
	netdata.read_size = 0;
	netdata.write_size = 0;

	::InitializeCriticalSection(&cs);
		

	HANDLE hProcess = ::GetCurrentProcess(); 
	DWORD process_id = ::GetCurrentProcessId();
	LPVOID sbBaseAddress = (LPVOID)GetModuleBaseAddress(process_id, "sb.exe");
	LPVOID addressAfterRead = (LPVOID)((DWORD)sbBaseAddress + 0x1111AC);
	LPVOID addressBeforeWrite = (LPVOID)((DWORD)sbBaseAddress + 0x361D7D);
	unsigned char buffer[128];
	ZeroMemory(buffer, 128);
	SIZE_T numRead = 0;
	DWORD glBaseAddress = GetModuleBaseAddress(process_id, "opengl32.dll");
	//0x3CA30
	readcallback = &MyReadCallback;
	writecallback = &MyWriteCallback;
	//SetNetCallbacks(MyReadCallback, MyWriteCallback);
	BOOL ret = ::ReadProcessMemory(hProcess, addressAfterRead, buffer, 32, &numRead);    	
	DWORD callAddress = (DWORD)readcallback - ((DWORD)sbBaseAddress + 0x1111C2);
#pragma warning (disable : 4309)
	char readJmpCode[3] = {	0xEB, 0x05, 
						0x90};

	char readCodeCave[21] = {	0x50,
								0x8B, 0xC5,
								0x83, 0xE8, 0x14,
								0x50,
								0xFF, 0x75,	0x0C,
								0xE8, 0x6E, 0xB8, 0xDE, 0xFF, //[11] + 4
								0x58,
								0x83, 0xC4, 0x1C,
								0xEB, 0xE7 };

	char writeJmpCode[6] = {	0xE9, 0x73, 0x02, 0x00, 0x00,
								0x90 };
	char writeCodeCave[35] = {	0x50,
								0x8B, 0x46, 0x0C,
								0x8B, 0x40, 0x0C, 
								0x8B, 0x40, 0x08, 
								0x51,
								0x83, 0xC0, 0x0C,
								0x50, 
								0x83, 0xE8, 0x08, 
								0xFF, 0x30, 
								0xE8, 0x02, 0xCB, 0xBC, 0xFF, //21
								0x59, 
								0x58, 
								0xFF, 0x52, 0x54, 
								0xE9, 0x6A, 0xFD, 0xFF, 0xFF};
#pragma warning (default : 4309)
	memcpy(readCodeCave + 11, &callAddress, 4);
	::WriteProcessMemory(hProcess, addressAfterRead, readJmpCode, 3, &numRead);
	::WriteProcessMemory(hProcess, (LPVOID)((DWORD)sbBaseAddress + 0x1111B3), readCodeCave, 21, &numRead);

	callAddress = (DWORD)writecallback - ((DWORD)sbBaseAddress + 0x36200E);
	memcpy(writeCodeCave + 21, &callAddress, 4);
	::WriteProcessMemory(hProcess, addressBeforeWrite, writeJmpCode, 6, &numRead);
	::WriteProcessMemory(hProcess, (LPVOID)((DWORD)sbBaseAddress + 0x361ff5), writeCodeCave, 35, &numRead);
						
/*
721d7d


@ 00761ff5

50 push eax
8b46 0c mov eax, [esi + c]
8b40 oc mov eax, [eax + c}
8b40 08 mov eax, [eax + 8]
51 PUSH ECX
83c0 0c add eax, c
50 push eax
83e8 08 sub eax, 8
ff30 push eax
e8 03bcbbcff call 0032eb10 dest - start
59 pop ecx
58 pop eax
ff52 54 CALL [EDX+54]
e9 6afdffff jmp 00761d82


721ff5
*/
	

	return TRUE;
}


void CDataChecker::Draw()
{
	CTitleBox::Draw();
	input.Draw();

	string s = input.GetText();
	char data[4];
	::ZeroMemory(data, 4);
	int slen;

	if(s.size() == 0)
		return;

	for(int i = 0; i != s.size()/2; i++)
	{
		if(s.size() % 2 == 1)
			slen = 1;
		else
			slen = 2;
		data[i] = Str2Hex(s.substr(i * 2, slen));
	}

	char buffer[40];
	switch(format)
	{
	case UInt:
		sprintf(buffer, "uint: %u", *(long*)data);
		break;
	case NUInt:
		sprintf(buffer, "nuint: %u", ::ntohl(*(long*)data));
		break;
	case Int:
		sprintf(buffer, "int: %d", *(long*)data);
		break;
	case NInt:
		sprintf(buffer, "nint: %d", ::ntohl(*(long*)data));
		break;
	case Float:
		sprintf(buffer, "float: %g", (float)*(float*)data);
		break;
	case NFloat:
		sprintf(buffer, "nfloat %g", (float)::ntohl(*(long*)data));
		break;
	}

	value.SetText(buffer);
	value.Draw();
}

void CDataChecker::OnTick(int ticker, const KeyStates& ks)
{
	if(input.GetText().size() < 8 || ks.backspacedown)
		input.OnTick(ticker, ks);

	if((ks.darrowdown || ks.uarrowdown) && !arrowdown)
	{
		arrowdown = true;

		if(ks.uarrowdown)
		{
		if(format == NFloat)
			format = UInt;
		else
			format = (CheckFormat)((int)format + 1);
		}
		else
		{
			if(format == UInt)
				format = NFloat;
			else
				format = (CheckFormat)((int)format - 1);
		}
	}
	else if(!ks.darrowdown && !ks.uarrowdown)
		arrowdown = false;
}

void CDataText::SetData(BYTE *data, int length, int _index)
{
	list<DataPacket*>::iterator it;
	bool found = false;

	//search through the cached data (backwards)
	for(it = cached_data.end(); it != cached_data.begin(); it--)
	{
		if(it == cached_data.end() && cached_data.size() != 0)
			it--;

		if(memcmp((*it)->data, data, 2) == 0)	
		{
			//if the first two bytes match current packet, found = true an break
			found = true;
			break;
		}
	}

	//create a new packet 
	DataPacket *d = new DataPacket();
	memcpy(d->data, data, length);
	d->length = length;
	d->index = _index;
	cached_data.push_back(d);


	//set current packet
	current_packet = d;

	//pdata = (BYTE*)(*pushed)->data;
	data_length = length;
	
	//marker is set to -1 for not having anything set yet
	int marker = -1;

	//if there is a packet in cache_data matching near the end
	//(iterator points to previous packet)
	if(found)
	{
		//loop through all the data
		for(int i = 2; i < length; i++)
		{
			//if byte doesn't match
			if(data[i] != (*it)->data[i])
			{
				//if marker not set
				//set marker to current index
				if(marker == -1)
					marker = i;
			}
			//if bytes do match and marker isn't set
			else if(marker != -1)
			{
				//add the marker and the length
				//reset marker
				d->highlights.insert(pair<int, int>(marker, i - marker));
				marker = -1;
			}
		}

	}
}

int CDataText::GetTrueFirst(int start)
{
	int line_length = (8 + 1 + 8 + 2 + 8 + 1 + 8);
	int start1 = start % 32;
	if(start1 < 8)
		return start1;
	if(start1 >= 8 && start1 <= 15)
		return start1 + 1;
	if(start1 >= 15 && start1 <= 23)
		return start1 + 1 + 2;
	
	return start1 + 1 + 2 + 1;
}
int CDataText::GetTrueWidth(int start, int width)
{
	int line_length = (4 + 1 + 4 + 2 + 4 + 1 + 4);
	int start1 = start % 32;
	int len = width;
	if(2*width > 8 && 2*width <= 16)
		return 2*width + 1;
	if(2*width > 16 && 2*width <= 24)
		return 2*width + 1 + 2;

	return 2*width + 1 + 2 + 1;
}

void CDataText::Draw()
{
	CBox::Draw();

	if(text == "")
		return;

		int startx = posX;
		
		if(startx < 0)
			startx = 0;

		orig_glColor3f(font.color.red, font.color.green, font.color.blue);
		orig_glRasterPos2i(startx, posY + font.height + padding);
		for(int i = 0; i != 6; i++)
			glutBitmapCharacter(font.GetFontStyle(), text[i]);
		int y = posY + 2*font.height + padding;

		//highlights
		orig_glColor3f(1 - font.color.red, 1 - font.color.green, 1 - font.color.blue);
		orig_glRasterPos2i(startx, y);
		map<int,int>::iterator h_it;
		bool drawing_highlight = false;

		int box_x, box_y, box_width, box_height;
		int char_width = font.height * 0.5f;
		int line_length = (8 + 1 + 8 + 2 + 8 + 1 + 8);
		int continuing = 0;
		int first;
		int highlight_width;
		int truefirst, truewidth;
		int j = 0;

		DataPacket* pcurrent = current_packet;
		
		int size;
		if(pcurrent != NULL && pcurrent->highlights.size() != 0)
		{
			size = pcurrent->highlights.size();
		for(j = 0, h_it = pcurrent->highlights.begin(); h_it != pcurrent->highlights.end(); h_it++, j++)
		{ 
			do
			{
				if(j == 925)
					j = j;

				if(continuing)
				{
					first = 32 * ((h_it->first / 32) + continuing);
					highlight_width -= 32;
				}
				else
				{
					first = (*h_it).first;
					highlight_width = (*h_it).second;
				}

				truefirst = GetTrueFirst(first);
				box_x = posX + truefirst * char_width ;
				box_y = y + font.height * (first / line_length);
				truewidth =  GetTrueWidth(first, highlight_width);

				if(truefirst + truewidth > line_length) 
				{
					continuing++;
					box_width = (line_length - GetTrueFirst(first)) * char_width;
				}
				else
				{
					box_width = GetTrueWidth(first, highlight_width) * char_width;
					continuing = false;
				}
				box_height = font.height;

				orig_glBegin(GL_QUADS); 
				orig_glVertex2f(box_x, box_y ); 
				orig_glVertex2f(box_x + box_width, box_y); 
				orig_glVertex2f(box_x + box_width, box_y + box_height); 
				orig_glVertex2f(box_x, box_y + box_height); 
				orig_glEnd();

			}while(continuing);
		}
		}
		
		
		orig_glColor3f(font.color.red, font.color.green, font.color.blue);
		orig_glRasterPos2i(startx, y);
		for (int i = 6, x = 0; i != text.length(); i++) 
		{
			/*x += font.height * 0.5f;
			if(x >= width)
			{
				y += font.height;
				if(y >= height)
					break;
				x = 0;
				orig_glRasterPos2i(startx, posY + y);
			}*/
			if((i - 6) % (32 + 1 + 2 + 1) == 0)
			{
				y += font.height;
				if(y >= height)
					break;
				orig_glRasterPos2i(startx, y);
			}

			glutBitmapCharacter(font.GetFontStyle(), text[i]);
		}
		
}
