#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	trolley(100, 400, "trolley.png", CColor::Green(), 0),
	front(40, 380, 40, 40, CColor::Red(), 0),
	rear(160, 380, 40, 40, CColor::Red(), 0)
{
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::UpdateWheel(CSprite &wheel)
{
	Uint32 t = GetTime();
	CVector normal;

	// Normal
	if (wheel.GetX() <= slopeW)
		normal = CVector(sin(alpha), cos(alpha)); // slope
	else
		normal = CVector(0, 1); // flat floor

	// gravity and reaction (normal force)
	CVector gravity(0, -10);
	wheel.Accelerate(gravity);
	wheel.Accelerate(-Dot(gravity, normal) * normal);

	// collision
	CVector xnormal(normal.m_y, -normal.m_x);
	wheel.SetVelocity(Cross(wheel.GetVelocity(), normal) * xnormal);

	wheel.Update(t);
}

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	// simulation!
	if (IsGameMode())
	{
		// Update the wheels
		UpdateWheel(front);
		UpdateWheel(rear);

		// Average the speed of the wheels
		float speed = (front.GetSpeed() + rear.GetSpeed()) / 2;
		front.SetSpeed(speed);
		rear.SetSpeed(speed);

		// span the trolley on top of the wheels
		CVector position = (front.GetPosition() + rear.GetPosition()) / 2;
		trolley.SetPosition(position);

		// setup the trolley rotation (based on the position of the wheels)
		CVector d = front.GetPosition() - rear.GetPosition();
		trolley.SetRotation(RAD2DEG(atan2(-d.m_y, d.m_x)));
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
	g->DrawLine(CVector(0, yFloor + slopeW * tan(alpha)), CVector(slopeW, yFloor), 3, CColor::Black());
	g->DrawLine(CVector(slopeW, yFloor), CVector(GetWidth(), yFloor), 3, CColor::Black());
	trolley.Draw(g);
	//front.Draw(g);
 	//rear.Draw(g);

	// draw messages
	*g << font(14);
	*g << bottom << "Version 5.0" << endl << endl;
	if (IsMenuMode())
		*g << "Drag the trolley and the triangular wedge, press SPACE when ready";
	else
		*g << "Simulating...";
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	trolley.SetPivotFromCenter(0, -20);
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	trolley.SetPos(100, 400);
	trolley.SetVelocity(0, 0);

	CVector normal(sin(alpha), cos(alpha));
	trolley.SetY(yFloor + (slopeW - trolley.GetX()) * tan(alpha));
	trolley.Move(normal * 20);
	trolley.SetRotation(RAD2DEG(alpha));

	front.SetVelocity(0, 0);
	rear.SetVelocity(0, 0);

	CVector xnormal(cos(alpha), -sin(alpha));
	front.SetPosition(trolley.GetPosition() + 60 * xnormal);
	rear.SetPosition(trolley.GetPosition() - 60 * xnormal);

	bIsTrolleyDragged = false;
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	bIsTrolleyDragged = false;
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		if (IsMenuMode())
			StartGame();
		else
			PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	if (bIsTrolleyDragged && x + xOffs + 60 < slopeW)
	{
		trolley.SetX(x + xOffs);
		
		CVector normal(sin(alpha), cos(alpha));
		trolley.SetY(yFloor + (slopeW - trolley.GetX()) * tan(alpha));
		trolley.Move(normal * 20);

		CVector xnormal(cos(alpha), -sin(alpha));
		front.SetPosition(trolley.GetPosition() + 60 * xnormal);
		rear.SetPosition(trolley.GetPosition() - 60 * xnormal);
	}
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (trolley.HitTest(CVector(x, y)))
	{
		bIsTrolleyDragged = true;
		xOffs = trolley.GetX() - x;
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	bIsTrolleyDragged = false;
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
