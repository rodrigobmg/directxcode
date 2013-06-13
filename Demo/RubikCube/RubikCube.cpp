#include "RubikCube.h"
#include "DXErr.h"
#include <time.h>

RubikCube::RubikCube(void)
    : kNumCubes(27),
	  kNumFaces(6),
	  cube_length_(10.0f),
      gap_between_layers_(0.15f),
	  total_rotate_angle_(0),
	  rotate_speed_(1.5f),
	  is_hit_(false),
	  is_cubes_selected_(false),
	  rotate_finish_(true),
	  window_active_(false),
	  init_window_x_(0),
	  init_window_y_(0),
	  init_window_width_(100),
	  init_window_height_(100),
	  current_window_width_(init_window_width_),
	  current_window_height_(init_window_height_),
	  last_window_width_(current_window_width_),
	  last_window_height_(current_window_height_),
	  texture_width_(128),
	  texture_height_(128),
	  inner_textures_(NULL)
{
	d3d9 = new D3D9();

	world_arcball_ = new ArcBall();

	camera_ = new Camera();

	// Create 27 unit cubes
	cubes = new Cube[kNumCubes];

	// Create 6 faces
	faces = new Rect[kNumFaces];

	// in order to short the code line, use short temp variables here
	float length = cube_length_;
	float gap = gap_between_layers_;

	// The Rubik cube was build in the following order
	// The center of the cube is the origin of the coordiantes system, so we can 
	// Calulate the 8 corners of the Rubik cube with length and gaps
	D3DXVECTOR3 A(-(1.5f * length + gap),   1.5f * length + gap , -(1.5f * length + gap)); // The front-top-left corner
	D3DXVECTOR3 B(  1.5f * length + gap ,   1.5f * length + gap , -(1.5f * length + gap));
	D3DXVECTOR3 C(  1.5f * length + gap , -(1.5f * length + gap), -(1.5f * length + gap));
	D3DXVECTOR3 D(-(1.5f * length + gap), -(1.5f * length + gap), -(1.5f * length + gap));

	D3DXVECTOR3 E(-(1.5f * length + gap),   1.5f * length + gap ,  (1.5f * length + gap)); // The back-top-left corner
	D3DXVECTOR3 F(  1.5f * length + gap ,   1.5f * length + gap ,  (1.5f * length + gap));
	D3DXVECTOR3 G(  1.5f * length + gap , -(1.5f * length + gap),  (1.5f * length + gap));
	D3DXVECTOR3 H(-(1.5f * length + gap), -(1.5f * length + gap),  (1.5f * length + gap));

	Rect  FrontFace(A, B, C, D) ; 
	Rect   BackFace(E, F, G, H) ;
	Rect   LeftFace(E, A, D, H) ;
	Rect  RightFace(B, F, G, C) ;
	Rect    TopFace(E, F, B, A) ;
	Rect BottomFace(G, H, D, C) ;

	faces[0] = FrontFace;
	faces[1] = BackFace;
	faces[2] = LeftFace;
	faces[3] = RightFace;
	faces[4] = TopFace;
	faces[5] = BottomFace;

	texture_id_ = new int[kNumFaces];
	face_textures_ = new IDirect3DTexture9*[kNumFaces];

	for(int i = 0; i < kNumFaces; ++i)
	{
		texture_id_[i] = -1;
		face_textures_[i] = NULL;
	}
}

RubikCube::~RubikCube(void)
{
	// Delete d3d9 objects;
	delete d3d9;
	d3d9 = NULL;

	delete world_arcball_;
	world_arcball_ = NULL;

	// Delete camera
	delete camera_;
	camera_ = NULL;

	// Delete cubes
	delete []cubes;
	cubes = NULL;

	// Delete faces
	delete []faces;
	faces = NULL;

	delete []texture_id_;
	texture_id_ = NULL;

	// Release face textures
	for(int i = 0; i < kNumFaces; ++i)
	{
		if(face_textures_[i] != NULL)
		{
			face_textures_[i]->Release();
			face_textures_[i] = NULL;
		}
	}

	// Release inner texture
	if (inner_textures_ != NULL)
	{
		inner_textures_->Release();
		inner_textures_ = NULL;
	}
}

