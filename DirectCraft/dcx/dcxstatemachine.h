//------------------------------------------------------------------------------------------------
// File:    dcxstatemachine.h
//
// Desc:    Provides a simple interface for controlling a state machine based on an enum
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXSTATEMACHINE_H__
#define __DCXSTATEMACHINE_H__


namespace dcx {

    /**
     * This class is used to control a finite state machine that can be in a state that is
     * pulled from the enumeration that is passed as the template parameter.
     */
    template <typename T> class dcxStateMachine
    {
        /// The enumeration containing the state-space of this machine
        typedef T State;

        /// List of states to be executed
        typedef std::deque<State> Queue;

        public:

            /**
             * Initializes the state manager.  The advance() method should be called
             * to move into the startup state, otherwise the current state is
             * undefined.
             */
            dcxStateMachine( State startupState )
            {
                myCurrentState = (State)0xFFFFFFFF;
                myNextStates.push_back( startupState );
            }

            /**
             * Gets the state in which 
             */
            State currentState()
            {
                return myCurrentState;
            }

            /**
             * Erases all of the states currently in the machine.  If no state is
             * added to the system, tihs will cause the next call to advance() to
             * return false.
             */
            void clear()
            {
                myNextStates.clear();
            }

            /**
             * Moves the next state in the queue into the current state.  If there are
             * no states in the queue, this method returns 'false' and the current
             * state remains the same.
             */
            bool advance()
            {
                // If there are no available states, exit
                if( myNextStates.empty() ) return false;

                // Grab the next state
                myCurrentState = myNextStates.front();
                myNextStates.pop_front();

                // Success
                return true;
            }

            /**
             * The provided state will be reached after advance() has cycled
             * through all of the states already in the queue.
             */
            void queueState( State state )
            {
                myNextStates.push_back( state );
            }

            /**
             * Causes the state machine to go to the provided state machine on the
             * next call to advance().  Clears the state queue.
             */
            void jumpState( State state )
            {
                myNextStates.clear();
                myNextStates.push_back( state );
            }

            /**
             * Causes the state machine to go to the state that is provided in the
             * parameter the next time advance() is called.  Does not clear the
             * state queue.
             */
            void insertState( State state )
            {
                myNextStates.push_front( state );
            }

            /**
             * Clears the queue, then makes a "procedure call" to the given state.  The
             * advance() method will return the provided state next, then whatever the
             * current state is.
             */
            void branchState( State state )
            {
                myNextStates.clear();
                myNextStates.push_front( myCurrentState );
                myNextStates.push_front( state );
            }

            /**
             * Causes the state machine to move to the given state next, then back to
             * the current state.
             */
            void insertBranchState( State state )
            {
                myNextStates.push_front( myCurrentState );
                myNextStates.push_front( state );
            }

            /**
             * Causes the state machine to move to the given state after all of the
             * other states already in the queue have been processed, then back to
             * the current state.
             */
            void queueBranchState( State state )
            {
                myNextStates.push_back( myCurrentState );
                myNextStates.push_back( state );
            }


        protected:

            // The current state of the state machine
            State myCurrentState;

            // The list of states that come next in the machine
            Queue myNextStates;
    };
}

#endif