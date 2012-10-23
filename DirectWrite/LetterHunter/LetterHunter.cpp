#include "LetterHunter.h"
#include "Utilities.h"
#include "windows.h"

LetterHunter::LetterHunter(void)
{
}

LetterHunter::LetterHunter(HWND hwnd, int maxTextCount)
	:hwnd_(hwnd),
	maxTextCount_(100),
	d2d_(NULL),
	dinput_(NULL),
	soundManager_(NULL),
	textBuffer_(NULL),
	currentTextObject_(NULL)
{
	// Initialize Direct2D
	d2d_ = new D2D();
	d2d_->createDeviceIndependentResources();
	d2d_->createDeviceResources(hwnd);

	// Initialize Direct Input
	dinput_ = new DInput();
	soundManager_ = new SoundManager();
}

void LetterHunter::release()
{
	SAFE_DELETE(d2d_);
	SAFE_DELETE(dinput_);

	for(vector<TextObject*>::iterator itor = textBuffer_.begin(); itor != textBuffer_.end(); ++itor)
	{
		SAFE_DELETE(*itor);
	}
}

LetterHunter::~LetterHunter(void)
{
	release();
}

void LetterHunter::initialize()
{
	// Get screen resolution
	const HWND hDesktop = GetDesktopWindow();

	RECT desktopRect;
	GetWindowRect(hDesktop, &desktopRect);

	int width  = desktopRect.right;
	int height = desktopRect.bottom;

	setWindowWidth(width);
	setWindowHeight(height);

	// Initizlie text objects
	initializeText();
	initializeBullet();
}

void LetterHunter::update(float timeDelta)
{
	// Update text obejcts
	for(unsigned int i = 0; i < textBuffer_.size(); ++i)
	{
		if (textBuffer_[i]->getLiveState() == true)
		{
			textBuffer_[i]->update();

			// If text object was out of window, set it to dead.
			RECT windowRect = {0, 0, windowWidth, windowHeight};
			if (textBuffer_[i]->outofWindow(windowRect))
			{
				textBuffer_[i]->setLiveState(false);
				if (currentTextObject_ == textBuffer_[i])
				{
					currentTextObject_ = NULL;
				}
			}
		}
		else
		{
			// When a text object was dead, reset a new one
			// The code here need update, otherwise we can not end the game
			resetTextObject(textBuffer_[i]);
		}
	}

	// Update bullet objects
	for(unsigned int i = 0; i < bulletBuffer_.size(); ++i)
	{
		if(bulletBuffer_[i]->getLiveState() == true)
		{
			// Update bullet, position, live state and so on
			bulletBuffer_[i]->update(timeDelta);

			// Check whether bullet out of window, if true, set it's state to dead
			RECT windowRect = {0, 0, windowWidth, windowHeight};
			if(bulletBuffer_[i]->outofWindow(windowRect))
			{
				bulletBuffer_[i]->setLiveState(false);
				return;
			}

			if (currentTextObject_)
			{
				hitDetect(bulletBuffer_[i], currentTextObject_);
			}
		}
	}
}

void LetterHunter::render(float timeDelta)
{
	// Create device dependent resources
	d2d_->createDeviceResources(hwnd_);

	// Get Hwnd render target
	ID2D1HwndRenderTarget* rendertarget = d2d_->getD2DHwndRenderTarget();

	// Update text object before rendering
	update(timeDelta);

	rendertarget->BeginDraw();

	// Set render target background color to white
	rendertarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// render text objects
	for(unsigned int i = 0; i < textBuffer_.size(); ++i)
	{
		if(textBuffer_[i]->getLiveState() == true)
		{
			textBuffer_[i]->render();
		}
	}

	// render bullet objects
	for(unsigned int i = 0; i < bulletBuffer_.size(); ++i)
	{
		if(bulletBuffer_[i]->getLiveState() == true)
		{
			bulletBuffer_[i]->render();
		}
	}

	rendertarget->EndDraw();
}

void LetterHunter::handleKeyboardMessage()
{
	dinput_->update();

	// Get the key pressed
	char hitLetter = dinput_->getKey();

	// 0-9 was magic words
	if(hitLetter == '0')	// pause
	{
		setTextSpeedFactor(0);
	}

	if(hitLetter == ' ')
	{
		hitAll();
	}

	shootCheck(hitLetter);
}

void LetterHunter::resize(int width, int height)
{
	d2d_->onResize(width, height);
}

int LetterHunter::getwindowWidth() const
{
	return windowWidth;
}

void LetterHunter::setWindowWidth(int width)
{
	windowWidth = width;
}

int LetterHunter::getWindowHeight() const
{
	return windowHeight;
}

void LetterHunter::setWindowHeight(int height)
{
	windowHeight = height;
}

void LetterHunter::initializeText()
{
	ID2D1Factory*			D2DFactory		= d2d_->getD2DFactory();
	ID2D1HwndRenderTarget*	renderTarget	= d2d_->getD2DHwndRenderTarget();
	IDWriteFactory*			DWriteFactory	= d2d_->getDWriteFactory();

	for(int i = 0; i < TEXTCOUNT; ++i)
	{
		// Geneate a random string
		const int strLength = 1;
		wchar_t* strBuffer = new wchar_t[strLength + 1];
		randomString(strBuffer, strLength);

		TextObject* textObj = new TextObject(
			D2DFactory,
			renderTarget,
			DWriteFactory,
			strBuffer
			);

		SAFE_DELETE(strBuffer);

		// Generate 10 random numbers between 1 and 100
		float a[10] = {0};
		a[i] = randomFloat(0.1f, 0.5f);

		// Set text position
		textObj->setPosition(i * 200.0f, 0.0f);

		// Set text velocity
		textObj->setVelocity(0, a[i]);

		D2D1_COLOR_F fillColor = randomColor();
		textObj->setFillColor(fillColor);

		textBuffer_.push_back(textObj);
	}
}

