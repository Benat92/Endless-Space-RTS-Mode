/* AsteroidField.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "AsteroidField.h"

#include "SpriteSet.h"
#include "DrawList.h"
#include "Mask.h"
#include "Projectile.h"
#include "Random.h"
#include "Screen.h"

#include <cmath>
#include <cstdlib>

using namespace std;

namespace {
	static const double WRAP = 4096.;
}



// Clear the list of asteroids.
void AsteroidField::Clear()
{
	asteroids.clear();
	minables.clear();
}



// Add a new asteroid to the list, using the sprite with the given name.
void AsteroidField::Add(const string &name, int count, double energy)
{
	const Sprite *sprite = SpriteSet::Get("asteroid/" + name + "/spin");
	for(int i = 0; i < count; ++i)
		asteroids.emplace_back(sprite, energy);
}



void AsteroidField::Add(const Minable *minable, int count, double energy, double beltRadius)
{
	// Double check that the given asteroid is defined.
	if(!minable || !minable->GetMask().IsLoaded())
		return;
	
	// Place copies of the given minable asteroid throughout the system.
	for(int i = 0; i < count; ++i)
	{
		minables.emplace_back(new Minable(*minable));
		minables.back()->Place(energy, beltRadius);
	}
}



// Move all the asteroids forward one step.
void AsteroidField::Step(list<Effect> &effects, list<shared_ptr<Flotsam>> &flotsam)
{
	for(Asteroid &asteroid : asteroids)
		asteroid.Step();
	
	// Step through the minables. Since they are destructible, we may need to
	// remove them from the list.
	auto it = minables.begin();
	while(it != minables.end())
	{
		if((*it)->Move(effects, flotsam))
			++it;
		else
			it = minables.erase(it);
	}
}



// Draw the asteroids, centered on the given location.
void AsteroidField::Draw(DrawList &draw, const Point &center, double zoom) const
{
	for(const Asteroid &asteroid : asteroids)
		asteroid.Draw(draw, center, zoom);
	for(const shared_ptr<Minable> &minable : minables)
		draw.Add(*minable);
}



// Check if the given projectile collides with any asteroids.
double AsteroidField::Collide(const Projectile &projectile, int step, double closestHit, Point *hitVelocity)
{
	// First, check for collisions with ordinary asteroids.
	for(const Asteroid &asteroid : asteroids)
	{
		double thisDistance = asteroid.Collide(projectile, step);
		if(thisDistance < closestHit)
		{
			closestHit = thisDistance;
			if(hitVelocity)
				*hitVelocity = asteroid.Velocity();
		}
	}
	// Now, check for collisions with minable asteroids. Because this is the
	// very last collision check to be done, if a minable asteroid is the
	// closest hit, it really is what the projectile struck - that is, we are
	// not going to later find a ship or something else that is closer.
	Minable *hit = nullptr;
	for(const shared_ptr<Minable> &minable : minables)
	{
		double thisDistance = minable->Collide(projectile, step);
		if(thisDistance < closestHit)
		{
			closestHit = thisDistance;
			hit = &*minable;
			if(hitVelocity)
				*hitVelocity = minable->Velocity();
		}
	}
	if(hit)
		hit->TakeDamage(projectile);
	
	return closestHit;
}



// Get the list of mainable asteroids.
const list<shared_ptr<Minable>> &AsteroidField::Minables() const
{
	return minables;
}



// Construct an asteroid with the given sprite and "energy level."
AsteroidField::Asteroid::Asteroid(const Sprite *sprite, double energy)
{
	// Energy level determines how fast the asteroid rotates.
	SetSprite(sprite);
	SetFrameRate(Random::Real() * 4. * energy + 5.);
	
	// Pick a random position within the wrapped square.
	position = Point(Random::Real() * WRAP, Random::Real() * WRAP);
	
	// In addition to the "spin" inherent in the animation, the asteroid should
	// spin in screen coordinates. This makes the animation more interesting
	// because every time it comes back to the same frame it is pointing in a
	// new direction, so the asteroids don't all appear to be spinning on
	// exactly the same axis.
	angle = Angle::Random();
	spin = Angle::Random(energy) - Angle::Random(energy);
	
	// The asteroid's velocity is also determined by the energy level.
	velocity = angle.Unit() * Random::Real() * energy;
	
	// Store how big an area the asteroid can cover, so we can figure out when
	// it is potentially on screen.
	size = Point(1., 1.) * Radius();
}



// Move the asteroid forward one time step.
void AsteroidField::Asteroid::Step()
{
	angle += spin;
	position += velocity;
	
	// Keep the position within the wrap square.
	if(position.X() < 0.)
		position = Point(position.X() + WRAP, position.Y());
	else if(position.X() >= WRAP)
		position = Point(position.X() - WRAP, position.Y());
	
	if(position.Y() < 0.)
		position = Point(position.X(), position.Y() + WRAP);
	else if(position.Y() >= WRAP)
		position = Point(position.X(), position.Y() - WRAP);
}



// Draw any instances of this asteroid that are on screen.
void AsteroidField::Asteroid::Draw(DrawList &draw, const Point &center, double zoom) const
{
	// Any object within this range must be drawn.
	Point topLeft = center + (Screen::TopLeft() - size) / zoom;
	Point bottomRight = center + (Screen::BottomRight() + size) / zoom;
	
	// Figure out the position of the first instance of this asteroid that is to
	// the right of and below the top left corner of the screen.
	double startX = fmod(position.X() - topLeft.X(), WRAP);
	startX += topLeft.X() + WRAP * (startX < 0.);
	double startY = fmod(position.Y() - topLeft.Y(), WRAP);
	startY += topLeft.Y() + WRAP * (startY < 0.);
	
	// Draw any instances of this asteroid that are on screen.
	for(double y = startY; y < bottomRight.Y(); y += WRAP)
		for(double x = startX; x < bottomRight.X(); x += WRAP)
			draw.Add(*this, Point(x, y));
}



// Check if the given projectile collides with this asteroid. If so, a value
// less than 1 is returned indicating how far along its path the collision occurs.
double AsteroidField::Asteroid::Collide(const Projectile &projectile, int step) const
{
	// Note: this only checks the instance of the asteroid that is closest to
	// the projectile. If a projectile has a range longer than 4096 pixels, it
	// may "pass through" asteroids near the end of its range.
	
	// Find the asteroid instance closest to the center of the projectile,
	// i.e. where it will be when halfway through its path.
	Point halfVelocity = .5 * projectile.Velocity();
	Point pos = position - (projectile.Position() + halfVelocity);
	pos = Point(-remainder(pos.X(), WRAP), -remainder(pos.Y(), WRAP));
	
	return GetMask(step).Collide(pos - halfVelocity, projectile.Velocity(), angle);
}
