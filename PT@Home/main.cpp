//
//  main.cpp
//
//  Created by Erika Dains on 10/12/15.
//  Copyright (c) 2015 Erika Dains. All rights reserved.
//

#include "stdafx.h"
#include "Display.h"
//#include "DisplayMain.h"
#include "Controller.h"

using namespace std;


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Display currDisplay;
	// currDisplay.run();
	Controller control;
	control.runDisplay();

	return 0;
}
