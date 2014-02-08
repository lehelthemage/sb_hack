#include "opengl32.h"
#include "overlay.h"
#include <gl\glut.h>
#include <windows.h>



namespace overlay
{
	int CObject::id_counter = 0;
	CGUI gui;

	void CGUI::Render()
	{
		counter++;
		//if(viewportcount != 0)
		//	return;
		counter = 0;

		if(!showgui)
			return;

		GLfloat color[4];
		orig_glPushMatrix(); 
		orig_glLoadIdentity(); 
		orig_glPushAttrib(GL_ALL_ATTRIB_BITS);
		orig_glGetFloatv(GL_CURRENT_COLOR, color);
		orig_glDisable(GL_TEXTURE_2D);
		orig_glDisable(GL_DEPTH_TEST);

		std::list<CObject*>::iterator it;
		for(it = guilist.begin(); it != guilist.end(); it++)
		{
			if((*it)->Visible)
				(*it)->Draw();
		}

		orig_glColor4fv(color);  
		orig_glPopAttrib();
		orig_glPopMatrix(); 

	}

	void CGUI::OnSwapBuffer()
	{
		timer++;

		if((GetAsyncKeyState(VK_F1) < 0))
			keystates.F1down = true;
		else if(!(GetAsyncKeyState(VK_F1) < 0))
			keystates.F1down = false;

		if((GetAsyncKeyState(VK_F2) < 0))
			keystates.F2down = true;
		else if(!(GetAsyncKeyState(VK_F2) < 0))
			keystates.F2down = false;

		if((GetAsyncKeyState(VK_F3) < 0))
			keystates.F3down = true;
		else if(!(GetAsyncKeyState(VK_F3) < 0))
			keystates.F3down = false;

		if((GetAsyncKeyState(VK_F4) < 0))
			keystates.F4down = true;
		else if(!(GetAsyncKeyState(VK_F4) < 0))
			keystates.F4down = false;

		if((GetAsyncKeyState(VK_F5) < 0))
			keystates.F5down = true;
		else if(!(GetAsyncKeyState(VK_F5) < 0))
			keystates.F5down = false;

		if((GetAsyncKeyState(VK_F6) < 0))
			keystates.F6down = true;
		else if(!(GetAsyncKeyState(VK_F6) < 0))
			keystates.F6down = false;

		if((GetAsyncKeyState(VK_SHIFT) < 0))
			keystates.shiftdown = true;
		else if(!(GetAsyncKeyState(VK_SHIFT) < 0))
			keystates.shiftdown = false;

		if((GetAsyncKeyState(VK_TAB) < 0))
			keystates.tabdown = true;
		else if(!(GetAsyncKeyState(VK_TAB) < 0))
			keystates.tabdown = false;

		
		if((GetAsyncKeyState(VK_UP) < 0))
			keystates.uarrowdown = true;
		else if(!(GetAsyncKeyState(VK_UP) < 0))
			keystates.uarrowdown = false;

		if((GetAsyncKeyState(VK_DOWN) < 0))
			keystates.darrowdown = true;
		else if(!(GetAsyncKeyState(VK_DOWN) < 0))
			keystates.darrowdown = false;

		if((GetAsyncKeyState(VK_LEFT) < 0))
			keystates.larrowdown = true;
		else if(!(GetAsyncKeyState(VK_LEFT) < 0))
			keystates.larrowdown = false;

		if((GetAsyncKeyState(VK_RIGHT) < 0))
			keystates.rarrowdown = true;
		else if(!(GetAsyncKeyState(VK_RIGHT) < 0))
			keystates.rarrowdown = false;

		if((GetAsyncKeyState(VK_BACK) < 0))
			keystates.backspacedown = true;
		else if(!(GetAsyncKeyState(VK_BACK) < 0))
			keystates.backspacedown = false;

		for(int i = 0; i != 10; i++)
		{
			if((GetAsyncKeyState(0x30 + i) < 0))
				keystates.numbers[i] = true;
			else if(!(GetAsyncKeyState(0x30 + i) < 0))
				keystates.numbers[i] = false;
		}

		for(int i = 0; i != 6; i++)
		{
			if((GetAsyncKeyState(0x41 + i) < 0))
				keystates.letters[i] = true;
			else if(!(GetAsyncKeyState(0x41 + i) < 0))
				keystates.letters[i] = false;
		}

		std::list<CObject*>::iterator it;

		if(focused != NULL && keystates.tabdown == true && keystates.tabbed == false)
		{
			for(it = guilist.begin(); it != guilist.end(); it++)
			{
			 if(focused == *it)
			 {
				 focused->SetFocus(false);
				 it++;
				 if(it == guilist.end())
					 it = guilist.begin();
				 
				(*it)->SetFocus(true);
				focused = *it;

				 keystates.tabbed = true;
				 

				 break;
			 }
			}
		}
		else if(keystates.tabdown == false)
			keystates.tabbed = false;

		for(it = guilist.begin(); it != guilist.end(); it++)
			if((*it)->HasFocus())
				(*it)->OnTick(timer, keystates);
	
		if(keystates.F1down)
			pressedF1 = true;
		else if(!keystates.F1down && pressedF1)
		{
			pressedF1 = false;
			showgui = !showgui;
		}


		
	}

