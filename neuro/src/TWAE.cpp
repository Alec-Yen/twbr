#include "TWAE.hpp"
#include "utils/params.h"
#include <fstream>
#include <cmath>
#include <sstream>
#include <map>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;
using namespace NeuroUtils;
const string PBAE::PARAMS_STEM = "polebalance";

class PBAERun {
	public:

		class PoleState {
			public:
				double theta;    //angular position of pole (radians)
				double thetad;   //angular speed of pole  (radians/second)
				double thetadd;  // angular acceleration of pole (radians/second/second)
		};

		class PBState {
			public:
				double x;     // x position of cart
				double xd;    // x speed of cart (x derivative)
				double xdd;   // x acceleration of cart
				std::vector<PoleState> poles;
				void Step(double tau);
		};

		class Pole {
			public:
				double startTheta;
				double startThetad;
				double mass;     // mass of the pole (kg)
				double ballMass; // mass of the ball on the end of the pole (kg)
				double length;   // length of the pole (meters)
				double muf;      // coefficient of friction
		};

		PBAERun();
		map<string, double*> doubleParams;

		int max_sim_time;
		double g;  // gravity
		double mc; // mass of cart in kg
		double cart_friction; // magnitude of force of friction on cart
		double steps_per_push; // how many steps to apply each push on the cart

		double tfail; // Failing theta value
		double xfail; // Failing x value
		double force; // Magnitude of force on cart
		double dt;    // Time step to use per move (in seconds)

		//Initial values
		double startX;
		double startXd;

		Pole defaultPole;
		vector <Pole> poles;
		double currentTime;
		int currentGame;

		PBState state;
		double lastMove;

		NeuroUtils::Params currentParams;
		PBAE::PBState::PlayerState playerState;
		bool UpdateState(double force, double tau);
		void UpdateSecondDerivatives(PBState &state, double force);
		void rk4(double f, double tau);
		void Reset();

		bool ApplyParams(NeuroUtils::Params params);
		void GetState(PBAE::PBState &state);
};

PBAE::PBAE() {
	size_t i;
	int game;
	ifstream fin;

	// Read in test cases
	vector<string> testCasesFiles = ParamsGetVector(PARAMS_STEM, "test_cases_files", false);
	game = -1;
	for (i = 0; i < testCasesFiles.size(); i++) {
		fin.open(testCasesFiles[i].c_str(), ifstream::in);
		if (!fin.is_open()) {
			fprintf(stderr, "PBAE: Error opening test cases file %s\n", testCasesFiles[i].c_str());
			perror("");
			exit(1);
		}

		do {
			game++;
			testCases.resize(game+1);
			testCases[game].inputFormat = Params::SON;
			testCases[game].SetName("PBAE Test Case");
			if (!testCases[game].ReadStream(fin)) {
				fprintf(stderr, "PBAE: Error parsing test case %d from file %s:\n%s\n", 
						game, testCasesFiles[i].c_str(), testCases[game].errorMessage.c_str());
				exit(1);
			}
			fin >> std::ws;
		}
		while (!fin.eof());
		fin.close();
	}
}

void* PBAE::NewRun() const {
	PBAERun* s = new PBAERun;
	StartNextGame(s);
	return s;
}

void PBAE::DeleteRun(void* run) const {
	PBAERun* s = (PBAERun*) run;
	delete s;
}

bool PBAE::StartNextGame(void* run) const {
	PBAERun* s = (PBAERun*) run;
	if (s->currentGame >= (int) testCases.size()) return false;

	if (!s->ApplyParams(testCases[s->currentGame])) {
		fprintf(stderr, "PBAE::StartNextGame error setting parameters for test case #%d\n", s->currentGame);
		exit(1);
	}
	s->currentGame++;
	return true;
}

bool PBAE::ApplyParams(void* run, Params &params) const {
	PBAERun* s = (PBAERun*) run;
	return s->ApplyParams(params);
}

Params& PBAE::GetParams(void* run) const {
	PBAERun* s = (PBAERun*) run;
	return s->currentParams;
}

/*-1 for right; 1 for left; 0 for no push*/
bool PBAE::UpdateState(void* run, double dir) const {
	int i;
	double fractional;
	PBAERun* s = (PBAERun*) run;
	s->lastMove = dir;

	// Do the integer part of the step
	for (i = 0; i < s->steps_per_push; i++) {
		if (!s->UpdateState(dir*s->force, s->dt)) return false;
	}

	// Do the fractional part of the step
	fractional = s->steps_per_push - (int) s->steps_per_push;
	if (fractional != 0) return s->UpdateState(dir*s->force, fractional);
	return true;
}