void RubikCube::Initialize(HWND hWnd)
{
	d3d9->InitD3D9(hWnd);
	this->hWnd = hWnd;

	InitTextures();

	// To short the code line
	float length = cube_length_;
	float gap = gap_between_layers_;

	// Initialize the top-front-left corner of each unit cubes, we use 27 unit
	// cubes to build up a Rubik cube.
	D3DXVECTOR3 initPos[] = 
	{
		// Front layer, from left to right, top to bottom. 9 cubes
		D3DXVECTOR3( -(1.5f * length + gap),    1.5f * length + gap, -(1.5f * length + gap) ),
		D3DXVECTOR3(         -0.5f * length,    1.5f * length + gap, -(1.5f * length + gap) ),
		D3DXVECTOR3(    0.5f * length + gap,    1.5f * length + gap, -(1.5f * length + gap) ),

		D3DXVECTOR3( -(1.5f * length + gap),          0.5f * length, -(1.5f * length + gap) ),
		D3DXVECTOR3(         -0.5f * length,          0.5f * length, -(1.5f * length + gap) ),
		D3DXVECTOR3(    0.5f * length + gap,          0.5f * length, -(1.5f * length + gap) ),

		D3DXVECTOR3( -(1.5f * length + gap), -(0.5f * length + gap), -(1.5f * length + gap) ),
		D3DXVECTOR3(         -0.5f * length, -(0.5f * length + gap), -(1.5f * length + gap) ),
		D3DXVECTOR3(    0.5f * length + gap, -(0.5f * length + gap), -(1.5f * length + gap) ),

		// Middle layer, from left to right, top to bottom. 9 cubes
		D3DXVECTOR3( -(1.5f * length + gap),    1.5f * length + gap,         -0.5f * length ),
		D3DXVECTOR3(         -0.5f * length,    1.5f * length + gap,         -0.5f * length ),
		D3DXVECTOR3(    0.5f * length + gap,    1.5f * length + gap,         -0.5f * length ),

		D3DXVECTOR3( -(1.5f * length + gap),          0.5f * length,         -0.5f * length ),
		D3DXVECTOR3(         -0.5f * length,          0.5f * length,         -0.5f * length ),
		D3DXVECTOR3(    0.5f * length + gap,          0.5f * length,         -0.5f * length ),

		D3DXVECTOR3( -(1.5f * length + gap), -(0.5f * length + gap),         -0.5f * length ),
		D3DXVECTOR3(         -0.5f * length, -(0.5f * length + gap),         -0.5f * length ),
		D3DXVECTOR3(    0.5f * length + gap, -(0.5f * length + gap),         -0.5f * length ),

		// Back layer, from left to right, top to bottom. 9 cubes
		D3DXVECTOR3( -(1.5f * length + gap),    1.5f * length + gap,    0.5f * length + gap ),
		D3DXVECTOR3(         -0.5f * length,    1.5f * length + gap,    0.5f * length + gap ),
		D3DXVECTOR3(    0.5f * length + gap,    1.5f * length + gap,    0.5f * length + gap ),

		D3DXVECTOR3( -(1.5f * length + gap),          0.5f * length,    0.5f * length + gap ),
		D3DXVECTOR3(         -0.5f * length,          0.5f * length,    0.5f * length + gap ),
		D3DXVECTOR3(    0.5f * length + gap,          0.5f * length,    0.5f * length + gap ),

		D3DXVECTOR3( -(1.5f * length + gap), -(0.5f * length + gap),    0.5f * length + gap ),
		D3DXVECTOR3(         -0.5f * length, -(0.5f * length + gap),    0.5f * length + gap ),
		D3DXVECTOR3(    0.5f * length + gap, -(0.5f * length + gap),    0.5f * length + gap ),
	};

	// Initialize the 27 unit cubes
	for(int i = 0; i < kNumCubes; i++)
	{
		cubes[i].SetDevice(d3d9->GetD3DDevice());
		cubes[i].Init(initPos[i]);
	}

	// Set texture for each cube and each face

	// Front face
	for(int i = 0; i <= 8; ++i)
	{
		cubes[i].SetTextureId(0, 0);
	}

	// Back face
	for(int i = 18; i <= 26; ++i)
	{
		cubes[i].SetTextureId(1, 1);
	}

	// Left face
	for(int i = 0; i <= 24; i += 3)
	{
		cubes[i].SetTextureId(2, 2);
	}

	// Right face
	for(int i = 2; i <= 26; i += 3)
	{
		cubes[i].SetTextureId(3, 3);
	}

	// Top face
	for(int i = 0; i <= 20; ++i)
	{
		if(i % 9 < 3)
		{
			cubes[i].SetTextureId(4, 4);
		}
	}

	// Bottom face
	for(int i = 6; i <= 26; ++i)
	{
		if(i % 9 > 5)
		{
			cubes[i].SetTextureId(5, 5);
		}
	}
}

