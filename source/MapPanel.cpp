/* MapPanel.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "MapPanel.h"


#include "AI.h"

#include "Angle.h"
#include "Dialog.h"
#include "FogShader.h"
#include "Font.h"
#include "FontSet.h"
#include "Fleet.h"
#include "Galaxy.h"
#include "GameData.h"
#include "Government.h"
#include "Information.h"
#include "Interface.h"
#include "LineShader.h"
#include "MapDetailPanel.h"
#include "MapOutfitterPanel.h"
#include "MapShipyardPanel.h"
#include "Mission.h"
#include "MissionPanel.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "PointerShader.h"
#include "Politics.h"
#include "Preferences.h"
#include "RingShader.h"
#include "Screen.h"
#include "Ship.h"
#include "SpriteShader.h"

//RTS Mode Sprites for menu SpriteShader
#include "SpriteSet.h"
//RTS Mode draw list for ships
#include "DrawList.h"
#include <set> //RTS set AwardShips()

#include "StellarObject.h"
#include "System.h"
#include "Trade.h"
#include "UI.h"



//#include <SDL_thread.h>
#include <thread>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>

#include <math.h>
#include <ctime> //AwardShips() @ time
//RTS Log
#include <fstream>
//#include "LogFile.h"
extern std::ofstream LogFile;
//RTS Mode Player info
#include "RTSPlayer.h"
//#include "GameControllerInput.cpp"

//typedef std::string

const std::string STARTINGLOCATION1 = "Sirius", STARTINGLOCATION2 = "Pollux", STARTINGLOCATION3 ="Caph", STARTINGLOCATION4 = "Fingol", STARTINGLOCATION5 ="Capella", STARTINGLOCATION6 ="Diphda",
STARTINGLOCATION7 = "Kugel", STARTINGLOCATION8 = "Nihal";
const std::string PLAYER1GOVERNMENT = "Player1", PLAYER2GOVERNMENT = "Player2", PLAYER3GOVERNMENT = "Player3", PLAYER4GOVERNMENT = "Player4", PLAYER5GOVERNMENT = "Player5", PLAYER6GOVERNMENT = "Player6";
const std::string PLAYER7GOVERNMENT = "Player7", PLAYER8GOVERNMENT = "Player8";


using namespace std;


//RTS Mode
bool rtsEnabled = 1;
extern SDL_GameController *GameController[];
extern const int JOYSTICK_DEAD_ZONE; //RTS controller

PlayerInfo PlayerInfoRTS[9];

void InitializeFlagships(PlayerInfo &Player);
void InitializeComships(PlayerInfo &Player);
void AwardShips();

shared_ptr<Ship> selectedShip = nullptr;

bool operator==(const Ship& lhs, const Ship& rhs);





void RTSPLayerMenu (int playNum);
int firstRun =0;

const double MapPanel::OUTER = 8.7; //6
const double MapPanel::INNER = 0; //2.5



MapPanel::MapPanel(PlayerInfo &player, int commodity, const System *special)
	: player(player), distance(player),
	playerSystem(player.GetSystem()),
	selectedSystem(special ? special : player.GetSystem()),
	specialSystem(special),
	commodity(commodity)
{
	SetIsFullScreen(true);
	SetInterruptible(false);


	//RTS mode initailize RTSPlayers
//	RTSPlayers RTSPlayers[9];



    int n =0;


	// Recalculate the fog each time the map is opened, just in case the player
	// bought a map since the last time they viewed the map.
	if (0 == 0)
	FogShader::Redraw();


	/* LogFile.open ("LogFile.txt", std::ofstream::out | std::ofstream::trunc);
	 if(LogFile.good())
        LogFile << "LOG FILE: OPENED Succesfully!\n";*/

	if(selectedSystem)
		center = Point(0., 0.) - selectedSystem->Position();


  if( SDL_NumJoysticks() < 1 )
        {
            printf( "Warning: No joysticks connected!\n" );
            LogFile << "No, Joysticks connected! \n";
        }
        else
        {
            //Load joysticks
LogFile << "\nNumber of Joysticks!" << SDL_NumJoysticks() << "\n";
           for( n = 0; n < SDL_NumJoysticks(); n++)
            {

                GameController[n+1] = SDL_GameControllerOpen( n );
                    if(GameController[n+1] == NULL)
                    {
                        LogFile << "Unable to open game controller " << n+ 1 << "! SDL Error: %s\n", SDL_GetError() ;
                    }
                    else
                        LogFile << "Game controller #" << n+1 << " connected! \n";
            }
      }
      for(int n =0; n <4;n++)
      {
          selectedMenuButtonHeight[n] =1;
          selectedMenuButtonWidth[n] = 0;
      }
      LogFile << "MapPanel initialized!\n\n\n";
    RTSInitializePlayers();
    for(int n = 1; n < 9; n++)
    {
        LogFile << endl << "n = " << n << endl;
        InitializeTravelPlans(PlayerInfoRTS[n]);
    }
    LogFile << "Flight plans initialized!" << endl;
}

void MapPanel::RTSInitializePlayers()
{
    LogFile << "Government of " << STARTINGLOCATION1 << " is " << GameData::Systems().Get(STARTINGLOCATION1)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[1].SetGovernment(GameData::PlayerOneGovernment());
    LogFile << "Player 1's government changed to " << PlayerInfoRTS[1].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION1)->SetGovernment(GameData::PlayerOneGovernment());
    LogFile << "Government of "<< STARTINGLOCATION1 << " set to " << GameData::Systems().Get(STARTINGLOCATION1)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[1].SetSystem(GameData::Systems().Get(STARTINGLOCATION1));

    LogFile <<endl << "Government of " << STARTINGLOCATION2 << " is " << GameData::Systems().Get(STARTINGLOCATION2)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[2].SetGovernment(GameData::PlayerTwoGovernment());
    LogFile << "Player 2's government changed to " << PlayerInfoRTS[2].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION2)->SetGovernment(GameData::PlayerTwoGovernment());
    LogFile << "Government of " << STARTINGLOCATION2 << " set to " << GameData::Systems().Get(STARTINGLOCATION2)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[2].SetSystem(GameData::Systems().Get(STARTINGLOCATION2));

    LogFile <<endl << "Government of "<< STARTINGLOCATION3 << " is " << GameData::Systems().Get(STARTINGLOCATION3)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[3].SetGovernment(GameData::PlayerThreeGovernment());
    LogFile << "Player 3's government changed to " << PlayerInfoRTS[3].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION3)->SetGovernment(GameData::PlayerThreeGovernment());
    LogFile << "Government of " << STARTINGLOCATION3 << " set to " << GameData::Systems().Get(STARTINGLOCATION3)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[3].SetSystem(GameData::Systems().Get(STARTINGLOCATION3));

    LogFile <<endl << "Government of " << STARTINGLOCATION4 << " is " << GameData::Systems().Get(STARTINGLOCATION4)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[4].SetGovernment(GameData::PlayerFourGovernment());
    LogFile << "Player 4's government changed to " << PlayerInfoRTS[4].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION4)->SetGovernment(GameData::PlayerFourGovernment());
    LogFile << "Government of "<< STARTINGLOCATION4 << " set to " << GameData::Systems().Get(STARTINGLOCATION4)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[4].SetSystem(GameData::Systems().Get(STARTINGLOCATION4));

    LogFile <<endl << "Government of " << STARTINGLOCATION5 << " is " << GameData::Systems().Get(STARTINGLOCATION5)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[5].SetGovernment(GameData::PlayerFiveGovernment());
    LogFile << "Player 5's government changed to " << PlayerInfoRTS[5].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION5)->SetGovernment(GameData::PlayerFiveGovernment());
    LogFile << "Government of " << STARTINGLOCATION5 << " set to " << GameData::Systems().Get(STARTINGLOCATION5)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[5].SetSystem(GameData::Systems().Get(STARTINGLOCATION5));

    LogFile <<endl << "Government of " << STARTINGLOCATION6 << " is " << GameData::Systems().Get(STARTINGLOCATION6)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[6].SetGovernment(GameData::PlayerSixGovernment());
    LogFile << "Player 6's government changed to " << PlayerInfoRTS[6].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION6)->SetGovernment(GameData::PlayerSixGovernment());
    LogFile << "Government of " << STARTINGLOCATION6 << " set to " << GameData::Systems().Get(STARTINGLOCATION6)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[6].SetSystem(GameData::Systems().Get(STARTINGLOCATION6));

    LogFile <<endl << "Government of " << STARTINGLOCATION7 << " is " << GameData::Systems().Get(STARTINGLOCATION7)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[7].SetGovernment(GameData::PlayerSevenGovernment());
    LogFile << "Player 7's government changed to " << PlayerInfoRTS[7].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION7)->SetGovernment(GameData::PlayerSevenGovernment());
    LogFile << "Government of " << STARTINGLOCATION7 << " set to " << GameData::Systems().Get(STARTINGLOCATION7)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[7].SetSystem(GameData::Systems().Get(STARTINGLOCATION7));

        LogFile <<endl << "Government of " << STARTINGLOCATION8 << " is " << GameData::Systems().Get(STARTINGLOCATION8)->GetGovernment()->GetName() << endl;
    PlayerInfoRTS[8].SetGovernment(GameData::PlayerEightGovernment());
    LogFile << "Player 8's government changed to " << PlayerInfoRTS[8].GetGovernment()->GetName() << endl;
   GameData::Systems().Get(STARTINGLOCATION8)->SetGovernment(GameData::PlayerEightGovernment());
    LogFile << "Government of " << STARTINGLOCATION8 << " set to " << GameData::Systems().Get(STARTINGLOCATION8)->GetGovernment()->GetName() << endl;
