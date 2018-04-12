#include "RTSPlayer.h"
#include "Ship.h"
#include "ShipEvent.h"


#include "StartConditions.h"
#include "StellarObject.h"
#include "System.h"
#include "UI.h"

#include <algorithm>
#include <ctime>
#include <sstream>

//RTSPlayer






// Get a pointer to the ship that the player controls. This is always the first
// ship in the list.
const Ship *RTSPlayers::Flagship() const
{
	return const_cast<RTSPlayers *>(this)->FlagshipPtr().get();
}



// Get a pointer to the ship that the player controls. This is always the first
// ship in the list.
Ship *RTSPlayers::Flagship()
{
	return FlagshipPtr().get();
}

const shared_ptr<Ship> &RTSPlayers::FlagshipPtr()
{
	if(!flagship)
	{
		for(const shared_ptr<Ship> &it : ships)
			if(!it->IsParked() && it->GetSystem() == system && it->CanBeFlagship())
			{
				flagship = it;
				break;
			}
	}

	static const shared_ptr<Ship> empty;
	return (flagship && flagship->IsYours()) ? flagship : empty;
}



// Access the full list of ships that the player owns.
const vector<shared_ptr<Ship>> &RTSPlayers::Ships() const
{
	return ships;
}



// Add a captured ship to your fleet.
void RTSPlayers::AddShip(const shared_ptr<Ship> &ship)
{
	ships.push_back(ship);
	ship->SetIsSpecial();
	ship->SetIsYours();
}



// Buy a ship of the given model, and give it the given name.
void RTSPlayers::BuyShip(const Ship *model, const string &name)
{
	int day = date.DaysSinceEpoch();
	int64_t cost = stockDepreciation.Value(*model, day);
	if(model && accounts.Credits() >= cost)
	{
		ships.push_back(shared_ptr<Ship>(new Ship(*model)));
		ships.back()->SetName(name);
		ships.back()->SetSystem(system);
		ships.back()->SetPlanet(planet);
		ships.back()->SetIsSpecial();
		ships.back()->SetIsYours();
		ships.back()->SetGovernment(GameData::PlayerGovernment());

		accounts.AddCredits(-cost);
		flagship.reset();

		// Record the transfer of this ship in the depreciation and stock info.
		depreciation.Buy(*model, day, &stockDepreciation);
		for(const auto &it : model->Outfits())
			stock[it.first] -= it.second;
	}
}


// Buy a ship of the given model, and give it the given name.
void PlayerInfo::BuyShip(const Ship *model, const string &name)
{
	int day = date.DaysSinceEpoch();
	int64_t cost = stockDepreciation.Value(*model, day);
	if(model && accounts.Credits() >= cost)
	{
		ships.push_back(shared_ptr<Ship>(new Ship(*model)));
		ships.back()->SetName(name);
		ships.back()->SetSystem(system);
		ships.back()->SetPlanet(planet);
		ships.back()->SetIsSpecial();
		ships.back()->SetIsYours();
		ships.back()->SetGovernment(GameData::PlayerGovernment());

		accounts.AddCredits(-cost);
		flagship.reset();

		// Record the transfer of this ship in the depreciation and stock info.
		depreciation.Buy(*model, day, &stockDepreciation);
		for(const auto &it : model->Outfits())
			stock[it.first] -= it.second;
	}
}
