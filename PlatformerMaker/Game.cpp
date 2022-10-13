#include <random>
#include "Game.h"
#include <Windows.h>
#include <fstream>
#include "Character.h"
#pragma comment(lib, "urlmon.lib")

using namespace std;

int game_Width			= 500;
int game_Height			= 500;
int screen_Min			= 500;
int iFrames_per_Second  = 60;
float screen_X_Offset	= 0;
float screen_Y_Offset	= 0;
float fCamera_Position_X = 0;
float fCamera_Position_Y = 0;

float fGrid_Cell_Size = 25;

int block_Number;

sf::Texture blocks_Block[5][13];
sf::Texture blocks_Mixed[5][13];
sf::Texture blocks_Spike[5][13];
sf::Texture blocks_Top[5][3];

sf::Texture decoration_Air[5][3];

sf::Texture character[5];

sf::Texture icon;

sf::Texture menu;

sf::Texture glow;

sf::Texture txtrTransparency_Pattern;

sf::Texture txtrStatic_Block[5][5][3];
sf::Texture txtrDynamic_Block[5][5][4];

Game::Particle_Group pgCharacter;

string Get_CMD_Response(const char* The_CMD) {
	char buffer[128];
	string result = "";
	FILE* pipe = _popen(The_CMD, "r");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}
	catch (...) {
		_pclose(pipe);
		throw;
	}
	_pclose(pipe);
	return result;
}

const bool Game::running() const
{
	return window->isOpen();
}

Game::Game()
{ 
	pgCharacter.iLifetime = 350;

	pgCharacter.fStart_Size_X = 8;		pgCharacter.fStart_Size_X_PM = 2;
	pgCharacter.fStart_Size_Y = 8;		pgCharacter.fStart_Size_Y_PM = 2;
	pgCharacter.fStart_Velocity_X = 0;	pgCharacter.fStart_Velocity_X_PM = 3;
	pgCharacter.fStart_Velocity_Y = 0;	pgCharacter.fStart_Velocity_Y_PM = 3;
	pgCharacter.fStart_Rotation = 0;	pgCharacter.fStart_Rotation_PM = 180;

	pgCharacter.colStart_Color = sf::Color(255, 255, 255, 255);

	pgCharacter.fEnd_Size_X = 0;		pgCharacter.fEnd_Size_X_PM = 0;
	pgCharacter.fEnd_Size_Y = 0;		pgCharacter.fEnd_Size_Y_PM = 0;
	pgCharacter.fEnd_Velocity_X = 0;	pgCharacter.fEnd_Velocity_X_PM = 2;
	pgCharacter.fEnd_Velocity_Y = 5;	pgCharacter.fEnd_Velocity_Y_PM = 1;
	pgCharacter.fEnd_Rotation = 0;		pgCharacter.fEnd_Rotation_PM = 180;

	pgCharacter.colEnd_Color = sf::Color(0, 0, 0, 0);

	pgCharacter.iTotal_Particles = 25;

	pgCharacter.Initialize();

	//for (int i = 0; i < 5; i++) {
	//	sbLevelMusic[i].loadFromFile("sound/music/" + to_string(i) + "/music.wav");
	//	sndLevelMusic[i] = sf::Sound(sbLevelMusic[i]);
	//	sndLevelMusic[i].setLoop(true);
	//}

	for (int i = 0; i < 5; i++) {
		ifstream load("saves/levels/" + to_string(i) + ".txt");

		vector<int> file;

		int input = 0;

		int num = 0;

		while (load >> input) {
			file.push_back(input);
			num++;
		}
		if (num == 0) {
			sf::Image imgCharacter;
			imgCharacter.create(8, 8);
			for (int pY = 0; pY < 8; pY++) {
				for (int pX = 0; pX < 8; pX++) {
					imgCharacter.setPixel(pX, pY, sf::Color(0, 0, 0, 0));
				}
			}
			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 3; y++) {
					imgStatic_Block[i][x][y].create(8, 8);
					for (int pY = 0; pY < 8; pY++) {
						for (int pX = 0; pX < 8; pX++) {
							imgStatic_Block[i][x][y].setPixel(pX, pY, sf::Color(0, 0, 0, 0));
						}
					}
				}
				for (int img = 0; img < 4; img++) {
					imgDynamic_Block[i][x][img].create(4, 4);
					for (int pY = 0; pY < 4; pY++) {
						for (int pX = 0; pX < 4; pX++) {
							imgDynamic_Block[i][x][img].setPixel(pX, pY, sf::Color(0, 0, 0, 0));
						}
					}
				}
			}

			character[i].loadFromImage(imgCharacter);
		}
		else {


			num = 0;

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					map[i][y][x] = file[num]; num++;
				}
			}


			for (int condition = 0; condition < 3; condition++) {
				for (int action = 0; action < 3; action++) {
					for (int direction = 0; direction < 4; direction++) {
						for (int type = 0; type < 2; type++) {
							if (type != 1 || (condition != 1 && action != 0)) { iControl_Settings[type][direction][condition][action][i] = file[num]; num++; }
						}
					}
				}
			}

			iMax_Velocity_X[i] = file[num]; num++;
			iMax_Velocity_Y[i] = file[num]; num++;

			iPull_Velocity_X[i] = file[num]; num++;
			iPull_Velocity_Y[i] = file[num]; num++;

			iDrag_Velocity_X[i] = file[num]; num++;
			iDrag_Velocity_Y[i] = file[num]; num++;

			iRotation_With_Velocity_X[i] = file[num]; num++; bRotation_With_Velocity_X[i] = file[num]; num++;
			iRotation_With_Velocity_Y[i] = file[num]; num++; bRotation_With_Velocity_Y[i] = file[num]; num++;
			bRotation_With_Velocity[i] = file[num]; num++;

			bFlip_X[i] = file[num]; num++;
			bFlip_Y[i] = file[num]; num++;

			sf::Image imgCharacter;
			imgCharacter.create(8, 8);

			for (int pY = 0; pY < 8; pY++) {
				for (int pX = 0; pX < 8; pX++) {
					imgCharacter.setPixel(pX, pY, sf::Color(file[num], file[num + 1], file[num + 2], file[num + 3])); num += 4;
				}
			}
			character[i].loadFromImage(imgCharacter);

			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 3; y++) {
					imgStatic_Block[i][x][y].create(8, 8);
					for (int pY = 0; pY < 8; pY++) {
						for (int pX = 0; pX < 8; pX++) {
							imgStatic_Block[i][x][y].setPixel(pX, pY, sf::Color(file[num], file[num + 1], file[num + 2], file[num + 3])); num += 4;
						}
					}
				}
			}
			for (int x = 0; x < 5; x++) {
				for (int img = 0; img < 4; img++) {
					imgDynamic_Block[i][x][img].create(4, 4);
				}
				for (int pY = 0; pY < 4; pY++) {
					for (int img = 0; img < 4; img++) {
						for (int pX = 0; pX < 4; pX++) {
							imgDynamic_Block[i][x][img].setPixel(pX, pY, sf::Color(file[num], file[num + 1], file[num + 2], file[num + 3])); num += 4;
						}
					}
				}
			}

			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 4; k++) {
					arrDecoration_Color[i][j][k] = file[num]; num++;
				}
			}

			iParticle_Acceleration_X[i] = file[num]; num++;
			iParticle_Acceleration_Y[i] = file[num]; num++;

			iParticle_Static_Velocity_X[i] = file[num]; num++; bParticle_Static_Velocity_X[i] = file[num]; num++;
			iParticle_Static_Velocity_Y[i] = file[num]; num++; bParticle_Static_Velocity_Y[i] = file[num]; num++;


			for (int x = 0; x < 80; x++) {
				bgTrigger[i][x].active = file[num]; num++;
				bgTrigger[i][x].color.r = file[num]; num++;
				bgTrigger[i][x].color.g = file[num]; num++;
				bgTrigger[i][x].color.b = file[num]; num++;
			}
		}
	}

	bCurrent_Tool_Active[0] = true;
	bCurrent_Tool_Active[1] = false;
	bCurrent_Tool_Active[2] = false;
	bCurrent_Tool_Active[3] = false;

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	icon.loadFromFile("images/extra/icon.png");

	menu.loadFromFile("images/extra/menu.png");

	glow.loadFromFile("images/decoration/glow.png");

	arrow_Velocity = arrow_Start_Velocity;
	arrow_Acceleration = -(2 * arrow_Start_Velocity / arrow_Acceleration);

	sf::Image img;

	img.create(2, 2);

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			img.setPixel(x, y, (x + y) % 2 == 0 ? sf::Color(167, 167, 167) : sf::Color(87, 87, 87));
		}
	}

	txtrTransparency_Pattern.loadFromImage(img);

	for (int world = 0; world < 5; world++) {
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 3; y++) {
				txtrStatic_Block[world][x][y].loadFromImage(imgStatic_Block[world][x][y]);
			}
			for (int img = 0; img < 4; img++) {
				txtrDynamic_Block[world][x][img].loadFromImage(imgDynamic_Block[world][x][img]);
			}
		}
	}

	Initialize_Variables();
	Initialize_Glow();
	Initialize_Window();
}

Game::~Game()
{
	delete window;
}

void Game::Initialize_Variables()
{
	window = nullptr;
}

void Game::Initialize_Glow() {
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist4000(1, 4000);
	uniform_int_distribution<mt19937::result_type> dist1000(1, 1000);
	uniform_int_distribution<mt19937::result_type> dist500(1, 500);
	uniform_int_distribution<mt19937::result_type> dist255(1, 255);
	for (int i = 0; i < iGlow_Total; i++) {
		arrGlow_Position_X[i] = dist4000(rng) - 750;
		arrGlow_Position_Y[i] = dist1000(rng) - 750;
		arrGlow_Scale[i] = dist500(rng) + 1000;
		arrGlow_Brightness[i] = float(dist255(rng)) / 255;
		//cout << arrGlow_Position_X[i] << ", " << arrGlow_Position_Y[i] << ", " << arrGlow_Scale[i] << ", " << arrGlow_Brightness[i] << "\n";
	}
}

void Game::Initialize_Decoration() {
	Initialize_Decoration_Particles();
}

void Game::Initialize_Decoration_Particles() {
	for (int i = 0; i < 64; i++) {
		bpBackground_Particle[i].fSize = iDecoration_Min_Size + rand() % (iDecoration_Max_Size - iDecoration_Min_Size);
		bpBackground_Particle[i].fMain_Velocity = iDecoration_Min_Main_Velocity + rand() % (iDecoration_Max_Main_Velocity - iDecoration_Min_Main_Velocity);
		bpBackground_Particle[i].fRotation = rand() % 90;
		bpBackground_Particle[i].fOffset_X = 0;
		bpBackground_Particle[i].fOffset_Y = 0;
		bpBackground_Particle[i].fVelocity_X = 0;
		bpBackground_Particle[i].fVelocity_Y = 0;
		bpBackground_Particle[i].fPosition_X = rand() % int(80 * fGrid_Cell_Size);
		bpBackground_Particle[i].fPosition_Y = rand() % int(20 * fGrid_Cell_Size);
		bpBackground_Particle[i].fAcceleration_X = bParticle_Static_Velocity_X[iCurrent_Chapter] ? 0 : iParticle_Acceleration_X[iCurrent_Chapter] ? float(rand() % (iParticle_Acceleration_X[iCurrent_Chapter] * 2) / 10) - float(iParticle_Acceleration_X[iCurrent_Chapter]) / 10 : 0;
		bpBackground_Particle[i].fAcceleration_Y = bParticle_Static_Velocity_Y[iCurrent_Chapter] ? 0 : iParticle_Acceleration_Y[iCurrent_Chapter] ? float(rand() % (iParticle_Acceleration_Y[iCurrent_Chapter] * 2) / 10) - float(iParticle_Acceleration_Y[iCurrent_Chapter]) / 10 : 0;
		bpBackground_Particle[i].fColor_Value = float(rand() % 101) / 100;
		if (bpBackground_Particle[i].fColor_Value > 0.5) {
			bpBackground_Particle[i].fColor_Velocity = -1;
		}
		else {
			bpBackground_Particle[i].fColor_Velocity = 1;
		}
		bParticle_Static_Velocity_X[iCurrent_Chapter] ? bpBackground_Particle[i].fVelocity_X = iParticle_Static_Velocity_X[iCurrent_Chapter] : bpBackground_Particle[i].fVelocity_X = 0;
		bParticle_Static_Velocity_Y[iCurrent_Chapter] ? bpBackground_Particle[i].fVelocity_Y = iParticle_Static_Velocity_Y[iCurrent_Chapter] : bpBackground_Particle[i].fVelocity_Y = 0;
	}
}

void Game::Initialize_Window()
{
	videoMode.height = game_Height;
	videoMode.width = game_Width;
	window = new sf::RenderWindow(videoMode, "Platformer Maker", sf::Style::Default);

	sf::Image icon;
	icon.loadFromFile("images/extra/icon.png");
	window->setIcon(32, 32, icon.getPixelsPtr());

	window->setFramerateLimit(iFrames_per_Second);
}

void Game::pollEvents()
{
	while (window->pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			Save();
			window->close();
			break;
		}
		typing_Inputs(event);
		if (event.type == sf::Event::MouseWheelMoved) {
			iMouse_Wheel_Moved = event.mouseWheel.delta;
		}
		if (event.type == sf::Event::KeyPressed) {
			for (int num = 0; num < 10; num++) {
				if (event.key.code == (num == 0 ? sf::Keyboard::Num0 : num == 1 ? sf::Keyboard::Num1 : num == 2 ? sf::Keyboard::Num2 : num == 3 ? sf::Keyboard::Num3 : num == 4 ? sf::Keyboard::Num4 : num == 5 ? sf::Keyboard::Num5 : num == 6 ? sf::Keyboard::Num6 : num == 7 ? sf::Keyboard::Num7 : num == 8 ? sf::Keyboard::Num8 : sf::Keyboard::Num9)) {
					bButton_Clicked_Num[num] = true;
				}
			}
			if (event.key.code == sf::Keyboard::Backspace) {
				bButton_Clicked_Backspace = true;
			}

			if (event.key.code == sf::Keyboard::Enter) {
				bButton_Held_Enter = true;
			}
			if (event.key.code == sf::Keyboard::Escape) {
				if (!bButton_Held_ESC)	bButton_Clicked_Esc = true;
										bButton_Held_ESC = true;
			}
			if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
				if (!bButton_Holding_Up)	bButton_Clicked_Up		= true;
											bButton_Holding_Up		= true;
			}
			if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
				if (!bButton_Holding_Right)	bButton_Clicked_Right	= true;
											bButton_Holding_Right	= true;
			}
			if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
				if (!bButton_Holding_Left)	bButton_Clicked_Left	= true;
											bButton_Holding_Left	= true;
			}
			if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
				if (!bButton_Holding_Down)	bButton_Clicked_Down	= true;
											bButton_Holding_Down	= true;
			}
			if (event.key.code == sf::Keyboard::Tab) {
				if (!bButton_Holding_Tab)	bButton_Clicked_Tab = true;
											bButton_Holding_Tab = true;
			}
			if (event.key.code == sf::Keyboard::C) {
				bButton_Held_C = true;
			}
			if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
				bButton_Held_Shift = true;
			}
			if (bEditing && bPlaying) {
				if (event.key.code == sf::Keyboard::Num1) {
					iLevel = 0;
				}
				if (event.key.code == sf::Keyboard::Num2) {
					iLevel = 1;
				}
				if (event.key.code == sf::Keyboard::Num3) {
					iLevel = 2;
				}
				if (event.key.code == sf::Keyboard::Num4) {
					iLevel = 3;
				}
				if (event.key.code == sf::Keyboard::Num5) {
					iLevel = 4;
				}
				if (event.key.code == sf::Keyboard::Q) {
					iBlock = 1;
				}
				if (event.key.code == sf::Keyboard::W) {
					iBlock = 2;
				}
				if (event.key.code == sf::Keyboard::E) {
					iBlock = 3;
				}
				if (event.key.code == sf::Keyboard::R) {
					iBlock = 4;
				}
				if (event.key.code == sf::Keyboard::T) {
					iBlock = 5;
				}
				if (event.key.code == sf::Keyboard::Left) {
					if (iCurrent_Chapter != 0) {
						iCurrent_Chapter--;
						fCamera_Position_X = 0;
					}
				}
				if (event.key.code == sf::Keyboard::Right) {
					if (iCurrent_Chapter != 4) {
						iCurrent_Chapter++;
						fCamera_Position_X = 0;
					}
				}
				if (event.key.code == sf::Keyboard::Backspace) {
					for (int y = 0; y < 20; y++) {
						for (int x = 0; x < 80; x++) {
							map[iCurrent_Chapter][y][x] = 1;
							if (y > 0 && y < 19 && x > 0 && x < 79) {
								map[iCurrent_Chapter][y][x] = 0;
							}
						}
					}
				}
			}
			if (event.key.code == sf::Keyboard::Space && bPlaying) {
				bEditing = !bEditing;
				if (bEditing) {
					fPosition_Y = -50;
					if (fRespawning_Timer < fRespawning_Time / 2) {
						fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
						fRespawning_Last_Timer = fRespawning_Timer;
					}
				}
				else {
					if (fRespawning_Timer < fRespawning_Time) {
						if (fRespawning_Timer > fRespawning_Time / 2) {
							fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
						}
					}
					else {
						fRespawning_Timer = 0;
					}
				}
			}
			break;
		}

		if (event.type == sf::Event::GainedFocus) {
			window_Is_Active = true;
		}
		if (event.type == sf::Event::LostFocus) {
			window_Is_Active = false;
		}
		if (event.type == sf::Event::MouseWheelMoved && bEditing) {
			fCamera_Position_X -= event.mouseWheel.delta * fGrid_Cell_Size;
			if (fCamera_Position_X < 0 * fGrid_Cell_Size) {
				fCamera_Position_X = 0;
			}
			if (fCamera_Position_X > 60 * fGrid_Cell_Size) {
				fCamera_Position_X = 60 * fGrid_Cell_Size;
			}
		}
		if (event.type == sf::Event::KeyReleased) {

			if (event.key.code	== sf::Keyboard::Enter) {
				bButton_Held_Enter					= false;
			}
			if (event.key.code	== sf::Keyboard::Escape) {
				bButton_Held_ESC					= false;
			}
			if (event.key.code	== sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
				bButton_Holding_Up					= false;
				bButton_Released_Up					= true;
			}
			if (event.key.code	== sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
				bButton_Holding_Right				= false;
				bButton_Released_Right				= true;
			}
			if (event.key.code	== sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
				bButton_Holding_Left				= false;
				bButton_Released_Left				= true;
			}
			if (event.key.code	== sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
				bButton_Holding_Down				= false;
				bButton_Released_Down				= true;
			}
			if (event.key.code == sf::Keyboard::Tab) {
				bButton_Holding_Tab = false;
				bButton_Released_Tab = true;
			}
			if (event.key.code	== sf::Keyboard::C) {
				bButton_Held_C						= false;
			}
			if (event.key.code	== sf::Keyboard::LShift) {
				bButton_Held_Shift					= false;
			}
			break;
		}
		if (event.type == sf::Event::Resized) {
			if (window->getSize().x > float(window->getSize().y) * (float(game_Width) / float(game_Height))) {
				sf::FloatRect visibleArea(game_Width / 2 - (game_Width * (float(window->getSize().x) / float(window->getSize().y) * (float(game_Height) / float(game_Width)))) / 2, 0, game_Width * (float(window->getSize().x) / float(window->getSize().y) * (float(game_Height) / float(game_Width))), game_Height);
				window->setView(sf::View(visibleArea));
				screen_Min = window->getSize().y;
				screen_X_Offset = float(window->getSize().x - screen_Min) / 2;
				screen_Y_Offset = 0;
			}
			else {
				sf::FloatRect visibleArea(0, game_Height / 2 - (game_Height * (float(window->getSize().y) / float(window->getSize().x) * (float(game_Width) / float(game_Height)))) / 2, game_Width, game_Height * (float(window->getSize().y) / float(window->getSize().x) * (float(game_Width) / float(game_Height))));
				window->setView(sf::View(visibleArea));
				screen_Min = window->getSize().x;
				screen_X_Offset = 0;
				screen_Y_Offset = float(window->getSize().y - screen_Min) / 2;
			}
		}
	}
}

