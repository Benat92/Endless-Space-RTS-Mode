
class GameControllerBottonData
{
    public:
    bool A;
    bool B;
    bool X;
    bool Y;
    bool rightButton;
    bool leftButton;

    int GameControllerRefresh(int complete);

};



SDL_GameControllerGetButton(GameController1, SDL_CONTROLLER_BUTTON_A);










int GameControllerButtonData::GameControllerRefresh(int complete)
{






    SDL_Event e;
    SDL_ControllerButtonEvent b;
    SDL_GameControllerButton button;
    //LogFile << "Controller Refresh!\n";




    while(SDL_PollEvent(&e) !=0) //Loop ends when all SDL events are handled.
{


if(e.type == SDL_CONTROLLERBUTTONDOWN)
{


    if(e.type == SDL_CONTROLLER_BUTTON_A)
    {
    LogFile << "Button A pressed!";

    }

}
}

    return 1;
    }