void PBAE::GetState(void* run, PBAE::PBState &stateOut) const {
	PBAERun* s = (PBAERun*) run;
	s->GetState(stateOut);
}

void PBAERun::GetState(PBAE::PBState &stateOut) {
	size_t i;
	stateOut.x = state.x;
	stateOut.xd = state.xd;

	stateOut.poles.resize(state.poles.size());
	for (i = 0; i < poles.size(); i++) {
		stateOut.poles[i].theta = state.poles[i].theta;
		stateOut.poles[i].thetad = state.poles[i].thetad;
	}
	stateOut.lastMove = lastMove;
	stateOut.playerState = playerState;
}

string PBAE::PBState::Serialize() {
	size_t i;
	stringstream ss;

	ss << "LastMove: " << lastMove;
	ss << ", x: " << x;
	ss << ", xd: " << xd;
	for (i = 0; i < poles.size(); i++) {
		ss << ", theta_" << i+1 << ": " << poles[i].theta;
		ss << ", thetad_" << i+1 << ": " << poles[i].thetad;
	}
	ss << ", PlayerState: ";
	switch (playerState) {
		case PBAE::PBState::ALIVE:
			ss << "alive";
			break;
		case PBAE::PBState::X_FAIL:
			ss << "x_fail";
			break;
		case PBAE::PBState::THETA_FAIL:
			ss << "theta_fail";
			break;
		case PBAE::PBState::TIME:
			ss << "time";
			break;
		default:
			// TODO: error message?
			ss << "MissingNo.";
			break;
	}
	return ss.str();
}

static void invalidStateString(const string &state, const string &s) {
	fprintf(stderr, "Invalid state string: %s: %s\n", s.c_str(), state.c_str());
}

bool PBAE::PBState::Parse(const string &stateStr) {
	stringstream ss;
	string s;
	ss.str(stateStr);

	// Skip lastMove
	getline(ss, s, ',');

	// Get x and xd
	if (!getline(ss, s, ',') || sscanf(s.c_str(), " x: %lf", &x) != 1) {
		invalidStateString(stateStr, "invalid x");
		return false;
	}
	if (!getline(ss, s, ',') || sscanf(s.c_str(), " xd: %lf", &xd) != 1) {
		invalidStateString(stateStr, "invalid xd");
		return false;
	}
	PoleState p;
	poles.clear();
	while (getline(ss, s, ',')) {

		// Process theta or PlayerState
		if (sscanf(s.c_str(), " theta_%*d: %lf", &p.theta) != 1) {
			stringstream keyVal(s);
			keyVal >> s;
			if (s != "PlayerState:") {
				invalidStateString(stateStr, "expected theta or PlayerState");
				return false;
			}
			keyVal >> s;
			if (s == "alive") playerState = PBState::ALIVE;
			else if (s == "x_fail") playerState = PBState::X_FAIL;
			else if (s == "theta_fail") playerState = PBState::THETA_FAIL;
			else if (s == "time") playerState = PBState::TIME;
			else {
				invalidStateString(stateStr, "invalid player state value");
				return false;
			}
			return true;
		}

		// Process thetad
		if (!getline(ss, s, ',') || sscanf(s.c_str(), " thetad_%*d: %lf", &p.thetad) != 1) {
			invalidStateString(stateStr, "invalid thetad");
			return false;
		}
		poles.push_back(p);
	}
	if (poles.size() == 0) return false;
	return true;
}

