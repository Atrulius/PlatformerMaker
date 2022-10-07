#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

using namespace std;

class Game {

private:


	sf::RenderWindow*	window;
	sf::VideoMode		videoMode;
	sf::Event			event;

	sf::Sprite			player;

	void	Initialize_Variables();
	void	Initialize_Window();
	void	Initialize_Glow();
	void	Initialize_Decoration();
	void	Initialize_Decoration_Particles();

public:

	float	fPosition_X;
	float	fPosition_Y;
	float	fVelocity_X;
	float	fVelocity_Y;
	float	fAcceleration_X;
	float	fAcceleration_Y;

	float	fFlip_X = 1;
	float	fFlip_Y = 1;

	float	fRotation = 0;

	float	fCamera_Offset_from_Screen_Edge_X = 200;
	float	fCamera_Offset_From_Screen_Edge_Y = 175;
	float	fCamera_Follow_Speed = 0.2;

	bool	bButton_Clicked_Up = false;
	bool	bButton_Clicked_Left = false;
	bool	bButton_Clicked_Right = false;
	bool	bButton_Clicked_Down = false;

	bool	bButton_Holding_Up = false;
	bool	bButton_Holding_Left = false;
	bool	bButton_Holding_Right = false;
	bool	bButton_Holding_Down = false;

	bool	bButton_Held_Up = false;
	bool	bButton_Held_Left = false;
	bool	bButton_Held_Right = false;
	bool	bButton_Held_Down = false;

	bool	bButton_Released_Up = false;
	bool	bButton_Released_Left = false;
	bool	bButton_Released_Right = false;
	bool	bButton_Released_Down = false;

	bool	bButton_Clicked_Tab = false;
	bool	bButton_Holding_Tab = false;
	bool	bButton_Held_Tab = false;
	bool	bButton_Released_Tab = false;

	bool	bButton_Held_C = false;

	bool	bButton_Held_ESC = false;
	bool	bButton_Pressed_ESC = false;
	bool	bButton_Clicked_Esc = false;

	bool	bButton_Held_Enter = false;
	bool	bButton_Pressed_Enter = false;

	bool	bButton_Held_Shift = false;
	bool	bButton_Pressed_Shift = false;
	bool	bButton_Released_Shift = false;

	bool	bButton_Held_Mouse1 = false;
	bool	bButton_Pressed_Mouse1 = false;
	bool	bButton_Released_Mouse1 = false;
	bool	bButton_Clicked_Mouse1 = false;
	bool	bButton_Held_Mouse2 = false;
	bool	bButton_Pressed_Mouse2 = false;
	bool	bButton_Released_Mouse2 = false;
	bool	bButton_Clicked_Mouse2 = false;

	bool	bEditor;

	int		iCurrent_Chapter = -1;

	int		iSelected_Difficulty = 0;
	int		selected_Option = 1;

	bool	bExit_Warning = false;

	bool	bPlaying = false;
	bool	bStart_Menu = true;

	float	mouseX = 0;
	float	mouseY = 0;

	int		iGlow_Total = 40;

	int		arrGlow_Position_X[40];
	int		arrGlow_Position_Y[40];
	int 	arrGlow_Scale[40];
	float	arrGlow_Brightness[40];
	float	arrGlow_Offset_X[40];
	float	arrGlow_Offset_Y[40];
	float	arrGlow_Velocity_X[40];
	float	arrGlow_Velocity_Y[40];

	bool	arrGlow_Velocity_Increase_X[40];
	bool	arrGlow_Velocity_Increase_Y[40];

	float	fGlow_Timer = 0;
	float	fGlow_Time = 0.5;

	float	fGlow_Velocity_Speed = 0.1;

	float	fGlow_Offset_based_on_Size_Multiplier = 0.1;

	bool	bStarted = false;

	bool	bFlying = false;
	bool	bWas_Flying = false;

	float	fRespawning_Last_Timer = 0.f;
	float	fRespawning_Timer = 0.5f;
	float	fRespawning_Time = 1.f;

	int		iRespawning_Max_Offset = 50;
	int		iRespawning_Max_Acceleration = 150;
	int		iRespawning_Min_Acceleration = 75;
	int		iRespawning_Max_Points = 6;
	int		iRespawning_Min_Points = 4;

	float	arrRespawning_Offset_X[5][5];
	float	arrRespawning_Offset_Y[5][5];
	float	arrRespawning_Acceleration[5][5];
	float	arrRespawning_Rotation[5][5];
	int		arrRespawning_Points[5][5];
	bool	arrRespawning_Rotating_Left[5][5];

	bool	bRespawning_Growing;
	bool	bRespawning_Shrinking;
	float	fRespawning_Size;

	int		iRespawning_Animation = 0;

	int		iDecoration_Max_Size = 10;
	int		iDecoration_Min_Size = 5;
	int		iDecoration_Max_Offset = 50;
	int		iDecoration_Max_Main_Velocity = 100;
	int		iDecoration_Min_Main_Velocity = 75;
	float	fDecoration_Randomness_Timer = 0;
	float	fDecoration_Randomness_Time = 0.5;