PlayerInfoRTS[8].SetSystem(GameData::Systems().Get(STARTINGLOCATION8));


for(int n=1; n < 9; n++)
{
    InitializeFlagships(PlayerInfoRTS[n]);
    InitializeComships(PlayerInfoRTS[n]);

}
LogFile <<endl << "RTS players initialized!\n\n\n";
    return;
}
void MapPanel::InitializeTravelPlans(PlayerInfo &Player)
{
    LogFile << "InilizeTravelPlans" << endl;
    Ship *tempShipPointer = Player.ReturnSelectedShip().get();

    System *toSystem = GameData::Systems().Get(STARTINGLOCATION4);
    tempShipPointer->SetTargetSystem(toSystem); //Set Target system

    RTSSelect(Player, toSystem , tempShipPointer);
 Angle shipAngle = Angle(tempShipPointer->TravelPlan().front()->Position());
    tempShipPointer->Place(Player.Flagship()->GetSystem()->Position(), shipAngle.Unit() * tempShipPointer->MaxVelocity(),shipAngle );

    tempShipPointer->SetIsNotParked(); //Ship launched

    LogFile << "RTSSelect Complete!" << endl << endl;
    return;
}

void InitializeFlagships(PlayerInfo &Player)
{

Player.RTSAddShip(Player, Player.GetSystem(), GameData::Ships().Get("Mothership"), "MotherShip");
LogFile << "MotherShip: " + Player.GetGovernment()->GetName() << endl << "System: " << Player.GetSystem()->Name() << endl << endl;
//LogFile <<"Flagship test! " << Player.Flagship()->Name() << " for " << Player.Flagship()->GetGovernment()->GetName() << endl;
return;
}

void InitializeComships(PlayerInfo &Player)
{

for(int n =1; n <11; n++)
{Player.RTSAddShip(Player, Player.Flagship()->GetSystem(), GameData::Ships().Get("Command Ship"), "CommandShip");
LogFile << "Command Ship # " << to_string(n) << Player.GetGovernment()->GetName() << endl << "System: " << Player.GetSystem()->Name() << endl << endl;
}
return;
}

