#include "LetterObjectBase.h"

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

LetterObjectBase::LetterObjectBase()
{
}

LetterObjectBase::LetterObjectBase(
		ID2D1Factory* d2dFactory, 
		ID2D1HwndRenderTarget* rendertarget, 
		IDWriteFactory*	dwriteFactory,
		wchar_t letter, 
		float fontSize_,
		float width,
		D2D1_COLOR_F fillColor_,
		D2D1_COLOR_F outlineColor_
		)
		:
	pD2DFactory(d2dFactory),
	pRenderTarget(rendertarget),
	pDWriteFactory(dwriteFactory),
	letter_(letter),
	outlineWidth_(width),
	isVisible_(true), 
	isLive_(true),
	liveTime_(0.0f),
	speedFactor_(1.0f),
	pFontFile(NULL), 
	matrix_(D2D1::Matrix3x2F::Identity()),
	boundaryBrush_(NULL),
	boundaryBackgroundBrush_(NULL),
	pPathGeometry(NULL),
	pTransformedGeometry_(NULL)
{
	letter_ = letter;
	fontSize_ = fontSize_;
	this->fillColor_ = fillColor_;
	this->outlineColor_ = outlineColor_;

	velocity_ = D2D1::Point2F(0, 0);

	// Create font file reference
	const WCHAR* filePath = L"C:/Windows/Fonts/SCHLBKB.TTF";
	HRESULT hr = pDWriteFactory->CreateFontFileReference(
		filePath,
		NULL,
		&pFontFile
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create font file reference failed!", L"Error", 0);
		return;
	}

	// Create font face
	IDWriteFontFile* fontFileArray[] = { pFontFile };
	pDWriteFactory->CreateFontFace(
		DWRITE_FONT_FACE_TYPE_TRUETYPE,
		1,
		fontFileArray,
		0,
		DWRITE_FONT_SIMULATIONS_NONE,
		&pFontFace
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create font file face failed!", L"Error", 0);
		return;
	}

	// Create fill brush
	hr = pRenderTarget->CreateSolidColorBrush(fillColor_, &pFillBrush);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create fill brush failed!", L"Error", 0);
		return;
	}

	// Create outline brush
	hr = pRenderTarget->CreateSolidColorBrush(outlineColor_, &pOutlineBrush);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create outline brush failed!", L"Error", 0);
		return;
	}

	// Create boundary_ brush
	hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &boundaryBrush_);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create boundary_ brush failed!", L"Error", 0);
		return;
	}

	// Crreate boundary_ background brush
	hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DeepSkyBlue), &boundaryBackgroundBrush_);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create boundary_ background brush failed!", L"Error", 0);
		return;
	}

	wchar_t textString[] = {letter_, '\0'};

	// Get text length
	UINT32 textLength = (UINT32)wcslen(textString);

	UINT32* pCodePoints		= new UINT32[textLength];
	ZeroMemory(pCodePoints, sizeof(UINT32) * textLength);

	UINT16*	pGlyphIndices	= new UINT16[textLength];
	ZeroMemory(pGlyphIndices, sizeof(UINT16) * textLength);

	for(unsigned int i = 0; i < textLength; ++i)
	{
		pCodePoints[i] = textString[i];
	}

	// Get glyph indices
	hr =pFontFace->GetGlyphIndicesW(
		pCodePoints,
		textLength,
		pGlyphIndices
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Get glyph indices failed!", L"Error", 0);
		return;
	}

	// Create path geometry
	hr = pD2DFactory->CreatePathGeometry(&pPathGeometry);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create path geometry failed!", L"Error", 0);
		return;
	}

	// Open sink
	hr = pPathGeometry->Open(&pGeometrySink);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Open geometry sink failed!", L"Error", 0);
		return;
	}

	// Get glyph run outline
	hr = pFontFace->GetGlyphRunOutline(
		fontSize_,				// font size
		pGlyphIndices,
		NULL,
		NULL,
		textLength,
		FALSE,
		FALSE,
		pGeometrySink
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Get glyph run outline failed!", L"Error", 0);
		return;
	}

	// Close sink
	pGeometrySink->Close();

	if(pCodePoints)
	{
		delete [] pCodePoints;
		pCodePoints = NULL;
	}

	if(pGlyphIndices)
	{
		delete [] pGlyphIndices;
		pGlyphIndices = NULL;
	}
}

LetterObjectBase::~LetterObjectBase(void)
{
	SAFE_RELEASE(pTransformedGeometry_);
	SAFE_RELEASE(pGeometrySink);
	SAFE_RELEASE(pPathGeometry);
	SAFE_RELEASE(pFontFile);
	SAFE_RELEASE(pFontFace);
	SAFE_RELEASE(pDWriteFactory);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(pFillBrush);
	SAFE_RELEASE(pOutlineBrush);
	SAFE_RELEASE(pD2DFactory);
}

bool LetterObjectBase::isLive() const
{
	return isLive_;
}

void LetterObjectBase::setLiveState(bool liveFlag)
{
	isLive_ = liveFlag;
}

bool LetterObjectBase::getVisible() const
{
	return isVisible_;
}

void LetterObjectBase::setVisible(bool visibleFlag)
{
	isVisible_ = visibleFlag;
}

wchar_t LetterObjectBase::getLetter() const
{
	return letter_;
}

void LetterObjectBase::setLetter(wchar_t letter)
{
	letter_ = letter;
}

D2D1_COLOR_F LetterObjectBase::getFillColor() const
{
	return fillColor_;
}