void Game::typing_Inputs(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::A) sTyping_Input += "a";
		if (event.key.code == sf::Keyboard::B) sTyping_Input += "b";
		if (event.key.code == sf::Keyboard::C) sTyping_Input += "c";
		if (event.key.code == sf::Keyboard::D) sTyping_Input += "d";
		if (event.key.code == sf::Keyboard::E) sTyping_Input += "e";
		if (event.key.code == sf::Keyboard::F) sTyping_Input += "f";
		if (event.key.code == sf::Keyboard::G) sTyping_Input += "g";
		if (event.key.code == sf::Keyboard::H) sTyping_Input += "h";
		if (event.key.code == sf::Keyboard::I) sTyping_Input += "i";
		if (event.key.code == sf::Keyboard::J) sTyping_Input += "j";
		if (event.key.code == sf::Keyboard::K) sTyping_Input += "k";
		if (event.key.code == sf::Keyboard::L) sTyping_Input += "l";
		if (event.key.code == sf::Keyboard::M) sTyping_Input += "m";
		if (event.key.code == sf::Keyboard::N) sTyping_Input += "n";
		if (event.key.code == sf::Keyboard::O) sTyping_Input += "o";
		if (event.key.code == sf::Keyboard::P) sTyping_Input += "p";
		if (event.key.code == sf::Keyboard::Q) sTyping_Input += "q";
		if (event.key.code == sf::Keyboard::R) sTyping_Input += "r";
		if (event.key.code == sf::Keyboard::S) sTyping_Input += "s";
		if (event.key.code == sf::Keyboard::T) sTyping_Input += "t";
		if (event.key.code == sf::Keyboard::U) sTyping_Input += "u";
		if (event.key.code == sf::Keyboard::V) sTyping_Input += "v";
		if (event.key.code == sf::Keyboard::W) sTyping_Input += "w";
		if (event.key.code == sf::Keyboard::X) sTyping_Input += "x";
		if (event.key.code == sf::Keyboard::Y) sTyping_Input += "y";
		if (event.key.code == sf::Keyboard::Z) sTyping_Input += "z";

		if (event.key.code == sf::Keyboard::Num0) sTyping_Input += "0";
		if (event.key.code == sf::Keyboard::Num1) sTyping_Input += "1";
		if (event.key.code == sf::Keyboard::Num2) sTyping_Input += "2";
		if (event.key.code == sf::Keyboard::Num3) sTyping_Input += "3";
		if (event.key.code == sf::Keyboard::Num4) sTyping_Input += "4";
		if (event.key.code == sf::Keyboard::Num5) sTyping_Input += "5";
		if (event.key.code == sf::Keyboard::Num6) sTyping_Input += "6";
		if (event.key.code == sf::Keyboard::Num7) sTyping_Input += "7";
		if (event.key.code == sf::Keyboard::Num8) sTyping_Input += "8";
		if (event.key.code == sf::Keyboard::Num9) sTyping_Input += "9";

		if (event.key.code == sf::Keyboard::Equal) sTyping_Input += "=";
		if (event.key.code == sf::Keyboard::Subtract) sTyping_Input += "-";
		if (event.key.code == sf::Keyboard::Add) sTyping_Input += "+";
		if (event.key.code == sf::Keyboard::Multiply) sTyping_Input += "*";
		if (event.key.code == sf::Keyboard::Divide) sTyping_Input += "/";
		if (event.key.code == sf::Keyboard::Period) sTyping_Input += ".";
		if (event.key.code == sf::Keyboard::Comma) sTyping_Input += ",";
		if (event.key.code == sf::Keyboard::Space) sTyping_Input += " ";

		if (event.key.code == sf::Keyboard::Backspace) bErase = true;
		if (event.key.code == sf::Keyboard::Delete) bDelete = true;
		if (event.key.code == sf::Keyboard::Return) bReturn = true;
	}
}

void Game::Update()
{
	bool editing_Triggers = false;
	for (int x = 0; x < 80; x++) {
		if (bgTrigger[iCurrent_Chapter][x].editing) {
			editing_Triggers = true;
			break;
		}
	}
	int iLast_Level = 0;

	iLast_Level = iCurrent_Chapter;

	pollEvents();

	int iLevel_Sound;
	iLevel_Sound = iCurrent_Chapter;
	if (bEditing) {
		if (!bUsed_Editor) {
			//PlaySound(TEXT("sounds/editor.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		bUsed_Editor = true;
	}
	else {
		if (iLevel_Sound == 0 && (iLast_Level_Sound != 0 || bUsed_Editor)) {
			//sndLevelMusic[0].play();
		}
		if (iLevel_Sound == 1 && (iLast_Level_Sound != 1 || bUsed_Editor)) {
			//sndLevelMusic[1].play();
		}
		if (iLevel_Sound == 2 && (iLast_Level_Sound != 2 || bUsed_Editor)) {
			//sndLevelMusic[2].play();
		}
		if (iLevel_Sound == 3 && (iLast_Level_Sound != 3 || bUsed_Editor)) {
			//sndLevelMusic[3].play();
		}
		if (iLevel_Sound == 4 && (iLast_Level_Sound != 4 || bUsed_Editor)) {
			//sndLevelMusic[4].play();
		}
		bUsed_Editor = false;
	}
	if (iLevel_Sound != iLast_Level_Sound && iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4) {
		Initialize_Decoration();
	}
	iLast_Level_Sound = iLevel_Sound;
	if (bEditing) {
		fPosition_X = fCamera_Position_X + game_Width / 2;
	}
	arrow_Velocity += arrow_Acceleration * (1.f / iFrames_per_Second);
	arrow_Distance += arrow_Velocity * (1.f / iFrames_per_Second);
	if (arrow_Distance < 0) {
		arrow_Velocity = arrow_Start_Velocity;
		arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
	}

	if (bButton_Holding_Up || bButton_Holding_Down || bButton_Holding_Left || bButton_Holding_Right) {
		bFlying = true;
	}
	else {
		bFlying = false;
	}
	if (bFlying) {
		bStarted = true;
	}

	bWas_Flying = bFlying;

	mouseX = float(sf::Mouse::getPosition(*window).x - float(screen_X_Offset)) * float(game_Width) / float(screen_Min);
	mouseY = float(sf::Mouse::getPosition(*window).y - float(screen_Y_Offset)) * float(game_Height) / float(screen_Min);

	bButton_Held_Mouse1 = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	bButton_Held_Mouse2 = sf::Mouse::isButtonPressed(sf::Mouse::Right);

	if (bButton_Pressed_Mouse1 && !bButton_Held_Mouse1) {
		bButton_Released_Mouse1 = true;
	}
	if (bButton_Pressed_Mouse2 && !bButton_Held_Mouse2) {
		bButton_Released_Mouse2 = true;
	}
	if (bButton_Pressed_Shift && !bButton_Held_Shift) {
		bButton_Released_Shift = true;
	}
	bButton_Clicked_Mouse1 = false;
	if (bButton_Held_Mouse1 && !bButton_Pressed_Mouse1) {
		bButton_Clicked_Mouse1 = true;
	}
	bButton_Clicked_Mouse2 = false;
	if (bButton_Held_Mouse2 && !bButton_Pressed_Mouse2) {
		bButton_Clicked_Mouse2 = true;
	}

	bButton_Pressed_Mouse1 = bButton_Held_Mouse1;
	bButton_Pressed_Mouse2 = bButton_Held_Mouse2;
	bButton_Pressed_Shift = bButton_Held_Shift;

	if (bButton_Held_ESC && !bButton_Pressed_ESC && !bStart_Menu) {
		bButton_Pressed_ESC = true;
		if (bPlaying) {
			bPlaying = false;
		}
		else {
			if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4 && !(bEditing_Character || bEditing_Blocks || bEditing_Background || bEditing_Particles || editing_Triggers)) {
				bPlaying = true;
			}
		}
	}
	else if (bButton_Held_ESC && !bButton_Pressed_ESC && bStart_Menu) {
		bButton_Pressed_ESC = true;
		bExit_Warning = !bExit_Warning;
	}
	else if (!bButton_Held_ESC) {
		bButton_Pressed_ESC = false;
	}

	if (bPlaying && bStarted && iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4 && !bEditing) {
		pgCharacter.Update();
		Update_Segment();
		if (fRespawning_Timer > fRespawning_Time) {
			Update_Editor_X();
			Update_Editor_Y();
		}
		Update_Camera_Position();

		if (bPlaying) {
			Update_Collision();
		}
	}
	if (bPlaying) {
		Update_Decoration();
		Update_Glow();
	}
	if (fRespawning_Timer < fRespawning_Time && bPlaying) {
		Update_Dead();
	}
	if (bEditing && bPlaying) {
		Update_Editor();
	}
	if (iCurrent_Chapter != iLast_Level && iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4) {
		Initialize_Decoration();
	}
	bButton_Clicked_Left		= false;
	bButton_Released_Left		= false;
	bButton_Clicked_Right		= false;
	bButton_Released_Right		= false;
	bButton_Clicked_Up			= false;
	bButton_Released_Up			= false;
	bButton_Clicked_Down		= false;
	bButton_Released_Down		= false;

	bButton_Clicked_Tab			= false;
	bButton_Released_Tab		= false;
}

void Game::Update_Segment() {
	if (fPosition_X > 80) {
		fPosition_X = 1;
		fCamera_Position_X = 0;
	}
	if (fPosition_X < 0) {
		fPosition_X = 79;
		fCamera_Position_X = 80 * fGrid_Cell_Size;
	}
}

