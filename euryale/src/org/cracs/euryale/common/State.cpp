/*
 *  Copyright 2012 Rolando Martins, CRACS & INESC-TEC, DCC/FCUP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *   
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 */
/*
 * State.cpp
 *
 *  Created on: Mar 5, 2008
 *      Author: rmartins
 */

#include "State.h"

namespace pt {

namespace efacec {

namespace rto {

namespace runtime {

State::State() :
	m_state(NOT_INITIALIZE) {
	// TODO Auto-generated constructor stub

}

State::~State() {
	// TODO Auto-generated destructor stub	
}

int State::stop() {
	return setState(STOPPED);
}
int State::start() {
	return setState(RUNNING);
}

int State::destroy() {
	if (stop() != 0) {
		return -1;
	}
	return setState(DESTROYED);
}

int State::getState() {
	return m_state;
}

void State::setState_i(int newState) {
	m_state = newState;
}

int State::setState(int newState) {
	switch (m_state) {
		case NOT_INITIALIZE: {
			switch (newState) {
				case RUNNING:
					if (leavingNotInitializedState() != 0) {
						setState_i(NOT_INITIALIZE);
						return -1;
					}
					setState_i(STARTING);
					if (enterRunningState() != 0) {
						setState_i(NOT_INITIALIZE);
						return -1;
					}
					
					setState_i(RUNNING);
					if (runningState() != 0) {
						setState_i(NOT_INITIALIZE);
						return -1;
					}
					
					return 0;
				case DESTROYED:
					if (leavingStoppedState() != 0) {
						setState_i(NOT_INITIALIZE);
						return -1;
					}
					
					setState_i(DESTROYED);
					if (enterDestroyedState() != 0) {
						setState_i(NOT_INITIALIZE);
						return -1;
					}
					//The final state
					destroyedState();
					return 0;
					
				default:
					return -1;
					
			}
			
		}
			
		case RUNNING: {
			switch (newState) {
				case STOPPED:
					if (leavingRunningState() != 0) {
						setState_i(RUNNING);
						return -1;
					}
					
					setState_i(STOPPING);
					if (enterStoppedState() != 0) {
						setState_i(RUNNING);
						return -1;
					}
					setState_i(STOPPED);
					if (stoppedState() != 0) {
						setState_i(RUNNING);
						return -1;
					}
					return 0;
					
				default:
					return -1;
			}
			
		}
		case STOPPED: {
			switch (newState) {
				case RUNNING:
					if (leavingStoppedState() != 0) {
						setState_i(STOPPED);
						return -1;
					}
					setState_i(STARTING);
					if (enterRunningState() != 0) {
						setState_i(STOPPED);
						return -1;
					}
					setState_i(RUNNING);
					if (runningState() != 0) {
						setState_i(STOPPED);
						return -1;
					}
					return 0;
				case DESTROYED:
					if (leavingStoppedState() != 0) {
						setState_i(STOPPED);
						return -1;
					}
					
					setState_i(DESTROYED);
					if (enterDestroyedState() != 0) {
						setState_i(STOPPED);
						return -1;
					}
					//The final state
					destroyedState();
					return 0;
				default:
					return -1;
			}
		}
			
		case DESTROYED: {
			return -1;
		}
		default:
			return -1;
	}
}

}

}

}

}