PBAERun::PBAERun() {
	lastMove = 0;
	currentGame = 0;

	//Set up default values
	max_sim_time = ParamsGetInt(PBAE::PARAMS_STEM, "max_sim_time_timesteps", true);
	currentParams.SetInt("max_sim_time_timesteps", max_sim_time);

	g = 9.81;
	mc = 1.0;

	tfail = ParamsGetDouble(PBAE::PARAMS_STEM, "max_theta", true);
	xfail = ParamsGetDouble(PBAE::PARAMS_STEM, "max_x", true);
	force = 10;
	dt = 0.02;

	startX = -1.2;
	startXd = 0;
	cart_friction = 0;
	steps_per_push = 1;

	defaultPole.startTheta = 0.15;
	defaultPole.startThetad = 0;
	defaultPole.length = 1;
	defaultPole.mass = 0.1;
	defaultPole.ballMass = 0;
	defaultPole.muf = 0;

	poles.push_back(defaultPole);
	currentParams.SetDouble("num_poles", poles.size());

	currentParams.SetDouble("gravity", g);
	currentParams.SetDouble("cart_mass", mc);
	currentParams.SetDouble("max_theta", tfail);
	currentParams.SetDouble("max_x", xfail);
	currentParams.SetDouble("force_magnitude", force);
	currentParams.SetDouble("dt", dt);
	currentParams.SetDouble("x", startX);
	currentParams.SetDouble("xd", startXd);
	currentParams.SetDouble("cart_friction", cart_friction);
	currentParams.SetDouble("steps_per_push", steps_per_push);

	currentParams.SetDouble("default_pole_theta", defaultPole.startTheta);
	currentParams.SetDouble("default_pole_thetad", defaultPole.startThetad);
	currentParams.SetDouble("default_pole_length", defaultPole.length);
	currentParams.SetDouble("default_pole_mass", defaultPole.mass);
	currentParams.SetDouble("default_pole_ball_mass", defaultPole.ballMass);
	currentParams.SetDouble("default_pole_friction", defaultPole.muf);

	currentParams.SetName("PBAERun");

	doubleParams["gravity"] = &g;
	doubleParams["cart_mass"] = &mc;
	doubleParams["cart_friction"] = &cart_friction;
	doubleParams["steps_per_push"] = &steps_per_push;
	doubleParams["max_theta"] = &tfail;
	doubleParams["max_x"] = &xfail;
	doubleParams["force_magnitude"] = &force;
	doubleParams["dt"] = &dt;
	doubleParams["x"] = &startX;
	doubleParams["xd"] = &startXd;

	doubleParams["default_pole_theta"] = &defaultPole.startTheta;
	doubleParams["default_pole_thetad"] = &defaultPole.startThetad;
	doubleParams["default_pole_length"] = &defaultPole.length;
	doubleParams["default_pole_mass"] = &defaultPole.mass;
	doubleParams["default_pole_ball_mass"] = &defaultPole.ballMass;
	doubleParams["default_pole_friction"] = &defaultPole.muf;
}
static bool startsWith(const string &s, const string &prefix) {
	return s.compare(0, prefix.size(), prefix) == 0;
}
bool PBAERun::ApplyParams(NeuroUtils::Params params) {
	int n;
	stringstream ss;
	string s;
	map <string, detail::NeuroParamVal>::const_iterator pit;
	map <string, detail::NeuroParamVal>::iterator start, end;
	map <string, double*>::iterator dit;

	// Parse each param to make sure there aren't any extras
	for (pit = params.paramsMap.begin(); pit != params.paramsMap.end(); pit++) {
		dit = doubleParams.find(pit->first);
		if (dit != doubleParams.end()) {
			if (pit->second.d == DSENTINEL) {
				fprintf(stderr, "PBAERun::ApplyParams error: invalid value for parameter '%s'. Expected a double but found %s\n",
						pit->first.c_str(), pit->second.s.c_str());
				return false;
			}
			*dit->second = pit->second.d;
			if (startsWith(pit->first, "default")) {
				// Apply this default value to each of the poles
				for (n = 0; n < (int) poles.size(); n++) {
					*(dit->second - (double*)&defaultPole + (double*) &poles[n]) = pit->second.d;
				}
			}
		}
		else if (pit->first == "num_poles") {
			if (pit->second.i == ISENTINEL) {
				fprintf(stderr, "PBAERun::ApplyParams error: invalid value for parameter '%s'. Expected an integer but found %s\n",
						pit->first.c_str(), pit->second.s.c_str());
				return false;
			}
			// Delete the parameters of the poles that are being removed
			for (n = (int) poles.size(); n > pit->second.i; n--) {
				ss.clear(); ss.str("");
				ss << "pole_" << n << "_";
				s = ss.str();
				start = currentParams.paramsMap.lower_bound(s);
				if (start != currentParams.paramsMap.end()) {
					s[s.size()-1]++;
					end = currentParams.paramsMap.upper_bound(s);
					currentParams.paramsMap.erase(start, end);
				}
			}
			poles.resize(pit->second.i, defaultPole);
		}
		else if (pit->first == "max_sim_time") {
			if (pit->second.i == ISENTINEL) {
				fprintf(stderr, "PBAERun::ApplyParams error: invalid value for parameter '%s'. Expected an integer but found %s\n",
						pit->first.c_str(), pit->second.s.c_str());
				return false;
			}
			max_sim_time = pit->second.i;
		}
		else {
			const char* pc = pit->first.c_str();
			// See if it's pole-specific info
			if (sscanf(pc, "pole_%d", &n) != 1) {
				fprintf(stderr, "PBAERun::ApplyParams error: this is not a valid param: %s\n", pit->first.c_str());
				return false;
			}
			else {
				// Jump to identifier (add 5 because "pole_" is five characters) 
				pc = strchr(pc+5,'_');
				if (pc == NULL)  {
					fprintf(stderr, "PBAERun::ApplyParams error: pole param is missing underscore: %s\n", pit->first.c_str());
					return false;
				}
				pc++;

				if ((int) poles.size() < n) {
					fprintf(stderr, "PBAERun::ApplyParams error: found parameter %s for pole #%d but num_poles is %d\n",
							pit->first.c_str(), n, (int) poles.size());
				}
				else {
					Pole &p = poles[n-1];

					if (pit->second.d == DSENTINEL) {
						fprintf(stderr, "PBAERun::ApplyParams error: invalid value for parameter '%s'. Expected a double but found %s\n",
								pit->first.c_str(), pit->second.s.c_str());
						return false;
					}

					// Set value based on id
					if (strcmp(pc, "length") == 0) p.length = pit->second.d;
					else if (strcmp(pc, "mass") == 0) p.mass = pit->second.d;
					else if (strcmp(pc, "ball_mass") == 0) p.ballMass = pit->second.d;
					else if (strcmp(pc, "theta") == 0) p.startTheta = pit->second.d;
					else if (strcmp(pc, "thetad") == 0) p.startThetad = pit->second.d;
					else if (strcmp(pc, "friction") == 0) p.muf = pit->second.d;
					else  {
						fprintf(stderr, "PBAERun::ApplyParams error: this pole param does not exist: %s\n", pc);
						return false;
					}
				}
			}
		}
	}

	// Copy params into currentParams
	currentParams.Apply(params);

	Reset();
	for (size_t i = 0; i < poles.size(); i++) {
		Pole &p = poles[i];
		if (p.muf != 0 && p.ballMass != 0) {
			fprintf(stderr, "PBAERun::ApplyParams error: current simulation cannot handle a pole that has both friction and a ball on the end\n");
			return false;
		}
	}
	return true;
}

