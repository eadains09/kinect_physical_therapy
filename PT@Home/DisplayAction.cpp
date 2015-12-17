//
// DisplayAction.cpp
//

#include <Kinect.h>
#include <quaternion.h>
#include <time.h>
#include "DisplayAction.h"
#include "DisplayMain.h"
#include "DisplayPatientMenu.h"
#include "DisplayPhysicianMenu.h"
#include "stdafx.h"
#include "QuatFrame.h"

int ActionDisplay::saveCount = 0;

ActionDisplay::ActionDisplay() : DisplayBase() {
	playback = LIVE;
	bodyCount = 1;
	prevScreen = DISPLAY_MAIN;
	playbackFile = "";
	comparisonOn = false;
	exerciseCount = 0;
	constructUniversalActionDisplay();
}

ActionDisplay::ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d) : DisplayBase(c, w, r) {
	playback = p;
	prevScreen = d;
	playbackFile = "";

	if (playback == LIVE) {
		bodyCount = 1;
		comparisonOn = false;
		exerciseCount = 0;
		playCount = 1;
	}
	else if (playback == RECORDED) {
		bodyCount = 1;
		comparisonOn = false;
		exerciseCount = 1;
		playCount = 1;
		playbackFile = "";//for completeness, this clause should never be enetered
	}
	else { /* LIVE_RECORD */
		//simultaneous playback
		bodyCount = 2;
		comparisonOn = true;
		//TODO if LIVE_RECORD, circle through all exercises for playback (because if it were a single exercise, other constructor would've been used)
		//playbackFile = getNextFile?
		playbackFile = "";
		//exerciseCount = getNumFiles in movement folder
		playCount = 2;
	}

	constructUniversalActionDisplay();

}

ActionDisplay::ActionDisplay(Controller *c, SDL_Window *w, SDL_Renderer *r, PlaybackType p, DisplayType d, string pfile) : DisplayBase(c, w, r) {
	playback = p;
	prevScreen = d;
	playbackFile = pfile;

	if (playback == LIVE) {
		bodyCount = 1;
		comparisonOn = false;
		exerciseCount = 0;
		playCount = 1;
	}
	else if (playback == RECORDED) {
		bodyCount = 1;
		comparisonOn = false;
		exerciseCount = 1;
		playCount = 1;
	}
	else {
		bodyCount = 2;
		comparisonOn = true;
		exerciseCount = 1;
		playCount = 2;
	}

	constructUniversalActionDisplay();
}

void ActionDisplay::constructUniversalActionDisplay() {
	frameNumber = 0;
	keyframeCaptured = false;
	playing = true;
	playFileName = trimAddress(playbackFile);
	pauseTime = 0;
	sysPaused = false;
	firstFrame = true;
	pauseCount = 0;

	displayBodies = new BodyFrame*[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayBodies[i] = new BodyFrame();

	displayQuats = new QuatFrame*[TOTAL_BODIES];
	for (int i = 0; i < TOTAL_BODIES; i++)
		displayQuats[i] = new QuatFrame();

	log.open("logData.txt");
}


string ActionDisplay::trimAddress(string pfile) {
	size_t found = pfile.find_last_of("\\");
	return pfile.substr(found + 1);
}

bool ActionDisplay::init() {
	bool success = true;
	IBodyFrameSource* pBodyFrameSource = NULL;

	GetDefaultKinectSensor(&m_pKinectSensor);
	if (playback == LIVE || playback == LIVE_RECORD)
	{
		if (!m_pKinectSensor ||
			!SUCCEEDED(m_pKinectSensor->Open()) ||
			!SUCCEEDED(m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper)) ||
			!SUCCEEDED(m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource)) ||
			!SUCCEEDED(pBodyFrameSource->OpenReader(&m_pBodyFrameReader)))
		{
			log << "Could not find a connected kinect\n" << std::endl;
			return false;
		}
		SafeRelease(pBodyFrameSource);
	}

	return success;
}