void Game::Update_Editor_X() {
		if (																bButton_Clicked_Left																										) {
																			fPosition_X				+=	0 -			iControl_Settings[0][2][0][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][2][0][1]		[iCurrent_Chapter]		)	fVelocity_X				=	0 -			iControl_Settings[0][2][0][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][2][0][2]		[iCurrent_Chapter]		)	fAcceleration_X			=	0 -			iControl_Settings[0][2][0][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Clicked_Right																										) {
																			fPosition_X				+=				iControl_Settings[0][3][0][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][3][0][1]		[iCurrent_Chapter]		)	fVelocity_X				=				iControl_Settings[0][3][0][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][3][0][2]		[iCurrent_Chapter]		)	fAcceleration_X			=				iControl_Settings[0][3][0][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Holding_Left																										) {
																			fPosition_X				+=	0 - float(	iControl_Settings[0][2][1][0]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fVelocity_X				+=	0 - float(	iControl_Settings[0][2][1][1]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fAcceleration_X			+=	0 - float(	iControl_Settings[0][2][1][2]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
	}	if (																bButton_Holding_Right																										) {
																			fPosition_X				+=		float(	iControl_Settings[0][3][1][0]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fVelocity_X				+=		float(	iControl_Settings[0][3][1][1]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fAcceleration_X			+=		float(	iControl_Settings[0][3][1][2]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
	}	if (																bButton_Released_Left																										) {
																			fPosition_X				+=	0 -			iControl_Settings[0][2][2][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][2][2][1]		[iCurrent_Chapter]		)	fVelocity_X				=	0 -			iControl_Settings[0][2][2][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][2][2][2]		[iCurrent_Chapter]		)	fAcceleration_X			=	0 -			iControl_Settings[0][2][2][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Released_Right																										) {
																			fPosition_X				+=				iControl_Settings[0][3][2][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][3][2][1]		[iCurrent_Chapter]		)	fVelocity_X				=				iControl_Settings[0][3][2][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][3][2][2]		[iCurrent_Chapter]		)	fAcceleration_X			=				iControl_Settings[0][3][2][2]	[iCurrent_Chapter]								;
	}																		fVelocity_X				+=				fAcceleration_X											/	iFrames_per_Second	;
																			fVelocity_X				+=		float(	iPull_Velocity_X				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (																fVelocity_X				<	0																								) {
																			fVelocity_X				+=		float(	iDrag_Velocity_X				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (fVelocity_X	>	0											)	fVelocity_X				=	0																							;
	}	if (																fVelocity_X				>	0																								) {
																			fVelocity_X				+=	0 -	float(	iDrag_Velocity_X				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (fVelocity_X	<	0											)	fVelocity_X				=	0																							;
	}	if (fVelocity_X	>		iMax_Velocity_X		[iCurrent_Chapter]	)	fVelocity_X				=				iMax_Velocity_X					[iCurrent_Chapter]								;
		if (fVelocity_X	<	0 - iMax_Velocity_X		[iCurrent_Chapter]	)	fVelocity_X				=	0 -			iMax_Velocity_X					[iCurrent_Chapter]								;
																			fPosition_X				+=				fVelocity_X												/	iFrames_per_Second	;
}

void Game::Update_Editor_Y() {
		if (																bButton_Clicked_Up																											) {
																			fPosition_Y				+=	0 -			iControl_Settings[0][0][0][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][0][0][1]		[iCurrent_Chapter]		)	fVelocity_Y				=	0 -			iControl_Settings[0][0][0][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][0][0][2]		[iCurrent_Chapter]		)	fAcceleration_Y			=	0 -			iControl_Settings[0][0][0][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Clicked_Down																										) {
																			fPosition_Y				+=				iControl_Settings[0][1][0][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][1][0][1]		[iCurrent_Chapter]		)	fVelocity_Y				=				iControl_Settings[0][1][0][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][1][0][2]		[iCurrent_Chapter]		)	fAcceleration_Y			=				iControl_Settings[0][1][0][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Holding_Up																											) {
																			fPosition_Y				+=	0 -	float(	iControl_Settings[0][0][1][0]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fVelocity_Y				+=	0 -	float(	iControl_Settings[0][0][1][1]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fAcceleration_Y			+=	0 -	float(	iControl_Settings[0][0][1][2]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
	}	if (																bButton_Holding_Down																										) {
																			fPosition_Y				+=		float(	iControl_Settings[0][1][1][0]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fVelocity_Y				+=		float(	iControl_Settings[0][1][1][1]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
																			fAcceleration_Y			+=		float(	iControl_Settings[0][1][1][2]	[iCurrent_Chapter]	)	/	iFrames_per_Second	;
	}	if (																bButton_Released_Up																											) {
																			fPosition_Y				+=	0 -			iControl_Settings[0][0][2][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][0][2][1]		[iCurrent_Chapter]		)	fVelocity_Y				=	0 -			iControl_Settings[0][0][2][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][0][2][2]		[iCurrent_Chapter]		)	fAcceleration_Y			=	0 -			iControl_Settings[0][0][2][2]	[iCurrent_Chapter]								;
	}	if (																bButton_Released_Down																										) {
																			fPosition_Y				+=				iControl_Settings[0][1][2][0]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][1][2][1]		[iCurrent_Chapter]		)	fVelocity_Y				=				iControl_Settings[0][1][2][1]	[iCurrent_Chapter]								;
		if (iControl_Settings[1][1][2][2]		[iCurrent_Chapter]		)	fAcceleration_Y			=				iControl_Settings[0][1][2][2]	[iCurrent_Chapter]								;
	}																		fVelocity_Y				+=				fAcceleration_Y											/	iFrames_per_Second	;
																			fVelocity_Y				+=		float(	iPull_Velocity_Y				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (																fVelocity_Y				<	0																								) {
																			fVelocity_Y				+=		float(	iDrag_Velocity_Y				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (fVelocity_Y	>	0											)	fVelocity_Y				=	0																							;
	}	if (																fVelocity_Y				>	0																								) {
																			fVelocity_Y				+=	0 -	float(	iDrag_Velocity_Y				[iCurrent_Chapter]	)	/	iFrames_per_Second	;
		if (fVelocity_Y	<	0											)	fVelocity_Y				=	0																							;
	}	if (fVelocity_Y	>		iMax_Velocity_Y		[iCurrent_Chapter]	)	fVelocity_Y				=				iMax_Velocity_Y					[iCurrent_Chapter]								;
		if (fVelocity_Y	<	0 -	iMax_Velocity_Y		[iCurrent_Chapter]	)	fVelocity_Y				=	0 -			iMax_Velocity_Y					[iCurrent_Chapter]								;
																			fPosition_Y				+=				fVelocity_Y												/	iFrames_per_Second	;
}

void Game::Update_Camera_Position() {
	if (fPosition_X * fGrid_Cell_Size - fCamera_Position_X > 500 - fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X += float(fPosition_X * fGrid_Cell_Size - 500 + fCamera_Offset_from_Screen_Edge_X - fCamera_Position_X) * fCamera_Follow_Speed;
	}
	if (fPosition_X * fGrid_Cell_Size - fCamera_Position_X < fCamera_Offset_from_Screen_Edge_X) {
		fCamera_Position_X += float(fPosition_X * fGrid_Cell_Size - fCamera_Offset_from_Screen_Edge_X - fCamera_Position_X) * fCamera_Follow_Speed;
	}
	if (fCamera_Position_X < 0) {
		fCamera_Position_X = 0;
	}
	if (!bEditing) {
		if (fCamera_Position_X > 80 * fGrid_Cell_Size - 500) {
			fCamera_Position_X = 80 * fGrid_Cell_Size - 500;
		}
	}
}

void Game::Update_Collision() {
	for (int column = round(fPosition_X) - 1; column < round(fPosition_X) + 1; column++) {
		for (int row = round(fPosition_Y) - 1; row < round(fPosition_Y) + 1; row++) {
			if (map[iCurrent_Chapter][row][column] != 0) {
				if (fPosition_X * fGrid_Cell_Size - 3 * fGrid_Cell_Size / 16 < (column + 1) * fGrid_Cell_Size && fPosition_X * fGrid_Cell_Size + 3 * fGrid_Cell_Size / 16 > column * fGrid_Cell_Size && fPosition_Y * fGrid_Cell_Size - 3 * fGrid_Cell_Size / 16 < (row + 1) * fGrid_Cell_Size && fPosition_Y * fGrid_Cell_Size + 3 * fGrid_Cell_Size / 16 > row * fGrid_Cell_Size && fRespawning_Timer >= fRespawning_Time / 2) {
					if (fRespawning_Timer < fRespawning_Time) {
						if (fRespawning_Timer > fRespawning_Time / 2) {
							fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
						}
					}
					else {
						fRespawning_Timer = 0;
					}
				}
			}
		}
	}
}

void Game::Update_Glow() {
	fGlow_Timer += 1.f / iFrames_per_Second;
	if (fGlow_Timer > fGlow_Time) {
		fGlow_Timer -= fGlow_Time;
		for (int i = 0; i < iGlow_Total; i++) {
			arrGlow_Velocity_Increase_X[i] = rand() % 2 == 1;
			arrGlow_Velocity_Increase_Y[i] = rand() % 2 == 1;
		}
	}
	for (int i = 0; i < iGlow_Total; i++) {
		if (arrGlow_Offset_X[i] < -0.5 && arrGlow_Velocity_X[i] < 0) {
			arrGlow_Velocity_Increase_X[i] = true;
		}
		else if (arrGlow_Offset_X[i] > 0.5 && arrGlow_Velocity_X[i] > 0) {
			arrGlow_Velocity_Increase_X[i] = false;
		}
		if (arrGlow_Velocity_Increase_X[i]) {
			arrGlow_Velocity_X[i] += 1.f / iFrames_per_Second * fGlow_Velocity_Speed;
		}
		else {
			arrGlow_Velocity_X[i] -= 1.f / iFrames_per_Second * fGlow_Velocity_Speed;
		}
		if (arrGlow_Offset_Y[i] < -0.5 && arrGlow_Velocity_Y[i] < 0) {
			arrGlow_Velocity_Increase_Y[i] = true;
		}
		else if (arrGlow_Offset_Y[i] > 0.5 && arrGlow_Velocity_Y[i] > 0) {
			arrGlow_Velocity_Increase_Y[i] = false;
		}
		if (arrGlow_Velocity_Increase_Y[i]) {
			arrGlow_Velocity_Y[i] += 1.f / iFrames_per_Second * fGlow_Velocity_Speed;
		}
		else {
			arrGlow_Velocity_Y[i] -= 1.f / iFrames_per_Second * fGlow_Velocity_Speed;
		}
		arrGlow_Offset_X[i] += arrGlow_Velocity_X[i] * fGlow_Velocity_Speed;
		arrGlow_Offset_Y[i] += arrGlow_Velocity_Y[i] * fGlow_Velocity_Speed;
	}
}

void Game::Update_Editor() {
	//Update_Editor_Sidebar();
	if (bButton_Clicked_Tab) {
		bEditing_Blocks = !bEditing_Blocks;
	}
	if (!bEditing_Blocks) {
		if (bButton_Clicked_Num[1]) {
			iLevel_Editing_Layer = 1;
		}
		if (bButton_Clicked_Num[2]) {
			iLevel_Editing_Layer = 2;
		}

		if (iLevel_Editing_Layer == 1) {
			Update_Editor_Layer_1();
		}
		if (iLevel_Editing_Layer == 2) {
			Update_Editor_Layer_2();
		}
	}
}

void Game::Update_Editor_Layer_1() {
	if (mouseX > 0 && mouseX < 500 && mouseY > 0 && mouseY < 500 && window_Is_Active && !bEditor_Resizing) {
		if (bButton_Held_Mouse1 || bButton_Released_Mouse1) {
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					fillMap[y][x] = 0;
				}
			}
			if (bButton_Held_Shift && bButton_Released_Mouse1 && bEditor_Selecting_Area) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragX > dragX2) {
					float save_dragX = dragX;
					dragX = int(dragX2);
					dragX2 = ceil(save_dragX);
				}
				else {
					dragX = int(dragX);
					dragX2 = ceil(dragX2);
				}
				if (dragY > dragY2) {
					float save_dragY = dragY;
					dragY = int(dragY2);
					dragY2 = ceil(save_dragY);
				}
				else {
					dragY = int(dragY);
					dragY2 = ceil(dragY2);
				}
				for (int y = dragY; y < dragY2; y++) {
					for (int x = dragX; x < dragX2; x++) {
						map[iCurrent_Chapter][y][x] = iBlock;
					}
				}
			}
			if (!bButton_Held_Shift || bButton_Released_Mouse1) {
				bEditor_Selecting_Area = false;
			}
			if (bButton_Held_Shift && bButton_Clicked_Mouse1 && !bEditor_Selecting_Area) {
				dragX = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				dragY = mouseY / fGrid_Cell_Size;
				bEditor_Selecting_Area = true;
			}
			if (bButton_Held_Shift && bButton_Held_Mouse1 && bEditor_Selecting_Area) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragY < dragY2) {
					for (int y = int(dragY); y < ceil(dragY2); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
				else {
					for (int y = int(dragY2); y < ceil(dragY); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
			}
		}
		else {
			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 80; x++) {
					fillMap[y][x] = 0;
				}
			}
			if (bButton_Held_Shift && bButton_Released_Mouse2 && bEditor_Selecting_Area) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragX > dragX2) {
					float save_dragX = dragX;
					dragX = int(dragX2);
					dragX2 = ceil(save_dragX);
				}
				else {
					dragX = int(dragX);
					dragX2 = ceil(dragX2);
				}
				if (dragY > dragY2) {
					float save_dragY = dragY;
					dragY = int(dragY2);
					dragY2 = ceil(save_dragY);
				}
				else {
					dragY = int(dragY);
					dragY2 = ceil(dragY2);
				}
				for (int y = dragY; y < dragY2; y++) {
					for (int x = dragX; x < dragX2; x++) {
						map[iCurrent_Chapter][y][x] = 0;
					}
				}
			}
			if (!bButton_Held_Shift || bButton_Released_Mouse2) {
				bEditor_Selecting_Area = false;
			}
			if (bButton_Held_Shift && bButton_Clicked_Mouse2 && !bEditor_Selecting_Area) {
				dragX = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				dragY = mouseY / fGrid_Cell_Size;
				bEditor_Selecting_Area = true;
			}
			if (bButton_Held_Shift && bButton_Held_Mouse2 && bEditor_Selecting_Area) {
				float dragX2 = (mouseX + fCamera_Position_X) / fGrid_Cell_Size;
				float dragY2 = mouseY / fGrid_Cell_Size;
				if (dragY < dragY2) {
					for (int y = int(dragY); y < ceil(dragY2); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
				else {
					for (int y = int(dragY2); y < ceil(dragY); y++) {
						if (dragX < dragX2) {
							for (int x = int(dragX); x < ceil(dragX2); x++) {
								fillMap[y][x] = 1;
							}
						}
						else {
							for (int x = int(dragX2); x < ceil(dragX); x++) {
								fillMap[y][x] = 1;
							}
						}
					}
				}
			}
		}

		if (!bButton_Held_Shift || !bEditor_Selecting_Area) {
			if (bButton_Held_Mouse1) {
				map[iCurrent_Chapter][int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = iBlock;
			}
			if (bButton_Held_Mouse2) {
				map[iCurrent_Chapter][int(mouseY / fGrid_Cell_Size)][int((mouseX + fCamera_Position_X) / fGrid_Cell_Size)] = 0;
			}
		}
	}
}

void Game::Update_Editor_Layer_2() {
	if (bButton_Released_Mouse1) {
		bgTrigger[iCurrent_Chapter][int(float(mouseX + fCamera_Position_X) / 25)].editing = true;
		bButton_Released_Mouse1 = false;
		bPlaying = false;
	}
	if (bButton_Released_Mouse2) {
		bgTrigger[iCurrent_Chapter][int(float(mouseX + fCamera_Position_X) / 25)].active = false;
		bButton_Released_Mouse2 = false;
	}
}

//void Game::Update_Editor_Sidebar() {
//	if (mouseX > 500 - iEditor_Sidebar_Width - 4 && mouseX < 500 - iEditor_Sidebar_Width + 4 && mouseY > 0 && mouseY < 500) {
//		SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
//		if (bButton_Clicked_Mouse1) {
//			bEditor_Resizing = true;
//			bResizing_On_Edge = false;
//			if (fCamera_Position_X - iEditor_Sidebar_Width >= 60 * fGrid_Cell_Size) {
//				fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
//				bResizing_On_Edge = true;
//			}
//		}
//	}
//	if (bEditor_Resizing) {
//		if (bButton_Released_Mouse1) {
//			bEditor_Resizing = false;
//			return;
//		}
//		iEditor_Sidebar_Width = game_Width - mouseX;
//		if (iEditor_Sidebar_Width > iEditor_Sidebar_Max_Width) {
//			iEditor_Sidebar_Width = iEditor_Sidebar_Max_Width;
//		}
//		if (iEditor_Sidebar_Width < iEditor_Sidebar_Min_Width) {
//			iEditor_Sidebar_Width = iEditor_Sidebar_Min_Width;
//		}
//		if (fCamera_Position_X - iEditor_Sidebar_Width > 60 * fGrid_Cell_Size) {
//			fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
//		}
//		if (bResizing_On_Edge) {
//			fCamera_Position_X = iEditor_Sidebar_Width + 60 * fGrid_Cell_Size;
//		}
//	}
//}

void Game::Update_Dead() {
	if (fRespawning_Timer == 0) {
		fRespawning_Size = 0;
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				arrRespawning_Offset_X[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Offset_Y[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Acceleration[x][y] = rand() % (iRespawning_Max_Acceleration - iRespawning_Min_Acceleration) + iRespawning_Min_Acceleration;
				arrRespawning_Rotation[x][y] = rand() % 360;
				arrRespawning_Points[x][y] = rand() % (iRespawning_Max_Points - (iRespawning_Min_Points - 1)) + iRespawning_Min_Points;
				arrRespawning_Rotating_Left[x][y] = rand() % 2;
			}
		}
	}
	fRespawning_Timer += 1.f / iFrames_per_Second;
	if (fRespawning_Timer > fRespawning_Time / 2 && fRespawning_Last_Timer < fRespawning_Time / 2) {
		fRespawning_Size = 450;
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				arrRespawning_Offset_X[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Offset_Y[x][y] = rand() % (iRespawning_Max_Offset * 2) + iRespawning_Max_Offset;
				arrRespawning_Acceleration[x][y] = rand() % (iRespawning_Max_Acceleration - iRespawning_Min_Acceleration) + iRespawning_Min_Acceleration;
				arrRespawning_Rotation[x][y] = rand() % 360;
				arrRespawning_Points[x][y] = rand() % (iRespawning_Max_Points - (iRespawning_Min_Points - 1)) + iRespawning_Min_Points;
				arrRespawning_Rotating_Left[x][y] = rand() % 2;
			}
		}
		if (died_Next_Chapter != -1) {
			iCurrent_Chapter = died_Next_Chapter;
		}
		died_Next_Chapter = -1;
		died_Next_Level = -1;
	}
	if (fRespawning_Timer <= fRespawning_Time / 2) {
		fRespawning_Size = (fRespawning_Timer / (fRespawning_Time / 2)) * (fRespawning_Timer / (fRespawning_Time / 2)) * 450;
	}
	if (fRespawning_Timer > fRespawning_Time / 2) {
		fRespawning_Size = ((fRespawning_Time - fRespawning_Timer) / (fRespawning_Time / 2)) * ((fRespawning_Time - fRespawning_Timer) / (fRespawning_Time / 2)) * 450;
	}
	if (fRespawning_Timer >= fRespawning_Time) {
		fRespawning_Size = 0;
	}
	if (fRespawning_Timer > fRespawning_Time / 2 && fRespawning_Last_Timer < fRespawning_Time / 2) {
		Respawn();
		pgCharacter.Initialize();
	}
	else if (fRespawning_Timer < fRespawning_Time / 2) {
		bStarted = false;
		fVelocity_X = 0;
		fVelocity_Y = 0;
		fPosition_Y = -100;
	}
	fRespawning_Last_Timer = fRespawning_Timer;
}

void Game::Update_Decoration() {
	Update_Particles();
}

void Game::Update_Particles() {
	fDecoration_Randomness_Timer += 1.f / iFrames_per_Second;
	if (fDecoration_Randomness_Timer > fDecoration_Randomness_Time) {
		fDecoration_Randomness_Timer -= fDecoration_Randomness_Time;
		for (int i = 0; i < 64; i++) {
			bpBackground_Particle[i].fAcceleration_X = ((rand() % 2) * 2 - 1) * iParticle_Acceleration_X[iCurrent_Chapter];
			bpBackground_Particle[i].fAcceleration_Y = ((rand() % 2) * 2 - 1) * iParticle_Acceleration_Y[iCurrent_Chapter];
			bpBackground_Particle[i].fColor_Acceleration = (rand() % 2) * 2 - 1;
		}
	}
	for (int i = 0; i < 64; i++) {
		bpBackground_Particle[i].fColor_Velocity += bpBackground_Particle[i].fColor_Acceleration / iFrames_per_Second;
		bpBackground_Particle[i].fColor_Value += bpBackground_Particle[i].fColor_Velocity / iFrames_per_Second;
		if (bpBackground_Particle[i].fColor_Value < 0) {
			bpBackground_Particle[i].fColor_Value *= -1;
			bpBackground_Particle[i].fColor_Velocity = abs(bpBackground_Particle[i].fColor_Velocity) / 2;
		}
		if (bpBackground_Particle[i].fColor_Value > 1) {
			bpBackground_Particle[i].fColor_Value = 1 - (bpBackground_Particle[i].fColor_Value - 1);
			bpBackground_Particle[i].fColor_Velocity = 0 - abs(bpBackground_Particle[i].fColor_Velocity) / 2;
		}

		if (bParticle_Static_Velocity_X[iCurrent_Chapter]) {
			bpBackground_Particle[i].fVelocity_X = iParticle_Static_Velocity_X[iCurrent_Chapter] * 3;
			bpBackground_Particle[i].fPosition_X += bpBackground_Particle[i].fVelocity_X / iFrames_per_Second;
			if (bpBackground_Particle[i].fPosition_X < -50) {
				bpBackground_Particle[i].fPosition_X = 80 * fGrid_Cell_Size + 50;
			}
			if (bpBackground_Particle[i].fPosition_X > 80 * fGrid_Cell_Size + 50) {
				bpBackground_Particle[i].fPosition_X = -50;
			}
		}
		else {
			if (bpBackground_Particle[i].fOffset_X < 0 - iDecoration_Max_Offset && bpBackground_Particle[i].fVelocity_X < 0) {
				bpBackground_Particle[i].fAcceleration_X = iParticle_Acceleration_X[iCurrent_Chapter];
			}
			if (bpBackground_Particle[i].fOffset_X > iDecoration_Max_Offset && bpBackground_Particle[i].fVelocity_X > 0) {
				bpBackground_Particle[i].fAcceleration_X = 0 - iParticle_Acceleration_X[iCurrent_Chapter];
			}
			bpBackground_Particle[i].fOffset_X += bpBackground_Particle[i].fVelocity_X / iFrames_per_Second;
		}
		if (bParticle_Static_Velocity_Y[iCurrent_Chapter]) {
			bpBackground_Particle[i].fVelocity_Y = iParticle_Static_Velocity_Y[iCurrent_Chapter] * 3;
			bpBackground_Particle[i].fPosition_Y += bpBackground_Particle[i].fVelocity_Y / iFrames_per_Second;
			if (bpBackground_Particle[i].fPosition_Y < -50) {
				bpBackground_Particle[i].fPosition_Y = 20 * fGrid_Cell_Size + 50;
			}
			if (bpBackground_Particle[i].fPosition_Y > 20 * fGrid_Cell_Size + 50) {
				bpBackground_Particle[i].fPosition_Y = -50;
			}
		}
		else {
			if (bpBackground_Particle[i].fOffset_Y < 0 - iDecoration_Max_Offset && bpBackground_Particle[i].fVelocity_Y < 0) {
				bpBackground_Particle[i].fAcceleration_Y = iParticle_Acceleration_Y[iCurrent_Chapter];
			}
			if (bpBackground_Particle[i].fOffset_Y > iDecoration_Max_Offset && bpBackground_Particle[i].fVelocity_Y > 0) {
				bpBackground_Particle[i].fAcceleration_Y = 0 - iParticle_Acceleration_Y[iCurrent_Chapter];
			}
			bpBackground_Particle[i].fOffset_Y += bpBackground_Particle[i].fVelocity_Y / iFrames_per_Second;
		}
		bpBackground_Particle[i].fVelocity_X += (bpBackground_Particle[i].fAcceleration_X) / iFrames_per_Second;
		bpBackground_Particle[i].fVelocity_Y += (bpBackground_Particle[i].fAcceleration_Y) / iFrames_per_Second;
		bpBackground_Particle[i].fRotation = atan2(bpBackground_Particle[i].fVelocity_Y, bpBackground_Particle[i].fVelocity_X) * 180 / 3.141;
	}
}

void Game::Respawn() {
	Initialize_Glow();
	bStarted = false;
	fPosition_X = 10;
	fPosition_Y = 10;
	fVelocity_X = 0;
	fVelocity_Y = 0;
	fAcceleration_X = 0;
	fAcceleration_Y = 0;
	fCamera_Position_X = 0;
	fCamera_Position_Y = 0;
	fRotation = atan2(0, 1) * 180 / 3.141;
	fFlip_X = 1;
	fFlip_Y = 1;
 }

void Game::Draw()
{
	if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4) {

		int trigger1 = NULL;
		int trigger2 = NULL;

		sf::Color color1;
		sf::Color color2;

		float percent;

		sf::Color main;

		float center;

		center = bEditing ? iLevel_Editing_Layer == 2 ? float(mouseX + fCamera_Position_X - 25.f / 2) / 25 : fCamera_Position_X / 25 + 10 : fPosition_X - 0.5;

		for (int x = 0; x < 80; x++) {
			if (bgTrigger[iCurrent_Chapter][x].active) {
				if (center < x) {
					color2 = bgTrigger[iCurrent_Chapter][x].color;
					trigger2 = x + 1;
					break;
				}
				color1 = bgTrigger[iCurrent_Chapter][x].color;
				trigger1 = x + 1;
			}
		}

		if (trigger2 != NULL) {
			if (trigger1 != NULL) {
				percent = float((center + 1) - trigger1) / (trigger2 - trigger1);
				main = sf::Color(	color1.r * (1 - percent) + color2.r * percent,
									color1.g * (1 - percent) + color2.g * percent,
									color1.b * (1 - percent) + color2.b * percent);
			}
			else {
				main = color2;
			}
		}
		else {
			if (trigger1 != NULL) {
				main = color1;
			}
		}

		window->clear(iLevel_Editing_Layer == 2 && bEditing ? sf::Color(float(main.r) / 2, float(main.g) / 2, float(main.b) / 2) : main);

	}
	else {
		window->clear(sf::Color(0, 0, 0, 255));
	}

	if (iCurrent_Chapter >= 0 && iCurrent_Chapter <= 4) {
		Draw_Game();
	}

	//sf::RectangleShape hitBox;
	//hitBox.setPosition(Get_Position_with_Zoom("x", fPosition_X - 3 * fGrid_Cell_Size / 16 - fVelocity_X * 2), Get_Position_with_Zoom("y", fPosition_Y - 3 * fGrid_Cell_Size / 16 - fVelocity_Y * 2));
	//hitBox.setSize(sf::Vector2f(Get_Size_with_Zoom() * 6 * fGrid_Cell_Size / 16, Get_Size_with_Zoom() * 6 * fGrid_Cell_Size / 16));
	//hitBox.setFillColor(sf::Color(255, 0, 0, 255));
	//window->draw(hitBox);
	
	bool editing_Triggers = false;
	for (int x = 0; x < 80; x++) {
		if (bgTrigger[iCurrent_Chapter][x].editing) {
			window->clear(sf::Color(7, 7, 7));
			Draw_Background_Menu(x);
			editing_Triggers = true;
			break;
		}
	}

	if (bEditing_Character || bEditing_Blocks || bEditing_Particles || bEditing_Song) {
		window->clear(sf::Color(7, 7, 7));
				if (bEditing_Controls)			Draw_Controls_Menu();
		else	if (bEditing_Controls_Extra)	Draw_Controls_Extra_Menu();
		else	if (bEditing_Character)			Draw_Character_Menu();

				if (bEditing_Blocks)			Draw_Blocks_Menu();

				if (bEditing_Particles)			Draw_Particles_Menu();

				if (bEditing_Song)				Draw_Song_Menu();
	}
	else if (!bPlaying && !bStart_Menu && !editing_Triggers) {
		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f(500, 500));
		rect.setFillColor(sf::Color(0, 0, 0, 127));
		window->draw(rect);

		if (!bEditor) {
			window->clear(sf::Color(7, 7, 7));
			Draw_Menu();
		}
		else if (!bEditing_Character && !bEditing_Blocks && !bEditing_Background && !bEditing_Particles && !editing_Triggers) {
			Draw_Editor_Menu();
		}
	}
	else if (bStart_Menu) {
		sf::Sprite sprite;
		sprite.setPosition(0, 0);
		sprite.setScale(500.f / 48, 500.f / 48);
		sprite.setTexture(menu);
		window->draw(sprite);

		sf::RectangleShape rect;
		rect.setPosition(0, 0);
		rect.setSize(sf::Vector2f(500, 500));
		rect.setFillColor(sf::Color(0, 0, 0, 127));
		window->draw(rect);

		Draw_Start_Menu();
	}
	
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color(0, 0, 0, 255));
	rect.setPosition(-10000, 0);
	rect.setSize(sf::Vector2f(10000, game_Height));
	window->draw(rect);
	rect.setPosition(game_Width, 0);
	rect.setSize(sf::Vector2f(10000, game_Height));
	window->draw(rect);
	rect.setPosition(0, -10000);
	rect.setSize(sf::Vector2f(game_Width, 10000));
	window->draw(rect);
	rect.setPosition(0, game_Height);
	rect.setSize(sf::Vector2f(game_Width, 10000));
	window->draw(rect);

	window->display();

	bButton_Released_Mouse1 = false;
	bButton_Released_Mouse2 = false;
	iMouse_Wheel_Moved = 0;
	for (int num = 0; num < 10; num++) {
		bButton_Clicked_Num[num] = false;
	}
	bButton_Clicked_Backspace = false;

	bButton_Clicked_Esc = false;

	sTyping_Input = "";
	bErase = false;
	bDelete = false;
	bReturn = false;
}