void AwardShips()
{ //ToDo: Count number of stars owned by each player than add all ships to system otherShip is located unless motherShip is traveling in which case the  ships should be placed inside mothership.
int unClaimedSystems =0;
unsigned int numStarsPlayer1 = 0, numStarsPlayer2 =0, numStarsPlayer3=0, numStarsPlayer4=0, numStarsPlayer5=0, numStarsPlayer6=0, numStarsPlayer7=0, numStarsPlayer8=0;
GameData::Systems().begin();
     for (auto &it : GameData::Systems())
    {

        if(it.second.GetGovernment()->GetName() == PLAYER1GOVERNMENT){
             numStarsPlayer1++;
                LogFile << PLAYER1GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else  if(it.second.GetGovernment()->GetName() == PLAYER2GOVERNMENT){
              //  PlayerInfoRTS[2].Flagship()->RTSAddFigs(1);
                numStarsPlayer2++;
                LogFile << PLAYER2GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else if(it.second.GetGovernment()->GetName() == PLAYER3GOVERNMENT){
           numStarsPlayer3++;
                LogFile << PLAYER3GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else if(it.second.GetGovernment()->GetName() == PLAYER4GOVERNMENT){
                numStarsPlayer4++;
                LogFile << PLAYER4GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else if(it.second.GetGovernment()->GetName() == PLAYER5GOVERNMENT){
                numStarsPlayer5++;
                LogFile << PLAYER5GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else  if(it.second.GetGovernment()->GetName() == PLAYER6GOVERNMENT){
                numStarsPlayer6++;
                LogFile << PLAYER6GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else if (it.second.GetGovernment()->GetName() == PLAYER7GOVERNMENT){
                numStarsPlayer7++;
                LogFile << PLAYER7GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
        }
        else  if(it.second.GetGovernment()->GetName() == PLAYER8GOVERNMENT){
                numStarsPlayer8++;
                LogFile << PLAYER8GOVERNMENT << " owns system " << it.second.Name() << " thus player recieves 1 fig ship." << endl;
                continue;
         }
         else
            {unClaimedSystems++;continue;} //System not owned by human player. Do nothing
    }

    if(PlayerInfoRTS[1].IsDead() == false)
     {

        if(PlayerInfoRTS[1].Flagship()->IsParked() == true)
            {GameData::Systems().Get(PlayerInfoRTS[1].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer1);
            LogFile << "Ships for " << PlayerInfoRTS << " spawned "<< numStarsPlayer1 << " into system MotherShip is parked." << endl;
            }
        else
            {
            PlayerInfoRTS[1].Flagship()->AddFigs(numStarsPlayer1);
            LogFile << "Ships for " << PlayerInfoRTS[1].GetGovernment()->GetName() << " spawned " << numStarsPlayer1 << " into moving motherShip. " << endl;
            }
     }

    if(PlayerInfoRTS[2].IsDead() == false)
    {


        if(PlayerInfoRTS[2].Flagship()->IsParked() == true)
            {GameData::Systems().Get(PlayerInfoRTS[2].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer2);
            LogFile << "Ships for " << PlayerInfoRTS[2].GetGovernment()->GetName() << " spawned " << numStarsPlayer2 << " into system MotherShip is parked." << endl;
        }
        else
            {PlayerInfoRTS[2].Flagship()->AddFigs(numStarsPlayer2);
            LogFile << "Ships for " << PlayerInfoRTS[2].GetGovernment()->GetName() << " spawned "<< numStarsPlayer2 <<" into moving motherShip. " << endl;
            }
    }

        if(PlayerInfoRTS[3].IsDead() == false)
    {
    if(PlayerInfoRTS[3].Flagship()->IsParked() == true)
     {GameData::Systems().Get(PlayerInfoRTS[3].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer3);
     LogFile << "Ships for " << PlayerInfoRTS[3].GetGovernment()->GetName() << " spawned "<< numStarsPlayer3 << " into system MotherShip is parked." << endl;
     }
     else
        {PlayerInfoRTS[3].Flagship()->AddFigs(numStarsPlayer3);
        LogFile << "Ships for " << PlayerInfoRTS[3].GetGovernment()->GetName() << " spawned " << numStarsPlayer3 << " into moving motherShip. " << endl;
        }
    }

        if(PlayerInfoRTS[4].IsDead() == false)
    {
            if(PlayerInfoRTS[4].Flagship()->IsParked() == true)
                {GameData::Systems().Get(PlayerInfoRTS[4].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer4);
                LogFile << "Ships for " << PlayerInfoRTS[4].GetGovernment()->GetName() << " spawned " << numStarsPlayer4 << " into system MotherShip is parked." << endl;
                }
            else
                {PlayerInfoRTS[4].Flagship()->AddFigs(numStarsPlayer4);
                LogFile << "Ships for " << PlayerInfoRTS[4].GetGovernment()->GetName() << " spawned " << numStarsPlayer4 << " into moving motherShip. " << endl;
                }
    }

        if(PlayerInfoRTS[5].IsDead() == false)
    {

            if(PlayerInfoRTS[5].Flagship()->IsParked() == true)
     {GameData::Systems().Get(PlayerInfoRTS[5].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer5);
     LogFile << "Ships for " << PlayerInfoRTS[5].GetGovernment()->GetName() << " spawned " << numStarsPlayer5 << " into system MotherShip is parked." << endl;
     }
     else
        {PlayerInfoRTS[5].Flagship()->AddFigs(numStarsPlayer5);
        LogFile << "Ships for " << PlayerInfoRTS[5].GetGovernment()->GetName() << " spawned " << numStarsPlayer5 << " into moving motherShip. " << endl;
        }
    }

        if(PlayerInfoRTS[6].IsDead() == false)
    {
            if(PlayerInfoRTS[6].Flagship()->IsParked() == true)
     {GameData::Systems().Get(PlayerInfoRTS[6].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer6);
     LogFile << "Ships for " << PlayerInfoRTS[6].GetGovernment()->GetName() << " spawned " << numStarsPlayer6 << " into system MotherShip is parked." << endl;
     }
     else
       {
       PlayerInfoRTS[6].Flagship()->AddFigs(numStarsPlayer6);
        LogFile << "Ships for " << PlayerInfoRTS[6].GetGovernment()->GetName() << " spawned " << numStarsPlayer6 << " into moving motherShip. " << endl;
       }
    }

    if(PlayerInfoRTS[7].IsDead() == false)
    {
            if(PlayerInfoRTS[7].Flagship()->IsParked() == true)
     {GameData::Systems().Get(PlayerInfoRTS[7].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer7);
     LogFile << "Ships for " << PlayerInfoRTS[7].GetGovernment()->GetName() << " spawned " << numStarsPlayer7 << " into system MotherShip is parked." << endl;
     }
     else
        {PlayerInfoRTS[7].Flagship()->AddFigs(numStarsPlayer7);
        LogFile << "Ships for " << PlayerInfoRTS[7].GetGovernment()->GetName() << " spawned " << numStarsPlayer7 << " into moving motherShip. " << endl;
        }
    }

        if(PlayerInfoRTS[8].IsDead() == false)
    {
            if(PlayerInfoRTS[8].Flagship()->IsParked() == true)
     {GameData::Systems().Get(PlayerInfoRTS[8].Flagship()->GetSystem()->Name())->AddFigs(numStarsPlayer8);
     LogFile << "Ships for " << PlayerInfoRTS[8].GetGovernment()->GetName() << " spawned " << numStarsPlayer8 << " into system MotherShip is parked." << endl;
     }
     else
        {PlayerInfoRTS[8].Flagship()->AddFigs(numStarsPlayer8);
        LogFile << "Ships for " << PlayerInfoRTS[8].GetGovernment()->GetName() << " spawned " << numStarsPlayer8 << " into moving motherShip. " << endl;
        }
    }

    LogFile << "Unclaimed Systems: " << unClaimedSystems << endl << endl;
    return;
}

void MapPanel::Draw()
{
   static int frames =5350;
     std::time_t result = std::time(nullptr);

    if(frames > 5400) //If aprox. 1.5 minuetes has passed.
    {
 frames =0; //Reset frame counter
    AwardShips();

        LogFile << "Ships awarded! @ " << std::asctime(std::localtime(&result)) << endl;
    }
    else
        frames++;

	glClear(GL_COLOR_BUFFER_BIT);



    if(firstRun > 1000000)
        firstRun = 1;


	for(const auto &it : GameData::Galaxies())
		SpriteShader::Draw(it.second.GetSprite(), Zoom() * (center + it.second.Position()), Zoom());


    if (rtsEnabled == 0)
    {

        if(Preferences::Has("Hide unexplored map regions"))
            FogShader::Draw(center, Zoom(), player);
}

	DrawTravelPlan();

	for(int16_t n=1; n < 9; n++) //Draw the TravelPlan for each of the player's selectedShip
    {
        if(PlayerInfoRTS[n].IsDead() == true)
            continue;
	RTSDrawPlayerTravelPlan(PlayerInfoRTS[n]);
    }

	// Draw the "visible range" circle around your current location.
	if(rtsEnabled == 1)
    {

    }
    else
    {
	Color dimColor(.1, 0.);
	RingShader::Draw(Zoom() * (playerSystem ? playerSystem->Position() + center : center),
		(System::NEIGHBOR_DISTANCE + .5) * Zoom(), (System::NEIGHBOR_DISTANCE - .5) * Zoom(), dimColor);
	Color brightColor(.4, 0.);
	RingShader::Draw(Zoom() * (selectedSystem ? selectedSystem->Position() + center : center),
		11., 9., brightColor);
    }


	DrawWormholes();
	DrawLinks();
	DrawSystems();
	DrawNames();
	DrawMissions();

	for(int n=1; n <9; n++)
        {

            CalculatePositionOfShips(PlayerInfoRTS[n]);
        }
    DrawSelectedShips();         //RTS addition

	if(!distance.HasRoute(selectedSystem))
	{
		const Font &font = FontSet::Get(18);

		static const string NO_ROUTE = "You have not yet mapped a route to this system.";
		Color black(0., 1.);
		Color red(1., 0., 0., 1.);
		Point point(-font.Width(NO_ROUTE) / 2, Screen::Top() + 40);

		font.Draw(NO_ROUTE, point + Point(1, 1), black);
		font.Draw(NO_ROUTE, point, red);
	}
}



void MapPanel::DrawButtons(const string &condition)
{

   Point uiPoint(Screen::Left()+40, Screen::Top()+18);
   int menuSpace = 20;
	// Remember which buttons we're showing.
	buttonCondition = condition;

	const Font &font = FontSet::Get(18);
	const Font &bigFont = FontSet::Get(20);

	const Sprite *motherShipBlue = SpriteSet::Get("ship/behemoth-blue");
	const Sprite *maxBlue = SpriteSet::Get("ui/max-blue");
	const Sprite *autumnLeafBlue = SpriteSet::Get("ui/autumn leaf blue");
	const Sprite *plusFiveBlue = SpriteSet::Get("ui/5-blue");
	const Sprite *plueFive = SpriteSet::Get("ui/5");

	const Sprite *negativeFiveBlue = SpriteSet::Get("ui/5-neg-blue");
	const Sprite *negativeFive = SpriteSet::Get("ui/5-neg");
	const Sprite *maxSprite = SpriteSet::Get("ui/max");


    Color bright = *GameData::Colors().Get("bright");
    Color dim = *GameData::Colors().Get("dim");
    uint16_t dimImage = 0;
    if(PlayerInfoRTS[1].IsDead() == false)
    {

Ship *selectedShip = PlayerInfoRTS[1].Flagship();

	if(rtsEnabled)
    {

    if(selectedMenuButtonWidth[1] == 0 && selectedMenuButtonHeight[1] ==3) //If player selected 0 width and 3 height show MAX for figthers/berserker as selected.
        {
        SpriteShader::Draw(maxBlue, uiPoint, 1.2);
        }
    else
        {

        SpriteShader::Draw(maxSprite, uiPoint, 1);
        }

    uiPoint.X() +=54;

        if(selectedMenuButtonWidth[1] == 1 && selectedMenuButtonHeight[1] ==3) //If player selected 1 width and 3 height show MAX for all com ships as selected.
        {
        SpriteShader::Draw(maxBlue, uiPoint, 1.2);
        }
    else
        {

        SpriteShader::Draw(maxSprite, uiPoint, 1);
        }

        uiPoint.X()+=54;

        //Second Row
    uiPoint.Y() += 25;
    uiPoint.X() -=108;



        if(selectedMenuButtonWidth[1] == 0 && selectedMenuButtonHeight[1] ==2) //If player selected 0 width and 2 height show +5 for figthers/berserker as selected.
        {
        SpriteShader::Draw(plusFiveBlue, uiPoint, 1.2);
        }
    else
        {

        SpriteShader::Draw(plueFive, uiPoint, 1);
        }


    uiPoint.X()+=54;

        if(selectedMenuButtonWidth[1] == 1 && selectedMenuButtonHeight[1] ==2) //If player selected 1 width and 2 height show +5 for figthers/berserker as selected.
        {
        SpriteShader::Draw(plusFiveBlue, uiPoint, 1.2);
        }
    else
        {

        SpriteShader::Draw(plueFive, uiPoint, 1);
        }


        uiPoint.X()+=54;

        //Third Row

    uiPoint.Y() += 60;
    uiPoint.X() -=108;

    if(PlayerInfoRTS[1].GetNumSendFigs() > 0)
        dimImage = 1;
    else
        dimImage = 0;

        if(selectedMenuButtonWidth[1] == 0 && selectedMenuButtonHeight[1] ==1) //If player selected 0 width and 1 height show berserker as selected.
        {const Sprite *berSerkerSpriteBlue = SpriteSet::Get("ui/berserker-selected-blue");
        SpriteShader::Draw(berSerkerSpriteBlue, uiPoint, .50, dimImage);
        }
    else
        {
        const Sprite *berSerkerSprite = SpriteSet::Get("ship/berserker");
        SpriteShader::Draw(berSerkerSprite, uiPoint, .40, dimImage);
        }
    uiPoint.X() -= 5;
    uiPoint.Y() -= 4;

        if(PlayerInfoRTS[1].GetNumSendFigs() > 0)
            font.Draw(to_string(PlayerInfoRTS[1].GetNumSendFigs()), uiPoint, bright);


    uiPoint.Y() += 4;
    uiPoint.X()+=59;

    if(PlayerInfoRTS[1].GetNumSendComs() > 0)
        dimImage = 1;
    else
        dimImage = 0;

    if(selectedMenuButtonWidth[1] == 1 && selectedMenuButtonHeight[1]==1) //If player selected 1 width and 1 height show com ship as selected.
        {

        SpriteShader::Draw(autumnLeafBlue, uiPoint, .40, dimImage);

        }
    else
        {

        const Sprite *autumnLeaf = SpriteSet::Get("ship/autumn leaf");
        SpriteShader::Draw(autumnLeaf, uiPoint, .35, dimImage);
        }

   uiPoint.X() -= 5;
    uiPoint.Y() -= 4;

        if(PlayerInfoRTS[1].GetNumSendFigs() > 0)
            font.Draw(to_string(PlayerInfoRTS[1].GetNumSendComs()), uiPoint, bright);


 uiPoint.Y() += 4;

    uiPoint.X()+=59;

    if(PlayerInfoRTS[1].IsMotherShipSend() > 0)
        dimImage = 1;
    else
        dimImage = 0;

    if(selectedMenuButtonWidth[1] == 2 && selectedMenuButtonHeight[1] >= 1) //If player selected 2 width and any height show mothership as selected.
        {
        SpriteShader::Draw(motherShipBlue, uiPoint, .25, dimImage);

        }
    else
        {

        const Sprite *motherShipImage = SpriteSet::Get("ship/behemoth");
        SpriteShader::Draw(motherShipImage, uiPoint, .22, dimImage);
        }

         uiPoint.X() -= 5;
    uiPoint.Y() -= 4;

        if(PlayerInfoRTS[1].IsMotherShipSend())
            font.Draw(to_string(PlayerInfoRTS[1].IsMotherShipSend()), uiPoint, bright);


 uiPoint.Y() += 4;


    uiPoint.X()+=85;

    if(selectedMenuButtonWidth[1] == 3 ) //If player selected any button on the last section of menu then show blue selected continue button.
        {const Sprite *continueButtonBlue = SpriteSet::Get("ui/continue-blue");
        SpriteShader::Draw(continueButtonBlue, uiPoint, .90);
        if(selectedMenuButtonHeight[1] > 3)
            selectedMenuButtonHeight[1] = 3;
        else if (selectedMenuButtonHeight[1] < 0)
            selectedMenuButtonHeight[1] = 0;
        }
    else
        {

        const Sprite *continueButton = SpriteSet::Get("ui/continue");
        SpriteShader::Draw(continueButton, uiPoint, .75);
        }


    //Fourth Row - '-5'
    uiPoint.Y() += 55;
    uiPoint.X() -=188;

      if(selectedMenuButtonWidth[1] == 0 && selectedMenuButtonHeight[1] ==0) //If player selected 0 width and 3 height show MAX for figthers/berserker as selected.
        {//const Sprite *negativeFiveBlue = SpriteSet::Get("ui/5-neg-blue");

        SpriteShader::Draw(negativeFiveBlue, uiPoint, 1.2);
        }
    else
        {
        //const Sprite *negativeFive = SpriteSet::Get("ui/5-neg");
        SpriteShader::Draw(negativeFive, uiPoint, 1);
        }


        uiPoint.X()+=54;

        if(selectedMenuButtonWidth[1] == 1 && selectedMenuButtonHeight[1] ==0) //If player selected 0 width and 3 height show MAX for figthers/berserker as selected.
        {//const Sprite *negativeFiveBlue = SpriteSet::Get("ui/5-neg-blue");
        SpriteShader::Draw(negativeFiveBlue, uiPoint, 1.2);
        }
    else
        {
      //  const Sprite *negativeFive = SpriteSet::Get("ui/5-neg");
        SpriteShader::Draw(negativeFive, uiPoint, 1);
        }


          uiPoint.X()+=54;

        if(selectedMenuButtonWidth[1] == 2 && selectedMenuButtonHeight[1] ==0) //If player selected 0 width and 3 height show MAX for figthers/berserker as selected.
        {const Sprite *negativeOneBlue = SpriteSet::Get("ui/1-neg-blue");
        SpriteShader::Draw(negativeOneBlue, uiPoint, 1.2);
        }
    else
        {
        const Sprite *negativeOne = SpriteSet::Get("ui/1-neg");
        SpriteShader::Draw(negativeOne, uiPoint, 1);
        }

     uiPoint.X()-=114;
     uiPoint.Y() += 20;




     font.Draw(to_string(selectedShip->GetSystem()->GetNumFigs()), uiPoint, bright);
      uiPoint.X() +=54;
   font.Draw(to_string(selectedShip->GetSystem()->GetNumComShips()), uiPoint, bright);
      uiPoint.X() +=54;
    if(selectedShip->GetSystem()->MotherShipPresent())
    font.Draw("1", uiPoint, bright);
    else
        font.Draw("0", uiPoint, dim);

    uiPoint.X() +=54;
   font.Draw("In System", uiPoint, dim);


    }

	// Draw the buttons to switch to other map modes.
	Information info;
	info.SetCondition(condition);
	if(player.MapZoom() == 2)
		info.SetCondition("max zoom");
	if(player.MapZoom() == -2)
		info.SetCondition("min zoom");
    if(1 == 0) //Rts do not show buttons
    {
	const Interface *interface = GameData::Interfaces().Get("map buttons");

	interface->Draw(info, this);
    }
    }
}



void MapPanel::DrawMiniMap(const PlayerInfo &player, double alpha, const System *const jump[2], int step)
{
	/*const Font &font = FontSet::Get(14);
	Color lineColor(alpha, 0.);
	Point center = .5 * (jump[0]->Position() + jump[1]->Position());
	Point drawPos(0., Screen::Top() + 100.);
	set<const System *> seen;
	bool isLink = false;

	const Set<Color> &colors = GameData::Colors();
	Color currentColor = colors.Get("active mission")->Additive(alpha * 2.);
	Color blockedColor = colors.Get("blocked mission")->Additive(alpha * 2.);
	Color waypointColor = colors.Get("waypoint")->Additive(alpha * 2.);

	for(int i = 0; i < 2; ++i)
	{
		const System *system = jump[i];
		const Government *gov = system->GetGovernment();
		bool isKnown = player.KnowsName(system);
		Point from = system->Position() - center + drawPos;
		string name = isKnown ? system->Name() : "Unexplored System";
		font.Draw(name, from + Point(6., -.5 * font.Height()), lineColor);

		Color color = Color(.5 * alpha, 0.);
		if(player.HasVisited(system) && system->IsInhabited(player.Flagship()) && gov)
			color = Color(
				alpha * gov->GetColor().Get()[0],
				alpha * gov->GetColor().Get()[1],
				alpha * gov->GetColor().Get()[2], 0.);
		RingShader::Draw(from, 6., 3.5, color);

		for(const System *link : system->Links())
		{
			if(!player.HasVisited(system) && !player.HasVisited(link))
				continue;

			Point to = link->Position() - center + drawPos;
			Point unit = (from - to).Unit() * 7.;
			LineShader::Draw(from - unit, to + unit, 1.2, lineColor);

			isLink |= (link == jump[!i]);
			if(seen.count(link) || link == jump[!i])
				continue;
			seen.insert(link);

			gov = link->GetGovernment();
			Color color = Color(.5 * alpha, 0.);
			if(player.HasVisited(link) && link->IsInhabited(player.Flagship()) && gov)
				color = Color(
					alpha * gov->GetColor().Get()[0],
					alpha * gov->GetColor().Get()[1],
					alpha * gov->GetColor().Get()[2], 0.);
			RingShader::Draw(to, 6., 3.5, color);
		}

		Angle angle;
		for(const Mission &mission : player.Missions())
		{
			if(!mission.IsVisible())
				continue;

			if(mission.Destination()->IsInSystem(system))
			{
				bool blink = false;
				if(mission.Deadline())
				{
					int days = min(5, mission.Deadline() - player.GetDate()) + 1;
					if(days > 0)
						blink = (step % (10 * days) > 5 * days);
				}
				if(!blink)
				{
					bool isSatisfied = IsSatisfied(player, mission);
					DrawPointer(from, angle, isSatisfied ? currentColor : blockedColor, false);
				}
			}

			for(const System *waypoint : mission.Waypoints())
				if(waypoint == system)
					DrawPointer(from, angle, waypointColor, false);
			for(const Planet *stopover : mission.Stopovers())
				if(stopover->IsInSystem(system))
					DrawPointer(from, angle, waypointColor, false);
		}
	}

	Point from = jump[0]->Position() - center + drawPos;
	Point to = jump[1]->Position() - center + drawPos;
	Point unit = (to - from).Unit();
	from += 7. * unit;
	to -= 7. * unit;
	Color bright(2. * alpha, 0.);
	if(!isLink)
	{
		double length = (to - from).Length();
		int segments = static_cast<int>(length / 15.);
		for(int i = 0; i < segments; ++i)
			LineShader::Draw(
				from + unit * ((i * length) / segments + 2.),
				from + unit * (((i + 1) * length) / segments - 2.),
				1.2, bright);
	}
	LineShader::Draw(to, to + Angle(-30.).Rotate(unit) * -10., 1.2, bright);
	LineShader::Draw(to, to + Angle(30.).Rotate(unit) * -10., 1.2, bright);*/
}



bool MapPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{

	if(command.Has(Command::MAP) || key == 'd' || key == SDLK_ESCAPE
			|| (key == 'w' && (mod & (KMOD_CTRL | KMOD_GUI))))
		GetUI()->Pop(this);

	else if(key == 's' && buttonCondition != "is shipyards")
	{
		GetUI()->Pop(this);
		GetUI()->Push(new MapShipyardPanel(*this));
	}
	else if(key == 'o' && buttonCondition != "is outfitters")
	{
		GetUI()->Pop(this);
		GetUI()->Push(new MapOutfitterPanel(*this));
	}
	else if(key == 'i' && buttonCondition != "is missions")
	{
		GetUI()->Pop(this);
		GetUI()->Push(new MissionPanel(*this));
	}
	else if(key == 'p' && buttonCondition != "is ports")
	{
		GetUI()->Pop(this);
		GetUI()->Push(new MapDetailPanel(*this));
	}
	else if(key == 'f')
	{
		GetUI()->Push(new Dialog(
			this, &MapPanel::Find, "Search for:"));
		return true;
	}
	else if(key == '+' || key == '=')
		player.SetMapZoom(min(2, player.MapZoom() + 1));
	else if(key == '-')
		player.SetMapZoom(max(-2, player.MapZoom() - 1));
	else
		return false;

	return true;
}
bool MapPanel::ControllerJoystickMotion (SDL_ControllerAxisEvent joystickMotion, int playNum)
{
    LogFile << "Controller " << playNum << "sent joystick event from the " << joystickMotion.axis << " axis.\n";
    switch(joystickMotion.axis)
    {


    case 'x':
        {
        controllerXAxis[playNum] = joystickMotion.value;
        LogFile << "X-axis changed to " << joystickMotion.value;
        return true;
        }

    case 'y':
        {
        controllerYAxis[playNum] = joystickMotion.value;
        LogFile << "Y-axis changed to " << joystickMotion.value;

        return true;
        }
    }



}


bool MapPanel::ControllerButtonDown (Uint8 button, int playNum)
{
if(PlayerInfoRTS[playNum].IsDead() == true)
    return true; //ignore all input from the dead.

       switch(button)
       {


            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            {
                LogFile << "Controller " << playNum << " sent event from D-pad Left.\n";
                if(selectedMenuButtonWidth[playNum] < 1) //If player is currently at 0 position in menu, do nothing.
                    {
                    selectedMenuButtonWidth[playNum] = 0;
                    LogFile << "Controller " << playNum << " requested menu selection width be sent to the left. However width is already at the end!\n" << playNum << "height : " << selectedMenuButtonHeight[playNum] << " width : " << selectedMenuButtonWidth[playNum] << "\n";
                    return true;
                    }
                else
                    {selectedMenuButtonWidth[playNum]--;
                     LogFile << "Controller " << playNum << " requested and received menu selection width be sent to the left. \n" << playNum << ":\nheight : " << selectedMenuButtonHeight[playNum] << " \nwidth : " << selectedMenuButtonWidth[playNum] << "\n\n";
                    return true;}
            }
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            {
                LogFile << "Controller " << playNum << " sent event from D-pad right.\n";
                if(selectedMenuButtonWidth[playNum] > 2) //If player is currently at futherest width position in menu, do nothing.
                  {
                      selectedMenuButtonWidth[playNum] = 3;
                       LogFile << "Controller " << playNum << " requested menu selection width be sent to the right. However width is already at the end!\n" << playNum << "height : " << selectedMenuButtonHeight[playNum] << " width : " << selectedMenuButtonWidth[playNum] << "\n";
                        return true;
                    }
                else
                    {selectedMenuButtonWidth[playNum]++;
                     LogFile << "Controller " << playNum << " requested and received menu selection width be sent to the right. \n" << playNum << ":\nheight : " << selectedMenuButtonHeight[playNum] << " \nwidth : " << selectedMenuButtonWidth[playNum] << "\n\n";
                    return true;
                    }
            }

            case SDL_CONTROLLER_BUTTON_DPAD_UP:
            {
                LogFile << "Controller " << playNum << " sent event from D-pad up.\n";
                if(selectedMenuButtonHeight[playNum] > 2) //If player is currently at 0 position in menu, do nothing.
                      {
                          selectedMenuButtonHeight[playNum] = 3;
                       LogFile << "Controller " << playNum << " requested menu selection height be highered. However height is already on top!\n" << playNum << "height : " << selectedMenuButtonHeight[playNum] << " width : " << selectedMenuButtonWidth[playNum] << "\n";
                        return true;
                    }
                else
                   {
                    selectedMenuButtonHeight[playNum]++;
                     LogFile << "Controller " << playNum << " requested and received menu selection height highered. \n" << playNum << ":\nheight : " << selectedMenuButtonHeight[playNum] << " \nwidth : " << selectedMenuButtonWidth[playNum] << "\n\n";
                    return true;
                    }
            }
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            {
                LogFile << "Controller " << playNum << " sent event from D-pad down.\n";
                if(selectedMenuButtonHeight[playNum] <= 0)
                {
                    selectedMenuButtonHeight[playNum] = 0;
                    LogFile << "Controller " << playNum << " requested menu selection height be lowered. Height already on the bottom!\n" << playNum << "height : " << selectedMenuButtonHeight[playNum] << " width : " << selectedMenuButtonWidth[playNum] << "\n";
                    return true;

                }
                else if(selectedMenuButtonHeight[playNum] >= 1) //If player is currently at lowest hieght position in menu, do nothing.
                    {
                        selectedMenuButtonHeight[playNum] = 0;
                       LogFile << "Controller " << playNum << " requested menu selection height be lowered. Height sent to the bottom.\n" << playNum << "height : " << selectedMenuButtonHeight[playNum] << " width : " << selectedMenuButtonWidth[playNum] << "\n";
                        return true;
                    }
                else
                    {selectedMenuButtonHeight[playNum]--;
                    LogFile << "Controller " << playNum << " requested and received menu selection height lowered. \n" << playNum << ":\nheight : " << selectedMenuButtonHeight[playNum] << " \nwidth : " << selectedMenuButtonWidth[playNum] << "\n\n";
                    return true;}
            }


            case SDL_CONTROLLER_BUTTON_A:
            {
                LogFile << "Controller " << playNum << " sent event from button A.\n";
               //Accept
               LogFile << "Player.Flagship is @ " << player.Flagship() << endl;
                RTSSelect(PlayerInfoRTS[playNum], GameData::Systems().Get("Castor"), player.Flagship());
//                RTSDrawTravelPlan(player.Flagship());
                return true;


            }
            case SDL_CONTROLLER_BUTTON_X:
            {
                LogFile << "Controller " << playNum << " sent event from X button.\n";
              //Cancel last player action!

              return true;
            }

            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            {
               //SelectedComShip increment.
    LogFile << "Controller " << playNum << " sent event from right-shoulder button.\n";
        PlayerInfoRTS[playNum].NextSelectedShip();
        LogFile << "SelectedShip:[" << playNum << "]: " << PlayerInfoRTS[playNum].ReturnSelectedShip()->Name() << endl << endl;
        return true;
            }
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            {
                LogFile << "Controller " << playNum << " sent event from left-shoulder button.\n";
            PlayerInfoRTS[playNum].LastSelectedShip();
                    LogFile <<"SelectedShip:[" << playNum << "]: " << PlayerInfoRTS[playNum].ReturnSelectedShip()->Name() << endl << endl;
                    return true;
            }

            default:
                {
                LogFile << "Controller " << playNum << " sent event that was not handled.\n";
                return false;
                }
    }


}


bool MapPanel::Click(int x, int y, int clicks)
{
	// Figure out if a system was clicked on.
	Point click = Point(x, y) / Zoom() - center;

	for(const auto &it : GameData::Systems())
		if(click.Distance(it.second.Position()) < 10.
				 || &it.second == specialSystem)
		{
			Select(&it.second);
			break;
		}


	return true;
}



bool MapPanel::Drag(double dx, double dy)
{
	center += Point(dx, dy) / Zoom();
	return true;
}



bool MapPanel::Scroll(double dx, double dy)
{
	// The mouse should be pointing to the same map position before and after zooming.
	Point mouse = UI::GetMouse();
	Point anchor = mouse / Zoom() - center;
	if(dy > 0.)
		player.SetMapZoom(min(2, player.MapZoom() + 1));
	else
		player.SetMapZoom(max(-2, player.MapZoom() - 1));

	// Now, Zoom() has changed (unless at one of the limits). But, we still want
	// anchor to be the same, so:
	center = mouse / Zoom() - anchor;
	return true;
}



Color MapPanel::MapColor(double value)
{
	if(std::isnan(value))
		return UninhabitedColor();

	value = min(1., max(-1., value));
	if(value < 0.)
		return Color(
			.12 + .12 * value,
			.48 + .36 * value,
			.48 - .12 * value,
			.4);
	else
		return Color(
			.12 + .48 * value,
			.48,
			.48 - .48 * value,
			.4);
}



Color MapPanel::ReputationColor(double reputation, bool canLand, bool hasDominated)
{
	// If the system allows you to land, always show it in blue even if the
	// government is hostile.
	if(canLand)
		reputation = max(reputation, 0.);

	if(hasDominated)
		return Color(.1, .6, 0., .4);
	else if(reputation < 0.)
	{
		reputation = min(1., .1 * log(1. - reputation) + .1);
		return Color(.6, .4 * (1. - reputation), 0., .4);
	}
	else if(!canLand)
		return Color(.6, .54, 0., .4);
	else
	{
		reputation = min(1., .1 * log(1. + reputation) + .1);
		return Color(0., .6 * (1. - reputation), .6, .4);
	}
}



Color MapPanel::GovernmentColor(const Government *government)
{
	if(!government)
		return UninhabitedColor();

	return Color(
		.6 * government->GetColor().Get()[0],
		.6 * government->GetColor().Get()[1],
		.6 * government->GetColor().Get()[2],
		.4);
}



Color MapPanel::UninhabitedColor()
{
	return Color(.2, 0.);
}



Color MapPanel::UnexploredColor()
{
	return Color(.1, 0.);
}



double MapPanel::SystemValue(const System *system) const
{
	return 0.;
}


void MapPanel::RTSSelect(PlayerInfo &Player, const System *system, Ship *selectedShip)
{

		if(!system)
		return;
    LogFile << "Received selectedShip address @ " << selectedShip << endl;
		LogFile << "Player selected system: " << system->Name() << "\n";


    //selectedShip = player.Flagship();
     //New rts code
    vector<const System *> &plan = selectedShip->TravelPlan();
    if(!selectedShip || (!plan.empty() && system == plan.front()))
    return;


 LogFile << "SelectedShip's name is " << selectedShip->Name() << "\n";
 LogFile << "SelectedShip's current system is: " << selectedShip->GetSystem()->Name() << endl;
    LogFile << "SelectedShip's government is " << selectedShip->GetGovernment()->GetName() << "\n";
	//NEW RTS CODE
	bool isJumping = selectedShip->IsEnteringHyperspace();
	const System *source = isJumping ? selectedShip->GetTargetSystem() : selectedShip->GetSystem();

    plan.push_back(source);
    DistanceMap localDistance(Player, plan.front()); //Chnaged from plan.front() to system

	bool shift = (SDL_GetModState() & KMOD_SHIFT) && !plan.empty();
	if(system == source && !shift)
	{
	    LogFile << "system == source && shift button not pressed!" << endl;
		plan.clear();
		if(!isJumping)
			{//Old Code//player.Flagship()->SetTargetSystem(nullptr);
			    selectedShip->SetTargetSystem(nullptr);
			}

		else
        {

			plan.push_back(source);
        LogFile << source->Name() << " : Added to flight plan!" << endl;
	}
	}
	else if(shift)
	{
	    LogFile << "Shift detected!\n";

		if(localDistance.Days(system) <= 0)
			return;

		auto it = plan.begin();
		while(system != *it)
		{
		    LogFile << system->Name() << ": Entered to travel plan!" << endl; //RTS mode code
			it = ++plan.insert(it, system);
			system = localDistance.Route(system);
		}
	}
	else if(localDistance.Days(system) > 0)
	{
		plan.clear();
		if(!isJumping)
			{//Old code //player.Flagship()->SetTargetSystem(nullptr);
			    selectedShip->SetTargetSystem(nullptr);
			}

		while(system != source)
		{

			plan.push_back(system);
			LogFile << system->Name() << ": Added" << endl;
			system = localDistance.Route(system);

		}
		LogFile << "Loop ended!" << endl;
		if(isJumping)
			plan.push_back(source);
	}

	if(selectedShip->HasTravelPlan() == 0)
        LogFile << "Selected Ship has no travel plan!\n\n";

        LogFile << "Local distance is" << localDistance.Days(system) << endl;
}


void MapPanel::Select(const System *system)
{
		if(!system)
		return;
shared_ptr<Ship> selectedShip = nullptr;
		LogFile << "\nPlayer selected system: " << system->Name() << "\n";

	selectedSystem = system;

	/*vector<const System *> &plan = player.TravelPlan();
	if(!player.Flagship() || (!plan.empty() && system == plan.front()))
		return;*/ //None Rts code

    //New rts code
    selectedShip = player.ReturnSelectedShip();
    vector<const System *> &plan = selectedShip->TravelPlan();
    if(!selectedShip || (!plan.empty() && system == plan.front()))
    return;

    /*
	bool isJumping = player.Flagship()->IsEnteringHyperspace();
	const System *source = isJumping ? player.Flagship()->GetTargetSystem() : player.GetSystem();
*/
  LogFile << "SelectedShip's name is " << selectedShip->Name() << "\n";
    LogFile << "SelectedShip's government is " << selectedShip->GetGovernment()->GetName() << "\n";
	//NEW RTS CODE
	bool isJumping = selectedShip->IsEnteringHyperspace();
	const System *source = isJumping ? selectedShip->GetTargetSystem() : selectedShip->GetSystem();

	bool shift = (SDL_GetModState() & KMOD_SHIFT) && !plan.empty();
	if(system == source && !shift)
	{
		plan.clear();
		if(!isJumping)
			{//Old Code//player.Flagship()->SetTargetSystem(nullptr);
			    selectedShip->SetTargetSystem(nullptr);
			}
		else
			plan.push_back(source);
	}
	else if(shift)
	{
		DistanceMap localDistance(player, plan.front());
		if(localDistance.Days(system) <= 0)
			return;

		auto it = plan.begin();
		while(system != *it)
		{
		    LogFile << system->Name() << ": Entered to travel plan!" << endl; //RTS mode code
			it = ++plan.insert(it, system);
			system = localDistance.Route(system);
		}
	}

	else if(distance.Days(system) > 0)
	{
		plan.clear();
		if(!isJumping)
			{//Old code //player.Flagship()->SetTargetSystem(nullptr);
			    selectedShip->SetTargetSystem(nullptr);
			}

		while(system != source)
		{
			plan.push_back(system);
			system = distance.Route(system);
		}
		if(isJumping)
			plan.push_back(source);
	}
}



void MapPanel::Find(const string &name)
{
	int bestIndex = 9999;
	for(const auto &it : GameData::Systems())
		if(player.HasVisited(&it.second) || 1) //Rts mode sets this value to 1. All players can send ships to all stars.
		{
			int index = Search(it.first, name);
			if(index >= 0 && index < bestIndex)
			{
				bestIndex = index;
				selectedSystem = &it.second;
				center = Zoom() * (Point() - selectedSystem->Position());
				if(!index)
				{
					selectedPlanet = nullptr;
					return;
				}
			}
		}
	for(const auto &it : GameData::Planets())
		if(player.HasVisited(it.second.GetSystem()) || 1) //RTS mode sets this value to 1. All players can send ships to all stars.
		{
			int index = Search(it.first, name);
			if(index >= 0 && index < bestIndex)
			{
				bestIndex = index;
				selectedSystem = it.second.GetSystem();
				center = Zoom() * (Point() - selectedSystem->Position());
				if(!index)
				{
					selectedPlanet = &it.second;
					return;
				}
			}
		}
}



double MapPanel::Zoom() const
{
	return pow(1.5, player.MapZoom());
}



// Check whether the NPC and waypoint conditions of the given mission have
// been satisfied.
bool MapPanel::IsSatisfied(const Mission &mission) const
{
	return IsSatisfied(player, mission);
}



bool MapPanel::IsSatisfied(const PlayerInfo &player, const Mission &mission)
{
	for(const NPC &npc : mission.NPCs())
		if(!npc.HasSucceeded(player.GetSystem()))
			return false;

	return mission.Waypoints().empty() && mission.Stopovers().empty();
}



int MapPanel::Search(const string &str, const string &sub)
{
	auto it = search(str.begin(), str.end(), sub.begin(), sub.end(),
		[](char a, char b) { return toupper(a) == toupper(b); });
	return (it == str.end() ? -1 : it - str.begin());
}

void MapPanel::RTSDrawPlayerTravelPlan(PlayerInfo &Player)
{
   // LogFile << "RTSDrawPlayerTravelPlan" << endl;
    RTSDrawTravelPlan(Player.ReturnSelectedShip());
    return;
}

void MapPanel::RTSDrawTravelPlan(std::shared_ptr <Ship> selectedShip)
{
   // LogFile << "RTSDrawTravelPlan" << endl;
	/*if(!playerSystem)
		return;*/

	Color defaultColor(.5, .4, 0., 0.);
	Color outOfFlagshipFuelRangeColor(.55, .1, .0, 0.);
	Color withinFleetFuelRangeColor(.2, .5, .0, 0.);
	Color wormholeColor(0.5, 0.2, 0.9, 1.);

	// At each point in the path, we'll keep track of how many ships in the
	// fleet are able to make it this far.

	/*Ship *flagship = player.Flagship();

	if(!flagship)
		return;
	bool stranded = false;
	bool hasEscort = false;
	map<const Ship *, double> fuel;
	for(const shared_ptr<Ship> &it : player.Ships())
		if(!it->IsParked() && !it->CanBeCarried() && it->GetSystem() == flagship->GetSystem())
		{
			if(it->IsDisabled())
			{
				stranded = true;
				continue;
			}

			fuel[it.get()] = it->Fuel() * it->Attributes().Get("fuel capacity");
			hasEscort |= (it.get() != flagship);
		}
	stranded |= !hasEscort;*/

	const System *previous = selectedShip->GetSystem(); //RTS: Get system of selectedShip


	//RTS code changed "Player.TravelPlan() to "selectedShip->TravelPlan". 2 times below.
	for(int i = selectedShip->TravelPlan().size() - 1; i >= 0; --i)
	{
		const System *next = selectedShip->TravelPlan()[i];
 //  LogFile << "Draw flight path to " << next->Name() << "\n";
		/*bool isHyper = previous->Links().count(next);
		bool isJump = !isHyper && previous->Neighbors().count(next);
		bool isWormhole = false;
		for(const StellarObject &object : previous->Objects())
			isWormhole |= (object.GetPlanet() && player.HasVisited(object.GetPlanet())
				&& player.HasVisited(previous) && player.HasVisited(next)
				&& object.GetPlanet()->WormholeDestination(previous) == next);

		if(!isHyper && !isJump && !isWormhole)
			break;

		// Wormholes cost nothing to grow through. If this is not a wormhole,
		// check how much fuel every ship will expend to go through it.
		if(!isWormhole)
			for(auto &it : fuel)
				if(it.second >= 0.)
				{
					double cost = isJump ? it.first->JumpDriveFuel() : it.first->HyperdriveFuel();
					if(!cost || cost > it.second)
					{
						it.second = -1.;
						stranded = true;
					}
					else
						it.second -= cost;
				}*/

		// Color the path green if all ships can make it. Color it yellow if
		// the flagship can make it, and red if the flagship cannot.
		Color drawColor = outOfFlagshipFuelRangeColor;

		//RTS Mode choose draw color of ship rout based off Government color.
		drawColor = selectedShip->GetGovernment()->GetColor();


		Point from = Zoom() * (next->Position() + center);
		Point to = Zoom() * (previous->Position() + center);
		Point unit = (from - to).Unit() * 7.;
		LineShader::Draw(from - unit, to + unit, 3., drawColor);

		previous = next;
	}
	//LogFile << "\n";
}

void MapPanel::DrawTravelPlan()
{
   Ship * selectedShip = nullptr;
	if(!playerSystem)
		return;

	Color defaultColor(.5, .4, 0., 0.);
	Color outOfFlagshipFuelRangeColor(.55, .1, .0, 0.);
	Color withinFleetFuelRangeColor(.2, .5, .0, 0.);
	Color wormholeColor(0.5, 0.2, 0.9, 1.);

	// At each point in the path, we'll keep track of how many ships in the
	// fleet are able to make it this far.

	Ship *flagship = player.Flagship();

	if(!flagship)
		return;
	bool stranded = false;
	bool hasEscort = false;
	map<const Ship *, double> fuel;
	for(const shared_ptr<Ship> &it : player.Ships())
		if(!it->IsParked() && !it->CanBeCarried() && it->GetSystem() == flagship->GetSystem())
		{
			if(it->IsDisabled())
			{
				stranded = true;
				continue;
			}

			fuel[it.get()] = it->Fuel() * it->Attributes().Get("fuel capacity");
			hasEscort |= (it.get() != flagship);
		}
	stranded |= !hasEscort;

	const System *previous = playerSystem;

	selectedShip = player.ReturnSelectedShip().get();

	//RTS code changed "Player.TravelPlan() to "selectedShip->TravelPlan". 2 times below.
	for(int i = selectedShip->TravelPlan().size() - 1; i >= 0; --i)
	{
		const System *next = selectedShip->TravelPlan()[i];



		bool isHyper = previous->Links().count(next);
		bool isJump = !isHyper && previous->Neighbors().count(next);
		bool isWormhole = false;
		for(const StellarObject &object : previous->Objects())
			isWormhole |= (object.GetPlanet() && player.HasVisited(object.GetPlanet())
				&& player.HasVisited(previous) && player.HasVisited(next)
				&& object.GetPlanet()->WormholeDestination(previous) == next);

		if(!isHyper && !isJump && !isWormhole)
			break;

		// Wormholes cost nothing to grow through. If this is not a wormhole,
		// check how much fuel every ship will expend to go through it.
		if(!isWormhole)
			for(auto &it : fuel)
				if(it.second >= 0.)
				{
					double cost = isJump ? it.first->JumpDriveFuel() : it.first->HyperdriveFuel();
					if(!cost || cost > it.second)
					{
						it.second = -1.;
						stranded = true;
					}
					else
						it.second -= cost;
				}

		// Color the path green if all ships can make it. Color it yellow if
		// the flagship can make it, and red if the flagship cannot.
		Color drawColor = outOfFlagshipFuelRangeColor;

		//RTS Mode choose draw color of ship rout based off Government color.
		drawColor = selectedShip->GetGovernment()->GetColor();

		//RTS mode Remove color of path based off fule and distance.
		/*if(isWormhole)
			drawColor = wormholeColor;
		else if(!stranded)
			drawColor = withinFleetFuelRangeColor;
		else if(fuel[flagship] >= 0.)
			drawColor = defaultColor;*/


		Point from = Zoom() * (next->Position() + center);
		Point to = Zoom() * (previous->Position() + center);
		Point unit = (from - to).Unit() * 7.;
		LineShader::Draw(from - unit, to + unit, 3., drawColor);

		previous = next;
	}
}



void MapPanel::DrawWormholes()
{
	/*
	Color wormholeColor(0.5, 0.2, 0.9, 1.);
	Color wormholeDimColor(0.5 / 3., 0.2 / 3., 0.9 / 3., 1.);
	const double wormholeWidth = 1.2;
	const double wormholeLength = 4.;
	const double wormholeArrowHeadRatio = .3;

	map<const System *, const System *> drawn;
	for(const auto &it : GameData::Systems())
	{
		const System *previous = &it.second;
		for(const StellarObject &object : previous->Objects())
			if(object.GetPlanet() && object.GetPlanet()->IsWormhole() && player.HasVisited(object.GetPlanet()))
			{
				const System *next = object.GetPlanet()->WormholeDestination(previous);
				// Only draw a wormhole if both systems have been visited.
				if(!player.HasVisited(previous) || !player.HasVisited(next))
					continue;

				drawn[previous] = next;
				Point from = Zoom() * (previous->Position() + center);
				Point to = Zoom() * (next->Position() + center);
				Point unit = (from - to).Unit() * 7.;
				from -= unit;
				to += unit;

				Angle left(45.);
				Angle right(-45.);

				Point wormholeUnit = Zoom() * wormholeLength * unit;
				Point arrowLeft = left.Rotate(wormholeUnit * wormholeArrowHeadRatio);
				Point arrowRight = right.Rotate(wormholeUnit * wormholeArrowHeadRatio);

				// Don't double-draw the links.
				if(drawn[next] != previous)
					LineShader::Draw(from, to, wormholeWidth, wormholeDimColor);
				LineShader::Draw(from - wormholeUnit + arrowLeft, from - wormholeUnit, wormholeWidth, wormholeColor);
				LineShader::Draw(from - wormholeUnit + arrowRight, from - wormholeUnit, wormholeWidth, wormholeColor);
				LineShader::Draw(from, from - (wormholeUnit + Zoom() * 0.1 * unit), wormholeWidth, wormholeColor);
			}
	}*/
}



void MapPanel::DrawLinks()
{
	// Draw the links between the systems.
	Color closeColor(.6, .6);
	Color farColor(.3, .3);
	for(const auto &it : GameData::Systems())
	{
		const System *system = &it.second;
		if(!player.HasSeen(system))
			continue;

		for(const System *link : system->Links())
			if(link < system || !player.HasSeen(link))
			{
				// Only draw links between two systems if one of the two is
				// visited. Also, avoid drawing twice by only drawing in the
				// direction of increasing pointer values.
				/*if(!player.HasVisited(system) && !player.HasVisited(link))
					continue;*/ // RTS mode allows all stars to have edges.

				Point from = Zoom() * (system->Position() + center);
				Point to = Zoom() * (link->Position() + center);
				Point unit = (from - to).Unit() * 7.;
				from -= unit;
				to += unit;

				bool isClose = (system == playerSystem || link == playerSystem);
				LineShader::Draw(from, to, 1.2, isClose ? closeColor : farColor);
			}
	}
}



void MapPanel::DrawSystems()
{
    const Sprite *smallPlanet = SpriteSet::Get("planet/lightPlanet");

	if(commodity == SHOW_GOVERNMENT)
		closeGovernments.clear();

	// Draw the circles for the systems, colored based on the selected criterion,
	// which may be government, services, or commodity prices.
	for(const auto &it : GameData::Systems())
	{
		const System &system = it.second;
		// Referring to a non-existent system in a mission can create a spurious
		// system record. Ignore those.
		if(system.Name().empty())
			continue;


		Point pos = Zoom() * (system.Position() + center);

		Color color = UninhabitedColor();

				const Government *gov = system.GetGovernment();
				color = GovernmentColor(gov);


		RingShader::Draw(pos, OUTER, INNER, color);

SpriteShader::Draw(smallPlanet, pos, .06f);

	}
}



void MapPanel::DrawNames()
{
	// Don't draw if too small.
	if(Zoom() <= 0.5)
		return;

	// Draw names for all systems you have visited.
	const Font &font = FontSet::Get((Zoom() > 2.0) ? 18 : 14);
	Color closeColor(.6, .6);
	Color farColor(.3, .3);
	Point offset((Zoom() > 2.0) ? 8. : 6., -.5 * font.Height());
	for(const auto &it : GameData::Systems())
	{
		const System &system = it.second;
		if(!player.KnowsName(&system) || system.Name().empty())
			continue;

		font.Draw(system.Name(), Zoom() * (system.Position() + center) + offset,
			(&system == playerSystem) ? closeColor : farColor);
	}
}



void MapPanel::DrawMissions()
{
	// Draw a pointer for each active or available mission.
	/*map<const System *, Angle> angle;
	Color black(0., 1.);
	Color white(1., 1.);

	const Set<Color> &colors = GameData::Colors();
	const Color &availableColor = *colors.Get("available job");
	const Color &unavailableColor = *colors.Get("unavailable job");
	const Color &currentColor = *colors.Get("active mission");
	const Color &blockedColor = *colors.Get("blocked mission");
	const Color &waypointColor = *colors.Get("waypoint");
	for(const Mission &mission : player.AvailableJobs())
	{
		const System *system = mission.Destination()->GetSystem();
		DrawPointer(system, angle[system], mission.HasSpace(player) ? availableColor : unavailableColor);
	}
	++step;
	for(const Mission &mission : player.Missions())
	{
		if(!mission.IsVisible())
			continue;

		const System *system = mission.Destination()->GetSystem();
		bool blink = false;
		if(mission.Deadline())
		{
			int days = min(5, mission.Deadline() - player.GetDate()) + 1;
			if(days > 0)
				blink = (step % (10 * days) > 5 * days);
		}
		bool isSatisfied = IsSatisfied(player, mission);
		DrawPointer(system, angle[system], blink ? black : isSatisfied ? currentColor : blockedColor, isSatisfied);

		for(const System *waypoint : mission.Waypoints())
			DrawPointer(waypoint, angle[waypoint], waypointColor);
		for(const Planet *stopover : mission.Stopovers())
			DrawPointer(stopover->GetSystem(), angle[stopover->GetSystem()], waypointColor);
	}
	if(specialSystem)
	{
		// The special system pointer is larger than the others.
		Angle a = (angle[specialSystem] += Angle(30.));
		Point pos = Zoom() * (specialSystem->Position() + center);
		PointerShader::Draw(pos, a.Unit(), 20., 27., -4., black);
		PointerShader::Draw(pos, a.Unit(), 11.5, 21.5, -6., white);
	}*/
}

PlayerInfo *ReturnPlayerInfoFromGovernment(string nameSearchingGovName)
{
    for(int n=1; n < 9; n++)
        {
            if(PlayerInfoRTS[n].GetGovernment()->GetName() == nameSearchingGovName)
            {

                LogFile << PlayerInfoRTS[n].GetGovernment()->GetName() << " found!" << endl;
                return &PlayerInfoRTS[n];
            }

        }
    LogFile << "PlayerInfo not found! Returning nullptr!" << endl;
    return nullptr;
}

void MapPanel::ArriveAtStar(Ship *selectedShip, System *arriveSystem)
{
    if(!selectedShip)
        return;

    selectedShip->SetSystem(arriveSystem);

    selectedShip->PopTravel();
    selectedShip->ResetDistanceFromStar();

    if(selectedShip->TravelPlan().empty() == true)
       {
           selectedShip->SetIsParked(); //Ship landed

          if(arriveSystem->GetGovernment() == selectedShip->GetGovernment())
           {
               //LandShips (System *)
               LandShips(selectedShip, arriveSystem);
               LogFile << "Ships for " <<arriveSystem->GetGovernment()->GetName() <<" landed on " << arriveSystem->Name() << " safely." << endl;
               return;
           }
           else
           {
               RTSBattle(arriveSystem, selectedShip);
               LogFile << "Battle ended successfully!" << endl;
               return;
           }


       }
    else
    {
    FindDistanceToStar(selectedShip);
    }
}

void MapPanel::LandShips(Ship * landingShip, System * landingSystem)
{
    landingSystem->AddFigs(landingShip->GetNumFigs());
    LogFile << "Landing system now has " <<landingSystem->GetNumFigs() << endl;
    for(int n=1; n == landingShip->GetNumComShips(); n++ )
        landingSystem->AddComShip();

    if(landingShip->MotherShipPresent() == 1)
        {landingSystem->SetMotherShipPresent();
        LogFile << "MotherShip is now present on " << landingSystem->MotherShipPresent();
        }
            landingShip->SetIsParked();

return;
}



 bool operator==( Ship& lhs, Ship& rhs)
{
  if(lhs.Name() == rhs.Name() && lhs.GetGovernment() == rhs.GetGovernment() && lhs.GetSystem() == rhs.GetSystem() && lhs.GetNumFigs() == rhs.GetNumFigs())
    return true;
  return false;
}

void MapPanel::RTSBattle(System *disputedSystem, Ship *selectedShip)
{

    PlayerInfo *Player;
    const int ATTACKINGFORCECOMSHIP = 13, ATTACKINGFORCEMOTHERSHIP = 23, NUMFIGSREQUIRED = 5;
    bool allowCapitalShipsToFightAtSystem =0, allowCapitalShipsToFightInShip =0;
    int attackingForce, defendingForce;
    LogFile << "Battle at: " << disputedSystem->Name() << endl << "Between " << disputedSystem->GetGovernment()->GetName() << " and " << selectedShip->GetGovernment()->GetName() <<endl;

   int supportedCapitalShips = disputedSystem->GetNumFigs()/NUMFIGSREQUIRED;
      if( supportedCapitalShips >= disputedSystem->GetNumComShips()+disputedSystem->MotherShipPresent())
        defendingForce = disputedSystem->GetNumFigs()+disputedSystem->GetNumComShips()*ATTACKINGFORCECOMSHIP + disputedSystem->MotherShipPresent()*ATTACKINGFORCEMOTHERSHIP;
    else
        defendingForce = disputedSystem->GetNumFigs();

    supportedCapitalShips = selectedShip->GetNumFigs()/NUMFIGSREQUIRED;
        if( supportedCapitalShips >= selectedShip->GetNumComShips()+selectedShip->MotherShipPresent())
           attackingForce = selectedShip->GetNumFigs()+selectedShip->GetNumComShips()*ATTACKINGFORCECOMSHIP+selectedShip->MotherShipPresent()*ATTACKINGFORCEMOTHERSHIP;
        else
            attackingForce = selectedShip->GetNumFigs();

   LogFile << "Defending force: " << defendingForce << endl << "Attacking Force: " << attackingForce << endl;


   if(attackingForce <= defendingForce)
   {
       selectedShip->MinusFigs(selectedShip->GetNumFigs());
       selectedShip->MinusComShips(selectedShip->GetNumComShips());

       Player = ReturnPlayerInfoFromGovernment(selectedShip->GetGovernment()->GetName());

       if(Player->Flagship() == selectedShip)
            {
                Player->Die(true);
               LogFile << Player->GetGovernment()->GetName() << " Destroyed!" << endl;
            }

       //entire attack fleet destroyed!
       //Defenders weakened
       LogFile << "Attackers destroyed! " << "Defenders now have " << disputedSystem->GetNumFigs() << " figs left." << endl;
   }

   else
   {
       //entire defence fleet destroyed!
       //Attacker fleet weakened.
       Player = ReturnPlayerInfoFromGovernment(disputedSystem->GetGovernment()->GetName());
       disputedSystem->MinusFigs(disputedSystem->GetNumFigs());
       disputedSystem->MinusComShips(disputedSystem->GetNumComShips());
       if(disputedSystem == Player->Flagship()->GetSystem())
         {Player->Die(true);
         LogFile << Player->GetGovernment()->GetName();
         }

       disputedSystem->SetGovernment(selectedShip->GetGovernment());

       LogFile << "Defenders Destroyed! " << " Attackers now have " << selectedShip->GetNumFigs() << " figs left." << endl;
       LandShips(selectedShip, disputedSystem);
   }

   LogFile << endl;

}
void MapPanel::FindDistanceToStar(Ship *selectedShip)
{
        float distance = sqrt(pow(selectedShip->TravelPlan().back()->Position().X() - selectedShip->GetSystem()->Position().X(),2) + pow(selectedShip->TravelPlan().back()->Position().Y()-selectedShip->GetSystem()->Position().Y(), 2));
        selectedShip->SetDistanceToStar(distance);
        return;
}

void MapPanel::CalculatePositionOfShips(PlayerInfo &Player)
{
if(Player.IsDead() == true)
    return;
    Point toStar;
    Point shipPosition;
    Angle shipAngle = 0;


    //LogFile << "CalculatePosition Of Ships for: " << Player.GetGovernment()->GetName() << endl;
    for(shared_ptr<Ship> selectedShip : Player.Ships())
        {
            if(!selectedShip)
                continue;

            if(selectedShip->TravelPlan().empty() == true)
                {toStar = selectedShip->GetSystem()->Position();
              // LogFile << "No travel plan!" << endl;
                 shipAngle = 0;
                 selectedShip->SetDistanceToStar(0); //If no Ship has no TravelPlan, reset distance to nextStar
                selectedShip->Place(Zoom() * (selectedShip->GetSystem()->Position()+center), shipAngle.Unit(),  shipAngle );
                }
            else
                {
                toStar = selectedShip->TravelPlan().back()->Position();
                Point fromStar(selectedShip->GetSystem()->Position());
                selectedShip->AddDistanceFromStar(.1);
             //  LogFile << "Pointing from " << selectedShip->GetSystem()->Name() << " X: " << selectedShip->GetSystem()->Position().X() << " Y: " << selectedShip->GetSystem()->Position().Y() << " to " << selectedShip->TravelPlan().back()->Name() << " X: " <<
//                selectedShip->TravelPlan().back()->Position().X() << " Y: " << selectedShip->TravelPlan().back()->Position().Y() << endl; */

                double degrees = (atan2(-1*(fromStar.Y() - toStar.Y()),(toStar.X() - fromStar.X())));
                shipAngle = 180/M_PI*degrees+90;

              //  LogFile << "Angle found to be " << shipAngle.Degrees() << " DistanceFromStar: " << selectedShip->GetDistanceFromStar() << endl;
                shipPosition.Set((selectedShip->GetDistanceFromStar()*cos(degrees)), selectedShip->GetDistanceFromStar()*sin(degrees));
              //  LogFile << "Ship position changed by: X: " << shipPosition.X() << " Y: " << shipPosition.Y() << endl;
                selectedShip->Place(Zoom() * (selectedShip->GetSystem()->Position()+shipPosition+center), shipAngle.Unit(),  shipAngle );

                if(selectedShip->GetDistanceToStar() == 0) //If ship just recieved a TravelPlan calculate distance to first star
                    FindDistanceToStar(selectedShip.get());

                if(selectedShip->GetDistanceToStar() < selectedShip->GetDistanceFromStar() )
                    ArriveAtStar(selectedShip.get(), GameData::Systems().Get(selectedShip->TravelPlan().back()->Name()));
                }

        }
    //   LogFile << endl;

}

void MapPanel::DrawSelectedShips()
{
    DrawList draw;
    shared_ptr<Ship> selectedShip;
Ship * deleteShip;


  //  LogFile << "DrawSelectedShips()" << endl;

    for(int n=1; n < 9; n++)
        {
            if(PlayerInfoRTS[n].IsDead() == true)
            continue;
     //   LogFile << "Drawing ReturnSelectedShip(): " << n << endl;
        selectedShip = PlayerInfoRTS[n].ReturnSelectedShip();
        RingShader::Draw(selectedShip->Position(), 34., 29., selectedShip->GetGovernment()->GetColor());
        if(selectedShip->Name() == "MotherShip" )
            {draw.Add(*selectedShip, -50);

            }
        else //if(selectedShip->Mass() < 500)
             draw.Add(*selectedShip, -25);
        }
    draw.Draw();
 //   LogFile << endl;
}

void MapPanel::DrawPointer(const System *system, Angle &angle, const Color &color, bool bigger)
{
	//DrawPointer(Zoom() * (system->Position() + center), angle, color, true, bigger);
}



void MapPanel::DrawPointer(Point position, Angle &angle, const Color &color, bool drawBack, bool bigger)
{
/*	static const Color black(0., 1.);

	angle += Angle(30.);
	if(drawBack)
		PointerShader::Draw(position, angle.Unit(), 14. + bigger, 19. + 2 * bigger, -4., black);
	PointerShader::Draw(position, angle.Unit(), 8. + bigger, 15. + 2 * bigger, -6., color);*/
}