void LetterObjectBase::setFillColor(D2D1_COLOR_F color)
{
	fillColor_ = color;
	pFillBrush->SetColor(fillColor_);
}

D2D1_COLOR_F LetterObjectBase::getOutlineColor() const
{
	return outlineColor_;
}

void LetterObjectBase::setOutlineColor(D2D1_COLOR_F color)
{
	outlineColor_ = color;
	pOutlineBrush->SetColor(outlineColor_);
}

float LetterObjectBase::getSpeedFactor() const
{
	return speedFactor_;
}

void LetterObjectBase::setSpeedFactor(float speedFactor)
{
	speedFactor_ = speedFactor;
}

void LetterObjectBase::setBoundaryColor(D2D1_COLOR_F& color)
{
	boundaryBrush_->SetColor(color);
}

void LetterObjectBase::setBoundaryBackgroundColor(D2D1_COLOR_F& color)
{
	boundaryBackgroundBrush_->SetColor(color);
}

float LetterObjectBase::getoutlineWidth() const
{
	return outlineWidth_;
}

void LetterObjectBase::setOutlineWidth(float width)
{
	outlineWidth_ = width;
}

int LetterObjectBase::getSize() const
{
	return fontSize_;
}

void LetterObjectBase::getBound(D2D1_RECT_F* rect) const
{
	pTransformedGeometry_->GetBounds(D2D1::Matrix3x2F::Identity(), rect);
}

D2D1_POINT_2F LetterObjectBase::getPosition() const
{
	D2D1_POINT_2F pos;

	// Get the position_ information from the matrix_
	pos.x = matrix_._31;
	pos.y = matrix_._32;

	return pos;
}

void LetterObjectBase::setPosition(D2D1_POINT_2F pos)
{
	translate(pos.x, pos.y);
}

void LetterObjectBase::setPosition(float x, float y)
{
	translate(x, y);
}

D2D1_POINT_2F LetterObjectBase::getVelocity() const
{
	return velocity_;
}

void LetterObjectBase::setVelocity(D2D1_POINT_2F velocity)
{
	velocity_ = velocity;
}

void LetterObjectBase::setVelocity(float x, float y)
{
	velocity_.x = x;
	velocity_.y = y;
}

D2D1_RECT_F	LetterObjectBase::computeBoundary() const
{
	D2D1_RECT_F rect;
	pTransformedGeometry_->GetBounds(D2D1::Matrix3x2F::Identity(), &rect);
	return rect;
}

void LetterObjectBase::setTransform(D2D1_MATRIX_3X2_F& matrix)
{
	// Update transform matrix.
	matrix_ = matrix;

	// Compute a new geometry based on the transform matrix.
	pD2DFactory->CreateTransformedGeometry(
		pPathGeometry,
		matrix_,
		&pTransformedGeometry_
	);
}

void LetterObjectBase::translate(float x, float y)
{
	matrix_ = D2D1::Matrix3x2F::Translation(x, y);
	setTransform(matrix_);
}

void LetterObjectBase::drawBoundary() const
{
	D2D1_RECT_F rect = computeBoundary();
	pRenderTarget->DrawRectangle(&rect, boundaryBrush_);
}

void LetterObjectBase::drawBoundaryBackground() const
{
	// Get boundary rectangle
	D2D1_RECT_F rect = computeBoundary();

	// Calculate boundary width and height
	float boundWidth = rect.right - rect.left;
	float boundHeight = rect.bottom - rect.top;
	
	if(boundWidth >= boundHeight)
	{
		// Extend boundHeight, so it equal to boundWidth
		rect.top -= (boundWidth - boundHeight) / 2;
		rect.bottom += (boundWidth - boundHeight) / 2;
	}
	else // boundWidth < boundHeight
	{
		// Extend boundWidth, so it equals to boundHeight
		rect.left -= (boundHeight - boundWidth) / 2;
		rect.right += (boundHeight - boundWidth) / 2;
	}

	// Extend rect by 10 at each side
	rect.left	-= (rect.right - rect.left) * 0.1f;
	rect.right	+= (rect.right - rect.left) * 0.1f;
	rect.top	-= -(rect.bottom - rect.top) * 0.1f;
	rect.bottom += -(rect.bottom - rect.top) * 0.2f;

	D2D1_ROUNDED_RECT roundRect = D2D1::RoundedRect(
		rect,
		(rect.right - rect.left) * 0.2f,
		(rect.top - rect.bottom) * 0.2f
		);

	pRenderTarget->FillRoundedRectangle(&roundRect, boundaryBackgroundBrush_);
}

ID2D1TransformedGeometry* LetterObjectBase::getTransformedGeometry() const
{
	return pTransformedGeometry_;
}

void LetterObjectBase::update()
{
	// Accumulate total time
	liveTime_ = 2.0f;

	// Get previous position_
	D2D1_POINT_2F currentPos = getPosition();

	// Calculate new position_ based on previous postion and velocity_
	float newX = currentPos.x + velocity_.x * liveTime_ * speedFactor_;
	float newY = currentPos.y + velocity_.y * liveTime_ * speedFactor_;

	// Translate to current position_
	translate(newX, newY);
}

void LetterObjectBase::render()
{
	// Draw outline
	pRenderTarget->DrawGeometry(pTransformedGeometry_, pOutlineBrush, outlineWidth_);

	// Draw background boundary
	// drawBoundaryBackground();

	// Fill text geometry
	pRenderTarget->FillGeometry(pTransformedGeometry_, pFillBrush);
}