void LetterHunter::initializeBullet()
{
	ID2D1Factory*			D2DFactory		= d2d_->getD2DFactory();
	ID2D1HwndRenderTarget*	renderTarget	= d2d_->getD2DHwndRenderTarget();

	for(int i = 0; i < BULLETCOUNT; ++i)
	{
		Bullet* bullet = new Bullet(D2DFactory, renderTarget);
		bulletBuffer_.push_back(bullet);
	}
}

void LetterHunter::resetTextObject(TextObject* textObject)
{
	// Set text position
	float posX = randomFloat(0, 1700);

	// Set text velocity
	float velocityX = 0;
	float velocityY = randomFloat(0.1f, 0.5f);

	// Create text string
	const int strLength = 1;
	wchar_t* strBuffer = new wchar_t[strLength + 1]; // one more space for '\0'
	randomString(strBuffer, strLength);

	// Create text color
	D2D1_COLOR_F fillColor = randomColor();
	textObject->setFillColor(fillColor);

	// Reset text object
	textObject->reset(strBuffer, posX, 0, velocityX, velocityY, fillColor);

	SAFE_DELETE(strBuffer);
}

TextObject* LetterHunter::findTarget(wchar_t hitKey)
{
	float maxBottomofTextObject = 0;
	TextObject* lowestTextObject = NULL;

	for (unsigned int i = 0; i < textBuffer_.size(); ++i)
	{
		if (textBuffer_[i]->getLiveState() == true)
		{
			Letter* letterObject = textBuffer_[i]->getFirstActiveLetter();
			if (letterObject->getLetter() == hitKey)
			{
				D2D1_RECT_F rect;
				letterObject->getBound(&rect);
				if(rect.bottom > maxBottomofTextObject)
				{
					maxBottomofTextObject = rect.bottom;
					lowestTextObject = textBuffer_[i];
				}
			}
		}
	}

	return lowestTextObject;
}

void LetterHunter::shootCheck(wchar_t key)
{
	if (currentTextObject_)
	{
		Letter* letterOjbect = currentTextObject_->getFirstActiveLetter();
		if (key == letterOjbect->getLetter())
		{
			// Set the bullet object based on the letter object.
			setBulletObject(letterOjbect);

			// Play sound for the bullet
			soundManager_->onShoot();
		}
		else
		{
			// Warn user where is the target text object is, may be need some highlight mechnism.
		}
	}

	else
	{
		TextObject* targetTextObject = findTarget(key);
		if(targetTextObject)
		{
			currentTextObject_ = targetTextObject;
			Letter* letterObject = targetTextObject->getFirstActiveLetter();
			setBulletObject(letterObject);
			soundManager_->onShoot();
		}
	}
}

void LetterHunter::hitDetect(Bullet* bullet, TextObject* textObject)
{
	// We trate the letter as hit when the bullet pass over the letter, that is the bullet.top < letter.bottom
	D2D1_RECT_F bulletRect;
	bullet->getBoundRect(&bulletRect);

	D2D1_RECT_F textObejctRect;
	textObject->getBoundaryRect(textObejctRect);

	if(bulletRect.top < textObejctRect.bottom)
	{
		textObject->onHit();
		if(textObject->getLiveState() == false)
		{
			currentTextObject_ = NULL;
		}
		bullet->onHit();
		soundManager_->onHit();
	}
}

void LetterHunter::hitAll()
{
	currentTextObject_ = NULL;
	for(unsigned int i = 0; i < textBuffer_.size(); ++i)
	{
		textBuffer_[i]->setLiveState(false);
		soundManager_->onHit();
	}
}

void LetterHunter::setBulletObject(Letter* letterObject)
{
	// find a invalid bullet
	for(unsigned int i = 0; i < bulletBuffer_.size(); ++i)
	{
		if(bulletBuffer_[i]->getLiveState() == false)
		{
			// reset state to true
			bulletBuffer_[i]->setLiveState(true);

			// Set target letter
			wchar_t letter = letterObject->getLetter();
			bulletBuffer_[i]->setTargetLetter(letter);

			// Get the letter position
			D2D1_POINT_2F letterPos = letterObject->getPosition();

			// set bullet position at the bottom of the letter being hit
			D2D1_POINT_2F bulletPos = {letterPos.x, (float)windowHeight};
			bulletBuffer_[i]->setPostion(bulletPos);

			// Set velocity of the bullet, the velocity should based on the distance of the letter and the window bottom
			D2D_VECTOR_2F velocity = {0, -10};
			bulletBuffer_[i]->setVelocity(velocity);

			break;
		}
	}
}

void LetterHunter::addNewTextObject()
{
	TextObject textObj;
	resetTextObject(&textObj);

	textBuffer_.push_back(&textObj);
}

void LetterHunter::setTextSpeedFactor(float speedFactor)
{
	for(unsigned int i = 0; i < textBuffer_.size(); ++i)
	{
		textBuffer_[i]->setLetterSpeedFactor(speedFactor);
	}
}