void ActionDisplay::run() {
	if (!init()) {
		printf("Failed to initialize Kinect!\n");
		exit(0);
	}

	if(!loadMedia()) {
		printf("Failed to load media!\n");
		exit(0);
	}

	SDL_Delay(2000);

	quit = false;
	SDL_Event e;

	GetLocalTime(&granularPrevTime);

	sysPaused = true;
	pauseCount = 1;
	updatePauseTime(sysPaused);

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				handleKeyPresses(e);
			} else {
            	for (size_t i = 0; i < gButtons.size(); i++) {
					handleButtonEvent(&e, gButtons.at(i));
            	}
            }
		}
		
		if (!quit) {
			renderScreen();
		}
	}
}

double granularDiff(const SYSTEMTIME& from, const SYSTEMTIME& to)
{
	double result = 0;
	double temp;
	temp = abs(from.wHour - to.wHour);
	result += temp * 3600000;
	temp = abs(from.wMinute - to.wMinute);
	result += temp * 60000;
	temp = abs(from.wSecond - to.wSecond);
	result += temp * 1000;
	temp = abs(from.wMilliseconds - to.wMilliseconds);
	result += temp;
	return result;
}

bool ActionDisplay::renderScreen() {
	double elapsedTime;
	int bitField = 0;
	bool endOfFrame = false;

	if (playing )//&& playCount > 0) {
	{
		if (playback == LIVE || playback == LIVE_RECORD) {
			frameFromKinect();
		}

		if (playback == RECORDED || (playback == LIVE_RECORD && !sysPaused))
		{
			if (getSingleFrameFromFile())//elapsedTime))//we have reached the end of the keyFrame
			{
				pauseTime = 0;
				frameNumber++;
				endOfFrame = true;
				
				if (frameNumber == moveFromFile->getCurrFrameCount())//we have reached the end of the file
				{
					//frameNumber = 0;
					//playCount--;
					//sysPaused = true;
					//pauseCount = 1;
					//updatePauseTime(sysPaused);
					log.open("testPlayback.txt", std::ofstream::app);
					log << "end of file, restarting frame Number == 0" << endl;
					log.close();
				}
			}
		}
		if (playback == LIVE_RECORD )//&& playCount == 1)
		{

			//Compare whether joints match
			bitField = displayQuats[0]->compare(displayQuats[bodyCount - 1]);

			//super simple way to wait for patient to catch up:

			if (endOfFrame || sysPaused)
			{
				int sum = 0;
				for (int i = 0; i < JOINT_TOTAL; i++)
					if ((1<<i)&bitField)
						if(tracking(i))
							sum++;
				if (sum > 2) {
					sysPaused = true;
					if (pauseCount == 0)
					{
						updatePauseTime(sysPaused);
						pauseCount++;
					}
				}
				else {
					sysPaused = false;
					if (pauseCount == 1) {
						updatePauseTime(sysPaused);
						pauseCount = 0;
					//	frameNumber = 0;
					}
				}
			}
		}
	}
	
	renderFrame(bitField);

	return true;
}

