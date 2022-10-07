#include "Game.h"
#include <Windows.h>

int screen_Width			= 500;
int screen_Height			= 500;

int iFrames_per_Second		= 60;

void Game::initVariables()
{
	this->window			= nullptr;
	fPosition_X				= screen_Width	/ 2;
	fPosition_Y				= screen_Height	/ 4;
}

void Game::initWindow()
{
	this->videoMode.height	= screen_Height;
	this->videoMode.width	= screen_Width;
	this->window			= new sf::RenderWindow(this->videoMode, "The Best Game In The World", sf::Style::Titlebar | sf::Style::Close);

	this->window->setFramerateLimit(iFrames_per_Second);
}

void Game::initPlayer()
{
	this->player.setPosition	(Get_Position_with_Zoom("x", fPosition_X), Get_Position_with_Zoom("y", fPosition_Y));
	this->player.setOrigin		(Get_Origin_with_Zoom("x") / 3, Get_Origin_with_Zoom("y") / 3);
	this->player.setRotation	(fRocket_Rotation);
	this->player.setScale		(sf::Vector2f((Get_Size_with_Zoom() * (iRocket_Width + iRocket_Width * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity)) / 8, (Get_Size_with_Zoom() * (iRocket_Height - fRocket_Height_based_on_Velocity_Sensitivity * ((iRocket_Height * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity) / iRocket_Height))) / 8));
}

sf::Texture blocks_Grass[13];
sf::Texture blocks_Bush[13];

sf::Texture decoration_Grass[3];
sf::Texture decoration_Bush[3];

sf::Texture ship;

Game::Game()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	for (int i = 0; i < 13; i++) {
		blocks_Grass[i].loadFromFile("images/blocks/grass" + std::to_string(i) + ".png");
		blocks_Bush[i].loadFromFile("images/blocks/bush" + std::to_string(i) + ".png");
		if (i < 3) {
			decoration_Grass[i].loadFromFile("images/decoration/grass" + std::to_string(i) + ".png");
			decoration_Bush[i + 1].loadFromFile("images/decoration/bush" + std::to_string(i) + ".png");
		}
	}

	ship.loadFromFile("images/extra/ship.png");

	this->player.setTexture(ship);

	this->initVariables();
	this->initWindow();
	this->initPlayer();
}

Game::~Game()
{
	delete this->window;
}

const bool Game::running() const
{
	return this->window->isOpen();
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->event)) {

		switch (event.type) {

		case sf::Event::Closed:
			this->window->close();
			break;

		case sf::Event::KeyPressed:
			if (this->event.key.code	== sf::Keyboard::Escape) {
				this->window->close();
			}
			if (this->event.key.code	== sf::Keyboard::Up && bButton_Held_Up == false) {
				bButton_Held_Up						= true;
			}
			if (this->event.key.code	== sf::Keyboard::Right) {
				bButton_Held_Right					= true;
			}
			if (this->event.key.code	== sf::Keyboard::Left) {
				bButton_Held_Left					= true;
			}
			if (this->event.key.code	== sf::Keyboard::Down) {
				bButton_Held_Down					= true;
			}
			if (this->event.key.code	== sf::Keyboard::C) {
				bButton_Held_C						= true;
			}
			break;
		case sf::Event::KeyReleased:
			if (this->event.key.code	== sf::Keyboard::Up) {
				bButton_Held_Up						= false;
			}
			if (this->event.key.code	== sf::Keyboard::Right) {
				bButton_Held_Right					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Left) {
				bButton_Held_Left					= false;
			}
			if (this->event.key.code	== sf::Keyboard::Down) {
				bButton_Held_Down					= false;
			}
			if (this->event.key.code	== sf::Keyboard::C) {
				bButton_Held_C						= false;
			}
			break;
		}
	}
}

void Game::update()
{
	check_If_Player_Is_Entering_Different_Segment();
	this->pollEvents();

	update_Simulation_Speed();

	update_Hard_Fall();
	update_Vertical_Movement();
	update_Horizontal_Movement();
	update_Camera_Position();

	check_If_Player_Is_Colliding_Tiles();

	fPosition_X += fVelocity_X * fSpeed_of_Simulation;
	fPosition_Y += fVelocity_Y * fSpeed_of_Simulation;

	fVelocity_Forward = sqrt(fVelocity_X * fVelocity_X + fVelocity_Y * fVelocity_Y);

	fRocket_Rotation = atan2(fVelocity_Y, fVelocity_X) * 180 / 3.141;

	initPlayer();
}

