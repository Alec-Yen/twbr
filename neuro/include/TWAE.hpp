#pragma once
#include <string>
#include <vector>
#include "utils/params.h"

/*
	 PBAE
	 May 17, 2016
	 Runs the pole balancing game, in simulated time. 
 */

class PBAE {
	public:
		static const std::string PARAMS_STEM;
		class PoleState {
			public:
				double theta;
				double thetad;
		};

		class PBState {
			public:
				double x;
				double xd;
				std::vector<PoleState> poles;

				enum PlayerState { ALIVE, X_FAIL, THETA_FAIL, TIME };
				PlayerState playerState;
				double lastMove;

				/* ParseState():
					 This parses the given state string.*/
				bool Parse(const std::string &stateString);
				std::string Serialize();
		};

		/* The constructor sets up all the test cases that the engine will run. */
		PBAE();

		/* NewRun() returns a void* filled with information about the run. You
			 should use that void* with other PBAE calls and delete it with
			 DeleteRun(). */
		void* NewRun() const;

		/* GetState() fills in the given PBState representing the physical state of
			 the system. */
		void GetState(void* run, PBState &state) const;

		/* ApplyParams() allows you to change params after creating a run.
			 It returns false if any params in the given Params object
			 are invalid. This resets the current game, if a game has been started. */
		bool ApplyParams(void* run, NeuroUtils::Params &params) const;

		/* GetParams() returns a reference to the current params for the given
			 run. You can use this to access a parameter you want or to print
			 out the params. TODO: this might not explicitly give access to the second pole's params. */
		NeuroUtils::Params& GetParams(void* run) const;


		/* StartNextGame() returns false if there are no more test cases.
			 Otherwise, it applies the parameters for the next game. */
		bool StartNextGame(void* run) const;

		/* UpdateState() advances the game forward one step, applying the given
			 a force to the cart in the direction specified, which can be:
			 -1: push left
0: apply no force
1: push right
It returns false if the game is over, and true otherwise.
		 */
		bool UpdateState(void* run, double dir) const; // dir is -1 for left; 0 for none; 1 for right

		/* DeleteRun() frees the memory for the given run. */
		void DeleteRun(void* run) const;

	protected:
		std::vector<NeuroUtils::Params> testCases;
};