void RubikCube::Render()
{
	// Window was inactive(minimized or hidden by other apps), yields 25ms to other program
	if(!window_active_)
	{
		Sleep(25) ;
	}

	// Update frame
	d3d9->FrameMove() ;

	d3d9->SetupMatrix();

	d3d9->SetupLight();

	LPDIRECT3DDEVICE9 d3ddevice_ = d3d9->GetD3DDevice();

	// Clear the back buffer to a black color
	d3ddevice_->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x4F94CD, 1.0f, 0);

	if( SUCCEEDED(d3ddevice_->BeginScene()))
	{
		//draw all unit cubes to build the Rubik cube
		for(int i = 0; i < kNumCubes; i++)
		{
			cubes[i].Draw();
		}

		// Restore world matrix since the Draw function in class Cube has set the world matrix for each cube
		D3DXMATRIX matWorld = camera_->GetWorldMatrix() ;
		d3ddevice_->SetTransform(D3DTS_WORLD, &matWorld) ;

		d3ddevice_->EndScene();
	}

	// Present the back buffer contents to the display
	HRESULT hr = d3ddevice_->Present(NULL, NULL, NULL, NULL);

	// Render failed, try to reset device
	if(FAILED(hr))
	{
		d3d9->ResetDevice() ;
	}
}

void RubikCube::Shuffle()
{
	if(!rotate_finish_)
		return ;
	rotate_finish_ = false ;

	// Set the random seed
	srand((unsigned int)time(0));

	// Generate direction
	char dir = ' ' ;
	if(rand() % 2 == 0)
		dir = 'c' ;
	else
		dir = 'r' ;

	// Rotate 20 times
	for(int i = 0; i < 20; i++)
	{
		// Generate the layer
		int layer = rand() % 9 ;
		//RotationLayer(layer, dir, D3DX_PI / 2) ;
		//UpdateLayerInfo(layer, dir, 1) ;
	}

	rotate_finish_ = true ;
}

// Switch from window mode and full-screen mode
void RubikCube::ToggleFullScreen()
{
	wp.length = sizeof(WINDOWPLACEMENT) ;

	// Window -> Full-Screen
	if(d3d9->GetIsFullScreen() == false)
	{
		d3d9->SetIsFullScreen(true);

		// Get and save window placement
		GetWindowPlacement(hWnd, &wp) ;

		// Update back buffer to desktop resolution
		d3d9->SetBackBufferWidth(d3d9->GetScreenWidth());
		d3d9->SetBackBufferHeight(d3d9->GetScreenHeight());
	}
	else // Full-Screen -> Window
	{
		d3d9->SetIsFullScreen(false);

		// Update back buffer size
		d3d9->SetBackBufferWidth(last_window_width_);
		d3d9->SetBackBufferHeight(last_window_height_);

		// When swith from Full-Screen mode to window mode and the wp structe was not initialized
		// The window position and size was unavailable, this will happened when the app start as full-screen mode
		// give a defaul value of it.
		//

		// Restore window placement
		SetWindowPlacement(hWnd, &wp) ;
	}

	// Display mode changed, we need to reset device
	d3d9->ResetDevice() ;
}