void Game::update_Simulation_Speed() {
	if (bButton_Held_C) {
		fSpeed_of_Simulation -= fTime_before_simulation_Speed_is_done_Changing / iFrames_per_Second;
		if (fSpeed_of_Simulation < fMin_Simulation_Speed) {
			fSpeed_of_Simulation = fMin_Simulation_Speed;
		}
	}
	else {
		fSpeed_of_Simulation += fTime_before_simulation_Speed_is_done_Changing / iFrames_per_Second;
		if (fSpeed_of_Simulation > 1) {
			fSpeed_of_Simulation = 1;
		}
	}
}

void Game::update_Hard_Fall() {
	if (bButton_Held_Down) {
		fVelocity_Y			+= fRocket_Gravity / iFrames_per_Second		* fRocket_Gravity_Multiplier_when_Down_is_Held * fSpeed_of_Simulation;
		if (fVelocity_Y		> fRocket_Max_Fall_Speed					* fRocket_Max_Fall_Speed_Multiplier_when_Down_is_Held) {
			fVelocity_Y		= fRocket_Max_Fall_Speed					* fRocket_Max_Fall_Speed_Multiplier_when_Down_is_Held;
		}
	}
	else {
		fVelocity_Y			+= fRocket_Gravity / iFrames_per_Second		* fSpeed_of_Simulation;
		if (fVelocity_Y		> fRocket_Max_Fall_Speed) {
			fVelocity_Y		-= 2 * fRocket_Gravity / iFrames_per_Second	* fSpeed_of_Simulation;
			if (fVelocity_Y	< fRocket_Max_Fall_Speed) {
				fVelocity_Y	= fRocket_Max_Fall_Speed;
			}
		}
	}
}

void Game::update_Vertical_Movement() {
	if (bButton_Held_Up) {
		if (fVelocity_Y	> 0) {
			fVelocity_Y	-= 2.625	* fRocket_Gravity / iFrames_per_Second * fSpeed_of_Simulation;
		}
		else {
			fVelocity_Y	-= 2		* fRocket_Gravity / iFrames_per_Second * fSpeed_of_Simulation;
		}
	}
	if (fVelocity_Y		< -fRocket_Max_Fall_Speed) {
		fVelocity_Y		= -fRocket_Max_Fall_Speed;
	}
}

void Game::update_Horizontal_Movement() {
	if         (bButton_Held_Left		== bButton_Held_Right) {
		if     (fVelocity_X	> 0) {
				fVelocity_X	-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
			if (fVelocity_X	< 0) {
				fVelocity_X	= 0;
			}
		}
		if	   (fVelocity_X	< 0) {
				fVelocity_X	+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
			if (fVelocity_X	> 0) {
				fVelocity_X	= 0;
			}
		}
	}
	else if (bButton_Held_Left) {
		if (fVelocity_X		> 0) {
			fVelocity_X		-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		}
		fVelocity_X			-= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		if (fVelocity_X		< -fRocket_Max_Velocity_X) {
			fVelocity_X		= -fRocket_Max_Velocity_X;
		}
	}
	else {
		if (fVelocity_X		< 0) {
			fVelocity_X		+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		}
		fVelocity_X			+= ((1.f / fTime_before_Rocket_X_Velocity_is_Done_Changing) / iFrames_per_Second) * fRocket_Max_Velocity_X * fSpeed_of_Simulation;
		if (fVelocity_X		> fRocket_Max_Velocity_X) {
			fVelocity_X		= fRocket_Max_Velocity_X;
		}
	}
}

void Game::update_Camera_Position() {
	if (fPosition_X - fCamera_Position_X > screen_Width - fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X = fPosition_X - screen_Width + fCamera_Offset_from_Screen_Edge_X;
	}
	if (fPosition_X - fCamera_Position_X < fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X = fPosition_X - fCamera_Offset_from_Screen_Edge_X;
	}
	if (fCamera_Position_X < 0) {
		fCamera_Position_X = 0;
	}
	if (fCamera_Position_X > 80 * fGrid_Cell_Size - screen_Width) {
		fCamera_Position_X = 80 * fGrid_Cell_Size - screen_Width;
	}
}

