#pragma once
#include <string>
#include <vector>
#include "params.h"


class TW_Static {
	public:
		int W;
};

class TW_RunStatic {
	public:
		double startTheta;
		double startThetad;
		double startX;
		double startXd;

		double mbody; // mass of body
		double height; // height of body
		double width;
		double depth;
		double muf; // coefficient of friction

		double max_sim_time;
		double g; // gravity
		double mtotal; // total mass
		double steps_per_push; // how many steps to apply each push on the wheels

		double tfail; // failing theta value
		double xfail; //failing x value
		double dt; // time step to use per move

};

class TW_RunDynamic {
	public:
		class TiltState {
			public:
				double theta;
				double thetad;
				double thetadd;
		};

		class TWState {
			public:
				double x;
				double xd;
				double xdd;
				std::vector<TiltState> tilts;
				enum PlayerState { ALIVE, X_FAIL, THETA_FAIL, TIME };
				PlayerState playerState;
				double lastMove;
		};

		double currentTime;
		int currentGame;
		TWState currentState;
		double lastMove;

		double friction; // friction force
		double force; // magnitude of force on the wheels

		NeuroUtils::Params currentParams;
		TWState::PlayerState playerState;
};

class TWAE {
	public:
		TWAE();
		~TWAE();
		void* NewRun();
		void GetStaticRunInfo(void* run, TW_RunStatic *s) const;
		void GetDynamicRunInfo(void* run, TW_RunDynamic *s) const;
		bool UpdateState(void* run, double dir) const; // dir is -1 for left; 0 for none; 1 for right
		void DeleteRun(void* run) const;

	protected:
		void *state;	
};

std::string TW_RunDynamic_To_String (TW_RunDynamic *r);
bool TW_String_ToRunDynamic (std::string &s, TW_RunDynamic *r);

// For Later
/*

		bool ApplyParams(void* run, NeuroUtils::Params &params) const; //?
		NeuroUtils::Params& GetParams(void* run) const; //?
		bool StartNextGame(void* run) const;
*/
