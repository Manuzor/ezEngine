#pragma once

#include <Core/Application/Application.h>
#include "Level.h"
#include <Core/Input/VirtualThumbStick.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Time/DefaultTimeStepSmoothing.h>

class SampleGameApp : public ezApplication
{
public:
  SampleGameApp();

  virtual void AfterEngineInit() override;
  virtual void BeforeEngineShutdown() override;

  virtual ezApplication::ApplicationExecution Run() override;



private:
  friend LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  void RenderSingleFrame();
  void UpdateInput(ezTime UpdateDiff);
  void RenderProjectiles();
  void RenderAsteroids();
  void RenderPlayerShips();

  void GameLoop();
  void SetupInput();
  void CreateGameLevel();
  void DestroyGameLevel();

  bool m_bActiveRenderLoop;
  Level* m_pLevel;
  class GameWindow* m_pWindow;

  ezDefaultTimeStepSmoothing m_TimeStepSmoother;
  ezVirtualThumbStick* m_pThumbstick;
  ezVirtualThumbStick* m_pThumbstick2;
};