/*
	 PBAERun::Reset() resets the cart and pole(s) to their starting positions
	 and velocities.
 */
void PBAERun::Reset() {
	size_t i;

	//Reset state to initial values
	state.x = startX;
	state.xd = startXd;
	lastMove = 0;
	currentTime = 0;
	playerState = PBAE::PBState::ALIVE;

	state.poles.resize(poles.size());

	for (i = 0; i < poles.size(); i++) {
		PoleState &s = state.poles[i];
		s.theta = poles[i].startTheta;
		s.thetad = poles[i].startThetad;
	}
}

/*
	 PBAERun::UpdateState() moves the simulation forward by dt seconds while applying
	 the given force. It uses the Euler method or the fourth-order Runge-Kutta method.

	 It returns false if the cart has hit a wall or if any of the poles has fallen
	 pass the failing angle or if the max simulation time has been reached, and true otherwise.
 */
bool PBAERun::UpdateState(double force, double tau) {
	size_t i;

	if (playerState != PBAE::PBState::ALIVE) return false;

	if (currentTime >= max_sim_time) {
		playerState = PBAE::PBState::TIME;
		return false;
	}

	currentTime++;

	if (poles.size() <= 1) {
		// A single pole system doesn't need to be simulated very accurately
		UpdateSecondDerivatives(state, force);
		state.Step(tau);
	}
	else {
		const bool rungeKutta = true;
		if (rungeKutta) {
			const int numSteps = 2;
			const double stepSize = tau/numSteps;

			for (i = 0; i < numSteps; i++) {
				UpdateSecondDerivatives(state, force);
				rk4(force, stepSize);
			}
		}
		else {
			// Euler Method
			const int numSteps = 8;
			const double stepSize = tau/numSteps;
			for (i = 0; i < numSteps; i++) {
				UpdateSecondDerivatives(state, force);
				state.Step(stepSize);
			}
		}
	}

	// See if cart has hit wall or if any poles have fallen too far
	if (abs(state.x) >= xfail) {
		playerState = PBAE::PBState::X_FAIL;
		return false;
	}
	for (i = 0; i < state.poles.size(); i++) {
		if (abs(state.poles[i].theta) >= tfail) {
			playerState = PBAE::PBState::THETA_FAIL;
			return false;
		}
	}
	return true;
}


/*
	 PBAERun::UpdateSecondDerivatives() uses the pole balancing physics equations 
	 to update the second derivatives (xdd and thetadd) of the given state, with
	 the given applied force.
 */
