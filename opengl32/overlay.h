#pragma once

#define _USING_OPENGL

#include <list>
#include <string>
#include <gl\glut.h>
 
void DrawBox(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
void DrawMenu( void );
void glPrint1(int x, int y, char *string, GLfloat red, GLfloat green, GLfloat blue,void *font);

namespace overlay
{

#ifdef _USING_OPENGL
#define _MAX_HUE 1.0f
#define _NULL_HUE -1.0f
#else
#endif

class Color 
{
public:
	float red;
	float green;
	float blue;
	float opacity;
	Color() :  red(0), green(0), blue(0), opacity(1.0f) {}
	Color(const Color& c) {blue = c.blue; green = c.green; red = c.red; opacity = c.opacity;}
	Color(float _red, float _green, float _blue, float _opacity = 1.0f) : red(_red), green(_green), blue(_blue), opacity(_opacity) {}
	
};


const Color green(0.0f, _MAX_HUE, 0.0f); 
const Color black(0.0f,0.0f,0.0f);
const Color nullcolor(_NULL_HUE, _NULL_HUE, _NULL_HUE);


typedef void* GLFONT;
typedef void* DXFONT;


class Font
{
public:
	enum FontStyle
	{
		TimesNewRoman10,
		TimesNewRoman24,
		Helvetica10,
		Helvetica12,
		Helvetica18
	}fontstyle;

	int height;
	Color color;
	Font() :  color(green){SetFontStyle(TimesNewRoman10);} 
	Font(FontStyle f, const Color& c) : color(c) {SetFontStyle(f);}
	Font(const Font& f) : color(f.color) {SetFontStyle(f.fontstyle);}
	void SetFontStyle(FontStyle f) {
		fontstyle = f;

		switch(fontstyle) {
		default:
		case Helvetica10:
		case TimesNewRoman10:
			height = 10;
			break;
		case TimesNewRoman24:
			height = 24;
			break;
		case Helvetica12:
			height = 12;
			break;
		case Helvetica18:
			height = 18;
			break;
		}
	}

#ifdef _USING_OPENGL
	GLFONT 
		GetFontStyle() {
		switch(fontstyle) {
		default:
		case TimesNewRoman10:
			return GLUT_BITMAP_TIMES_ROMAN_10;
		case TimesNewRoman24:
			return GLUT_BITMAP_TIMES_ROMAN_24;
		case Helvetica10:
			return GLUT_BITMAP_HELVETICA_10;
		case Helvetica12:
			return GLUT_BITMAP_HELVETICA_12;
		case Helvetica18:
			return GLUT_BITMAP_HELVETICA_18;
		}
	}
#else
	DXFONT GetFont() {return NULL;}
#endif
};

class CGUI;
extern CGUI gui;

struct KeyStates
	{
		bool F1down, F2down, F3down, F4down, F5down, F6down, shiftdown, uarrowdown, darrowdown, larrowdown, rarrowdown;
	};

class CObject
{
private:
	int id;
	static int id_counter;
	bool hasfocus;
protected:
	float posX, posY, width, height;
public:
	CObject(float x, float y, float _width, float _height) : posX(x), posY(y), width(_width), height(_height), hasfocus(false), Visible(true) { id = ++id_counter;}
	virtual void Draw() = 0;
	virtual void OnTick(int ticker, const KeyStates& ks) = 0;
	int GetId() const {return id;}
	bool HasFocus() {return hasfocus;}
	bool Visible;
friend class CGUI;
};


class CGUI
{

	
private:
	std::list<CObject*> guilist;
	CObject* focused;
	int viewportcount;
	KeyStates keystates;
	unsigned long timer;
	bool showgui;
	int counter;
	bool pressedF1;
public:
	CGUI() : focused(NULL), counter(0), timer(0), viewportcount(0), showgui(false), pressedF1(false) {::ZeroMemory(&keystates, sizeof(KeyStates));}
	void Add(CObject* pGuiObject) { guilist.push_back(pGuiObject); }
	void Remove(CObject* pGuiObject) { guilist.remove(pGuiObject); }
	void Remove(int id) {
		std::list<CObject*>::iterator it;
		for(it = guilist.begin(); it != guilist.end(); it++)
		{
			if((*it)->id == id)
			{
				guilist.erase(it);
				delete *it;
				return;
			}
		}
	}
	void Render();
	
	void OnViewport() {
		viewportcount++;
		if(viewportcount == 5)
			viewportcount = 0;
	}

	void OnSwapBuffer(); 


	~CGUI() {
		std::list<CObject*>::iterator it;
		for(it = guilist.begin(); it != guilist.end(); it++)
			delete *it;
	}

	void ShiftFocus() {
		std::list<CObject*>::iterator it;
		if(focused == NULL)
			focused = *guilist.begin();
		else
		{
			for(it = guilist.begin(); it != guilist.end(); it++)
			{
				if(*it == focused)
				{
					it++;
					if(it == guilist.end())
						it = guilist.begin();

					focused->hasfocus = false;
					focused = *it;
					break;
				}
			}
		}

		focused->hasfocus = true;
	}

};



class CBox : public CObject
{
protected:
	Color fillcolor, bordercolor;
	float borderwidth;
public:
	CBox(float x, float y, float _width, float _height, const Color& _fillcolor = nullcolor, const Color& _bordercolor = nullcolor, float _borderwidth = 1.0f) : CObject(x, y, _width, _height), fillcolor(_fillcolor), bordercolor(_bordercolor), borderwidth(_borderwidth)
	{
	}
	virtual void Draw();
	virtual void OnTick(int ticker, const KeyStates& ks) {}
	void SetBorderColor(const Color& c) {bordercolor = c;}
	void SetFillColor(const Color& c) {fillcolor = c;}
};

class CText : public CBox
{
public:
	enum Justification
	{
		Left,
		Right,
		Center
	}justification;
protected:
	Font font;
	std::string text;
	std::wstring wtext;
	bool usewide;
	int linemax;
	
	static const int nomax = -1;
public: static const int padding = 2;
public:
	CText(float x, float y, float _width, float _height, const Color& bgcolor, const Font& _font, char* pszText, Justification _justification = Left) : justification(_justification), CBox(x, y, _width, _height, bgcolor, bgcolor), font(_font), text(pszText), linemax(nomax), usewide(false) {}
	virtual void Draw();
	virtual void SetText(const std::string& _text) { usewide = false; text = _text; }
	virtual void SetText(WCHAR* pwszText) {  usewide = true; wtext = pwszText; }
	void SetLineMax(int max) {linemax = max;}

};

class CTitleBox : public CBox
{
protected:
	CText titlebox;
public:
	CTitleBox(float x, float y, float _width, float _height, const Font& titlefont, char* pszTitle, const Color& _bordercolor = nullcolor, const Color& _fillcolor = nullcolor, const Color& titlebg = black) : CBox(x, y, _width, _height, _fillcolor, _bordercolor), titlebox(x, y, _width, titlefont.height + 2*CText::padding, titlebg, titlefont, pszTitle, CText::Center) {}
	virtual void Draw();
};

class CTextBox : public CTitleBox
{
protected:
	CText text;
public:
	CTextBox(float x, float y, float _width, float _height, const Font& titlefont, char* pszTitle, const Font& textfont, char* pszText, const Color& _bordercolor = nullcolor, const Color& _fillcolor = nullcolor, const Color& titlebg = black) : CTitleBox(x, y, _width, _height, titlefont, pszTitle, _bordercolor, _fillcolor, titlebg), text(x, y + titlefont.height + 2*CText::padding, _width, _height - (titlefont.height + 2*CText::padding), _fillcolor, textfont, pszText) {}
	virtual void Draw();
	void SetTitleText(const std::string& title) { titlebox.SetText(title); }
	virtual void SetText(const std::string& _text) { text.SetText(_text); }
	virtual void SetText(WCHAR* pwszText) { text.SetText(pwszText); }
};


class CMenu : public CTitleBox
{
protected:
	std::list<CText*> items;
	Color texthighlight, bghighlight;
	int selecteditem;
	static const int notselected = -1;
public:
	CMenu(float x, float y, float _width, float _itemheight, char* pszTitle, const Font& titlefont, const Font& itemfont, const Color& _bordercolor = nullcolor, const Color& _fillcolor = nullcolor, const Color& titlebg = black) 
		: CTitleBox(x, y, _width, _itemheight, titlefont, pszTitle, _bordercolor, _fillcolor, titlebg), selecteditem(notselected) {}  
	virtual void Draw();
	int AddItem(char* pszText);
	int GetSelectedItem() { return selecteditem; }
	void RemoveItem(int i);
};

}