void Game::check_If_Player_Is_Colliding_Tiles() {
	for (int column = floor(fPosition_X / fGrid_Cell_Size + 0.5) - 1; column < floor(fPosition_X / fGrid_Cell_Size + 0.5) + 1; column++) {
		for (int row = floor(fPosition_Y / fGrid_Cell_Size + 0.5) - 1; row < floor(fPosition_Y / fGrid_Cell_Size + 0.5) + 1; row++) {
			if (map[iLevel_Difficulty][iCurrent_Level_Segment][row][column] == 1 || map[iLevel_Difficulty][iCurrent_Level_Segment][row][column] == 3) {
				if (fPosition_X - 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 < (column + 1) * fGrid_Cell_Size && fPosition_X + 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2 > column * fGrid_Cell_Size && fPosition_Y - 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 < (row + 1) * fGrid_Cell_Size && fPosition_Y + 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2 > row * fGrid_Cell_Size) {
					fPosition_X = 200;
					fPosition_Y = 200;
					fVelocity_X = 0;
					fVelocity_Y = 0;
					fCamera_Position_X = 0;
					fCamera_Position_Y = 0;
				}
			}
		}
	}
}

void Game::check_If_Player_Is_Entering_Different_Segment() {
	if (fPosition_X - fCamera_Position_X > screen_Width - fGrid_Cell_Size) {
		iCurrent_Level_Segment++;
	}
	if (fPosition_X - fCamera_Position_X < fGrid_Cell_Size) {
		iCurrent_Level_Segment--;
	}
}

void Game::render()
{
	this->window->clear(sf::Color(31, 191, 255, 255));

	particles_Render();

	draw();

	this->window->display();
}

void Game::draw() {
	draw_Decoration();

	particles_Draw();

	this->window->draw(this->player);

	draw_Gameplay();
}

void Game::particles_Render() {
	if (fParticle_Timer >= fParticle_Respawn_Rate / iParticle_Total_at_Once) {
		arr_Particle_Velocity_X[iParticles_Init_Num] = 0;
		arr_Particle_Velocity_Y[iParticles_Init_Num] = 0;
		arr_Particle_Position_X[iParticles_Init_Num] = fPosition_X;
		arr_Particle_Position_Y[iParticles_Init_Num] = fPosition_Y;
		arr_Particle_Start_Size[iParticles_Init_Num] = rand() % 3 + 2;
		arr_Particle_Size[iParticles_Init_Num] = arr_Particle_Start_Size[iParticles_Init_Num];
		arr_Particle_Rotation_Speed[iParticles_Init_Num] = rand() % 360;
		arr_Particle_Rotation[iParticles_Init_Num] = rand() % 360;
		arr_Particle_Timer[iParticles_Init_Num] += 1.f / iFrames_per_Second * fSpeed_of_Simulation;
		iParticles_Init_Num++;
		fParticle_Timer -= fParticle_Respawn_Rate / iParticle_Total_at_Once;
	}
	if (iParticles_Init_Num >= iParticle_Total_at_Once) {
		iParticles_Init_Num = 0;
	}
	fParticle_Timer += 1.f / iFrames_per_Second * fSpeed_of_Simulation;

	for (int i = 0; i < iParticle_Total_at_Once; i++) {
		arr_Particle_Velocity_Y[i] += fParticle_Gravity * (1.f / iFrames_per_Second) * fSpeed_of_Simulation;
		arr_Particle_Position_X[i] += arr_Particle_Velocity_X[i] * fSpeed_of_Simulation;
		arr_Particle_Position_Y[i] += arr_Particle_Velocity_Y[i] * fSpeed_of_Simulation;
		arr_Particle_Rotation[i] += arr_Particle_Rotation_Speed[i] * (1.f / iFrames_per_Second) * fSpeed_of_Simulation;
	}
}

void Game::particles_Draw() {
	for (int i = 0; i < iParticle_Total_at_Once; i++) {
		sf::RectangleShape particle;
		particle.setPosition(Get_Position_with_Zoom("x", arr_Particle_Position_X[i]), Get_Position_with_Zoom("y", arr_Particle_Position_Y[i]));
		particle.setSize(sf::Vector2f(arr_Particle_Size[i] * Get_Size_with_Zoom(), arr_Particle_Size[i] * Get_Size_with_Zoom()));
		particle.setRotation(arr_Particle_Rotation[i]);
		particle.setFillColor(sf::Color(255, 255, 255, 255));
		this->window->draw(particle);
	}
}

void Game::draw_Decoration() {
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size); row < int((screen_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] == 2 || map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] == 4) {
				sf::Sprite sprite;
				sprite.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", column * fGrid_Cell_Size));
				sprite.setScale((fGrid_Cell_Size * Get_Size_with_Zoom()) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
				//if (map[level_Difficulty][0][column][row] == 2) {
				//	sprite.setTexture(decoration_Bush[get_Sprite_Number("bush", 2, column, row, false)]);
				//}
				//if (map[level_Difficulty][0][column][row] == 4) {
				//	sprite.setTexture(decoration_Grass[rand()%3]);
				//}
				this->window->draw(sprite);
			}
		}
	}
}