void PBAERun::UpdateSecondDerivatives(PBState &state, double force) {
	size_t i;
	double effectiveForce, effectiveMass;
	double gmass;

	// Sum effective forces and effective masses from each pole on cart
	effectiveForce = 0;
	effectiveMass = 0;
	for (i = 0; i < state.poles.size(); i++){
		PoleState &s = state.poles[i];
		Pole &p = poles[i];
		if (p.ballMass != 0) {
			gmass = p.ballMass + p.mass/2.0;

			effectiveForce += gmass*sin(s.theta)*(p.length*s.thetad*s.thetad - gmass/(p.ballMass+p.mass/3.0)*g*cos(s.theta));
			effectiveMass += p.ballMass + p.mass - gmass*gmass/(p.ballMass+p.mass/3.0)*cos(s.theta)*cos(s.theta);
		}
		else {
			effectiveForce += p.mass*(p.length/2.0 * s.thetad*s.thetad * sin(s.theta) - 0.75*cos(s.theta) * (2.0*p.muf*s.thetad/(p.mass*p.length) + g*sin(s.theta)));
			effectiveMass += p.mass*(1.0 - 0.75*cos(s.theta)*cos(s.theta));
		}
	}

	// Cart friction
	if (state.xd > 0) effectiveForce -= cart_friction;
	else if (state.xd < 0) effectiveForce += cart_friction;

	state.xdd = (force + effectiveForce)/(mc + effectiveMass);

	// Update each pole's thetadd
	for (i = 0; i < poles.size(); i++){
		PoleState &s = state.poles[i];
		Pole &p = poles[i];
		if (p.ballMass != 0) {
			gmass = p.ballMass + p.mass/2.0;
			s.thetadd = gmass * (g*sin(s.theta) - state.xdd*cos(s.theta) - 2.0*p.muf*s.thetad/(p.mass*p.length)) / (p.length*(p.ballMass+p.mass/3.0));
		}
		else {
			s.thetadd =   1.5 * (g*sin(s.theta) - state.xdd*cos(s.theta) - 2.0*p.muf*s.thetad/(p.mass*p.length)) / p.length;
		}
	}
}

/*
	 PBState::Step() uses the Euler method to update state by the given tau
	 seconds, based on the current derivatives.
 */
void PBAERun::PBState::Step(double tau) {
	// Update cart's motion
	x = x + tau*xd;
	xd = xd + tau*xdd;

	// Update each pole's theta and thetad
	for (size_t i = 0; i < poles.size(); i++){
		PoleState &s = poles[i];
		s.theta = s.theta + tau*s.thetad;
		s.thetad = s.thetad + tau*s.thetadd;
	}
}

/*
	 PBAERun::ruk4() uses a fourth-order Runge-Kutta method to update the state 
	 of the cart and pole(s) by the given tau seconds with an applied force of f.
 */
void PBAERun::rk4(double f, double tau) {
	size_t i;
	double halfStep, s6;

	halfStep = tau * 0.5;
	s6 = tau / 6.0;

	// Move forward by half tau using current derivatives
	PBState midState1 = state;
	midState1.Step(halfStep);
	// Update derivatives based on these midpoint values
	UpdateSecondDerivatives(midState1, f);

	// Move forward by half tau using 1st approximation of midpoint derivatives
	PBState midState2 = state;
	midState2.xd = midState1.xd;
	for (i = 0; i < midState2.poles.size(); i++) {
		midState2.poles[i].thetad = midState1.poles[i].thetad;
	}
	midState2.Step(halfStep);
	// Update derivatives based on these new values for the midpoint
	UpdateSecondDerivatives(midState2, f);

	// Move forward by full tau using second midpoint derivatives approximation
	PBState endState = state;
	endState.xd = midState2.xd;
	for (i = 0; i < endState.poles.size(); i++) {
		endState.poles[i].thetad = midState2.poles[i].thetad;
	}
	endState.Step(tau);
	// Update derivatives based on the endpoint values
	UpdateSecondDerivatives(endState, f);


	// At this point, state and endState are at the endpoints, and midState1 and midState2 are in the middle

	// Update state based on weighted average of derivatives at midpoints and endpoints
	state.x = state.x + s6 * ( state.xd + endState.xd + 2.0*midState1.xd + 2.0*midState2.xd);
	state.xd = state.xd + s6 * ( state.xdd + endState.xdd + 2.0*midState1.xdd + 2.0*midState2.xdd);
	for (i = 0; i < poles.size(); i++) {
		PoleState &s = state.poles[i];
		s.theta = s.theta + s6 * (s.thetad + endState.poles[i].thetad + 2.0*midState1.poles[i].thetad + 2.0*midState2.poles[i].thetad);
		s.thetad = s.thetad + s6 * (s.thetadd + endState.poles[i].thetadd + 2.0*midState1.poles[i].thetadd + 2.0*midState2.poles[i].thetadd);
	}
}