void RubikCube::OnLeftButtonDown(int x, int y)
{
	if(!rotate_finish_) // another rotate is in process, return directly
		return ;
	rotate_finish_ = false ; // Prevent the other rotation during this rotate

	// Clear total angle
	total_rotate_angle_ = 0;

	Ray ray = d3d9->CalculatePickingRay(x, y) ;

	previous_vector_ = d3d9->ScreenToVector3(x, y);

	D3DXVECTOR3 currentHitPoint;	// hit point on the face
	float maxDist = 100000.0f ;

	// Select the face nearest to the camera
	for(int i = 0; i < kNumFaces; i++)
	{
		if(RayRectIntersection(ray, faces[i], currentHitPoint))
		{
			is_hit_ = true ;

			// distance from the origin of the ray and to the hit point
			float distance = SquareDistance(ray.origin, currentHitPoint);

			if(distance < maxDist)
			{
				maxDist = distance ;
				previous_hitpoint_ = currentHitPoint ;
			}
		}
	}

	// no action if the picking ray is not intersection with cube 
	if(!is_hit_)
		return ;

	// if the ray intersect with either of the two triangles, then it intersect with the rectangle
	world_arcball_->OnBegin(x, y) ;
}

void RubikCube::OnMouseMove(int x, int y)
{
	if (!is_hit_)
		return;

	world_arcball_->OnMove(x, y) ;

	current_vector_ = d3d9->ScreenToVector3(x, y);

	// Get the picked face
	Face face = GetPickedFace(previous_hitpoint_);

	Ray picking_ray = d3d9->CalculatePickingRay(x, y);
	RayRectIntersection(picking_ray, faces[face], current_hitpoint_);

	D3DXPLANE plane;

	if (!is_cubes_selected_)
	{
		is_cubes_selected_ = true;

		// Calculate picking plane.
		plane = GeneratePlane(face, previous_hitpoint_, current_hitpoint_);
		MarkRotateCubes(plane);
	
		rotate_axis_ = GetRotateAxis(face, previous_hitpoint_, current_hitpoint_);
	}

	float angle = CalculateRotateAngle();

	rotate_direction_ = GetRotateDirection(face, rotate_axis_, previous_vector_, current_vector_);

	// Flip the angle if the direction is counter-clockwise
	// the positive direction is clockwise around the rotate axis when look through the axis toward the origin.
	if (rotate_direction_ == kCounterClockWise)
		angle = -angle;
	total_rotate_angle_ += angle;

	// Rotate
	Rotate(rotate_axis_, angle);

	// Update previous_hitpoint_
	previous_vector_ = current_vector_;
}

