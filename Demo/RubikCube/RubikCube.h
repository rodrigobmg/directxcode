#ifndef __RUBIK_CUBE_H__
#define __RUBIK_CUBE_H__

#include "Cube.h"
#include "Camera.h"
#include "D3D9.h"
#include "Math.h"

// The 6 faces of the Rubik Cube
enum Face
{
	kFrontFace =  0,
	kBackFace  =  1,
	kLeftFace  =  2,
	kRightFace =  3,
	kTopFace   =  4,
	kBottomFace = 5,

	kUnknownFace = 10
};

enum RotateDirection
{
	kClockWise = 0,
	kCounterClockWise = 1,

	kUnknownDirection = 2
};

class RubikCube
{
public:
	RubikCube(void);
	~RubikCube(void);

	void Initialize(HWND hWnd);
	void Render();
	LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int GetWindowPosX() const;
	int GetWindowPosY() const;
	int GetWindowWidth() const;
	int GetWindowHeight() const;

private:
	void Shuffle();
	void ToggleFullScreen();
	void OnLeftButtonDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnLeftButtonUp();
	void InitTextures();
	Face GetPickedFace(D3DXVECTOR3 hit_point) const;	// Get the face picking by mouse 
	D3DXPLANE GeneratePlane(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point);
	void MarkRotateCubes(D3DXPLANE& plane);			// Mark which cube to rotate, set is_selected = true
	D3DXVECTOR3 GetRotateAxis(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point);
	float CalculateRotateAngle();
	RotateDirection GetRotateDirection(Face face, D3DXVECTOR3& axis, D3DXVECTOR3& previous_vector, D3DXVECTOR3& current_vector);
	void Rotate(D3DXVECTOR3& axis, float angle);	// Rotate a layer

private:
	const int kNumCubes;	// Number of unit cubes, 27 unit cubes build up a rubik cube.
	Cube* cubes;			// Array to store 27 unit cubes

	const int kNumFaces;	// Number of faces
	Rect* faces;				// Store faces in rect

	float cube_length_;			// length of the unit cube.
	float gap_between_layers_;	// the length between two layers.

	bool is_hit_;				// The picking ray hit the RubikCube
	bool rotate_finish_;		// A rotaton action was finished.
	bool is_cubes_selected_;	// Does cubes selected in current rotation?
	bool window_active_;		// Window was inactive? turn when device lost, window minimized...

	D3DXVECTOR3 previous_hitpoint_;
	D3DXVECTOR3 current_hitpoint_;

	D3DXVECTOR3 current_vector_;		// Current hit point
	D3DXVECTOR3 previous_vector_;		// Last hit point

	float rotate_speed_;				// layer rotation speed
	float total_rotate_angle_;			// The angle rotated when mouse up
	D3DXVECTOR3 rotate_axis_;			// Rotate axis, X or Y or Z
	RotateDirection rotate_direction_;	// Rotate direction


	ArcBall* world_arcball_ ;
	Camera*	camera_;			// Model view camera

	HWND hWnd;
	WINDOWPLACEMENT wp ;		// Window position and size

	// Initial window position and size
	int init_window_x_;
	int init_window_y_;
	int init_window_width_;
	int init_window_height_;

	// Last window size
	int last_window_width_;
	int last_window_height_;

	// Current window size, used in resizing window 
	int current_window_width_;
	int current_window_height_;

	// Textures
	int texture_width_;						// Texture width in pixel.
	int texture_height_;					// Texture height in pixel.
	int* texture_id_;						// The index is the faceId, the value is the texture_id_.
	IDirect3DTexture9**	face_textures_;		// Face textures
	IDirect3DTexture9*	inner_textures_;	// Inner face texture.

	D3D9* d3d9;								// Objects from other classes
};

#endif // end __RUBIK_CUBE_H__