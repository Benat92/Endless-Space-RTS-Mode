#include "Planet.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>




class RTSPlayers
{


// Access the player's travel plan.
	bool HasTravelPlan() const;
	const std::vector<const System *> &TravelPlan() const;
	std::vector<const System *> &TravelPlan();
	// Remove the first or last system from the travel plan.
	void PopTravel();
	// Get or set the planet to land on at the end of the travel path.
	const Planet *TravelDestination() const;
	void SetTravelDestination(const Planet *planet);


	// Access the flagship (the first ship in the list). This returns null if
	// the player does not have any ships.
	const Ship *Flagship() const;
	Ship *Flagship();
	const std::shared_ptr<Ship> &FlagshipPtr();
	// Get the full list of ships the player owns.
	const std::vector<std::shared_ptr<Ship>> &Ships() const;
	// Add a captured ship to your fleet.
	void AddShip(const std::shared_ptr<Ship> &ship);


private:

    uint8_t playerNum;
    bool isDead = false;
    std::shared_ptr<Ship> flagship;
	std::vector<std::shared_ptr<Ship>> ships;
};