// When Left button up, complete the rotation of the left angle to align the cube and update the layer info
void RubikCube::OnLeftButtonUp()
{
	is_hit_ = false ;

	world_arcball_->OnEnd();

	float left_angle = 0.0f ;	// the angle need to rotate when mouse is up
	int   num_half_PI = 0;

	if (total_rotate_angle_ > 0)
	{
		while (total_rotate_angle_ >= D3DX_PI / 2)
		{
			total_rotate_angle_ -= D3DX_PI / 2;
			++num_half_PI;
		}

		if ((total_rotate_angle_ >= 0) && (total_rotate_angle_ <= D3DX_PI / 4))
		{
			left_angle = -total_rotate_angle_;
		}

		else // ((total_rotate_angle_ > D3DX_PI / 4) && (total_rotate_angle_ < D3DX_PI / 2))
		{
			++num_half_PI;
			left_angle = D3DX_PI / 2 - total_rotate_angle_;
		}

	}
	else // total_rotate_angle_ < 0
	{
		while (total_rotate_angle_ <= -D3DX_PI / 2)
		{
			total_rotate_angle_ += D3DX_PI / 2;
			--num_half_PI;
		}

		if ((total_rotate_angle_ >= -D3DX_PI / 4) && (total_rotate_angle_ <= 0))
		{
			left_angle = -total_rotate_angle_;
		}

		else // ((total_rotate_angle_ > -D3DX_PI / 2) && (total_rotate_angle_ < -D3DX_PI / 4))
		{
			--num_half_PI;
			left_angle = -D3DX_PI / 2 - total_rotate_angle_;
		}
	}

	Rotate(rotate_axis_, left_angle);

	// Make num_rotate_half_PI > 0, since we will mode 4 later
	// so add it 4 each time, -1 = 3, -2 = 2, -3 = 1
	// because - (pi / 2) = 3 * pi /2, -pi / 2 = pi / 2, - 3 * pi / 2 = pi / 2
	while (num_half_PI < 0)
		num_half_PI += 4;

	num_half_PI %= 4;

	for (int i = 0; i < kNumCubes; ++i)
	{
		if (cubes[i].GetIsSelected())
		{
			cubes[i].UpdateMinMaxPoints(rotate_axis_, num_half_PI);
		}
	}

	// Restore all selected flags of cubes
	for (int i = 0; i < kNumCubes; ++i)
	{
		cubes[i].SetIsSelected(false);
	}

	// When mouse up, one rotation was finished, no cube was selected
	is_cubes_selected_ = false;

	// Enable next rotation.
	rotate_finish_ = true ;
}

LRESULT RubikCube::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)    
	{
	case WM_CREATE:
		break;

	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
			window_active_ = false ;
		break ;

	case WM_LBUTTONDOWN:
		{	
			// Set current window to capture mouse event, so even if the mouse was release outside the window
			// the message still can be correctly processed.
			SetCapture(hWnd) ;
			int iMouseX = ( short )LOWORD( lParam );
			int iMouseY = ( short )HIWORD( lParam );
			OnLeftButtonDown(iMouseX, iMouseY);
		}
		break ;

	case WM_CAPTURECHANGED:
		OnLeftButtonUp();
		ReleaseCapture();
		break;

	case WM_LBUTTONUP:
		{
			OnLeftButtonUp();
			ReleaseCapture();
		}
		break ;

	case WM_MOUSEMOVE:
		{
			int iMouseX = ( short )LOWORD( lParam );
			int iMouseY = ( short )HIWORD( lParam );
			OnMouseMove(iMouseX, iMouseY) ;
		}
		break ;

	case WM_PAINT: 
		Render();
		break ;

	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case 'F':
				ToggleFullScreen() ;
				break;
			case VK_ESCAPE:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break ;
			default:
				break ;
			}
		}
		break ;

	case WM_SIZE: // why not use WM_EXITSIZEMOVE?
		{
			// inactive the app when window is minimized
			if(wParam == SIZE_MINIMIZED)
				window_active_ = false ;

			else if (wParam == SIZE_MAXIMIZED)
			{
				// Get current window size
				current_window_width_ = ( short )LOWORD( lParam );
				current_window_height_ = ( short )HIWORD( lParam );

				if(current_window_width_ != last_window_width_ || current_window_height_ != last_window_height_)
				{
					d3d9->SetBackBufferWidth(current_window_width_);
					d3d9->SetBackBufferHeight(current_window_height_);
					d3d9->ResetDevice();

					last_window_width_ = current_window_width_ ;
					last_window_height_ = current_window_height_ ;
				}
			}

			else if (wParam == SIZE_RESTORED)
			{
				window_active_ = true ;

				// Maximized -> Full Screen
				if (d3d9->GetIsFullScreen() == true)
				{
					// Update back buffer to desktop resolution
					d3d9->SetBackBufferWidth(d3d9->GetScreenWidth());
					d3d9->SetBackBufferHeight(d3d9->GetScreenHeight());

					// Reset device
					d3d9->ResetDevice();
				}
				else
				{
					// Get current window size
					current_window_width_ = ( short )LOWORD( lParam );
					current_window_height_ = ( short )HIWORD( lParam );

					// Reset device
					d3d9->SetBackBufferWidth(current_window_width_);
					d3d9->SetBackBufferHeight(current_window_height_);
					d3d9->ResetDevice();

					last_window_width_ = current_window_width_ ;
					last_window_height_ = current_window_height_ ;
				}
			}
		}
		break ;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;    
	}

	return d3d9->HandleMessages(hWnd, uMsg, wParam, lParam);
}