void Game::Draw_Menu() {
	sf::RectangleShape rect;
	sf::String btnText;
	for (int i = 0; i < 5; i++) {
		rect.setSize(sf::Vector2f(175, 50));
		rect.setPosition(500.f / 12 * 11.5f - rect.getSize().x, 500.f / 10 * ((i * 2) + 0.5f));
		rect.setFillColor(sf::Color(15, 15, 15, 255));
		if (Mouse_is_Hovering_Rect(rect)) {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(5, 5, 5, 255));
			}
			if (bButton_Released_Mouse1) {
				bEditor = true;
				Goto_Chapter(i);
			}
		}
		Fancy_Rect(rect, 10);
		btnText = "save " + to_string(i + 1);
		Attatch_Text_to_Rect(rect, btnText, "StokkenesIT", sf::Color(255, 255, 255), 1, true);
	}

	rect.setPosition(25, 25);
	rect.setSize(sf::Vector2f(50, 50));
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			bStart_Menu = true;
			selected_Option = 1;
			arrow_Velocity = arrow_Start_Velocity;
			arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "<", "StokkenesIT", sf::Color(255, 255, 255, 255), -1, true);
}

void Game::Draw_Editor_Menu() {
	sf::RectangleShape rect;
	sf::String btnText;
	rect.setSize(sf::Vector2f(230, 40));
	bool btnVisible;
	for (int i = 0; i < 5; i++) {
		btnVisible = i < 2 || bEditing;
		rect.setPosition(475 - rect.getSize().x, 475 - rect.getSize().y - float(float(rect.getSize().y) * 1.5) * i);
		if (Mouse_is_Hovering_Rect(rect)) {
			if (bButton_Released_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, btnVisible * 255));
				switch (i) {
				case 0:
					bEditor = false;
					break;
				case 1:
					bEditing = !bEditing;
					if (bEditing) {
						fPosition_Y = -50;
						if (fRespawning_Timer < fRespawning_Time / 2) {
							fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
							fRespawning_Last_Timer = fRespawning_Timer;
						}
					}
					else {
						if (fRespawning_Timer < fRespawning_Time) {
							if (fRespawning_Timer > fRespawning_Time / 2) {
								fRespawning_Timer = fRespawning_Time - fRespawning_Timer;
							}
						}
						else {
							fRespawning_Timer = 0;
						}
					}
					bPlaying = true;
					break;
				case 2:
					if (btnVisible) {
						bEditing_Character = true;
					}
					break;
				case 3:
					if (btnVisible) {
						bEditing_Particles = true;
					}
					break;
				case 4:
					if (btnVisible) {
						bEditing_Song = true;
					}
					break;
				}
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				rect.setFillColor(sf::Color(127, 127, 127, btnVisible * 255));
			}
			else {
				rect.setFillColor(sf::Color(191, 191, 191, btnVisible * 255));
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, btnVisible * 255));
		}
		Fancy_Rect(rect, 10);
		switch (i) {
		case 0:
			btnText = "main menu";
			break;
		case 1:
			bEditing ? btnText = "play" : btnText = "edit";
			break;
		case 2:
			bEditing ? btnText = "character" : btnText = "";
			break;
		case 3:
			bEditing ? btnText = "particles" : btnText = "";
			break;
		case 4:
			bEditing ? btnText = "song" : btnText = "";
			break;
		}
		Attatch_Text_to_Rect(rect, btnText, "StokkenesIT", sf::Color(0, 0, 0), 1, true);
	}
}

void Game::Draw_Character_Menu() {
	if (bEditing_Character_Text) {
		Draw_Character_Text();
		return;
	}

	sf::Image img_Editor_Character;

	img_Editor_Character = character[iCurrent_Chapter].copyToImage();

	if (bDrawing_Character) {
		Draw_Paint_Menu(bDrawing_Character, img_Editor_Character);
		character[iCurrent_Chapter].loadFromImage(img_Editor_Character);
		return;
	}

	sf::RectangleShape rect;
	sf::Sprite sprt;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bEditing_Character = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setSize(sf::Vector2f(float(game_Width) / 4 + 10, float(game_Height) / 4 + 10));
	rect.setPosition(float(game_Width) / 8 * 5 - 5, float(game_Height) / 8 * 1 - 5);
	rect.setFillColor(sf::Color(15, 15, 15));
	Fancy_Rect(rect, 5);

	sf::Sprite sprtTransparency_Pattern;

	sprtTransparency_Pattern.setTexture(txtrTransparency_Pattern);
	sprtTransparency_Pattern.setScale(sf::Vector2f(float(game_Width) / 8 / 4 / 2, float(game_Height) / 8 / 4 / 2));

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			sprtTransparency_Pattern.setPosition(float(game_Width) / 8 * 5 + float(x) * game_Width / 8 / 4, float(game_Height) / 8 * 1 + float(y) * game_Width / 8 / 4);
			window->draw(sprtTransparency_Pattern);
		}
	}

	sf::Texture txtr;
	txtr.loadFromImage(img_Editor_Character);

	sprt.setTexture(txtr);
	sprt.setScale(float(game_Width) / 4 / 8, float(game_Height) / 4 / 8);
	sprt.setPosition(float(game_Width) / 8 * 5, float(game_Height) / 8 * 1);
	window->draw(sprt);

	rect.setSize(sf::Vector2f(float(game_Width) / 4, float(game_Height) / 4));
	rect.setPosition(float(game_Width) / 8 * 5, float(game_Height) / 8 * 1);
	rect.setFillColor(sf::Color(0, 0, 0, 31));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(0, 0, 0, 63));
		}
		window->draw(rect);
		if (bButton_Released_Mouse1) {
			bDrawing_Character = true;
		}
	}

	//rect.setSize(sf::Vector2f(125, 50));
	//rect.setPosition(25, game_Height - 25 - rect.getSize().y);
	//rect.setFillColor(sf::Color(15, 15, 15, 255));
	//if (Mouse_is_Hovering_Rect(rect)) {
	//	if (bButton_Released_Mouse1) {
	//		rect.setFillColor(sf::Color(15, 15, 15, 255));
	//		bEditing_Character_Text = true;
	//		return;
	//	}
	//	else if (bButton_Held_Mouse1) {
	//		rect.setFillColor(sf::Color(5, 5, 5, 255));
	//	}
	//	else {
	//		rect.setFillColor(sf::Color(11, 11, 11, 255));
	//	}
	//}
	//Fancy_Rect(rect, 10);
	//Attatch_Text_to_Rect(rect, "text", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	sf::String btnText;

	for (int i = 0; i < 5; i++) {
		rect.setSize(sf::Vector2f(175, 50));
		rect.setPosition(25, 25 + float(float(i) / 4) * float(game_Width - 50 - rect.getSize().y));
		rect.setFillColor(sf::Color(15, 15, 15, 255));
		if (Mouse_is_Hovering_Rect(rect)) {
			if (bButton_Released_Mouse1) {
				if (i != 4) {
					iControl_Editing = i;
					bEditing_Controls = true;
				}
				else {
					bEditing_Controls_Extra = true;
				}
			}
			else if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(5, 5, 5, 255));
			}
			else {
				rect.setFillColor(sf::Color(11, 11, 11, 255));
			}
		}
		switch (i) {
		case 0:
			btnText = "up";
			break;
		case 1:
			btnText = "down";
			break;
		case 2:
			btnText = "left";
			break;
		case 3:
			btnText = "right";
			break;
		case 4:
			btnText = "extra";
			break;
		}
		Fancy_Rect(rect, 10);
		Attatch_Text_to_Rect(rect, btnText, "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);
	}
}

void Game::Draw_Character_Text() {

	using namespace Character::Movement::Text;

	if (bButton_Clicked_Esc) {
		bEditing_Character_Text = false;
	}

	if (sTyping_Input.length() > 0) {
		Type(sTyping_Input);
	}

	if (bReturn) Type("\n");

	if (bErase) { if (Selection::Get::Highest() == Selection::Get::Lowest()) { if (Selection::Get::Lowest() > 0) Cut::Back(); } else Cut::Selection(); };
	if (bDelete) { if (Selection::Get::Highest() == Selection::Get::Lowest()) { if (Selection::Get::Highest() < Get::Text().length()) Cut::Front(); } else Cut::Selection(); };

	//Text(Get::Text(), "StokkenesIT", 25, 75, 25, sf::Color(255, 255, 255, 255), -1, 1);

	if (mouseX > 25 && mouseX < game_Width - 25 && mouseY > 25 && mouseY < game_Height - 25) {
		if (bButton_Held_Mouse1) {
			Selection::Set::B(Get::Index_From_Point(floor(float(mouseY) / 25) - 1, floor(float(mouseX) / 25) - 1));
			if (bButton_Clicked_Mouse1) {
				Selection::Set::A(Get::Index_From_Point(floor(float(mouseY) / 25) - 1, floor(float(mouseX) / 25) - 1));
			}
		}
	}

	int index = 0;

	sf::Font font;
	sf::Text text;
	font.loadFromFile("fonts/StokkenesIT.ttf");
	text.setCharacterSize(16);
	text.setScale(float(40) / 16, float(40) / 16);
	const_cast<sf::Texture&>(font.getTexture(text.getCharacterSize())).setSmooth(false);
	text.setFont(font);
	text.setOrigin(float(0 + 1) / 2 * float(text.getLocalBounds().width), 7);
	text.setFillColor(sf::Color(255, 255, 255, 255));

	for (int y = 1; y < 20 - 1; y++) {
		for (int x = 1; x < 20 - 1; x++) {
			if (index >= Get::Text().length()) {
				return;
			}
			if (Get::Text().at(index) == '\n') {
				break;
			}
			text.setString(string(1, Get::Text().at(index)));
			text.setPosition(x * 25, y * 25);
			window->draw(text);

			index++;

		}

		index++;

	}

	bool Selected = true;

	for (int y = 0; y < 18; y++) {
		for (int x = 0; x < 18; x++) {
			if (Selection::Get::Lowest() == Get::Index_From_Point(y, x)) {
				Selected = true;
			}
			if (Selected) {
				sf::RectangleShape rect;
				rect.setPosition((x + 1) * 25, (y + 1) * 25);
				rect.setSize(sf::Vector2f(25, 25));
				rect.setFillColor(sf::Color(255, 255, 255, 127));
				Fancy_Rect(rect, 10);
			}
			if (Selection::Get::Highest() == Get::Index_From_Point(y, x)) {
				Selected = false;
			}
		}
	}
}

void Game::Draw_Controls_Menu() {
	sf::RectangleShape rect;
	sf::Sprite sprt;
	sf::String btnText;
	sf::String dirText;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			bEditing_Controls = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	switch (iControl_Editing) {
	case 0:
		dirText = "up";
		break;
	case 1:
		dirText = "down";
		break;
	case 2:
		dirText = "left";
		break;
	case 3:
		dirText = "right";
		break;
	}

	Text("You are currently\nediting the properties\nof " + dirText + " control", "StokkenesIT", 25, 425, 32, sf::Color(23, 23, 23), -1, false);

	for (int condition = 0; condition < 3; condition++) {
		rect.setSize(sf::Vector2f(125, 282.5f));
		rect.setPosition(25 + float(float(condition) / 2) * (game_Width - 50 - rect.getSize().x), 25);
		rect.setFillColor(sf::Color(11, 11, 11));
		Fancy_Rect(rect, 10);
		switch (condition) {
		case 0:
			btnText = "clicked";
			break;
		case 1:
			btnText = "holding";
			break;
		case 2:
			btnText = "released";
			break;
		}
		Text(btnText, "StokkenesIT", rect.getPosition().x + (rect.getSize().x) / 2, 25, 32, sf::Color(255, 255, 255), 0, true);
		for (int action = 0; action < 3; action++) {
			bool	bCurrent_Bool;
			int		iCurrent_Int;

			iCurrent_Int	= iControl_Settings[0][iControl_Editing][condition][action][iCurrent_Chapter];
			bCurrent_Bool	= iControl_Settings[1][iControl_Editing][condition][action][iCurrent_Chapter];

			rect.setSize(sf::Vector2f(100, 70));
			rect.setPosition(37.5f + float(float(condition) / 2) * (game_Width - 75 - 100), 60 + 82.5f * action);
			rect.setFillColor(sf::Color(7, 7, 7));
			Fancy_Rect(rect, 5);
			switch (condition) {
			case 0:
				switch (action) {
				case 0:
					btnText = "change\nposition";
					break;
				case 1:
					btnText = "set\nvelocity";
					break;
				case 2:
					btnText = "set\nacceleration";
					break;
				}
				break;
			case 1:
				switch (action) {
				case 0:
					btnText = "change\nposition";
					break;
				case 1:
					btnText = "change\nvelocity";
					break;
				case 2:
					btnText = "change\nacceleration";
					break;
				}
				break;
			case 2:
				switch (action) {
				case 0:
					btnText = "change\nposition";
					break;
				case 1:
					btnText = "set\nvelocity";
					break;
				case 2:
					btnText = "set\nacceleration";
					break;
				}
				break;
			}
			Text(btnText, "StokkenesIT", rect.getPosition().x + 3, 66 + 82.5f * action, 17, sf::Color(223, 223, 223), -1, true);

			iCurrent_Int = Value_Holder(42.5f + float(float(condition) / 2) * (game_Width - 75 - 100), 90 + 82.5f * action, 55, 35, 0, 99, iCurrent_Int);

			if (condition != 1 && action != 0) {
				bCurrent_Bool = Toggle_Switch(102.5f + float(float(condition) / 2) * (game_Width - 75 - 100), 90 + 82.5f * action, bCurrent_Bool, true);
			}
			if (condition == 1) {
				Text("/s", "StokkenesIT", 102.5f + float(float(condition) / 2) * (game_Width - 75 - 100), 90 + 82.5f * action, 35, sf::Color(127, 127, 127), -1, false);
			}

			iControl_Settings[0][iControl_Editing][condition][action][iCurrent_Chapter] = iCurrent_Int;
			iControl_Settings[1][iControl_Editing][condition][action][iCurrent_Chapter] = bCurrent_Bool;
		}
	}
}

void Game::Draw_Controls_Extra_Menu() {
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bEditing_Controls_Extra = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	Text("You are currently\nediting extra\nproperties", "StokkenesIT", 25, 425, 32, sf::Color(23, 23, 23), -1, false);

	rect.setSize(sf::Vector2f(450, 337.5));
	rect.setPosition(25, 25);
	rect.setFillColor(sf::Color(11, 11, 11));
	Fancy_Rect(rect, 10);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			int		iCurrent_Int;
			bool	bCurrent_Bool;
			sf::String btnText;
			sf::String btnText_Direction;

			if (i == 0) { j == 0 ? iCurrent_Int		= iMax_Velocity_X	[iCurrent_Chapter]	: j == 1 ? iCurrent_Int		= iMax_Velocity_Y	[iCurrent_Chapter]	: j == 2 ? iCurrent_Int		= 0												: iCurrent_Int	= 0												;}
			if (i == 1) { j == 0 ? iCurrent_Int		= iPull_Velocity_X	[iCurrent_Chapter]	: j == 1 ? iCurrent_Int		= iPull_Velocity_Y	[iCurrent_Chapter]	: j == 2 ? iCurrent_Int		= iRotation_With_Velocity_X	[iCurrent_Chapter]	: iCurrent_Int	= iRotation_With_Velocity_Y	[iCurrent_Chapter]	;}
			if (i == 2) { j == 0 ? iCurrent_Int		= iDrag_Velocity_X	[iCurrent_Chapter]	: j == 1 ? iCurrent_Int		= iDrag_Velocity_Y	[iCurrent_Chapter]	: j == 2 ? iCurrent_Int		= 0												: iCurrent_Int	= 0												;}

			if (i == 0) { j == 0 ? bCurrent_Bool	= 0										: j == 1 ? bCurrent_Bool	= 0										: j == 2 ? bCurrent_Bool	= bFlip_X					[iCurrent_Chapter]	: bCurrent_Bool	= bFlip_Y					[iCurrent_Chapter]	;}
			if (i == 1) { j == 0 ? bCurrent_Bool	= 0										: j == 1 ? bCurrent_Bool	= 0										: j == 2 ? bCurrent_Bool	= bRotation_With_Velocity_X	[iCurrent_Chapter]	: bCurrent_Bool	= bRotation_With_Velocity_Y	[iCurrent_Chapter]	;}
			if (i == 2) { j == 0 ? bCurrent_Bool	= 0										: j == 1 ? bCurrent_Bool	= 0										: j == 2 ? bCurrent_Bool	= bRotation_With_Velocity	[iCurrent_Chapter]	: bCurrent_Bool	= 0												;}

			j % 2 == 0 ? btnText_Direction = "x" : btnText_Direction = "y";

			switch (i) {
			case 0:
				btnText = j < 2 ? "max\nvelocity " + btnText_Direction : "flip " + btnText_Direction;
				break;
			case 1:
				btnText = j < 2 ? "pull\nvelocity " + btnText_Direction : "rotate with\nvelocity " + btnText_Direction;
				break;
			case 2:
				btnText = j < 2 ? "drag\nvelocity " + btnText_Direction : j == 2 ? "point towards\nvelocity" : "";
				break;
			}

			rect.setSize(sf::Vector2f(100, 70));
			rect.setPosition(37.5f + float(float(i) / 2) * (game_Width - 75 - 100), 40 + 82.5f * j);
			rect.setFillColor(sf::Color(7, 7, 7));
			if (!(i == 2 && j == 3)) Fancy_Rect(rect, 10);

			Text(btnText, "StokkenesIT", rect.getPosition().x + 3, 46 + 82.5f * j, 17, sf::Color(223, 223, 223), -1, true);

			if (j < 2 || i == 1)				iCurrent_Int	= Value_Holder	(42.5f + float(float(i) / 2) * (game_Width - 75 - 100), 70 + 82.5f * j, i == 1 && j < 2 ? 75 : 55, 35, i != 1 ? 0 : -99, 99, iCurrent_Int);

			if (j > 1 && !(j == 3 && i == 2))	bCurrent_Bool	= Toggle_Switch (i == 1 ? rect.getPosition().x + rect.getSize().x - 36.25f : 42.5f + float(float(i) / 2) * (game_Width - 75 - 100), 70 + 82.5f * j, bCurrent_Bool, true);

			if (i == 2 && j == 2 && bCurrent_Bool == 1) { bRotation_With_Velocity_X[iCurrent_Chapter] = 0; bRotation_With_Velocity_Y[iCurrent_Chapter] = 0; }
			if (i == 1 && j > 1 && bCurrent_Bool == 1) bRotation_With_Velocity[iCurrent_Chapter] = 0;

			if (i == 0) { j == 0 ? iMax_Velocity_X	[iCurrent_Chapter] = iCurrent_Int	: j == 1 ? iMax_Velocity_Y	[iCurrent_Chapter] = iCurrent_Int	: j == 2 ? 0																: 0																;}
			if (i == 1) { j == 0 ? iPull_Velocity_X	[iCurrent_Chapter] = iCurrent_Int	: j == 1 ? iPull_Velocity_Y	[iCurrent_Chapter] = iCurrent_Int	: j == 2 ? iRotation_With_Velocity_X	[iCurrent_Chapter] = iCurrent_Int	: iRotation_With_Velocity_Y	[iCurrent_Chapter] = iCurrent_Int	;}
			if (i == 2) { j == 0 ? iDrag_Velocity_X	[iCurrent_Chapter] = iCurrent_Int	: j == 1 ? iDrag_Velocity_Y	[iCurrent_Chapter] = iCurrent_Int	: j == 2 ? 0																: 0																;}
			
			if (i == 0) { j == 0 ? 0													: j == 1 ? 0													: j == 2 ? bFlip_X						[iCurrent_Chapter] = bCurrent_Bool	: bFlip_Y					[iCurrent_Chapter] = bCurrent_Bool	;}
			if (i == 1) { j == 0 ? 0													: j == 1 ? 0													: j == 2 ? bRotation_With_Velocity_X	[iCurrent_Chapter] = bCurrent_Bool	: bRotation_With_Velocity_Y	[iCurrent_Chapter] = bCurrent_Bool	;}
			if (i == 2) { j == 0 ? 0													: j == 1 ? 0													: j == 2 ? bRotation_With_Velocity		[iCurrent_Chapter] = bCurrent_Bool	: 0																;}
		}
	}
}