bool ActionDisplay::renderFrame(int bitField) {
	int i;
    int colorArray[2] = {0x00, 0xFF};

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);


	instructionTexture = SDL_CreateTextureFromSurface(renderer, instructionSurface);
	SDL_RenderCopy(renderer, instructionTexture, NULL, &instructionDestR);
	SDL_DestroyTexture(instructionTexture);
    
    renderButtons();
 
    //render bodies
	for (i = 0; i < bodyCount; i++) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i % 2], 0xFF);
		renderBody(displayQuats[i], bitField, colorArray[i%2]);
	}

	if (keyframeCaptured) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, colorArray[i % 2], 0xFF);
		renderBody(keyframes.getBackFrame(), bitField, colorArray[i%2]);
	}
	
	/*if (comparisonOn && playCount == 1) {
		SDL_FreeSurface(instructionSurface);
		if (errors.size() > 0) {
			std::set<int>::iterator iter;
			string error_text = "Adjust your ";
			iter = errors.begin();
			while(iter != errors.end()) {
				error_text = error_text + getJointString(*iter);
				errors.erase(iter);
				iter = errors.begin();
				if (errors.size() > 1) {
					error_text = error_text + " , ";
				}
				else if (errors.size() == 1) {
					error_text = error_text + " & ";
				}
			}
			SDL_Color textColor = { 255, 0, 0 };
			loadText(error_text, textColor);
		}
		else {
			SDL_Color textColor = { 0, 0, 0 };
			loadText("Doing good!", textColor);
		}
	} */

    SDL_RenderPresent(renderer);

    return true;
}

void ActionDisplay::renderBody(QuatFrame *currQuatBody, int bitField, int color) {
	BodyFrame *currBody = new BodyFrame();
	currQuatBody->initBodyFrame(currBody);

	//this causes memory leak, I guess to_string requires weird stuff to
	//not leak, but it seems like debugging a debug statement is something 
	//I can do when there's nothing else to do
	std::string quatStamp = std::to_string(frameNumber);
	SDL_Color textColor = { 255, 0, 0 };
	loadText(quatStamp, textColor);

	log.open("testPlayback.txt", std::ofstream::app);
	log << "in render body frame Number == " << frameNumber << endl;
	log.close();
	
	irr::core::vector3df **joints = (*currBody).getJoints();

	for (int i = 0; i < (*currBody).getCurrJointCount(); i++) {
		if (getParent(i) != i) {
			if (comparisonOn && tracking(i) && (getBit(bitField, i) || getBit(bitField, getParent(i)))) {
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0xFF);
				if (getBit(bitField, i)) {
					errors.insert(i);
				}
			} 
			else {
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, color, 0xFF);
			}
			SDL_RenderDrawLine(renderer, joints[i]->X, joints[i]->Y, joints[getParent(i)]->X, joints[getParent(i)]->Y);
		}
	} 

	delete currBody;
}

bool ActionDisplay::frameFromKinect()
{
	HRESULT hr;
	int j;
	QuatFrame *anorexia = NULL;
	IBodyFrame* pBodyFrame = NULL;

	if (!m_pBodyFrameReader)
		return false;

	hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (!pBodyFrame || !SUCCEEDED(hr))
		return false;

	Joint *joints = new Joint[JointType_Count];
	IBody* ppBodies[BODY_COUNT] = { 0 };

	hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
	//check?


	for (j = 0; j < _countof(ppBodies); j++)
	{
		BOOLEAN bTracked = false;
		hr = ppBodies[j]->get_IsTracked(&bTracked);

		if (!SUCCEEDED(hr) || !bTracked)
			continue;

		ppBodies[j]->GetJoints(JointType_Count, joints);
		anorexia = new QuatFrame();

		for (int i = 0; i < JointType_Count; i++)
		{
			//provide quatFrame with flipped image so it will display
			//properly on sdl, or stop scaling in QuatFrame and 
			//transform points afterwards
			anorexia->addJoint(joints[i].Position.X, -joints[i].Position.Y, joints[i].Position.Z);
		}
	
		delete displayQuats[bodyCount - 1];
		displayQuats[bodyCount - 1] = anorexia;

		if (playback == LIVE) {
			//displayQuats[bodyCount - 1].setMidSpine(200, 200); 
		}
		else if (playback == RECORDED) {
			//displayQuats[bodyCount - 1].setMidSpine(400, 200); 
		}
		else {
			//displayQuats[bodyCount - 1].setMidSpine(600, 200); 
		}

		break; //once we've set the one entry in displayBodies we are alotted
		//we are done no matter what
	}

	//default just in case no bodies are being tracked
	//TODO make sure default behaves like it is supposed to
	//Erika: Isn't this covered in our constructor now, so should we delete it?
	if (j == BODY_COUNT || anorexia == NULL)
	{
		delete displayQuats[bodyCount - 1];
		displayQuats[bodyCount - 1] = new QuatFrame();
	}

	SafeRelease(pBodyFrame);

	for (int i = 0; i < _countof(ppBodies); i++)
	{
		SafeRelease(ppBodies[i]);
	}

	delete[] joints;
	return true;
}

