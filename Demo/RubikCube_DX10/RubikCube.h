#ifndef __RUBIK_CUBE_H__
#define __RUBIK_CUBE_H__

#include "Cube.h"
#include "Camera.h"
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
	void Restore(); 
	void ToggleFullScreen();
	void OnLeftButtonDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnLeftButtonUp();
	void InitD3D10(HWND hWnd);
	void ResizeD3DScene(int width, int height);
	void SetupMatrix();
	void InitCubes();
	void InitEffects();
	void ResetLayerIds();
	void ResetTextures();
	D3DXVECTOR2 GetMaxScreenResolution();
	D3DXVECTOR3 ScreenToVector3(int x, int y);
	Ray CalculatePickingRay(int x, int y);
	Face GetPickedFace(D3DXVECTOR3 hit_point) const;	// Get the face picking by mouse 
	D3DXPLANE GeneratePlane(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point);
	D3DXVECTOR3 GetRotateAxis(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point);
	float CalculateRotateAngle();
	RotateDirection GetRotateDirection(Face face, D3DXVECTOR3& axis, D3DXVECTOR3& previous_vector, D3DXVECTOR3& current_vector);
	int  GetHitLayer(Face face, D3DXVECTOR3& rotate_axis, D3DXVECTOR3& hit_point);
	void RotateLayer(int layer, D3DXVECTOR3& axis, float angle);

private:
	const int kNumLayers;	// Number of layers in one direction, a 3 x 3 Rubik Cube has num_layers_ = 3.
	const int kNumCubes;	// Number of unit cubes, 27 unit cubes build up a rubik cube.
	Cube* cubes;			// Array to store 27 unit cubes

	const int kNumFaces;		// Number of faces
	Rect* faces;				// Store faces in rect

	float face_length_;			// Face length of the Rubik Cube
	float gap_between_layers_;	// the length between two layers.

	bool is_hit_;				// The picking ray hit the RubikCube
	int  hit_layer_;			// The layer hit by the picking Ray
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

	HWND hWnd_;
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

	ID3D10Device*			d3ddevice_;			// D3D9 Device
	IDXGISwapChain*			swap_chain_;		// DXGI swap chain
	ID3D10Effect*			effects_;			// D3D10 effect
	ID3D10InputLayout*		input_layout_;
	IDXGIOutput*			output_;			// DXGI Adapter output
	ID3D10RenderTargetView* rendertarget_view_; // Rendertarget view
	ID3D10DepthStencilView* depth_stencil_view_;	// Depth-stencil view
	DXGI_SWAP_CHAIN_DESC    sd_;				// SwapChain descriptioin
	D3D10_VIEWPORT			vp_;				// Viewport
	bool					is_fullscreen_;		// Is Game in Full-Screen mode?

	HWND hWnd;			// Handle of game window
	int screen_width_;	// The maximum resolution width
	int screen_height_;	// The maximum resolution height
};

#endif // end __RUBIK_CUBE_H__