	int	arrDecoration_Color[5][2][4];

	float	arrow_Distance = 0;
	float	arrow_Velocity = 0;
	float	arrow_Start_Velocity = 5;
	float	arrow_Acceleration = 0.75; // TIME IN SECONDS

	int		died_Next_Level = -1;
	int		died_Next_Chapter = -1;

	float	dragX;
	float	dragY;

	int		iEditor_Sidebar_Width = 75;
	int		iEditor_Sidebar_Min_Width = 40;
	int		iEditor_Sidebar_Max_Width = 150;
	float	fEditor_Items_Width = 0.8;

	bool	bResizing_On_Edge = false;

	bool	right_Side;
	bool	no_Selected;

	int		iLevel;

	int		iBlock = 1;

	bool	bEditing;

	bool	window_Is_Active = true;

	int		iLast_Level_Sound;

	bool	bEditor_Resizing = false;

	int		fillMap[20][80];

	int		map[5][20][80];

	bool	bUsed_Editor = false;

	bool	bEditor_Selecting_Area = false;

	bool	bEditing_Character = false;
	bool	bEditing_Controls = false;
	bool	bEditing_Controls_Extra = false;

	bool	bEditing_Blocks = false;

	bool	bEditing_Background = false;

	bool	bEditing_Particles = false;

	bool	bEditing_Song = false;
	bool	bTyping_ID = false;
	string	sTyping_ID = "";

	int		iControl_Editing = 0;

	bool	bDrawing_Remove_Pixel = false;

	bool	bStatic_Block_Selected[5][3];
	bool	bDynamic_Block_Selected[5];
	bool	bDrawing_Character;

	bool	bPaint_Select_Tool[8][8];
	bool	bPaint_Select_Tool_Used;

	sf::Color colCurrent_Drawing_Color;

	bool	bCurrent_Tool_Active[4];

	int		iMouse_Wheel_Moved;

	bool	bButton_Clicked_Num[10];
	bool	bButton_Holding_Num[10];
	bool	bButton_Held_Num[10];

	bool	bButton_Clicked_Backspace = false;

	bool	bMusic_Playtest = false;

	int		iLevel_Editing_Layer = 1;

	bool	bEditing_Character_Text = false;

	string	sTyping_Input = "";
	bool bErase = false;
	bool bDelete = false;
	bool bReturn = false;



	int		iControl_Settings				[2][4][3][3][5];


	int		iMax_Velocity_X						[5]			;
	int		iMax_Velocity_Y						[5]			;

	int		iPull_Velocity_X					[5]			;
	int		iPull_Velocity_Y					[5]			;

	int		iDrag_Velocity_X					[5]			;
	int		iDrag_Velocity_Y					[5]			;

	int		iRotation_With_Velocity_X			[5]			;	bool	bRotation_With_Velocity_X			[5]	;
	int		iRotation_With_Velocity_Y			[5]			;	bool	bRotation_With_Velocity_Y			[5]	;
																bool	bRotation_With_Velocity				[5]	;

																bool	bFlip_X								[5]	;
																bool	bFlip_Y								[5]	;


	sf::Image imgStatic_Block[5][5][3];
	sf::Image imgDynamic_Block[5][5];


	int		arrColor							[5][3][3]	;


	int		iParticle_Acceleration_X			[5]			;
	int		iParticle_Acceleration_Y			[5]			;

	int		iParticle_Static_Velocity_X			[5]			;	bool	bParticle_Static_Velocity_X			[5]	;
	int		iParticle_Static_Velocity_Y			[5]			;	bool	bParticle_Static_Velocity_Y			[5]	;



	sf::String	sMusic_Properties[5][3];

	sf::Sound sndLevelMusic[5];
	sf::SoundBuffer sbLevelMusic[5];




	const bool running() const;

	Game();
	virtual ~Game();

	void pollEvents();
	void typing_Inputs(sf::Event event);

	void Update();
	void Update_Segment();
	void Update_Editor_X();
	void Update_Editor_Y();
	void Update_Camera_Position();
	void Update_Collision();
	void Update_Glow();
	void Update_Editor();
	void Update_Editor_Layer_1();
	void Update_Editor_Layer_2();
	//void Update_Editor_Sidebar();
	void Update_Dead();
	void Update_Decoration();
	void Update_Particles();
	void Respawn();

