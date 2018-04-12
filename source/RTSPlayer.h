#include "Planet.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>



class Ship;



class RTSPlayers
{
    public:
    Ship *selectedShip = nullptr; //RTS mode selected ship

    uint16_t GetNumSendFigs(){return numSendFigs;};
    void SetNumSendFigs(uint16_t &numSendFigs){return;};
    uint16_t GetNumSendComs(){return numSendComShips;};
    void SetNumSendComs(uint16_t &numSendComShips) {return;};

    bool IsMotherShipSend(){return motherShipSend;};
    void SetMotherShipSend(bool &motherShipSend){return;};



	// Access the flagship (the first ship in the list). This returns null if
	// the player does not have any ships.
	const Ship *Flagship() const;
	Ship *Flagship();
	const std::shared_ptr<Ship> &FlagshipPtr();
	// Get the full list of ships the player owns.
	const std::vector<std::shared_ptr<Ship>> &Ships() const;
	// Add a captured ship to your fleet.
	void AddShip(const std::shared_ptr<Ship> &ship);

	// Check if the player has a hyperspace route set.
bool HasTravelPlan() const
{
	return !travelPlan.empty();
};



// Access the player's travel plan.
const std::vector<const System *> &TravelPlan() const
{
	return travelPlan;
};



std::vector<const System *> &TravelPlan()
{
	return travelPlan;
};



// This is called when the player enters the system that is their current
// hyperspace target.
void PopTravel()
{
	if(!travelPlan.empty())
	{
		travelPlan.pop_back();
	}
};

private:

//RTS Variables
    uint16_t numSendFigs = 0;
    uint16_t numSendComShips = 0;
    bool motherShipSend = false;

	//RTS mode code
	  std::vector<const System *> travelPlan;

    uint8_t playerNum;
    bool isDead = false;
    std::shared_ptr<Ship> flagship;
	std::vector<std::shared_ptr<Ship>> ships;
};
