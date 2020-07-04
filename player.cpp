#include"player.h"
#include"actor.h"
#include"stack.h"
#include"list.h"
#include"point.h"
#include"aquarium.h"
#include<iostream>

//  Player( ... )
//		Constructs and initializes the Player/Actor and its member variables
//		Remembers and discovers the starting point.
Player::Player(Aquarium* aquarium, Point p, std::string name, char sprite)
	:Actor(aquarium, p, name, sprite),
	m_look(),
	m_discovered(),
	m_btQueue(),
	m_btStack(),
	m_toggleBackTracking(false) {

	// Discover the starting point
	m_discovered.push_front(p);
	m_look.push(p);
}

//  stuck() 
//		See if the player is stuck in the maze (no solution)
bool Player::stuck() const {
	return Actor::getState() == State::STUCK;
}

//  foundExit()
//		See if the player has found the exit
bool Player::foundExit() const {
	return Actor::getState() == State::FREEDOM;
}
//  toggleBackTrack( ... )
//		Turn on/off backtracking
void Player::toggleBackTrack(bool toggle) {
	m_toggleBackTracking = toggle;
}

//  getTargetPoint()
//		Get the point the player wants to look around next.
//      If m_look is empty then return an invalid point.
Point Player::getTargetPoint() const {
	if (m_look.empty()) return Point(-1, -1);
	return m_look.peek();
}

// discovered
//		returns true if the item is in the list
bool Player::discovered(const Point &p) const {
	return (m_discovered.find(p) != -1);
}



// say()
//		What does the player say?
//		Already Implemented, nothing to do here
void Player::say() {

	// Freedom supercedes being eaten
	if (getState() == State::FREEDOM) {
		std::cout << getName() << ": WEEEEEEEEE!";
		return;
	} 
 
	// Being eaten supercedes being lost
	switch (getInteract()) {
	case Interact::ATTACK:
		std::cout << getName() << ": OUCH!";
		break;
	case Interact::GREET:
		break;
	case Interact::ALONE:
	default:
		switch (getState()) {
		case State::LOOKING:
			std::cout << getName() << ": Where is the exit?";
			break;
		case State::STUCK:
			std::cout << getName() << ": Oh no! I am Trapped!";
			break;
		case State::BACKTRACK:
			std::cout << getName() << ": Got to backtrack...";
			break;
		default:
			break;
		}

		break;
	}
}


//  update() 
//		This function implements an algorithm to look through the maze
//      for places to move (STATE::LOOKING). update also handles moving the 
//		player as well, if there is an open undiscovered cell to move to the  
//		player just moves there.  However, there will be cases in the next 
//		desired point to look around is not adjacent (more than one cell away), 
//		at this point the player must backtrack (State::BACKTRACK) to a point 
//		that is adjacent to the next point the player was planning to visit
//
//		The player can only do one thing each call, they can either LOOK or 
//		BACKTRACK not both.  Nor should the player move more than one cell per
//		call to update.  If you examine the function calls for the entire game
//		you should observe that this is already be called within a loop. Your
//		Implementation should NOT have any loops that pertain to actual movement 
//		of the player.  
//
//		Backtracking is challenging, save it for the very very very last thing.
//		Make sure the STATE::LOOKING aspect compiles and works first.
void Player::update() {
	setState(State::LOOKING);
	//Check if stuck
	if (m_look.empty() == true) 
		setState(State::STUCK);
	
	
	//Create temporary points for each if statement (I swear there should be a more efficient way of doing this but alas my brain is not there yet)
	Point p(getTargetPoint());
	Point t1(p.getX() - 1, p.getY());
	Point t2(p.getX() + 1, p.getY());
	Point t3(p.getX(), p.getY() + 1);
	Point t4(p.getX(), p.getY() - 1);
	setPosition(p);

	
	m_look.pop();

	//Check if on exit and set state to freedom if so
	if (getPosition() == getAquarium()->getEndPoint()) {
		setState(State::FREEDOM);
		say();
	}
	
	//Check each direction -> WENS
	//Discovers each direction and adds it to m_look and m_btStack
	if (discovered(t1) != true && getAquarium()->isCellOpen(t1) == true) {
		m_discovered.push_front(t1);
		m_look.push(t1);
		m_btStack.push(t1);
	}
	if (discovered(t2) != true && getAquarium()->isCellOpen(t2) == true) {
		m_discovered.push_front(t2);
		m_look.push(t2);
		m_btStack.push(t2);
	}
	if (discovered(t3) != true && getAquarium()->isCellOpen(t3) == true) {
		m_discovered.push_front(t3);
		m_look.push(t3);
		m_btStack.push(t3);
	}
	if (discovered(t4) != true && getAquarium()->isCellOpen(t4) == true) {
		m_discovered.push_front(t4);
		m_look.push(t4);
		m_btStack.push(t4);
	}
	
	// Set by the settings file and checked here
	if (m_toggleBackTracking) {

		//Checks the top of m_btStack with all four directions to see if any were added and it is not the endpoint; if none were added, means we have to backtrack
		//Pops m_btStack and pushes the top of m_btStack onto m_look. Also sets state to backtrack
		if (m_btStack.peek() != t1 && m_btStack.peek() != t2 && m_btStack.peek() != t3 && m_btStack.peek() != t4 && getPosition() != getAquarium()->getEndPoint()) {
			m_btStack.pop();
			m_look.push(m_btStack.peek());
			setState(State::BACKTRACK);
		}
	}
}