//returns whther or not we have just reached the end of a keyframe
bool ActionDisplay::getSingleFrameFromFile() {
	double seconds;
	int diff;

	GetLocalTime(&granularCurrent);

	diff = granularDiff(granularCurrent, granularPrevTime);
	if (diff > pauseTime)
		seconds = granularDiff(granularCurrent, granularPrevTime) - pauseTime;
	else
		seconds = diff;
		//seconds = 0;
//	granularPrevTime = granularCurrent;

	QuatFrame *preview = moveFromFile->getSingleFrame(frameNumber, seconds);

	if (preview->isReady())
	{
		delete displayQuats[0];
		displayQuats[0] = preview;
		//std::string quatStamp = std::to_string(seconds);
		//SDL_Color textColor = { 255, 0, 0 };
		//loadText(quatStamp, textColor);
	}
	else
	{
		GetLocalTime(&granularPrevTime);
		delete preview;
		return true;
	}
	//displayQuats[0].setMidSpine(200, 200); 

	return false;
}

void ActionDisplay::handleKeyPresses(SDL_Event e) {
	if (playback == LIVE) {
		switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				captureKeyframe();
				break;
			case SDLK_d:
				deleteLastKeyframe();
				break;
			case SDLK_s:
				//save current contents of stack to file
				saveKeyframes();
				break;
			case SDLK_BACKSPACE:
				//go back a screen
				loadPrevDisplay();
				break;
		}
	} else {
		switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				togglePlaying();
				break;
			case SDLK_BACKSPACE:
				//go back a screen
				loadPrevDisplay();
				break;
		}
	}
}

void ActionDisplay::handleButtonEvent(SDL_Event* e, Button *currButton)
{
	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Mouse is inside button
		if ((*currButton).isInside(e))
		{
			if (playback == LIVE) {
				switch ((*currButton).getType()) {
					case BUTTON_SPRITE_ADD:
						captureKeyframe();
						break;
					case BUTTON_SPRITE_DELETE:
						deleteLastKeyframe();
						break;
					case BUTTON_SPRITE_SAVE:
						saveKeyframes();
						break;
					case BUTTON_SPRITE_BACK:
						loadPrevDisplay();
						break;
					default:
						break;
				}
			} else {
				switch ((*currButton).getType()) {
					case BUTTON_SPRITE_PLAY:
						togglePlaying();
						break;
					case BUTTON_SPRITE_PAUSE:
						togglePlaying();
						break;
					case BUTTON_SPRITE_BACK:
						loadPrevDisplay();
						break;
					default:
						break;
				}
			}
			buttonLog.open("buttonLogData.txt", std::ofstream::app);
			buttonLog << (*currButton).getType() << " button clicked" << std::endl;
			buttonLog.close();
		}
	}
}

void ActionDisplay::captureKeyframe() {
	double seconds;
	keyframeCaptured = true;

	GetLocalTime(&granularCurrent);

	if (granularPrevTime.wYear != 0)
		seconds = granularDiff(granularCurrent, granularPrevTime)-pauseTime;
	else
		seconds = 0;

	granularPrevTime = granularCurrent;



//	frameFromKinect();

	QuatFrame *prevKeyframe = new QuatFrame(*displayQuats[bodyCount - 1]);
	//prevKeyframe.setMidSpine(600, 200); 
	prevKeyframe->setTimestamp(seconds);
	keyframes.pushBackFrame(new QuatFrame(*prevKeyframe));
	delete prevKeyframe;
}