void RubikCube::InitTextures()
{
	DWORD colors[] = 
	{
		0xffffffff, // White,   front face
		0xffffff00, // Yellow,	back face
		0xffff0000, // Red,		left face
		0xffffa500,	// Orange,	right face
		0xff00ff00, // Green,	top face
		0xff0000ff, // Blue,	bottom face
	};

	// Create face textures
	for(int i = 0; i < kNumFaces; ++i)
	{
		face_textures_[i] = d3d9->CreateTexture(texture_width_, texture_height_, colors[i]);
	}

	// Create inner texture
	inner_textures_ = d3d9->CreateInnerTexture(texture_width_, texture_height_, 0xffffffff);

	Cube::SetFaceTexture(face_textures_, kNumFaces);
	Cube::SetInnerTexture(inner_textures_);
}

int RubikCube::GetWindowPosX() const
{
	return init_window_x_;
}

int RubikCube::GetWindowPosY() const
{
	return init_window_y_;
}

int RubikCube::GetWindowWidth() const
{
	return init_window_width_;
}

int RubikCube::GetWindowHeight() const
{
	return init_window_height_;
}

/*
We get the picked face by check the hit_point's coordinates(x, y, z), suppose the length of the small cube
is cube_length, and the gap length between two layers is gaps, and the Rubik cube's center is coordiates origin.
then, if the left face was picked, then, x == -(1.5 * cube_length + gaps), if the top face was picked, then 
y == 1.5 * length + gaps
*/
Face RubikCube::GetPickedFace(D3DXVECTOR3 hit_point) const
{
	float float_epsilon = 0.001f;
	float face_length = 3 * cube_length_ + 2 * gap_between_layers_;
	float half_face_length = face_length / 2;

	if (fabs(hit_point.z + half_face_length) < float_epsilon) { return kFrontFace;  }
	if (fabs(hit_point.z - half_face_length) < float_epsilon) { return kBackFace;   }
	if (fabs(hit_point.x + half_face_length) < float_epsilon) { return kLeftFace;   }
	if (fabs(hit_point.x - half_face_length) < float_epsilon) { return kRightFace;  }
	if (fabs(hit_point.y - half_face_length) < float_epsilon) { return kTopFace;    }
	if (fabs(hit_point.y + half_face_length) < float_epsilon) { return kBottomFace; }

	return kUnknownFace;
}

D3DXPLANE RubikCube::GeneratePlane(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point)
{
	float abs_diff_x = fabs(previous_point.x - current_point.x);
	float abs_diff_y = fabs(previous_point.y - current_point.y);
	float abs_diff_z = fabs(previous_point.z - current_point.z);

	switch (face)
	{
	case kFrontFace:
	case kBackFace:
		if (abs_diff_x < abs_diff_y)
			return D3DXPLANE(1, 0, 0, -previous_point.x);
		else
			return D3DXPLANE(0, 1, 0, -previous_point.y);
		break;

	case kLeftFace:
	case kRightFace:
		if (abs_diff_y < abs_diff_z)
			return D3DXPLANE(0, 1, 0, -previous_point.y);
		else
			return D3DXPLANE(0, 0, 1, -previous_point.z);
		break;

	case kTopFace:
	case kBottomFace:
		if (abs_diff_x < abs_diff_z)
			return D3DXPLANE(1, 0, 0, -previous_point.x);
		else
			return D3DXPLANE(0, 0, 1, -previous_point.z);
		break;

	default:
		return D3DXPLANE(0, 0, 0, 0);
	}
}

