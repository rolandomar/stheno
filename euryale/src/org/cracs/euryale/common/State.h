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
 * State.h
 *
 *  Created on: Mar 5, 2008
 *      Author: rmartins
 */

#ifndef STATE_H_
#define STATE_H_

namespace pt {

namespace efacec {

namespace rto {

namespace runtime
{
class State {
public:
	State();
	virtual ~State();
	
/*
 * NOT_INITIALIZE => STOPPED => STARTING => RUNNING => STOPPING v => DESTROYED
 *                 ^ ============================================
 *                   
 */	
	enum
	{
		NOT_INITIALIZE,
		STARTING,		
		RUNNING,
		STOPPING,
		STOPPED,
		DESTROYED
	};
	
	
	int stop(); //sets the state to stopping
	int start(); //sets the state to starting
	int destroy();
	
	int getState();
	
	
protected:
	int m_state;
	
	int setState(int newState);
	void setState_i(int newState);
	

	virtual int leavingNotInitializedState(){return 0;}
	
	virtual int enterRunningState(){return 0;}
	virtual int runningState(){return 0;}
	virtual int leavingRunningState(){return 0;}
	
	virtual int enterStoppedState(){return 0;}
	virtual int stoppedState(){return 0;}
	virtual int leavingStoppedState(){return 0;}
	
	virtual int enterDestroyedState(){return 0;}
	virtual void destroyedState(){} //cleanup can be done here

	
};

}

}

}

}

using pt::efacec::rto::runtime::State;

#endif /* STATE_H_ */