void Game::draw_Gameplay() {
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size); row < int((screen_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] != 2) {
				sf::Sprite sprite;
				sprite.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", column * fGrid_Cell_Size));
				sprite.setScale((fGrid_Cell_Size * Get_Size_with_Zoom()) / 8, (fGrid_Cell_Size * Get_Size_with_Zoom()) / 8);
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] == 1) {
					sprite.setTexture(blocks_Grass[Get_Sprite_Number("block", 1, column, row, true)]);
					this->window->draw(sprite);
				}
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][column][row] == 3) {
					sprite.setTexture(blocks_Bush[Get_Sprite_Number("block", 3, column, row, false)]);
					this->window->draw(sprite);
				}
			}
		}
	}
}

float Game::Get_Size_with_Zoom() {
	return (1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity);
};

float Game::Get_Origin_with_Zoom(const char* The_Axis_Id) {
	if (The_Axis_Id == "x") {
		return ((1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) * (iRocket_Width + iRocket_Width * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity)) / 2;
	}
	if (The_Axis_Id == "y") {
		return ((1 + (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity) * (iRocket_Height - fRocket_Height_based_on_Velocity_Sensitivity * ((iRocket_Height * fVelocity_Forward * fRocket_Width_based_on_Velocity_Sensitivity) / iRocket_Height))) / 2;
	}
}

float Game::Get_Position_with_Zoom(const char* The_Axis_Id, float The_Axis_Pos) {
	if (The_Axis_Id == "x") {
		return The_Axis_Pos - fCamera_Position_X + (The_Axis_Pos - fCamera_Position_X - screen_Width / 2) * (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity;
	}
	if (The_Axis_Id == "y") {
		return The_Axis_Pos - fCamera_Position_Y + (The_Axis_Pos - fCamera_Position_Y - screen_Height / 2) * (1 - fSpeed_of_Simulation) * fZoom_based_on_Simulation_Speed_Sensitivity;
	}
};

int block_Number;

int Game::Get_Sprite_Number(const char* The_Sprite, int The_Block_Number, int The_Column, int The_Row, bool Is_Grouped) {
	if (The_Sprite == "block") {
		if (Is_Grouped) {
			if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] != The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] == The_Block_Number && The_Column != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] != The_Block_Number && The_Column == 19) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
					return 0;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number) {
					return 1;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
					return 2;
				}
			}
			else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] == The_Block_Number || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] == The_Block_Number && The_Column == 19 || The_Column == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] == The_Block_Number) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
					return 3;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number) {
					if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row - 1] != The_Block_Number && The_Column != 0 && The_Row != 0) {
						return 9;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row + 1] != The_Block_Number && The_Column != 0 && The_Row != 79) {
						return 10;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row + 1] != The_Block_Number && The_Column != 19 && The_Row != 79) {
						return 11;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row - 1] != The_Block_Number && The_Column != 19 && The_Row != 0) {
						return 12;
					}
					return 4;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
					return 5;
				}
			}
			else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] != The_Block_Number && The_Column != 19 || The_Column == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] != The_Block_Number) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != The_Block_Number && The_Row == 79 && The_Row != 0) {
					return 6;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == The_Block_Number) {
					return 7;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == The_Block_Number && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number && The_Row != 79) {
					return 8;
				}
			}
		}
		else {
			if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] != 0 && The_Column != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] == 0 && The_Column == 19) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
					return 0;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0) {
					return 1;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79) {
					return 2;
				}
			}
			else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] != 0 && The_Column == 19 || The_Column == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] != The_Block_Number) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
					return 3;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number) {
					if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row - 1] == 0 && The_Column != 0 && The_Row != 0) {
						return 9;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row + 1] == 0 && The_Column != 0 && The_Row != 79) {
						return 10;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row + 1] == 0 && The_Column != 19 && The_Row != 79) {
						return 11;
					}
					else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row - 1] == 0 && The_Column != 19 && The_Row != 0) {
						return 12;
					}
					return 4;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79) {
					return 5;
				}
			}
			else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column - 1][The_Row] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] == 0 && The_Column != 19 || The_Column == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column + 1][The_Row] == 0) {
				if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 && The_Row != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] == 0 && The_Row == 79 && The_Row != 0) {
					return 6;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != 0 || map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && The_Row == 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] != The_Block_Number) {
					return 7;
				}
				else if (map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row - 1] != 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79 || The_Row == 0 && map[iLevel_Difficulty][iCurrent_Level_Segment][The_Column][The_Row + 1] == 0 && The_Row != 79) {
					return 8;
				}
			}
		}
	}
}