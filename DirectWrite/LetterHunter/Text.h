#ifndef __TEXT_H__
#define __TEXT_H__

#include <d2d1.h>
#include <dwrite.h>

class Text
{
public:
	Text(
		ID2D1Factory*			d2dFactory, 
		ID2D1HwndRenderTarget*	rendertarget, 
		IDWriteFactory*			dwriteFactory,

		wchar_t*		text, 
		D2D1_COLOR_F	fillColor,
		D2D1_COLOR_F	outlineColor,
		float			outlineWidth,
		int				size
		);
	~Text(void);

public:
	bool			getLiveState() const;			// Get text state, live or die
	void			setLiveState(bool liveFlag);	// Set the live state, live or die

	bool			getVisible() const;
	void			setVisible(bool visibleFlag);

	wchar_t*		getText() const;
	void			setText(wchar_t* text);

	D2D1_COLOR_F	getFillColor() const;
	void			setFillColor(D2D1_COLOR_F color);

	D2D1_COLOR_F	getOutlineColor() const;
	void			setOutlineColor(D2D1_COLOR_F color);

	float			getoutlineWidth() const;
	void			setOutlineWidth(float width);

	int				getSize() const;

	D2D1_POINT_2F	getPosition() const;
	void			setPosition(D2D1_POINT_2F pos);
	void			setPosition(float x, float y);

	D2D1_POINT_2F	getVelocity() const;
	void			setVelocity(D2D1_POINT_2F velocity);
	void			setVelocity(float x, float y);

	D2D1_MATRIX_3X2_F	getTransformMatrix() const;
	void				setTransformMatrix(D2D1_MATRIX_3X2_F& matrix);

	void			setTransform(D2D1_MATRIX_3X2_F matrix);
	void			update();		// Update the text
	void			render();
	void			onDestroy();

private:
	bool			isLive;			// Is text alive?
	bool			isVisible;		// Is text visible?
	wchar_t*		text;			// string of the Text
	int				fontSize;		// font size
	float			outlineWidth;	// outline width of the font
	float			liveTime;		// Time since the text was generated
	D2D_POINT_2F	velocity;		// moving velocity

	D2D1_COLOR_F		outlineColor;	// text color
	D2D1_COLOR_F		fillColor;		// Color to fill the font
	D2D1_POINT_2F		position;		// font position
	D2D1_MATRIX_3X2_F	matrix;			// transform matrix

private:
	ID2D1Factory*				pD2DFactory;
	ID2D1HwndRenderTarget*		pRenderTarget;
	ID2D1SolidColorBrush*		pOutlineBrush;
	ID2D1SolidColorBrush*		pFillBrush;
	IDWriteFactory*				pDWriteFactory;
	IDWriteFontFace*			pFontFace;
	IDWriteFontFile*			pFontFile;
	ID2D1PathGeometry*			pPathGeometry;
	ID2D1GeometrySink*			pGeometrySink;
	ID2D1TransformedGeometry*	pTransformedGeometry;
};

#endif // end __TEXT_H__