void ActionDisplay::deleteLastKeyframe() {
	time_t currTime;

	//pop most recent from stack
	if (keyframes.getCurrFrameCount() > 0) {
		keyframes.popBackFrame();
	}

	if (keyframes.getCurrFrameCount() <= 0) {
		keyframeCaptured = false;
	}
	
	if (keyframeCaptured) {
		GetLocalTime(&granularCurrent);
		granularPrevTime = granularCurrent;
	}
}

void ActionDisplay::saveKeyframes() {
	saveCount++;
	string defaultFilename = "movements/testMovement" + to_string(saveCount);

	initFileSelector();
	SetCurrentDirectory("./movements");
	if (GetSaveFileName(&saveFile)) {
		keyframes.logFrames(saveFile.lpstrFile);
		keyframeCaptured = false;
		prevTime = NULL;
	}

	SetCurrentDirectory("..");
}

void ActionDisplay::initFileSelector() {
	ZeroMemory(&saveFile, sizeof(saveFile));
	saveFile.lStructSize = sizeof(saveFile);
	saveFile.hwndOwner = NULL;
	saveFile.lpstrFile = szFile;
	saveFile.lpstrFile[0] = '\0';
	saveFile.nMaxFile = sizeof(szFile);
	saveFile.lpstrFilter = "movement(*.dat)\0*.dat\0";
	saveFile.nFilterIndex = 1;
	saveFile.lpstrFileTitle = NULL;
	saveFile.nMaxFileTitle = 0;
	saveFile.lpstrInitialDir = NULL;
	saveFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void ActionDisplay::loadPrevDisplay() {
	if (prevScreen == PATIENT_MENU) {
		newDisplay = new PatientMenuDisplay(control, window, renderer);
	} else if (prevScreen == PHYSICIAN_MENU) {
		newDisplay = new PhysicianMenuDisplay(control, window, renderer);
	} else {
		newDisplay = new DisplayMain(control, window, renderer);
	}
	loadNewDisplay();
}

void ActionDisplay::updatePauseTime(bool currPlay) {
	SYSTEMTIME pauseLocal;

	if (currPlay) {
		//Program is currently playing movement and user has decided to pause it
		GetLocalTime(&granularBeginPauseTime);
	}
	else {
		//Program is currently paused and user has decided to resume playing
		GetLocalTime(&pauseLocal);
		pauseTime += granularDiff(granularBeginPauseTime, pauseLocal);
	}

//	frameNumber--;

}

void ActionDisplay::togglePlaying() {
	//pause time related stuff should stay the same
	SYSTEMTIME pauseLocal;

	updatePauseTime(playing);

	//swap playing variable: if true, becomes false, if false becomes true
	playing = !playing;

	for (size_t i = 0; i < gButtons.size(); i++) {
		Button* currButton = gButtons.at(i);
		if ((*currButton).getType() == BUTTON_SPRITE_PLAY) {
			(*currButton).freeButton();
			gButtons.erase(gButtons.begin() + i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PAUSE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/pause.bmp"));
		} else if ((*currButton).getType() == BUTTON_SPRITE_PAUSE) {
			(*currButton).freeButton();
			gButtons.erase(gButtons.begin() + i);
			gButtons.push_back(new Button(BUTTON_SPRITE_PLAY, (SCREEN_WIDTH-BUTTON_WIDTH)-10, 10, "art/PlaybackButtons/play.bmp"));
		}
	}
}

bool ActionDisplay::loadMedia() {
    bool success = true;
	string text = "";

    moveFromFile = new Movement();
    

	if (playback == RECORDED || playback == LIVE_RECORD) {
		if (playbackFile.length() != 0)
			moveFromFile->readKeyframes(playbackFile);
		else
		{
			WIN32_FIND_DATA fdFile;
			HANDLE hFind = NULL;

			char sPath[2048];
			sprintf_s(sPath, "%s\\*.dat", "./movements/");


			if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
			{
				printf("Path not found: [%s]\n", "./movements/");
				return false;
			}

			do
			{
				//Find first file will always return "."
				//    and ".." as the first two directories.
				if (strcmp(fdFile.cFileName, ".") != 0
					&& strcmp(fdFile.cFileName, "..") != 0)
				{
					//Build up our file path using the passed in
					//  [sDir] and the file/foldername we just found:
					sprintf_s(sPath, "%s\\%s", "./movements/", fdFile.cFileName);

					//Is the entity a File or Folder?
					if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
					{
						continue;
					}
					else {
						moveFromFile->readKeyframes(sPath);
					}
				}
			} while (FindNextFile(hFind, &fdFile)); //Find the next file.

			FindClose(hFind); //Always, Always, clean things up!

		}

		/* If loaded file was empty, getCurrFrameCount will be 0
		 * In that case, subtract one from bodyCount so that only
		 * the kinect body or no body will be rendered
		 */
		//TODO: test that this works correctly
		 if (moveFromFile->getCurrFrameCount() <= 0) {
			 bodyCount--;
		 }
	}

	currFont = TTF_OpenFont("./freefont-ttf/fonts/FreeMono.ttf", 16);
	if (currFont == NULL) {
		log << printf("Failed to load font. SDL_ttf Error: %s\n", TTF_GetError()) << std::endl;
		success = false;
	}
	else if (playback == LIVE) {
		text = "Press + button or space bar to capture keyframe.\nPress x button or d key to delete last keyframe.\nPress checkmark button or s key to save to movement file.\nPress back arrow or backspace key to go to previous screen.";
	}
	else if (playback == RECORDED) {
		text = "     Playing " + playFileName + ".";
	}
	else if (playback == LIVE_RECORD) {
		text = "     Beginning workout! Exercise will play once for you to see, then will start tracking you!";
	}
	SDL_Color textColor = { 0, 0, 0 };
	success = success && loadText(text, textColor);

	success = success && loadButtons(); // Must load buttons after font, as height of font determines y position of buttons

    return success;
}

bool ActionDisplay::loadText(std::string textureText, SDL_Color textColor) {
	bool success = true;
	instructionSurface = TTF_RenderText_Blended_Wrapped(currFont, textureText.c_str(), textColor, 630);
	if (instructionSurface == NULL) {
		log << printf("Unable to render text to surface. SDL_ttf Error: %s\n", TTF_GetError()) << std::endl;
		success = false;
	}
	
	instructionDestR.h = instructionSurface->h;
	instructionDestR.w = instructionSurface->w;
	instructionDestR.x = 60;
	instructionDestR.y = 10;

	return success;
}

bool ActionDisplay::loadButtons() {
	int yPos;
	if (instructionDestR.h >= 30) {
		yPos = (instructionDestR.h / 2) - 20;
	}
	else {
		yPos = 10;
	}
	gButtons.push_back(new Button(BUTTON_SPRITE_BACK, 10, yPos, "art/back.bmp"));
	if (playback == LIVE) {
		loadKeyframeButtons(yPos);
	} else {
		loadPlaybackButtons(yPos);
	}
	
	return true;
}

void ActionDisplay::loadKeyframeButtons(int yPos) {
	gButtons.push_back(new Button(BUTTON_SPRITE_ADD, (SCREEN_WIDTH-BUTTON_WIDTH*3)-30, yPos, "art/KeyframeButtons/add.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_DELETE, (SCREEN_WIDTH-BUTTON_WIDTH*2)-20, yPos, "art/KeyframeButtons/delete.bmp"));
	gButtons.push_back(new Button(BUTTON_SPRITE_SAVE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, yPos, "art/KeyframeButtons/save.bmp"));
}

void ActionDisplay::loadPlaybackButtons(int yPos) {
	gButtons.push_back(new Button(BUTTON_SPRITE_PAUSE, (SCREEN_WIDTH-BUTTON_WIDTH)-10, yPos, "art/PlaybackButtons/pause.bmp"));
}

void ActionDisplay::close() {
	if(playback == RECORDED || playback == LIVE_RECORD) {
		delete moveFromFile;
	}

	SDL_FreeSurface(instructionSurface);
	SDL_DestroyTexture(instructionTexture);

	closeButtons();

	// done with body frame reader
	SafeRelease(m_pBodyFrameReader);

	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}

bool ActionDisplay::tracking(int i) {
	switch (i) {
	case JointType_HandTipLeft:
	case JointType_ThumbLeft:
	case JointType_HandTipRight:
	case JointType_ThumbRight:
	case JointType_FootLeft:
	case JointType_FootRight:
		return false;
	default:
		return true;
	}
}


string ActionDisplay::getJointString(int type) {
	switch (type) {
	case JointType_SpineShoulder:
		return "upper back";
	case JointType_SpineBase:
		return "lower back";
	case JointType_SpineMid:
		return "mid back";
	case JointType_Neck:
		return "neck";
	case JointType_ShoulderLeft:
		return "left shoulder";
	case JointType_ShoulderRight:
		return "right shoulder";
	case JointType_HipLeft:
		return "left hip";
	case JointType_HipRight:
		return "right hip";
	case JointType_HandTipLeft:
		return "left hand tip";
	case JointType_ThumbLeft:
		return "left thumb";
	case JointType_HandTipRight:
		return "right hand tip";
	case JointType_ThumbRight:
		return "right thumb";
	case JointType_Head:
		return "head";
	case JointType_ElbowLeft:
		return "left elbow";
	case JointType_WristLeft:
		return "left wrist";
	case JointType_HandLeft:
		return "left hand";
	case JointType_ElbowRight:
		return "right elbow";
	case JointType_WristRight:
		return "right wrist";
	case JointType_HandRight:
		return "right hand";
	case JointType_KneeLeft:
		return "left knee";
	case JointType_AnkleLeft:
		return "left ankle";
	case JointType_FootLeft:
		return "left foot";
	case JointType_KneeRight:
		return "right knee";
	case JointType_AnkleRight:
		return "right ankle";
	case JointType_FootRight:
		return "right foot";

	default:
		return "";
	}
}

int getParent(int type) {
	switch (type) {
	case JointType_SpineShoulder:
	case JointType_SpineBase:
	case JointType_SpineMid:
		return JointType_SpineMid;
	case JointType_Neck:
	case JointType_ShoulderLeft:
	case JointType_ShoulderRight:
		return JointType_SpineShoulder;
	case JointType_HipLeft:
	case JointType_HipRight:
		return JointType_SpineBase;
	case JointType_HandTipLeft:
	case JointType_ThumbLeft:
		return JointType_HandLeft;
	case JointType_HandTipRight:
	case JointType_ThumbRight:
		return JointType_HandRight;
	case JointType_Head:
		return JointType_Neck;
	case JointType_ElbowLeft:
		return JointType_ShoulderLeft;
	case JointType_WristLeft:
		return JointType_ElbowLeft;
	case JointType_HandLeft:
		return JointType_WristLeft;
	case JointType_ElbowRight:
		return JointType_ShoulderRight;
	case JointType_WristRight:
		return JointType_ElbowRight;
	case JointType_HandRight:
		return JointType_WristRight;
	case JointType_KneeLeft:
		return JointType_HipLeft;
	case JointType_AnkleLeft:
		return JointType_KneeLeft;
	case JointType_FootLeft:
		return JointType_AnkleLeft;
	case JointType_KneeRight:
		return JointType_HipRight;
	case JointType_AnkleRight:
		return JointType_KneeRight;
	case JointType_FootRight:
		return JointType_AnkleRight;

	default:
		return type;
	}

}