	void Draw();
	void Draw_Menu();
	void Draw_Editor_Menu();
	void Draw_Character_Menu();
	void Draw_Character_Text();
	void Draw_Controls_Menu();
	void Draw_Controls_Extra_Menu();
	void Draw_Blocks_Menu();
	void Draw_Paint_Menu(bool &The_Boolean, sf::Image &The_Image);
	sf::Image Draw_Paint_Fill(int The_X, int The_Y, sf::Color The_Fill_Color, sf::Color The_Color_To_Fill, sf::Image The_Image);
	void Draw_Blocks_Dynamic_Menu(int x);
	void Draw_Background_Menu(int x);
	void Draw_Particles_Menu();
	void Draw_Song_Menu();
	void Draw_Start_Menu();
	void Draw_Game();
	void Draw_Player();
	void Draw_Respawn();
	void Draw_Glow();
	void Draw_Gameplay();
	void Draw_Editor_Triggers();
	void Draw_Decoration();
	void Draw_Particles();
	void Draw_Editor();
	void Draw_Editor_Grid();
	//void Draw_Editor_Sidebar();
	//void Draw_Editor_Items();

	void Text(string The_Text, string The_Font, float The_Position_X, float The_Position_Y, int The_Font_Size, sf::Color col, int The_Offset_X, bool Shadow);
	void Attatch_Text_to_Rect(sf::RectangleShape rect, string The_Text, string The_Font, sf::Color col, int The_Offset_X, bool Shadow);

	bool Toggle_Switch(float The_Position_X, float The_Position_Y, bool The_Boolean, bool Is_Vertical);
	int Value_Holder(float The_Position_X, float The_Position_Y, int The_Size_X, int The_Size_Y, int The_Minimum, int The_Maximum, int The_Int);

	bool Mouse_is_Hovering_Rect(sf::RectangleShape rect);

	void Download_Newgrounds_Song(string The_ID);
	string Download_Webpage_to_String(string The_Webpage);

	void Goto_Chapter(int The_Chapter);

	void Save();
	void Save_Sidebar_Width();
	void Save_Editor(ofstream& createFile, int i);
	void Save_Character_Properties(ofstream& createFile, int i);
	void Save_Block_Properties(ofstream& createFile, int i);
	void Save_Particle_Properties(ofstream& createFile, int i);
	void Save_Background_Properties(ofstream& createFile, int i);

	float Get_Modulus(float The_Number, float The_Modulus);

	static float	Get_Size_with_Zoom();
	float	Get_Origin_with_Zoom(const char* The_Axis_Id);
	static float	Get_Position_with_Zoom(const char* The_Axis_Id, float The_Axis_Pos);
	int		Get_Sprite_Number(const char* The_Sprite, int The_Block_Number, int The_Column, int The_Row, bool Is_Grouped);



	class Background_Particle {
	public:
		float	fSize;
		float	fRotation;
		float	fOffset_X;
		float	fOffset_Y;
		float	fPosition_X;
		float	fPosition_Y;
		float	fVelocity_X;
		float	fVelocity_Y;
		float	fMain_Velocity;
		float	fAcceleration_X;
		float	fAcceleration_Y;
		float	fColor_Value;
		float	fColor_Velocity;
		float	fColor_Acceleration;
	};

	Background_Particle bpBackground_Particle[64];

	class Particle_Group {
	public:

		class Particle {
		public:
			int			iTime;

			float		fSize_X;
			float		fSize_Y;
			float		fVelocity_X;
			float		fVelocity_Y;
			float		fRotation;

			float		fPosition_X;
			float		fPosition_Y;

			float		fOffset_X;
			float		fOffset_Y;

			sf::Color	colColor;

			float		fStart_Size_X;
			float		fStart_Size_Y;
			float		fStart_Velocity_X;
			float		fStart_Velocity_Y;
			float		fStart_Rotation;

			sf::Color	colStart_Color;

			float		fEnd_Size_X;
			float		fEnd_Size_Y;
			float		fEnd_Velocity_X;
			float		fEnd_Velocity_Y;
			float		fEnd_Rotation;

			sf::Color	colEnd_Color;
		};

		float		fPosition_X;
		float		fPosition_Y;

		int			iTotal_Particles;
		int			iLifetime;

		int			iTimer;

		float		fStart_Size_X;			float fStart_Size_X_PM;
		float		fStart_Size_Y;			float fStart_Size_Y_PM;
		float		fStart_Velocity_X;		float fStart_Velocity_X_PM;
		float		fStart_Velocity_Y;		float fStart_Velocity_Y_PM;
		float		fStart_Rotation;		float fStart_Rotation_PM;

		sf::Color	colStart_Color;

		float		fEnd_Size_X;			float fEnd_Size_X_PM;
		float		fEnd_Size_Y;			float fEnd_Size_Y_PM;
		float		fEnd_Velocity_X;		float fEnd_Velocity_X_PM;
		float		fEnd_Velocity_Y;		float fEnd_Velocity_Y_PM;
		float		fEnd_Rotation;			float fEnd_Rotation_PM;

		sf::Color	colEnd_Color;

		Particle particle[0];

		void Set_Total(int The_Total);

		void Initialize_Particle(int i);

		void Initialize();

		void Update_Particle(int i);

		void Update();

		void Draw_Particle(int i, sf::RenderWindow*& window);

		void Draw(sf::RenderWindow*& window);
	};

	class Background_Trigger{
	public:
		bool editing;

		bool active;
		sf::Color color;
	};

	Background_Trigger bgTrigger[5][80];
};