void Game::Draw_Blocks_Menu() {
	for (int x = 0; x < 5; x++) {
		if (bDynamic_Block_Selected[x]) { Draw_Blocks_Dynamic_Menu(x); return; }
	}
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 5; x++) {
			if (bStatic_Block_Selected[x][y]) { Draw_Paint_Menu(bStatic_Block_Selected[x][y], imgStatic_Block[iCurrent_Chapter][x][y]); txtrStatic_Block[iCurrent_Chapter][x][y].loadFromImage(imgStatic_Block[iCurrent_Chapter][x][y]); return; }
		}
	}

	sf::RectangleShape rect;
	sf::Sprite sprt;
	sf::Sprite sprtBlock;

	rect.setPosition(2 * 25, 25);
	rect.setSize(sf::Vector2f(16 * 25, 15 * 25));
	rect.setFillColor(sf::Color(11, 11, 11));
	Fancy_Rect(rect, 10);

	rect.setSize(sf::Vector2f(2 * 25, 2 * 25));

	Text("static blocks", "StokkenesIT", 3 * 25, 1.5 * 25, 25, sf::Color(255, 255, 255), -1, true);
	Text("dynamic blocks", "StokkenesIT", 3 * 25, 11.5 * 25, 25, sf::Color(255, 255, 255), -1, true);

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 5; x++) {

			sprt.setTexture(txtrTransparency_Pattern);
			sprt.setScale(float(2) * 25 / 2 / 8, float(2) * 25 / 2 / 8);

			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					sprt.setPosition((x + 1) * 3 * 25 + float(2) * 25 / 8 * i, (y + 1) * 3 * 25 + float(2) * 25 / 8 * j);
					window->draw(sprt);
				}
			}

			rect.setPosition((x + 1) * 3 * 25, (y + 1) * 3 * 25);
			rect.setFillColor(sf::Color(0, 0, 0, 0));

			if (Mouse_is_Hovering_Rect(rect)) {
				rect.setFillColor(sf::Color(0, 0, 0, 63));
				if (bButton_Held_Mouse1 || bButton_Held_Mouse2) rect.setFillColor(sf::Color(0, 0, 0, 127));
				if (bButton_Released_Mouse1) { iBlock = x + y * 5 + 1; bEditing_Blocks = false; }
				if (bButton_Released_Mouse2) bStatic_Block_Selected[x][y] = true;
			}

			sprtBlock.setTexture(txtrStatic_Block[iCurrent_Chapter][x][y]);
			sprtBlock.setPosition((x + 1) * 3 * 25, (y + 1) * 3 * 25);
			sprtBlock.setScale(float(2) * 25 / 8, float(2) * 25 / 8);

			window->draw(sprtBlock);
			window->draw(rect);
		}
	}

	for (int x = 0; x < 5; x++) {
		sprt.setTexture(txtrTransparency_Pattern);
		sprt.setScale(float(2) * 25 / 2 / 8, float(2) * 25 / 2 / 8);

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				sprt.setPosition((x + 1) * 3 * 25 + float(2) * 25 / 8 * i, 13 * 25 + float(2) * 25 / 8 * j);
				window->draw(sprt);
			}
		}

		rect.setPosition((x + 1) * 3 * 25, 13 * 25);
		rect.setFillColor(sf::Color(0, 0, 0, 0));

		if (Mouse_is_Hovering_Rect(rect)) {
			rect.setFillColor(sf::Color(0, 0, 0, 63));
			if (bButton_Held_Mouse1 || bButton_Held_Mouse2) rect.setFillColor(sf::Color(0, 0, 0, 127));
			if (bButton_Released_Mouse1) { iBlock = 15 + x + 1; bEditing_Blocks = false; }
			if (bButton_Released_Mouse2) bDynamic_Block_Selected[x] = true;
		}

		sf::Sprite sprtBlock;

		sprtBlock.setTexture(txtrDynamic_Block[iCurrent_Chapter][x][0]);
		sprtBlock.setScale(float(2) * 25 / 8, float(2) * 25 / 8);
		sprtBlock.setOrigin(2, 2);

		for (int Block_X = 0; Block_X < 2; Block_X++) {
			for (int Block_Y = 0; Block_Y < 2; Block_Y++) {
				if (Block_X == 0 && Block_Y == 0) sprtBlock.setRotation(0);
				if (Block_X == 1 && Block_Y == 0) sprtBlock.setRotation(90);
				if (Block_X == 0 && Block_Y == 1) sprtBlock.setRotation(-90);
				if (Block_X == 1 && Block_Y == 1) sprtBlock.setRotation(180);
				sprtBlock.setPosition((x + 1) * 3 * 25 + Block_X * 25 + 2.f / 4 * 25, 13 * 25 + Block_Y * 25 + 2.f / 4 * 25);
				window->draw(sprtBlock);
			}
		}

		window->draw(rect);
	}
}

void Game::Draw_Paint_Menu(bool &The_Boolean, sf::Image &The_Image) {
	sf::RectangleShape rect;
	sf::Sprite sprt;
	int iCurrent_Int;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			The_Boolean = false;
			for (int i = 0; i < The_Image.getSize().x; i++) for (int j = 0; j < The_Image.getSize().y; j++) bPaint_Select_Tool[i][j] = false;
			bCurrent_Tool_Active[0] = true;
			bCurrent_Tool_Active[1] = false;
			bCurrent_Tool_Active[2] = false;
			bCurrent_Tool_Active[3] = false;

			return;
		}
		else if (bButton_Held_Mouse1) {
		rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
		rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setPosition(1 * 25, 1 * 25);
	rect.setSize(sf::Vector2f(4 * 25, 18 * 25));
	rect.setFillColor(sf::Color(13, 13, 13));
	Fancy_Rect(rect, 5);

	Text("tools", "StokkenesIT", 1 * 25 + 2 * 25, 1 * 25, 25, sf::Color(255, 255, 255), 0, true);

	sprt.setTexture(txtrTransparency_Pattern);
	sprt.setScale(sf::Vector2f(0.25f * 1 * 25, 0.25f * 1 * 25));

	for (int x = 0; x < 6; x++) {
		sprt.setPosition(1.5f * 25 + x * 0.5f * 25, 2 * 25);
		window->draw(sprt);
	}

	rect.setPosition(1.5f * 25, 2 * 25);
	rect.setSize(sf::Vector2f(3 * 25, 0.5f * 25));
	rect.setFillColor(colCurrent_Drawing_Color);
	window->draw(rect);

	for (int color = 0; color < 4; color++) {
		iCurrent_Int = color == 0 ? colCurrent_Drawing_Color.r : color == 1 ? colCurrent_Drawing_Color.g : color == 2 ? colCurrent_Drawing_Color.b : colCurrent_Drawing_Color.a;

		iCurrent_Int = Value_Holder(2 * 25, 3 * 25 + color * 2 * 25, 60, 35, 0, 255, iCurrent_Int);

		Text(color == 0 ? "r" : color == 1 ? "g" : color == 2 ? "b" : "a", "StokkenesIT", 1 * 25, 3 * 25 + color * 2 * 25, 40, sf::Color(223, 223, 223), -1, true);

		color == 0 ? colCurrent_Drawing_Color.r = iCurrent_Int : color == 1 ? colCurrent_Drawing_Color.g = iCurrent_Int : color == 2 ? colCurrent_Drawing_Color.b = iCurrent_Int : colCurrent_Drawing_Color.a = iCurrent_Int;
	}
	for (int tool = 0; tool < 4; tool++) {
		sf::String btnText;
		bool bCurrent_Bool;
		switch (tool) {
		case 0:
			btnText = "paint";
			break;
		case 1:
			btnText = "picker";
			break;
		case 2:
			btnText = "bucket";
			break;
		case 3:
			btnText = "select";
			break;
		}

		bCurrent_Bool = bCurrent_Tool_Active[tool];

		Text(btnText, "StokkenesIT", 1.5f * 25, 10.75f * 25 - 5 + tool * 2 * 25, 25, sf::Color(255, 255, 255), -1, true);

		bCurrent_Bool = Toggle_Switch(1.5f * 25, 11.75f * 25 + tool * 2 * 25, bCurrent_Bool, false);

		if (bCurrent_Bool) {
			for (int i = 0; i < 4; i++) {
				bCurrent_Tool_Active[i] = false;
			}
		}

		bCurrent_Tool_Active[tool] = bCurrent_Bool;
	}

	bPaint_Select_Tool_Used = false;

	for (int i = 0; i < The_Image.getSize().x; i++) {
		for (int j = 0; j < The_Image.getSize().y; j++) {
			if (bPaint_Select_Tool[i][j]) bPaint_Select_Tool_Used = true;
		}
	}

	rect.setPosition(9 * 25, 1 * 25);
	rect.setSize(sf::Vector2f(10 * 25, 10 * 25));
	rect.setFillColor(sf::Color(15, 15, 15));
	Fancy_Rect(rect, 25);

	sprt.setScale((200.f / The_Image.getSize().x) / 2, (200.f / The_Image.getSize().y) / 2);

	for (int x = 0; x < The_Image.getSize().x; x++) {
		for (int y = 0; y < The_Image.getSize().y; y++) {
			sprt.setPosition(10 * 25 + x * (200.f / The_Image.getSize().x), 2 * 25 + y * (200.f / The_Image.getSize().y));
			window->draw(sprt);
		}
	}

	if (bCurrent_Tool_Active[0]) {
		if (bButton_Held_Mouse1 || bButton_Held_Mouse2) {
			int pixelX = floor(float(mouseX - 10 * 25) / (200.f / The_Image.getSize().x));
			int pixelY = floor(float(mouseY - 2 * 25) / (200.f / The_Image.getSize().y));
			if (pixelX >= 0 && pixelY >= 0 && pixelX <= The_Image.getSize().x - 1 && pixelY <= The_Image.getSize().y - 1 && (!bPaint_Select_Tool_Used || bPaint_Select_Tool[pixelX][pixelY])) The_Image.setPixel(pixelX, pixelY, bButton_Held_Mouse1 ? colCurrent_Drawing_Color : sf::Color(0, 0, 0, 0));
		}
	}

	if (bCurrent_Tool_Active[1]) {
		if (bButton_Held_Mouse1) {
			int pixelX = floor(float(mouseX - 10 * 25) / (200.f / The_Image.getSize().x));
			int pixelY = floor(float(mouseY - 2 * 25) / (200.f / The_Image.getSize().y));
			if (pixelX >= 0 && pixelY >= 0 && pixelX <= The_Image.getSize().x - 1 && pixelY <= The_Image.getSize().y - 1 && (!bPaint_Select_Tool_Used || bPaint_Select_Tool[pixelX][pixelY])) colCurrent_Drawing_Color = The_Image.getPixel(pixelX, pixelY);
		}
	}

	if (bCurrent_Tool_Active[2]) {
		if (bButton_Clicked_Mouse1 || bButton_Clicked_Mouse2) {
			int pixelX = floor(float(mouseX - 10 * 25) / (200.f / The_Image.getSize().x));
			int pixelY = floor(float(mouseY - 2 * 25) / (200.f / The_Image.getSize().y));
			if (pixelX >= 0 && pixelY >= 0 && pixelX <= The_Image.getSize().x - 1 && pixelY <= The_Image.getSize().y - 1 && (!bPaint_Select_Tool_Used || bPaint_Select_Tool[pixelX][pixelY])) The_Image = Draw_Paint_Fill(pixelX, pixelY, bButton_Clicked_Mouse1 ? colCurrent_Drawing_Color : sf::Color(0, 0, 0, 0), The_Image.getPixel(pixelX, pixelY), The_Image);
		}
	}

	if (bCurrent_Tool_Active[3]) {
		if (bButton_Held_Mouse1 || bButton_Held_Mouse2) {
			int pixelX = floor(float(mouseX - 10 * 25) / (200.f / The_Image.getSize().x));
			int pixelY = floor(float(mouseY - 2 * 25) / (200.f / The_Image.getSize().y));
			if (pixelX >= 0 && pixelY >= 0 && pixelX <= The_Image.getSize().x && pixelY <= The_Image.getSize().y) bPaint_Select_Tool[pixelX][pixelY] = bButton_Held_Mouse1;
		}
	}

	sf::Texture txtr;
	txtr.loadFromImage(The_Image);

	sf::Sprite sprtBlock;
	sprtBlock.setTexture(txtr);

	sprtBlock.setPosition(10 * 25, 2 * 25);
	sprtBlock.setScale(8.f * 25 / The_Image.getSize().x, 8.f * 25 / The_Image.getSize().y);
	window->draw(sprtBlock);

	if (bPaint_Select_Tool_Used) {
		rect.setSize(sf::Vector2f(1 * 25, 1 * 25));
		rect.setFillColor(sf::Color(0, 0, 0, 127));
		for (int i = 0; i < The_Image.getSize().x; i++) {
			for (int j = 0; j < The_Image.getSize().y; j++) {
				if (!bPaint_Select_Tool[i][j]) {
					rect.setPosition(10 * 25 + i * 25, 2 * 25 + j * 25);
					window->draw(rect);
				}
			}
		}
	}
 }

sf::Image Game::Draw_Paint_Fill(int The_X, int The_Y, sf::Color The_Fill_Color, sf::Color The_Color_To_Fill, sf::Image The_Image) {
	if (bPaint_Select_Tool_Used) { 
		for (int x = 0; x < The_Image.getSize().x; x++) {
			for (int y = 0; y < The_Image.getSize().y; y++) {
				if (bPaint_Select_Tool[x][y]) The_Image.setPixel(x, y, The_Fill_Color); 
			} 
		} 
	}
	else {
		if (The_Fill_Color == The_Color_To_Fill) return The_Image;
		The_Image.setPixel(The_X, The_Y, The_Fill_Color);
		if (The_X > 0							&& The_Image.getPixel(The_X - 1, The_Y) == The_Color_To_Fill) The_Image = Draw_Paint_Fill(The_X - 1, The_Y, The_Fill_Color, The_Color_To_Fill, The_Image);
		if (The_Y > 0							&& The_Image.getPixel(The_X, The_Y - 1) == The_Color_To_Fill) The_Image = Draw_Paint_Fill(The_X, The_Y - 1, The_Fill_Color, The_Color_To_Fill, The_Image);
		if (The_X < The_Image.getSize().x - 1	&& The_Image.getPixel(The_X + 1, The_Y) == The_Color_To_Fill) The_Image = Draw_Paint_Fill(The_X + 1, The_Y, The_Fill_Color, The_Color_To_Fill, The_Image);
		if (The_Y < The_Image.getSize().y - 1	&& The_Image.getPixel(The_X, The_Y + 1) == The_Color_To_Fill) The_Image = Draw_Paint_Fill(The_X, The_Y + 1, The_Fill_Color, The_Color_To_Fill, The_Image);
	}
	return The_Image;
}

