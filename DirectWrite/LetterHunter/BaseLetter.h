#ifndef __BASE_LETTER_H__
#define __BASE_LETTER_H__

#include <d2d1.h>
#include <dwrite.h>

class BaseLetter
{
public:
	BaseLetter();
	BaseLetter(
		ID2D1Factory*			d2dFactory, 
		ID2D1HwndRenderTarget*	rendertarget, 
		IDWriteFactory*			dwriteFactory,
		wchar_t					letter, 
		float					fontSize		= 200,
		float					outlineWidth	= 2.0f,
		D2D1_COLOR_F&			fillColor		= D2D1::ColorF(D2D1::ColorF::Black),
		D2D1_COLOR_F&			outlineColor	= D2D1::ColorF(D2D1::ColorF::White)
		);
	~BaseLetter(void);

public:
	bool			isLive() const;					// Get text state, live or die
	void			die();							// Set object as dead
	void			live();							// Set object to live

	wchar_t			getLetter() const;
	void			setLetter(wchar_t letter);

	D2D1_COLOR_F	getFillColor() const;
	void			setFillColor(D2D1_COLOR_F& color);

	D2D1_COLOR_F	getOutlineColor() const;
	void			setOutlineColor(D2D1_COLOR_F& color);

	float			getSpeedFactor() const;
	void			setSpeedFactor(float speedFactor);

	float			getoutlineWidth() const;
	void			setOutlineWidth(float width);

	D2D1_POINT_2F	getPosition() const;
	void			setPosition(D2D1_POINT_2F& pos);
	void			setPosition(float x, float y);

	D2D_VECTOR_2F	getVelocity() const;
	void			setVelocity(D2D_VECTOR_2F& velocity);
	void			setVelocity(float x, float y);

	void			drawBoundary() const;
	void			drawBoundaryBackground() const;

	void			setBoundaryColor(D2D1_COLOR_F& color);

	// Draw background of letter, if called, must call before filling in the letter color.
	void			setBoundaryBackgroundColor(D2D1_COLOR_F& color);

	ID2D1TransformedGeometry* getTransformedGeometry() const;

	// Translate letter to position (x, y)
	void			translate(float x, float y);

	// Set a transform matrix
	void			setTransform(D2D1_MATRIX_3X2_F& matrix);

	// Update the letter, this is alwarys moving the letter 
	void			update(float timeDelta);		// Update the text

	// Draw letter
	void			render();

	// What to do when a letter was hitted, the letter may disappear or draw an effect
	void			onHit();
	void			scaling(float timeDelta);

	// Calculate the boundary rectangle of the letter, this was computed dynamically, since the letter was moving
	// all the time
	D2D1_RECT_F		getBoundRect() const;

private:
	ID2D1Factory*				pD2DFactory;
	ID2D1HwndRenderTarget*		pRenderTarget;
	ID2D1SolidColorBrush*		pOutlineBrush;
	ID2D1SolidColorBrush*		pFillBrush;
	ID2D1SolidColorBrush*		boundaryBrush_;
	ID2D1SolidColorBrush*		boundaryBackgroundBrush_;
	IDWriteFactory*				pDWriteFactory;
	IDWriteFontFace*			pFontFace;
	IDWriteFontFile*			pFontFile;
	ID2D1PathGeometry*			pPathGeometry;
	ID2D1GeometrySink*			pGeometrySink;
	ID2D1TransformedGeometry*	pTransformedGeometry_;

	wchar_t				letter_;		// the letter value of the object
	bool				isLive_;		// Is letter alive?
	int					fontSize_;		// font size
	float				outlineWidth_;	// outline width of the letter
	float				liveTime_;		// Time since the letter was generated
	float				speedFactor_;	// This is the factor to control letter speed, defaul 1

	// Since D2D_VECTOR_2F has no constructor and not easy-use, so we use D2D1_POINT_2F instead, though D2D_VECTOR_2F
	// is more perspective to express velocity, but it's just a C-style structure and hard to use, hope MS can update 
	// it in the next release. it seems there is no D2D1_VECTOR_2F in d2dbasetypes.h, but I do see it on MSDN, why?
	D2D_VECTOR_2F		velocity_;		// moving velocity
	D2D1_COLOR_F		outlineColor_;	// font color
	D2D1_COLOR_F		fillColor_;		// Color to fill the letter
	D2D1_RECT_F			boundary_;		// Boundary rectange of the letter
	D2D1_MATRIX_3X2_F	matrix_;			// transform matrix
};

#endif // end __LETTER_OBJECT_BASE_H__