	void CBox::Draw()
	{
		orig_glEnable( GL_BLEND );
		orig_glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	
		
		orig_glColor4f(fillcolor.red, fillcolor.green, fillcolor.blue, fillcolor.opacity);
		
		orig_glBegin(GL_QUADS); 
		orig_glVertex2f(posX, posY); 
		orig_glVertex2f(posX + width, posY); 
		orig_glVertex2f(posX + width, posY + height); 
		orig_glVertex2f(posX, posY + height); 
		orig_glEnd(); 


		orig_glColor4f(bordercolor.red, bordercolor.green, bordercolor.blue, bordercolor.opacity);
		orig_glLineWidth( borderwidth );

		orig_glBegin(GL_LINE_LOOP);
		orig_glVertex2f(posX, posY); 
		orig_glVertex2f(posX + width, posY); 
		orig_glVertex2f(posX + width, posY + height); 
		orig_glVertex2f(posX, posY + height); 
		orig_glEnd();

		orig_glDisable( GL_BLEND );
	}

	void CText::Draw()
	{
		CBox::Draw();

		int startx = posX;
		if(justification == Center)
			startx = posX + (width - text.length()*font.height * 0.5f)/2;

		if(startx < 0)
			startx = 0;

		orig_glColor3f(font.color.red, font.color.green, font.color.blue);
		orig_glRasterPos2i(startx, posY + font.height + padding);
		for (int i = 0, x = 0, y = font.height; i != text.length(); i++) 
		{
			x += font.height * 0.5f;
			if(x >= width)
			{
				y += font.height;
				if(y >= height)
					break;
				x = 0;
				orig_glRasterPos2i(startx, posY + y);
			}

			glutBitmapCharacter(font.GetFontStyle(), text[i]);
		}
		
	}

	void CTitleBox::Draw()
	{
		CBox::Draw();
		titlebox.Draw();
	}

	void CTextBox::Draw()
	{
		CTitleBox::Draw();
		text.Draw();
	}

	void CMenu::Draw()
	{
	}

	int CMenu::AddItem(char* pszText)
	{
		return 0;
	}

	void CMenu::RemoveItem(int i)
	{
	}

	void CInputBox::Draw()
	{
		CText::Draw();
	}

	void CInputBox::OnTick(int ticker, const KeyStates& ks)
	{
		if(ks.backspacedown && backspacedown == false)
		{
			backspacedown = true;
			std::string s = GetText();
			SetText(s.substr(0, s.size() - 1));
		}
		else if(!ks.backspacedown)
			backspacedown = false;

		for(int i = OL_0; i <= OL_9; i++)
		{
			if(ks.numbers[i] && !numberdown[i])
			{
				numberdown[i] = true;
				SetText(GetText() + (char)((char)'0'+ (char)i));
			}
			else if(!ks.numbers[i])
				numberdown[i] = false;
		}

		for(int i = OL_A; i <= OL_F; i++)
		{
			if(ks.letters[i] && !letterdown[i])
			{
				letterdown[i] = true;
				SetText(GetText() + (char)((char)'A' + (char)i));
			}
			else if(!ks.letters[i])
				letterdown[i] = false;
		}
	}

}