void Game::Draw_Blocks_Dynamic_Menu(int x) {
	for (int i = 0; i < 4; i++) {
		if (bDynamic_Block_Corner_Selected[i]) { Draw_Paint_Menu(bDynamic_Block_Corner_Selected[i], imgDynamic_Block[iCurrent_Chapter][x][i]); txtrDynamic_Block[iCurrent_Chapter][x][i].loadFromImage(imgDynamic_Block[iCurrent_Chapter][x][i]); return; }
	}
	sf::RectangleShape rect;
	sf::RectangleShape overlay;
	sf::RectangleShape example;
	sf::Sprite sprt;
	sf::Sprite transparency;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(float(game_Width) / 2 - float(rect.getSize().x) / 2, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bDynamic_Block_Selected[x] = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setSize(sf::Vector2f(game_Width - 50, game_Height - 125));
	rect.setPosition(25, 25);
	rect.setFillColor(sf::Color(11, 11, 11));
	Fancy_Rect(rect, 10);

	rect.setSize(sf::Vector2f(135, 135));
	rect.setFillColor(sf::Color(31, 31, 31));
	sprt.setScale(125.f / 4, 125.f / 4);
	transparency.setTexture(txtrTransparency_Pattern);
	transparency.setScale(125.f / 4 / 2, 125.f / 4 / 2);
	example.setSize(sf::Vector2f(60, 60));
	example.setFillColor(sf::Color(23, 23, 23));
	for (int sprtY = 0; sprtY < 2; sprtY++) {
		for (int sprtX = 0; sprtX < 2; sprtX++) {
			rect.setPosition(50 + sprtX * (game_Width - 50 - sprt.getScale().x * 4 - 50) - 5, 50 + sprtY * (game_Height - 125 - sprt.getScale().y * 4 - 50) - 5);
			Fancy_Rect(rect, 5);
			for (int trY = 0; trY < 4; trY++) {
				for (int trX = 0; trX < 4; trX++) {
					transparency.setPosition(50 + sprtX * (game_Width - 50 - sprt.getScale().x * 4 - 50) + sprt.getScale().x * trX, 50 + sprtY * (game_Height - 125 - sprt.getScale().y * 4 - 50) + sprt.getScale().y * trY);
					window->draw(transparency);
				}
			}
			sprt.setPosition(50 + sprtX * (game_Width - 50 - sprt.getScale().x * 4 - 50), 50 + sprtY * (game_Height - 125 - sprt.getScale().y * 4 - 50));
			sprt.setTexture(txtrDynamic_Block[iCurrent_Chapter][x][sprtX + sprtY * 2]);
			window->draw(sprt);
			if (Mouse_is_Hovering_Rect(rect)) {
				overlay.setPosition(50 + sprtX * (game_Width - 50 - sprt.getScale().x * 4 - 50), 50 + sprtY * (game_Height - 125 - sprt.getScale().y * 4 - 50));
				overlay.setSize(sf::Vector2f(125, 125));
				overlay.setFillColor(sf::Color(0, 0, 0, 63));
				if (bButton_Held_Mouse1) {
					overlay.setFillColor(sf::Color(0, 0, 0, 127));
				}
				if (bButton_Released_Mouse1) {
					bDynamic_Block_Corner_Selected[sprtX + sprtY * 2] = true;
					return;
				}
				window->draw(overlay);
			}
			example.setPosition(sprtX == 0 ? 185 : game_Width - example.getSize().x - 185, 112.5f + sprtY * (game_Height - 125 - sprt.getScale().y * 4 - 50) - float(example.getSize().y) / 2);
			window->draw(example);

			sf::RectangleShape imgExample;
			imgExample.setSize(sf::Vector2f(float(example.getSize().x - 10) / 4, float(example.getSize().y - 10) / 4));
			for (int pX = 0; pX < 4; pX++) {
				for (int pY = 0; pY < 4; pY++) {
					imgExample.setFillColor(sf::Color(0, 0, 0));
					if (sprtX == 0 && sprtY == 0) {
						if (pX == 0 || pY == 0) {
							imgExample.setFillColor(sf::Color(255, 255, 255));
						}
					}
					if (sprtX == 1 && sprtY == 0) {
						if (pY == 0) {
							imgExample.setFillColor(sf::Color(255, 255, 255));
						}
					}
					if (sprtX == 0 && sprtY == 1) {
						if (pX == 0 && pY == 0) {
							imgExample.setFillColor(sf::Color(255, 255, 255));
						}
					}
					imgExample.setPosition(example.getPosition().x + 5 + imgExample.getSize().x * pX, example.getPosition().y + 5 + imgExample.getSize().y * pY);
					window->draw(imgExample);
				}
			}
		}
	}
}

void Game::Draw_Background_Menu(int x) {
	sf::RectangleShape rect;
	sf::String btnText;
	int iCurrent_Int;
	bool bCurrent_Bool;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bgTrigger[iCurrent_Chapter][x].editing = false;
			bButton_Released_Mouse1 = false;
			bPlaying = true;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setPosition(25, 25);
	rect.setSize(sf::Vector2f(395, 225));
	rect.setFillColor(sf::Color(11, 11, 11));
	Fancy_Rect(rect, 10);

	for (int layer = 0; layer < 3; layer++) {
		switch (layer) {
		case 0:
			btnText = "background";
			break;
		case 1:
			btnText = "color 1";
			break;
		case 2:
			btnText = "color 2";
			break;
		}
		Text(btnText, "StokkenesIT", 30, 25 + 75 * layer, 32, sf::Color(255, 255, 255), -1, true);
		if (layer == 0) bgTrigger[iCurrent_Chapter][x].active = Toggle_Switch(340, 60 + 75 * layer, bgTrigger[iCurrent_Chapter][x].active, true);
		else Toggle_Switch(340, 60 + 75 * layer, false, true);
		for (int color = 0; color < 3; color++) {
			if (layer == 0) iCurrent_Int = color == 0 ? bgTrigger[iCurrent_Chapter][x].color.r : color == 1 ? bgTrigger[iCurrent_Chapter][x].color.g : bgTrigger[iCurrent_Chapter][x].color.b;
			else iCurrent_Int = 0;

			iCurrent_Int = Value_Holder(55 + 100 * color, 60 + 75 * layer, 75, 35, 0, 255, iCurrent_Int);

			Text(color == 0 ? "r" : color == 1 ? "g" : "b", "StokkenesIT", 40 + 100 * color, 60 + 75 * layer, 32, sf::Color(223, 223, 223), 0, true);

			rect.setPosition(380 - 2, 60 + 75 * layer - 2);
			rect.setSize(sf::Vector2f(39, 39));
			rect.setFillColor(sf::Color(7, 7, 7));
			Fancy_Rect(rect, 10);
			
			rect.setPosition(380, 60 + 75 * layer);
			rect.setSize(sf::Vector2f(35, 35));
			rect.setFillColor(layer == 0 ? bgTrigger[iCurrent_Chapter][x].color : sf::Color(0, 0, 0));
			Fancy_Rect(rect, 10);
			
			if (layer == 0) if (color == 0) bgTrigger[iCurrent_Chapter][x].color.r = iCurrent_Int;
			if (layer == 0) if (color == 1) bgTrigger[iCurrent_Chapter][x].color.g = iCurrent_Int;
			if (layer == 0) if (color == 2) bgTrigger[iCurrent_Chapter][x].color.b = iCurrent_Int;
		}
	}
}

void Game::Draw_Particles_Menu() {
	sf::RectangleShape rect;
	sf::String btnText;
	int iCurrent_Int;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(game_Width - 25 - rect.getSize().x, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bEditing_Particles = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setPosition(25, 25);
	rect.setSize(sf::Vector2f(450, 205));
	rect.setFillColor(sf::Color(11, 11, 11));
	Fancy_Rect(rect, 10);

	Text("particles", "StokkenesIT", 30, 25, 32, sf::Color(255, 255, 255), -1, true);

	for (int layer = 0; layer < 2; layer++) {
		switch (layer) {
		case 0:
			btnText = "color 1";
			break;
		case 1:
			btnText = "color 2";
			break;
		}
		Text(btnText, "StokkenesIT", 30, 60 + 75 * layer, 32, sf::Color(255, 255, 255), -1, true);
		for (int color = 0; color < 4; color++) {
			iCurrent_Int = arrDecoration_Color[iCurrent_Chapter][layer][color];

			iCurrent_Int = Value_Holder(55 + 85 * color, 95 + 75 * layer, 60, 35, 0, 255, iCurrent_Int);

			Text(color == 0 ? "r" : color == 1 ? "g" : color == 2 ? "b" : "a", "StokkenesIT", 40 + 85 * color, 95 + 75 * layer, 32, sf::Color(223, 223, 223), 0, true);

			arrDecoration_Color[iCurrent_Chapter][layer][color] = iCurrent_Int;
		}
	}

	rect.setPosition(415 - 2, 95 - 2);
	rect.setSize(sf::Vector2f(35 + 4, 110 + 4));
	rect.setFillColor(sf::Color(7, 7, 7));
	window->draw(rect);

	sf::Sprite sprt;
	sprt.setTexture(txtrTransparency_Pattern);
	sprt.setScale(35.f / 2 / 2, 110.f / 6 / 2);

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 6; y++) {
			sprt.setPosition(415 + float(x) * 35 / 2, 95 + float(y) * 110 / 6);
			window->draw(sprt);
		}
	}

	int steps = 110;
	float height = 110;
	rect.setPosition(415, 95);
	rect.setSize(sf::Vector2f(35, height / steps));
	for (int step = 0; step < steps; step++) {
		rect.setFillColor(sf::Color(float(arrDecoration_Color[iCurrent_Chapter][1][0]) * step / (steps - 1) + float(arrDecoration_Color[iCurrent_Chapter][0][0]) * (1 - float(step) / (steps - 1)), float(arrDecoration_Color[iCurrent_Chapter][1][1]) * step / (steps - 1) + float(arrDecoration_Color[iCurrent_Chapter][0][1]) * (1 - float(step) / (steps - 1)), float(arrDecoration_Color[iCurrent_Chapter][1][2]) * step / (steps - 1) + float(arrDecoration_Color[iCurrent_Chapter][0][2]) * (1 - float(step) / (steps - 1)), float(arrDecoration_Color[iCurrent_Chapter][1][3]) * step / (steps - 1) + float(arrDecoration_Color[iCurrent_Chapter][0][3]) * (1 - float(step) / (steps - 1))));
		window->draw(rect);
		rect.setPosition(rect.getPosition().x, rect.getPosition().y + rect.getSize().y);
	}

	rect.setPosition(25, 255);
	rect.setSize(sf::Vector2f(285, 215));
	rect.setFillColor(sf::Color(11, 11, 11));
	window->draw(rect);

	for (int dir = 0; dir < 2; dir++) {
		for (int type = 0; type < 2; type++) {
			bool	bCurrent_Bool;
			int		iCurrent_Int;

			type == 0 ? dir == 0 ? iCurrent_Int		= iParticle_Static_Velocity_X[iCurrent_Chapter] : iCurrent_Int	= iParticle_Static_Velocity_Y[iCurrent_Chapter] : dir == 0 ? iCurrent_Int	= iParticle_Acceleration_X[iCurrent_Chapter]	: iCurrent_Int	= iParticle_Acceleration_Y[iCurrent_Chapter];
			type == 0 ? dir == 0 ? bCurrent_Bool	= bParticle_Static_Velocity_X[iCurrent_Chapter] : bCurrent_Bool	= bParticle_Static_Velocity_Y[iCurrent_Chapter] : dir == 0 ? bCurrent_Bool	= 0												: bCurrent_Bool	= 0											;

			rect.setSize(sf::Vector2f(105, 70));
			rect.setPosition(50 + type * 130, 280 + 95 * dir);
			rect.setFillColor(sf::Color(7, 7, 7));
			window->draw(rect);
			switch (type) {
			case 0:
				switch (dir) {
				case 0:
					btnText = "static\nvelocity x";
					break;
				case 1:
					btnText = "static\nvelocity y";
					break;
				}
				break;
			case 1:
				switch (dir) {
				case 0:
					btnText = "acceleration\nx";
					break;
				case 1:
					btnText = "acceleration\ny";
					break;
				}
				break;
			}
			Text(btnText, "StokkenesIT", rect.getPosition().x + 3, 280 + 5 + 95 * dir, 17, sf::Color(223, 223, 223), -1, true);

			iCurrent_Int = Value_Holder(50 + 6.25 + type * 130, 280 + 28.75 + 95 * dir, 55, 35, type == 0 ? -99 : 0, 99, iCurrent_Int);

			if (type == 0) bCurrent_Bool = Toggle_Switch(55 + 70 - 7.25 + type * 125, 280 + 28.75 + 95 * dir, bCurrent_Bool, true);
			
			type == 0 ? dir == 0 ? iParticle_Static_Velocity_X[iCurrent_Chapter] = iCurrent_Int		: iParticle_Static_Velocity_Y[iCurrent_Chapter] = iCurrent_Int	: dir == 0 ? iParticle_Acceleration_X[iCurrent_Chapter] = iCurrent_Int	: iParticle_Acceleration_Y[iCurrent_Chapter] = iCurrent_Int	;
			type == 0 ? dir == 0 ? bParticle_Static_Velocity_X[iCurrent_Chapter] = bCurrent_Bool	: bParticle_Static_Velocity_Y[iCurrent_Chapter] = bCurrent_Bool	: dir == 0 ? 0															: 0															;
		}
	}
}

void Game::Draw_Song_Menu() {
	sf::RectangleShape rect;
	sf::String btnText;
	int iCurrent_Int;
	rect.setSize(sf::Vector2f(125, 50));
	rect.setPosition(float(game_Width) / 2 - float(rect.getSize().x) / 2, game_Height - 25 - rect.getSize().y);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bEditing_Song = false;
			bTyping_ID = false;
			bMusic_Playtest = false;
			return;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "back", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);


	rect.setSize(sf::Vector2f(250, 2 * 25));
	rect.setPosition(float(game_Width) / 2 - float(rect.getSize().x) / 2, 1 * 25);
	rect.setFillColor(sf::Color(15, 15, 15));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bTyping_ID = true;
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	else {
		if (bButton_Clicked_Mouse1) {
			bTyping_ID = false;
		}
	}
	if (bTyping_ID) {
		rect.setFillColor(sf::Color(11, 11, 11, 255));
	}
	Fancy_Rect(rect, 10);

	if (bTyping_ID) {
		for (int num = 0; num < 10; num++) {
			if (bButton_Clicked_Num[num]) {
				if (sTyping_ID.size() < 7) {
					sTyping_ID += to_string(num);
				}
			}
		}
		if (bButton_Clicked_Backspace) {
			if (!sTyping_ID.empty()) sTyping_ID.pop_back();
		}
	}

	Text(sTyping_ID, "StokkenesIT", float(game_Width) / 2, 1 * 25, 2 * 25, sf::Color(255, 255, 255), 0, true);

	rect.setSize(sf::Vector2f(250, 50));
	rect.setPosition(float(game_Width) / 2 - float(rect.getSize().x) / 2, 3 * 25);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			Download_Newgrounds_Song(sTyping_ID);
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);
	Attatch_Text_to_Rect(rect, "download", "StokkenesIT", sf::Color(255, 255, 255, 255), 0, true);

	rect.setSize(sf::Vector2f(250, 50));
	rect.setPosition(float(game_Width) / 2 - float(rect.getSize().x) / 2, 6 * 25);
	rect.setFillColor(sf::Color(15, 15, 15, 255));
	Fancy_Rect(rect, 10);

	Text("id : " + sMusic_Properties[iCurrent_Chapter][0], "StokkenesIT", rect.getPosition().x + 3, rect.getPosition().y + 25, 20, sf::Color(255, 255, 255, 255), -1, true);
	Text(sMusic_Properties[iCurrent_Chapter][1], "StokkenesIT", rect.getPosition().x + 3, rect.getPosition().y, 20, sf::Color(255, 255, 255, 255), -1, true);
	Text(sMusic_Properties[iCurrent_Chapter][2], "StokkenesIT", rect.getPosition().x + rect.getSize().x - 8, rect.getPosition().y + 28, 20, sf::Color(255, 255, 255, 255), 1, true);
	Text("by", "StokkenesIT", float(game_Width) / 2 + float(rect.getSize().x) / 2 - 8, rect.getPosition().y + 13, 15, sf::Color(255, 255, 255, 255), 1, true);

	rect.setSize(sf::Vector2f(50, 50));
	rect.setPosition(float(game_Width) / 2 + 150, 6 * 25);
	if (Mouse_is_Hovering_Rect(rect)) {
		if (bButton_Released_Mouse1) {
			rect.setFillColor(sf::Color(15, 15, 15, 255));
			bMusic_Playtest = !bMusic_Playtest;
			if (bMusic_Playtest) PlaySound(TEXT("sound/music/4/music.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		else if (bButton_Held_Mouse1) {
			rect.setFillColor(sf::Color(5, 5, 5, 255));
		}
		else {
			rect.setFillColor(sf::Color(11, 11, 11, 255));
		}
	}
	Fancy_Rect(rect, 10);

	if (!bMusic_Playtest) {
		sf::CircleShape triangle(20, 3);
		triangle.setPosition(float(game_Width) / 2 + 190, 6.2f * 25);
		triangle.setFillColor(sf::Color(255, 255, 255, 255));
		triangle.setRotation(90);
		window->draw(triangle);
	}
	else {
		rect.setPosition(rect.getPosition().x + 10, rect.getPosition().y + 10);
		rect.setSize(sf::Vector2f(10, 30));
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		Fancy_Rect(rect, 10);
		rect.setPosition(rect.getPosition().x + 20, rect.getPosition().y);
		Fancy_Rect(rect, 10);
	}
}

void Game::Draw_Start_Menu() {
	sf::RectangleShape rect;
	sf::String btnText;
	rect.setSize(sf::Vector2f(125, 50));
	for (int i = 0; i < 2; i++) {
		rect.setPosition(475 - rect.getSize().x, 475 - rect.getSize().y - float(float(rect.getSize().y) * 1.5) * i);
		if (Mouse_is_Hovering_Rect(rect)) {
			if (bButton_Released_Mouse1) {
				switch (i) {
				case 0:
					no_Selected = false;
					bExit_Warning = true;
					break;
				case 1:
					bStart_Menu = false;
					arrow_Velocity = arrow_Start_Velocity;
					arrow_Distance = arrow_Velocity * (1.f / iFrames_per_Second);
					right_Side = true;
					break;
				}
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			else {
				rect.setFillColor(sf::Color(191, 191, 191, 255));
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		Fancy_Rect(rect, 10);
		switch (i) {
		case 0:
			btnText = "exit";
			break;
		case 1:
			btnText = "play";
			break;
		}
		Attatch_Text_to_Rect(rect, btnText, "StokkenesIT", sf::Color(0, 0, 0), 1, true);
	}

	if (bExit_Warning) {
		sf::RectangleShape rect;
		rect.setPosition(75, 125);
		rect.setSize(sf::Vector2f(350, 250));
		rect.setFillColor(sf::Color(0, 0, 0, 225));
		window->draw(rect);
		Text("do you want\nto exit?", "StokkenesIT", 250, 150, 72, sf::Color(255, 255, 255, 255), 0, true);
		rect.setPosition(100, 300);
		rect.setSize(sf::Vector2f(137.5f, 50));
		if (mouseX > 100 && mouseX < 100 + 137.5f && mouseY > 300 && mouseY < 300 + 50) {
			rect.setFillColor(sf::Color(191, 191, 191, 255));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			if (bButton_Released_Mouse1) {
				Save();
				window->close();
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		Fancy_Rect(rect, 10);
		Attatch_Text_to_Rect(rect, "yes", "StokkenesIT", sf::Color(0, 0, 0, 255), 0, true);
		rect.setPosition(262.5f, 300);
		rect.setSize(sf::Vector2f(137.5f, 50));
		if (mouseX > 262.5f && mouseX < 262.5f + 137.5f && mouseY > 300 && mouseY < 300 + 50) {
			rect.setFillColor(sf::Color(191, 191, 191, 255));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(127, 127, 127, 255));
			}
			if (bButton_Released_Mouse1) {
				bExit_Warning = false;
			}
		}
		else {
			rect.setFillColor(sf::Color(255, 255, 255, 255));
		}
		Fancy_Rect(rect, 10);
		Attatch_Text_to_Rect(rect, "no", "StokkenesIT", sf::Color(0, 0, 0, 255), 0, true);
	}
}

void Game::Draw_Game() {
	//Draw_Glow();

	if (!bEditing) {
		Draw_Player();
		Draw_Gameplay();
	}
	else {
		if (iLevel_Editing_Layer == 1) {
			Draw_Gameplay();
		}
		if (iLevel_Editing_Layer == 2) {
			Draw_Editor_Triggers();
		}
	}

	//Draw_Particles();

	Draw_Respawn();

	if (bEditing) {
		Draw_Editor();
	}
}

void Game::Draw_Player() {
	sf::Sprite sprite;

	player.setTexture(character[iCurrent_Chapter]);
	sprite.setTexture(character[iCurrent_Chapter]);

	sprite.setColor(sf::Color(0, 0, 0, 127));

	if (bFlip_X[iCurrent_Chapter]) { if (fVelocity_X < 0) fFlip_X = -1; if (fVelocity_X > 0) fFlip_X = 1; }
	else { fFlip_X = 1; }
	if (bFlip_Y[iCurrent_Chapter]) { if (fVelocity_Y < 0) fFlip_Y = -1; if (fVelocity_Y > 0) fFlip_Y = 1; }
	else { fFlip_Y = 1; }

	fRotation = 0;

	if (bRotation_With_Velocity_X[iCurrent_Chapter]) fRotation += fFlip_Y * fVelocity_X * float(iRotation_With_Velocity_X[iCurrent_Chapter]) / 10;
	if (bRotation_With_Velocity_Y[iCurrent_Chapter]) fRotation += fFlip_X * fVelocity_Y * float(iRotation_With_Velocity_Y[iCurrent_Chapter]) / 10;
	if (bRotation_With_Velocity[iCurrent_Chapter]) fRotation = fFlip_X * fFlip_Y * atan2(fVelocity_Y * fFlip_Y, fVelocity_X * fFlip_X) * 180 / 3.141;

	sprite.setPosition(Get_Position_with_Zoom("x", fPosition_X * fGrid_Cell_Size + 24.f / 8), Get_Position_with_Zoom("y", fPosition_Y * fGrid_Cell_Size + 24.f / 8));
	sprite.setOrigin(fGrid_Cell_Size / 6, fGrid_Cell_Size / 6);
	sprite.setScale(sf::Vector2f(Get_Size_with_Zoom() * fGrid_Cell_Size / 8 * fFlip_X, Get_Size_with_Zoom() * fGrid_Cell_Size / 8 * fFlip_Y));
	sprite.setRotation(fRotation);

	player.setPosition(Get_Position_with_Zoom("x", fPosition_X * fGrid_Cell_Size), Get_Position_with_Zoom("y", fPosition_Y * fGrid_Cell_Size));
	player.setOrigin(fGrid_Cell_Size / 6, fGrid_Cell_Size / 6);
	player.setScale(sf::Vector2f(Get_Size_with_Zoom() * fGrid_Cell_Size / 8 * fFlip_X, Get_Size_with_Zoom() * fGrid_Cell_Size / 8 * fFlip_Y));
	player.setRotation(fRotation);

	pgCharacter.fPosition_X = Get_Position_with_Zoom("x", fPosition_X * fGrid_Cell_Size);
	pgCharacter.fPosition_Y = Get_Position_with_Zoom("y", fPosition_Y * fGrid_Cell_Size);
	pgCharacter.Draw(window);

	window->draw(sprite);
	window->draw(player);
}

void Game::Draw_Respawn() {
	if (iRespawning_Animation == 0) {
		sf::CircleShape respawning_Shape;
		respawning_Shape.setFillColor(sf::Color(255, 255, 255, 255));
		float size;
		float multi = 1.05;
		for (int i = 0; i < 2; i++) {
			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 5; y++) {
					size = multi * (fRespawning_Size * (float(arrRespawning_Acceleration[x][y]) / 100)) / 2;
					respawning_Shape.setPointCount(arrRespawning_Points[x][y]);
					respawning_Shape.setOrigin(size, size);
					if (arrRespawning_Rotating_Left[x][y]) {
						respawning_Shape.setRotation(-arrRespawning_Acceleration[x][y] / multi * size / 200 + arrRespawning_Rotation[x][y]);
					}
					else {
						respawning_Shape.setRotation(arrRespawning_Acceleration[x][y] / multi * size / 200 + arrRespawning_Rotation[x][y]);
					}
					respawning_Shape.setRadius(size);
					respawning_Shape.setPosition(500.f * (float(float(x) - 0.5) / 4) + arrRespawning_Offset_X[x][y], 500.f * (float(float(y) - 0.5) / 4) + arrRespawning_Offset_Y[x][y]);
					window->draw(respawning_Shape);
				}
			}
			multi = 1;
			respawning_Shape.setFillColor(sf::Color(0, 0, 0, 255));
		}
	}
	if (iRespawning_Animation == 1) {
		sf::RectangleShape respawning_Shape;
		respawning_Shape.setFillColor(sf::Color(0, 0, 0, 255));
		for (int x = 0; x < 5; x++) {
			float size = (fRespawning_Size * (float(arrRespawning_Acceleration[x][0] * 1.5 - 87.5f) / 100));
			for (int y = 0; y < 2; y++) {
				int top = 0;
				if (y == 1) { top = 1; size = (fRespawning_Size * (350.f / 100)) / 2 - size; }
				respawning_Shape.setSize(sf::Vector2f(500.f / 5, size));
				respawning_Shape.setPosition((500.f / 5) * x, y * 500 - size * top);
				window->draw(respawning_Shape);
			}
		}
	}
}

void Game::Draw_Glow() {
	sf::Sprite sprite;
	sprite.setTexture(glow);
	for (int i = 0; i < iGlow_Total; i++) {
		sprite.setPosition(Get_Position_with_Zoom("x", arrGlow_Position_X[i] + arrGlow_Offset_X[i] * arrGlow_Scale[i] * fGlow_Offset_based_on_Size_Multiplier), Get_Position_with_Zoom("y", arrGlow_Position_Y[i] + arrGlow_Offset_Y[i] * arrGlow_Scale[i] * fGlow_Offset_based_on_Size_Multiplier));
		sprite.setScale(sf::Vector2f(Get_Size_with_Zoom() * arrGlow_Scale[i] / 256, Get_Size_with_Zoom() * arrGlow_Scale[i] / 256));
		sprite.setColor(sf::Color(arrColor[iCurrent_Chapter][1][0] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][0] - arrColor[iCurrent_Chapter][1][0]), arrColor[iCurrent_Chapter][1][1] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][1] - arrColor[iCurrent_Chapter][1][1]), arrColor[iCurrent_Chapter][1][2] + arrGlow_Brightness[i] * (arrColor[iCurrent_Chapter][2][2] - arrColor[iCurrent_Chapter][1][2])));
		window->draw(sprite);
	}
}