void RubikCube::MarkRotateCubes(D3DXPLANE& plane)
{
	for (int i = 0; i < kNumCubes; ++i)
	{
		// Build a box with the cube's min/max points
		D3DXVECTOR3 min_point = cubes[i].GetMinPoint();
		D3DXVECTOR3 max_point = cubes[i].GetMaxPoint();
		Box box(min_point, max_point);

		// Test whether box intersection with the plane
		// if true, set the cube as selected, the selected cube will be rotated 
		if (PlaneBoxIntersection(plane, box))
		{
			cubes[i].SetIsSelected(true);
		}
	}
}

D3DXVECTOR3 RubikCube::GetRotateAxis(Face face, D3DXVECTOR3& previous_point, D3DXVECTOR3& current_point)
{
	float abs_diff_x = fabs(previous_point.x - current_point.x);
	float abs_diff_y = fabs(previous_point.y - current_point.y);
	float abs_diff_z = fabs(previous_point.z - current_point.z);

	switch (face)
	{
	case kFrontFace:
	case kBackFace:
		if (abs_diff_x < abs_diff_y)
			return D3DXVECTOR3(1, 0, 0);
		else
			return D3DXVECTOR3(0, 1, 0);
		break;

	case kLeftFace:
	case kRightFace:
		if (abs_diff_y < abs_diff_z)
			return D3DXVECTOR3(0, 1, 0);
		else
			return D3DXVECTOR3(0, 0, 1);
		break;

	case kTopFace:
	case kBottomFace:
		if (abs_diff_x < abs_diff_z)
			return D3DXVECTOR3(1, 0, 0);
		else
			return D3DXVECTOR3(0, 0, 1);
		break;

	default:
		return D3DXVECTOR3(0, 0, 0);
	}
}

RotateDirection RubikCube::GetRotateDirection(Face face, D3DXVECTOR3& axis, D3DXVECTOR3& previous_vector, D3DXVECTOR3& current_vector)
{
	float delta_x = previous_vector.x - current_vector.x;
	float delta_y = previous_vector.y - current_vector.y;
	float delta_z = previous_vector.z - current_vector.z;

	// Rotate around x-axis
	if (axis.x != 0)
	{
		switch (face)
		{
			case kFrontFace:
				if (delta_y > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kBackFace:
				if (delta_y < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kTopFace:
				if (delta_z > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kBottomFace:
				if (delta_z < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;
			default:
				return kUnknownDirection;
		}
	}

	// Rotate around y-axis
	else if (axis.y != 0)
	{
		switch (face)
		{
			case kFrontFace:
				if (delta_x < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kBackFace:
				if (delta_x > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kLeftFace:
				if (delta_z > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kRightFace:
				if (delta_z < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			default:
				return kUnknownDirection;
		}
	}

	// Rotate around z-axis
	else // axis.z != 0
	{
		switch (face)
		{
			case kLeftFace:
				if (delta_y < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kRightFace:
				if (delta_y > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kTopFace:
				if (delta_x < 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			case kBottomFace:
				if (delta_x > 0) { return kCounterClockWise; }
				else             { return kClockWise; }
				break;

			default:
				return kUnknownDirection;
		}
	}
}

float RubikCube::CalculateRotateAngle()
{
	// Get the rotation increment
	D3DXQUATERNION quat = world_arcball_->GetRotationQuatIncreament();

	//extract rotation angle from quaternion
	float angle = 2.0f * acosf(quat.w) * rotate_speed_ ;

	return angle;
}

void RubikCube::Rotate(D3DXVECTOR3& axis, float angle)
{
	for (int i = 0; i < kNumCubes; ++i)
	{
		if (cubes[i].GetIsSelected())
		{
			cubes[i].Rotate(axis, angle);
		}
	}
}