void Game::Draw_Gameplay() {
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size) - 1; row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (row >= 0 && row <= 79) {
				if (map[iCurrent_Chapter][column][row] != 0) {
					if (map[iCurrent_Chapter][column][row] < 16) {
						Draw_Static_Block(map[iCurrent_Chapter][column][row], row, column);
					} else {
						Draw_Dynamic_Block(map[iCurrent_Chapter][column][row], row, column);
					}
				}
			}
		}
	}
	for (int column = 0; column < 20; column++) {
		for (int row = int(fCamera_Position_X / fGrid_Cell_Size); row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
			if (fillMap[column][row] == 1) {
				sf::RectangleShape rect;
				rect.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), Get_Position_with_Zoom("y", (column)*fGrid_Cell_Size));
				rect.setSize(sf::Vector2f(fGrid_Cell_Size, fGrid_Cell_Size));
				rect.setFillColor(sf::Color(0, 0, 0, 127));
				window->draw(rect);
			}
		}
	}
}

void Game::Draw_Static_Block(int Block, int Map_X, int Map_Y) {
	sf::Sprite sprite;
	sf::Sprite shadow;

	shadow.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);

	shadow.setPosition(Get_Position_with_Zoom("x", Map_X * fGrid_Cell_Size + fGrid_Cell_Size / 8), Get_Position_with_Zoom("y", Map_Y * fGrid_Cell_Size + fGrid_Cell_Size / 8));

	shadow.setColor(sf::Color(0, 0, 0, 127));


	sprite.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);

	sprite.setPosition(Get_Position_with_Zoom("x", Map_X * fGrid_Cell_Size), Get_Position_with_Zoom("y", Map_Y * fGrid_Cell_Size));

	shadow.setTexture(txtrStatic_Block[iCurrent_Chapter][(Block - 1) % 5][int(float(Block - 1) / 5)]);
	sprite.setTexture(txtrStatic_Block[iCurrent_Chapter][(Block - 1) % 5][int(float(Block - 1) / 5)]);

	window->draw(shadow);
	window->draw(sprite);
}

void Game::Draw_Dynamic_Block(int Block, int Map_X, int Map_Y) {
	sf::Sprite sprite;
	sf::Sprite shadow;

	int Corner;

	bool Surround_Map[3][3];

	bool Corner_Map[2][2][2][2];

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (Map_X + x - 1 < 0 || Map_X + x - 1 > 79 || Map_Y + y - 1 < 0 || Map_Y + y - 1 > 19) {
				Surround_Map[x][y] = true;
			}
			else {
				Surround_Map[x][y] = map[iCurrent_Chapter][Map_Y + y - 1][Map_X + x - 1] == Block;
			}
		}
	}

	for (int Corner_Y = 0; Corner_Y < 2; Corner_Y++) {
		for (int Corner_X = 0; Corner_X < 2; Corner_X++) {
			for (int Block_Y = 0; Block_Y < 2; Block_Y++) {
				for (int Block_X = 0; Block_X < 2; Block_X++) {
					Corner_Map[Corner_X][Corner_Y][Block_X][Block_Y] = Surround_Map[Corner_X == 0 ? 1 - Block_X : 1 + Block_X][Corner_Y == 0 ? 1 - Block_Y : 1 + Block_Y];
				}
			}
		}
	}

	sprite.setOrigin(2, 2);
	shadow.setOrigin(2, 2);

	for (int Corner_X = 0; Corner_X < 2; Corner_X++) {
		for (int Corner_Y = 0; Corner_Y < 2; Corner_Y++) {

			sprite.setPosition(Get_Position_with_Zoom("x", Map_X * fGrid_Cell_Size + fGrid_Cell_Size / 2 * Corner_X + 2.f * fGrid_Cell_Size / 8), Get_Position_with_Zoom("y", Map_Y * fGrid_Cell_Size + fGrid_Cell_Size / 2 * Corner_Y + 2.f * fGrid_Cell_Size / 8));
			shadow.setPosition(Get_Position_with_Zoom("x", Map_X * fGrid_Cell_Size + fGrid_Cell_Size / 2 * Corner_X + 2.f * fGrid_Cell_Size / 8 + fGrid_Cell_Size / 8), Get_Position_with_Zoom("y", Map_Y * fGrid_Cell_Size + fGrid_Cell_Size / 2 * Corner_Y + 2.f * fGrid_Cell_Size / 8 + fGrid_Cell_Size / 8));

			sprite.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);
			shadow.setScale(Get_Size_with_Zoom() * fGrid_Cell_Size / 8, Get_Size_with_Zoom() * fGrid_Cell_Size / 8);

			if (!Corner_Map[Corner_X][Corner_Y][1][0] && !Corner_Map[Corner_X][Corner_Y][0][1]) {
				Corner = 0;

				if (Corner_X == 0 && Corner_Y == 0) sprite.setRotation(0);
				if (Corner_X == 1 && Corner_Y == 0) sprite.setRotation(90);
				if (Corner_X == 0 && Corner_Y == 1) sprite.setRotation(-90);
				if (Corner_X == 1 && Corner_Y == 1) sprite.setRotation(180);
			}
			else if ((Corner_Map[Corner_X][Corner_Y][1][0] && !Corner_Map[Corner_X][Corner_Y][0][1]) || (!Corner_Map[Corner_X][Corner_Y][1][0] && Corner_Map[Corner_X][Corner_Y][0][1])) {
				Corner = 1;

				if (Corner_Map[Corner_X][Corner_Y][1][0]) {
					if (Corner_Y == 0) sprite.setRotation(0);
					if (Corner_Y == 1) sprite.setRotation(180);
				}
				else {
					if (Corner_X == 0) sprite.setRotation(-90);
					if (Corner_X == 1) sprite.setRotation(90);
				} 
			}
			else if (Corner_Map[Corner_X][Corner_Y][1][0] && Corner_Map[Corner_X][Corner_Y][0][1] && !Corner_Map[Corner_X][Corner_Y][1][1]) {
				Corner = 2;

				if (Corner_X == 0 && Corner_Y == 0) sprite.setRotation(0);
				if (Corner_X == 1 && Corner_Y == 0) sprite.setRotation(90);
				if (Corner_X == 0 && Corner_Y == 1) sprite.setRotation(-90);
				if (Corner_X == 1 && Corner_Y == 1) sprite.setRotation(180);
			}
			else {
				Corner = 3;

				if (Corner_X == 0 && Corner_Y == 0) sprite.setRotation(0);
			}

			sprite.setTexture(txtrDynamic_Block[iCurrent_Chapter][Block - 16][Corner]);

			shadow.setTexture(txtrDynamic_Block[iCurrent_Chapter][Block - 16][Corner]);


			shadow.setRotation(sprite.getRotation());

			shadow.setColor(sf::Color(0, 0, 0, 127));

			window->draw(shadow);
			window->draw(sprite);

		}
	}
}

void Game::Draw_Editor_Triggers() {
	for (int row = int(fCamera_Position_X / fGrid_Cell_Size) - 1; row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 1; row++) {
		if (bgTrigger[iCurrent_Chapter][row].active) {
			sf::RectangleShape rect;
			rect.setPosition(Get_Position_with_Zoom("x", row * fGrid_Cell_Size), 0);
			rect.setSize(sf::Vector2f(fGrid_Cell_Size, game_Height));
			rect.setFillColor(sf::Color(bgTrigger[iCurrent_Chapter][row].color));
			window->draw(rect);
		}
	}
}

void Game::Draw_Decoration() {
	Draw_Particles();
}

void Game::Draw_Particles() {
	sf::RectangleShape rect;
	float posX;
	float posY;
	for (int i = 0; i < 64; i++) {
		posX = bpBackground_Particle[i].fPosition_X + bpBackground_Particle[i].fOffset_X;
		posY = bpBackground_Particle[i].fPosition_Y + bpBackground_Particle[i].fOffset_Y;
		rect.setOrigin(bpBackground_Particle[i].fSize / 4, bpBackground_Particle[i].fSize / 4);
		rect.setRotation(bpBackground_Particle[i].fRotation);
		rect.setSize(sf::Vector2f(Get_Size_with_Zoom() * bpBackground_Particle[i].fSize / 2, Get_Size_with_Zoom() * bpBackground_Particle[i].fSize / 2));
		rect.setPosition(Get_Position_with_Zoom("x", posX + 2), Get_Position_with_Zoom("y", posY + 2));
		rect.setFillColor(sf::Color(0, 0, 0, ((arrDecoration_Color[iCurrent_Chapter][0][3] + 1) / 4 - 1) + bpBackground_Particle[i].fColor_Value * (((arrDecoration_Color[iCurrent_Chapter][1][3] + 1) / 4 - 1) - ((arrDecoration_Color[iCurrent_Chapter][0][3] + 1) / 4 - 1))));
		window->draw(rect);
		rect.setFillColor(sf::Color(arrDecoration_Color[iCurrent_Chapter][0][0] + bpBackground_Particle[i].fColor_Value * (arrDecoration_Color[iCurrent_Chapter][1][0] - arrDecoration_Color[iCurrent_Chapter][0][0]), arrDecoration_Color[iCurrent_Chapter][0][1] + bpBackground_Particle[i].fColor_Value * (arrDecoration_Color[iCurrent_Chapter][1][1] - arrDecoration_Color[iCurrent_Chapter][0][1]), arrDecoration_Color[iCurrent_Chapter][0][2] + bpBackground_Particle[i].fColor_Value * (arrDecoration_Color[iCurrent_Chapter][1][2] - arrDecoration_Color[iCurrent_Chapter][0][2]), arrDecoration_Color[iCurrent_Chapter][0][3] + bpBackground_Particle[i].fColor_Value * (arrDecoration_Color[iCurrent_Chapter][1][3] - arrDecoration_Color[iCurrent_Chapter][0][3])));
		rect.setPosition(Get_Position_with_Zoom("x", posX), Get_Position_with_Zoom("y", posY));
		window->draw(rect);
	}
}

void Game::Draw_Editor() {
	Draw_Editor_Grid();
	//Draw_Editor_Sidebar();
}

void Game::Draw_Editor_Grid() {
	sf::RectangleShape rect;
	if (bPlaying) {
		rect.setPosition(int((mouseX + round(Get_Modulus(fCamera_Position_X, fGrid_Cell_Size))) / fGrid_Cell_Size) * fGrid_Cell_Size - round(Get_Modulus(fCamera_Position_X, fGrid_Cell_Size)), iLevel_Editing_Layer == 2 ? 0 : int(mouseY / fGrid_Cell_Size) * fGrid_Cell_Size);
		rect.setSize(sf::Vector2f(fGrid_Cell_Size, iLevel_Editing_Layer == 2 ? game_Height : fGrid_Cell_Size));
		rect.setFillColor(sf::Color(0, 0, 0, 63));
		window->draw(rect);
	}
	float line_Width = 1.f / (float(window->getSize().x) / game_Width);
	if (iLevel_Editing_Layer == 1) {
		for (int column = 0; column < 20 + 1; column++) {
			rect.setPosition(0, column * fGrid_Cell_Size);
			rect.setSize(sf::Vector2f(game_Width, line_Width));
			rect.setFillColor(sf::Color(0, 0, 0, 255));
			window->draw(rect);
		}
	}
	for (int row = int(fCamera_Position_X / fGrid_Cell_Size) - 1; row < int((game_Width + fCamera_Position_X) / fGrid_Cell_Size) + 2; row++) {
		rect.setPosition(int((row - int(fCamera_Position_X / fGrid_Cell_Size) - 1) * fGrid_Cell_Size - round(Get_Modulus(fCamera_Position_X, fGrid_Cell_Size))), 0);
		rect.setSize(sf::Vector2f(line_Width, game_Height));
		rect.setFillColor(sf::Color(0, 0, 0, 255));
		window->draw(rect);
	}
}

//void Game::Draw_Editor_Sidebar() {
//	sf::RectangleShape rect;
//	rect.setFillColor(sf::Color(31, 31, 31, 255));
//	rect.setPosition(game_Width - iEditor_Sidebar_Width, 0);
//	rect.setSize(sf::Vector2f(iEditor_Sidebar_Width, game_Height));
//	window->draw(rect);
//	Draw_Editor_Items();
//}
//
//void Game::Draw_Editor_Items(){
//	sf::Sprite sprt;
//	sf::Sprite shadow;
//	sf::Texture txtr;
//	for (int i = 0; i < 5; i++) {
//		if (iBlock - 1 != i) {
//			shadow.setColor(sf::Color(0, 0, 0, 127));
//		}
//		else {
//			shadow.setColor(sf::Color(0, 0, 0, 0));
//		}
//		if (bPlaying && mouseX > 500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 && mouseX < 500 - float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 && mouseY > float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width && mouseY < float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * fEditor_Items_Width) {
//			shadow.setColor(sf::Color(0, 0, 0, 95));
//			if (bButton_Held_Mouse1) {
//				shadow.setColor(sf::Color(0, 0, 0, 127));
//			}
//			else if (bButton_Released_Mouse1) {
//				iBlock = i + 1;
//			}
//		}
//
//		txtr.loadFromImage(imgStatic_Block[iCurrent_Chapter][i][0]);
//		sprt.setTexture(txtr);
//		shadow.setTexture(txtr);
//
//		sprt.setScale((float(iEditor_Sidebar_Width)* fEditor_Items_Width) / 8, (float(iEditor_Sidebar_Width)* fEditor_Items_Width) / 8);
//		shadow.setScale((float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 8, (float(iEditor_Sidebar_Width) * fEditor_Items_Width) / 8);
//
//		sprt.setPosition(500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2, float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width);
//		shadow.setPosition(500 - iEditor_Sidebar_Width + float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2, float(iEditor_Sidebar_Width) * (1 - fEditor_Items_Width) / 2 + i * iEditor_Sidebar_Width);
//
//		window->draw(sprt);
//		window->draw(shadow);
//	}
//}

void Game::Text(string The_Text, string The_Font, float The_Position_X, float The_Position_Y, int The_Font_Size, sf::Color col, int The_Offset_X, bool Shadow) {
	sf::Font font;
	sf::Text text;
	font.loadFromFile("fonts/" + The_Font + ".ttf");
	text.setString(The_Text);
	text.setCharacterSize(16);
	text.setScale(float(The_Font_Size) / 16, float(The_Font_Size) / 16);
	const_cast<sf::Texture&>(font.getTexture(text.getCharacterSize())).setSmooth(false);
	text.setFont(font);
	text.setOrigin(float(The_Offset_X + 1) / 2 * float(text.getLocalBounds().width), float(text.getLocalBounds().height) / 2);
	if (Shadow) {
		text.setFillColor(sf::Color(0, 0, 0, int(float(col.a) / 2)));
		text.setPosition(The_Position_X + 3, The_Position_Y + 3);
		window->draw(text);
	}
	text.setFillColor(col);
	text.setPosition(The_Position_X, The_Position_Y);
	window->draw(text);
}

void Game::Attatch_Text_to_Rect(sf::RectangleShape rect, string The_Text, string The_Font, sf::Color col, int The_Offset_X, bool Shadow) {
	sf::Font font;
	sf::Text text;
	float posX;
	float posY;
	font.loadFromFile("fonts/" + The_Font + ".ttf");
	text.setString(The_Text);
	text.setCharacterSize(16);
	const_cast<sf::Texture&>(font.getTexture(text.getCharacterSize())).setSmooth(false);
	text.setFont(font);
	//text.setScale(int(float(rect.getSize().y) * 1.5 / 16), int(float(rect.getSize().y) * 1.5 / 16));
	text.setScale(float(rect.getSize().y) * 1.25 / 16, float(rect.getSize().y) * 1.25 / 16);
	text.setOrigin(float(The_Offset_X + 1) / 2 * float(text.getLocalBounds().width), float(text.getLocalBounds().height) / 2);
	posX = rect.getPosition().x + float(rect.getSize().x) / 2 * 0.9625 + The_Offset_X * float(rect.getSize().x) / 2 - The_Offset_X * float(rect.getSize().y) * 0.15;
	posY = rect.getPosition().y - float(text.getLocalBounds().height) * 0.85;
	if (Shadow) {
		text.setFillColor(sf::Color(0, 0, 0, int(float(col.a) / 2)));
		text.setPosition(posX + 3, posY + 3);
		window->draw(text);
	}
	text.setFillColor(col);
	text.setPosition(posX, posY);
	window->draw(text);
}

int Game::Value_Holder(float The_Position_X, float The_Position_Y, int The_Size_X, int The_Size_Y, int The_Minimum, int The_Maximum, int The_Int) {
	sf::RectangleShape rect;

	rect.setPosition(The_Position_X, The_Position_Y + 5);
	rect.setSize(sf::Vector2f(The_Size_X, The_Size_Y - 5));
	rect.setFillColor(sf::Color(15, 15, 15));
	window->draw(rect);
	if (Mouse_is_Hovering_Rect(rect)) {
		The_Int += iMouse_Wheel_Moved;
		if (The_Int < The_Minimum) {
			The_Int = The_Minimum;
		}
		if (The_Int > The_Maximum) {
			The_Int = The_Maximum;
		}
	}

	Attatch_Text_to_Rect(rect, to_string(The_Int), "StokkenesIT", sf::Color(191, 191, 191), -1, false);

	rect.setPosition(The_Position_X, The_Position_Y);
	rect.setSize(sf::Vector2f(The_Size_X, 5));
	rect.setFillColor(sf::Color(31, 31, 31));
	if (Mouse_is_Hovering_Rect(rect)) {
		rect.setFillColor(sf::Color(39, 39, 39));
		if (bButton_Clicked_Mouse1) {
			The_Int = ((mouseX - The_Position_X) / float(The_Size_X)) * (The_Maximum - The_Minimum) + The_Minimum;
		}
	}
	window->draw(rect);

	rect.setSize(sf::Vector2f(9, 9));
	rect.setPosition(The_Position_X + (float(The_Size_X) * (float(The_Int - The_Minimum) / (The_Maximum - The_Minimum))) - rect.getSize().x / 2, The_Position_Y - 2);
	rect.setFillColor(sf::Color(23, 23, 23));
	if (Mouse_is_Hovering_Rect(rect)) {
		rect.setFillColor(sf::Color(27, 27, 27));
		if (bButton_Held_Mouse1) {
			The_Int = ((mouseX - The_Position_X) / float(The_Size_X)) * (The_Maximum - The_Minimum) + The_Minimum;
			if (The_Int < The_Minimum) {
				The_Int = The_Minimum;
			}
			if (The_Int > The_Maximum) {
				The_Int = The_Maximum;
			}
		}
	}
	Fancy_Rect(rect, 2);

	return The_Int;
}

bool Game::Toggle_Switch(float The_Position_X, float The_Position_Y, bool The_Boolean, bool Is_Vertical) {
	sf::RectangleShape rect;
	if (Is_Vertical) {
		rect.setPosition(The_Position_X, The_Position_Y);
		rect.setSize(sf::Vector2f(30, 35));
		rect.setFillColor(sf::Color(11, 11, 11));
		if (Mouse_is_Hovering_Rect(rect)) {
			rect.setFillColor(sf::Color(15, 15, 15));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(9, 9, 9));
			}
			if (bButton_Released_Mouse1) {
				The_Boolean = !The_Boolean;
			}
		}
		Fancy_Rect(rect, 4);
		rect.setSize(sf::Vector2f(rect.getSize().x, float(rect.getSize().y) / 2));
		rect.setPosition(rect.getPosition().x, rect.getPosition().y + rect.getSize().y * The_Boolean);
		rect.setFillColor(sf::Color(rect.getFillColor().r + 8, rect.getFillColor().g + 8, rect.getFillColor().b + 8));
		Fancy_Rect(rect, 2);
		Text(The_Boolean ? "on" : "off", "StokkenesIT", rect.getPosition().x + float(rect.getSize().x) / 2 - 1, rect.getPosition().y + rect.getSize().y * !The_Boolean * 2 - rect.getSize().y + 1, 16, sf::Color(191, 191, 191), 0, false);
	}
	else {
		rect.setPosition(The_Position_X, The_Position_Y);
		rect.setSize(sf::Vector2f(60, 35.f / 2));
		rect.setFillColor(sf::Color(11, 11, 11));
		if (Mouse_is_Hovering_Rect(rect)) {
			rect.setFillColor(sf::Color(15, 15, 15));
			if (bButton_Held_Mouse1) {
				rect.setFillColor(sf::Color(9, 9, 9));
			}
			if (bButton_Released_Mouse1) {
				The_Boolean = !The_Boolean;
			}
		}
		Fancy_Rect(rect, 4);
		rect.setSize(sf::Vector2f(float(rect.getSize().x) / 2, rect.getSize().y));
		rect.setPosition(rect.getPosition().x + rect.getSize().x * The_Boolean, rect.getPosition().y);
		rect.setFillColor(sf::Color(rect.getFillColor().r + 8, rect.getFillColor().g + 8, rect.getFillColor().b + 8));
		Fancy_Rect(rect, 2);
		Text(The_Boolean ? "on" : "off", "StokkenesIT", rect.getPosition().x + rect.getSize().x * !The_Boolean * 2 - float(rect.getSize().x) / 2 - 1, rect.getPosition().y + 1, 16, sf::Color(191, 191, 191), 0, false);
	}
	return The_Boolean;
}

void Game::Fancy_Rect(sf::RectangleShape rect, int Round_Edge) {
	sf::RectangleShape TEMP = rect;

	TEMP.setSize(sf::Vector2f(rect.getSize().x, rect.getSize().y - Round_Edge * 2));
	TEMP.setPosition(rect.getPosition().x, rect.getPosition().y + Round_Edge);
	window->draw(TEMP);

	TEMP.setSize(sf::Vector2f(rect.getSize().x - Round_Edge * 2, rect.getSize().y));
	TEMP.setPosition(rect.getPosition().x + Round_Edge, rect.getPosition().y);
	window->draw(TEMP);

	sf::CircleShape Corner;
	Corner.setFillColor(rect.getFillColor());
	Corner.setRadius(Round_Edge);

	Corner.setPosition(rect.getPosition());
	window->draw(Corner);
	Corner.setPosition(rect.getPosition().x + rect.getSize().x - Corner.getRadius() * 2, rect.getPosition().y + rect.getSize().y - Corner.getRadius() * 2);
	window->draw(Corner);
	Corner.setPosition(rect.getPosition().x + rect.getSize().x - Corner.getRadius() * 2, rect.getPosition().y);
	window->draw(Corner);
	Corner.setPosition(rect.getPosition().x, rect.getPosition().y + rect.getSize().y - Corner.getRadius() * 2);
	window->draw(Corner);
}

bool Game::Mouse_is_Hovering_Rect(sf::RectangleShape rect) {
	return (mouseX > rect.getPosition().x && mouseY > rect.getPosition().y && mouseX < rect.getPosition().x + rect.getSize().x && mouseY < rect.getPosition().y + rect.getSize().y);
}

void Game::Download_Newgrounds_Song(string The_ID) {
	wstring wsID(The_ID.begin(), The_ID.end());
	
	wstring wsDownload_URL = L"https://www.newgrounds.com/audio/download/" + wsID;
	wstring wsSave_Path = L"sound/music/" + to_wstring(iCurrent_Chapter) + L"/music.wav";

	URLDownloadToFile(NULL, wsDownload_URL.c_str(), wsSave_Path.c_str(), 0, NULL);

	string sHTML = Download_Webpage_to_String("https://www.newgrounds.com/audio/listen/" + The_ID);

	string sSong = sHTML;
	string sArtist = sHTML;

	sSong.erase(0, 49);
	sSong.erase(sSong.find("<"));

	sArtist.erase(0, sArtist.find("\"artist\":\"") + 10);
	sArtist.erase(sArtist.find("\""));

	ofstream createFile("sound/music/" + to_string(iCurrent_Chapter) + "/music_properties.txt");

	sMusic_Properties[iCurrent_Chapter][0] = The_ID;
	sMusic_Properties[iCurrent_Chapter][1] = sSong;
	sMusic_Properties[iCurrent_Chapter][2] = sArtist;

	createFile << The_ID	<< "\n";
	createFile << sSong		<< "\n";
	createFile << sArtist	<< "\n";
}

string Game::Download_Webpage_to_String(string The_Webpage) {
	string sCMD_Line = "curl -sl " + The_Webpage;
	return Get_CMD_Response(sCMD_Line.c_str());
}

void Game::Goto_Chapter(int The_Chapter) {
	iCurrent_Chapter = The_Chapter;
	bPlaying = true;
}

void Game::Save() {
	for (int i = 0; i < 5; i++) {
		ofstream createFile("saves/levels/" + to_string(i) + ".txt");
		Save_Editor(createFile, i);
		Save_Character_Properties(createFile, i);
		Save_Block_Properties(createFile, i);
		Save_Particle_Properties(createFile, i);
		Save_Background_Properties(createFile, i);
		createFile.close();
	}
}

void Game::Save_Editor(ofstream& createFile, int i) {
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 80; x++) {
			createFile << map[i][y][x] << " ";
		}
		createFile << "\n";
	}
	createFile << "\n\n";
}

void Game::Save_Character_Properties(ofstream& createFile, int i) {
	for (int condition = 0; condition < 3; condition++) {
		for (int action = 0; action < 3; action++) {
			for (int direction = 0; direction < 4; direction++) {
				for (int type = 0; type < 2; type++) {
					if (type != 1 || (condition != 1 && action != 0)) createFile << iControl_Settings[type][direction][condition][action][i];
					type == 1 ? createFile << "\n" : createFile << " ";
				}
			}
		}
	}

	createFile << iMax_Velocity_X[i] << "\n";
	createFile << iMax_Velocity_Y[i] << "\n";

	createFile << iPull_Velocity_X[i] << "\n";
	createFile << iPull_Velocity_Y[i] << "\n";

	createFile << iDrag_Velocity_X[i] << "\n";
	createFile << iDrag_Velocity_Y[i] << "\n";

	createFile << iRotation_With_Velocity_X[i] << " " << bRotation_With_Velocity_X[i] << "\n";
	createFile << iRotation_With_Velocity_Y[i] << " " << bRotation_With_Velocity_Y[i] << "\n";
	createFile << " " << bRotation_With_Velocity[i] << "\n";

	createFile << " " << bFlip_X[i] << "\n";
	createFile << " " << bFlip_Y[i] << "\n";

	createFile << "\n";

	for (int pY = 0; pY < 8; pY++) {
		for (int pX = 0; pX < 8; pX++) {
			createFile << int(character[i].copyToImage().getPixel(pX, pY).r) << " ";
			createFile << int(character[i].copyToImage().getPixel(pX, pY).g) << " ";
			createFile << int(character[i].copyToImage().getPixel(pX, pY).b) << " ";
			createFile << int(character[i].copyToImage().getPixel(pX, pY).a) << "  ";
		}
		createFile << "\n";
	}

	createFile << "\n\n";
}

void Game::Save_Block_Properties(ofstream& createFile, int i) {
	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 3; y++) {
			for (int pY = 0; pY < 8; pY++) {
				for (int pX = 0; pX < 8; pX++) {
					createFile << int(imgStatic_Block[i][x][y].getPixel(pX, pY).r) << " ";
					createFile << int(imgStatic_Block[i][x][y].getPixel(pX, pY).g) << " ";
					createFile << int(imgStatic_Block[i][x][y].getPixel(pX, pY).b) << " ";
					createFile << int(imgStatic_Block[i][x][y].getPixel(pX, pY).a) << "  ";
				}
				createFile << "\n";
			}
			createFile << "\n";
		}
	}
	for (int x = 0; x < 5; x++) {
		for (int pY = 0; pY < 4; pY++) {
			for (int img = 0; img < 4; img++) {
				for (int pX = 0; pX < 4; pX++) {
					createFile << int(imgDynamic_Block[i][x][img].getPixel(pX, pY).r) << " ";
					createFile << int(imgDynamic_Block[i][x][img].getPixel(pX, pY).g) << " ";
					createFile << int(imgDynamic_Block[i][x][img].getPixel(pX, pY).b) << " ";
					createFile << int(imgDynamic_Block[i][x][img].getPixel(pX, pY).a) << "  ";
				}
				createFile << " ";
			}
			createFile << "\n";
		}
		createFile << "\n";
	}
	createFile << "\n";
}

void Game::Save_Particle_Properties(ofstream& createFile, int i) {
	for (int j = 0; j < 2; j++) {
		for (int k = 0; k < 4; k++) {
			createFile << arrDecoration_Color[i][j][k] << " ";
		}
		createFile << "\n";
	}

	createFile << iParticle_Acceleration_X[i] << "\n";
	createFile << iParticle_Acceleration_Y[i] << "\n";

	createFile << iParticle_Static_Velocity_X[i] << " " << bParticle_Static_Velocity_X[i] << "\n";
	createFile << iParticle_Static_Velocity_Y[i] << " " << bParticle_Static_Velocity_Y[i] << "\n";

	createFile << "\n\n";
}

void Game::Save_Background_Properties(ofstream& createFile, int i) {

	for (int x = 0; x < 80; x++) {
		createFile << bgTrigger[i][x].active << " ";
		createFile << int(bgTrigger[i][x].color.r) << " ";
		createFile << int(bgTrigger[i][x].color.g) << " ";
		createFile << int(bgTrigger[i][x].color.b) << "\n";
	}

	createFile << "\n";
}

float Game::Get_Modulus(float The_Number, float The_Modulus) {
	while (The_Number >= The_Modulus) {
		The_Number -= The_Modulus;
	}
	return The_Number;
}

float Game::Get_Size_with_Zoom() {
	return 1.f / (500.f / game_Height);
};

float Game::Get_Origin_with_Zoom(const char* The_Axis_Id) {
	if (The_Axis_Id == "x") {
		return (fGrid_Cell_Size) / 2;
	}
	if (The_Axis_Id == "y") {
		return (fGrid_Cell_Size) / 2;
	}
}

float Game::Get_Position_with_Zoom(const char* The_Axis_Id, float The_Axis_Pos) {
	if (The_Axis_Id == "x") {
		return (The_Axis_Pos - fCamera_Position_X) / (500.f / game_Width);
	}
	if (The_Axis_Id == "y") {
		return (The_Axis_Pos - fCamera_Position_Y) / (500.f / game_Height);
	}
};

int Game::Get_Dynamic_Block_Corner(int Block, int Map_X, int Map_Y, int Block_X, int Block_Y) {

	return 0;
}

void Game::Particle_Group::Set_Total(int The_Total) {
	Particle* particle = new Particle[The_Total];
	Initialize();
}

void Game::Particle_Group::Initialize_Particle(int i) {
	particle[i].iTime = (iTimer + int(float(iLifetime) / iTotal_Particles * i)) % iLifetime;

	particle[i].fStart_Size_X		= fStart_Size_X		; if (fStart_Size_X_PM		!= 0)	particle[i].fStart_Size_X		+= rand()%int(fStart_Size_X_PM		* 2) - fStart_Size_X_PM;
	particle[i].fStart_Size_Y		= fStart_Size_Y		; if (fStart_Size_Y_PM		!= 0)	particle[i].fStart_Size_Y		+= rand()%int(fStart_Size_Y_PM		* 2) - fStart_Size_Y_PM;
	particle[i].fStart_Velocity_X	= fStart_Velocity_X	; if (fStart_Velocity_X_PM	!= 0)	particle[i].fStart_Velocity_X	+= rand()%int(fStart_Velocity_X_PM	* 2) - fStart_Velocity_X_PM;
	particle[i].fStart_Velocity_Y	= fStart_Velocity_Y	; if (fStart_Velocity_Y_PM	!= 0)	particle[i].fStart_Velocity_Y	+= rand()%int(fStart_Velocity_Y_PM	* 2) - fStart_Velocity_Y_PM;
	particle[i].fStart_Rotation		= fStart_Rotation	; if (fStart_Rotation_PM	!= 0)	particle[i].fStart_Rotation		+= rand()%int(fStart_Rotation_PM	* 2) - fStart_Rotation_PM;
	particle[i].colStart_Color		= colStart_Color;
	
	particle[i].fEnd_Size_X			= fEnd_Size_X		; if (fEnd_Size_X_PM		!= 0)	particle[i].fEnd_Size_X			+= rand()%int(fEnd_Size_X_PM		* 2) - fEnd_Size_X_PM;
	particle[i].fEnd_Size_Y			= fEnd_Size_Y		; if (fEnd_Size_Y_PM		!= 0)	particle[i].fEnd_Size_Y			+= rand()%int(fEnd_Size_Y_PM		* 2) - fEnd_Size_Y_PM;
	particle[i].fEnd_Velocity_X		= fEnd_Velocity_X	; if (fEnd_Velocity_X_PM	!= 0)	particle[i].fEnd_Velocity_X		+= rand()%int(fEnd_Velocity_X_PM	* 2) - fEnd_Velocity_X_PM;
	particle[i].fEnd_Velocity_Y		= fEnd_Velocity_Y	; if (fEnd_Velocity_Y_PM	!= 0)	particle[i].fEnd_Velocity_Y		+= rand()%int(fEnd_Velocity_Y_PM	* 2) - fEnd_Velocity_Y_PM;
	particle[i].fEnd_Rotation		= fEnd_Rotation		; if (fEnd_Rotation_PM		!= 0)	particle[i].fEnd_Rotation		+= rand()%int(fEnd_Rotation_PM		* 2) - fEnd_Rotation_PM;
	particle[i].colEnd_Color		= colEnd_Color;

	particle[i].fPosition_X			= fPosition_X + fCamera_Position_X;
	particle[i].fPosition_Y			= fPosition_Y;

	particle[i].fOffset_X			= 0;
	particle[i].fOffset_Y			= 0;
}

void Game::Particle_Group::Initialize() {
	iTimer = 0;

	for (int i = 0; i < iTotal_Particles; i++) {
		Initialize_Particle(i);
	}
}

void Game::Particle_Group::Update_Particle(int i) {
	int temp = particle[i].iTime;
	particle[i].iTime = (iTimer + int(float(iLifetime) / iTotal_Particles * i)) % iLifetime;

	if (temp > particle[i].iTime) {
		Initialize_Particle(i);
	}

	float fStart_Value		= float(iLifetime - particle[i].iTime) / iLifetime;
	float fEnd_Value		= float(			particle[i].iTime) / iLifetime;
	
	particle[i].fSize_X		=	particle[i].fStart_Size_X		* fStart_Value + particle[i].fEnd_Size_X		* fEnd_Value;
	particle[i].fSize_Y		=	particle[i].fStart_Size_Y		* fStart_Value + particle[i].fEnd_Size_Y		* fEnd_Value;
	
	particle[i].fVelocity_X	=	particle[i].fStart_Velocity_X	* fStart_Value + particle[i].fEnd_Velocity_X	* fEnd_Value;
	particle[i].fVelocity_Y	=	particle[i].fStart_Velocity_Y	* fStart_Value + particle[i].fEnd_Velocity_Y	* fEnd_Value;
	
	particle[i].fRotation	=	particle[i].fStart_Rotation		* fStart_Value + particle[i].fEnd_Rotation		* fEnd_Value;
	
	particle[i].fOffset_X	+=	particle[i].fVelocity_X * 25 / iFrames_per_Second;
	particle[i].fOffset_Y	+=	particle[i].fVelocity_Y * 25 / iFrames_per_Second;
	
	particle[i].colColor	=	sf::Color(	particle[i].colStart_Color.r	* fStart_Value	+ 
											particle[i].colEnd_Color.r		* fEnd_Value	,
											particle[i].colStart_Color.g	* fStart_Value	+
											particle[i].colEnd_Color.g		* fEnd_Value	,
											particle[i].colStart_Color.b	* fStart_Value	+
											particle[i].colEnd_Color.b		* fEnd_Value	,
											particle[i].colStart_Color.a	* fStart_Value	+
											particle[i].colEnd_Color.a		* fEnd_Value	);
}

void Game::Particle_Group::Update() {
	iTimer += int(1000.f / iFrames_per_Second);

	for (int i = 0; i < iTotal_Particles; i++) {
		Update_Particle(i);
	}
}

void Game::Particle_Group::Draw_Particle(int i, sf::RenderWindow*& window) {
	sf::RectangleShape rect;

	rect.setOrigin		(float(particle[i].fSize_X) / 2, float(particle[i].fSize_Y) / 2);
	rect.setPosition	(Get_Position_with_Zoom("x", particle[i].fPosition_X + particle[i].fOffset_X), Get_Position_with_Zoom("y", particle[i].fPosition_Y + particle[i].fOffset_Y));
	rect.setSize		(sf::Vector2f(Get_Size_with_Zoom() * particle[i].fSize_X, Get_Size_with_Zoom() * particle[i].fSize_Y));
	rect.setRotation	(particle[i].fRotation);
	rect.setFillColor	(particle[i].colColor);

	window->draw(rect);
}

void Game::Particle_Group::Draw(sf::RenderWindow*& window) {
	int iParticle;

	for (int i = 0; i < iTotal_Particles; i++) {
		iParticle = iTotal_Particles - int(floor(float(particle[i].iTime) / iLifetime * iTotal_Particles)) % iTotal_Particles - 1;
		Draw_Particle(iParticle, window);
	}
}