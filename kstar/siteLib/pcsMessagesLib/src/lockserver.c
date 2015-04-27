/* %W% %G%
******************************************************************************
FILE: lockserver.c

Introduction
------------
This file contains the bulk of the code for the lockout
server for the plasma control system. (The main routine for the
lockout server is in lockmain.c. Installation-specific functions for
the lockout server are in the file lockinstall.h in the installation-specific
code area.) 

The lockout server process coordinates the
actions of the various PCS processes during the execution of a shot
sequence. When a shot sequence is not in progress, this process is
basically idle; it is simply monitoring for the command to start a
shot sequence.

The logic here implements a "state machine."
This process receives notification of various events and responds to
the events by changing the PCS "state." The various PCS processes
monitor the PCS state and respond appropriately to changes in the
state.  Also, this process stimulates action by other processes in
some cases by sending commands to those processes.

Communication with this process is primarily through a socket.  
The user interface and the realtime
host routines send messages to this socket.  This routine sends messages to the
waveform server.  

There is also provision for a secondary method
to communicate with this process, which would typically be through a
hardware interface.  For instance, this interface might be a digital
i/o board that detects the occurrence of various trigger signals
during the shot sequence.  At appropriate times, this process calls
standard functions that execute installation-specific code to obtain
the necessary information from this secondary communication
mechanism.  All of this secondary communication is optional.  Many of
these installation specific functions can be empty, if desired.

The mechanism here to implement a complete shot cycle is specific to
the PCS.  There might, however, be a desire to notify another tokamak
control system of the progress being made by the PCS.  For this
purpose, there
is also provision here to generate "event notices" at
appropriate places in the shot cycle.  The mechanism by which these
event notices are generated is entirely installation-specific.  For
instance, it might be desirable to notify the main tokamak control
system at the point the PCS is beginning shot setup, or at the point
the PCS completes processing for a shot.

Operation mode flags
--------------------
There are 3 flags that the lockout server uses to affect some of
its behavior.
1. The argument to the primary function lockserver called
input_option is copied to the global variable
HARDWARE_AVAILABLE.  This is a global variable is available to the
installation-specific functions described below.

This setting of this flag is determined by a 
command line argument for the lockout server process.

If HARDWARE_AVAILABLE is 0 then the secondary communication mechanism
as implemented by the installation-specific functions described below 
is not used (the installation-specific functions are not called).  
HARDWARE_AVAILABLE would typically be set to 0 when the PCS is
operated in a testing mode in which the lockout server should not
interact with the other tokamak systems.  For instance, the lockout
server shouldn't expect to receive (installation-specific) hardware
trigger signals and the lockout server shouldn't try to implement
(installation-specific) interactions with the real time hardware.

2.  The global variable TEST_MODE is set to 1 for shots that are
either in a hardware or a software test mode (as opposed to "normal"
mode as set by the user interface).  This variable is set for each shot.

If TEST_MODE is a nonzero value, then the installation-specific functions
described below that implement the secondary communication mechanism are not
called during a shot sequence.  (The exception is ls_init_hardware 
because it is
called, if HARDWARE_AVAILABLE is set, when the lockout server process is
started rather than during a shot cycle.) This would typically prevent the PCS
from interacting with the other tokamak timing systems during PCS testing 
modes. This is particularly relevant for  the standalone software test mode
in which the pcs isn't actually executed on the real time processor hardware.
It is also relevant for other testing modes because in these modes the PCS
would typically be triggered by hand (using the "manual cycle control"
window of the user interface) and the status of any watchdog timer would not
matter.

3. FORCED_TEST_MODE is a global variable that is set for a single
shot when the message START_TEST_SHOT is received on the lockout server's
input socket.  (This message is sent as part of the message sequence
generated when the operator presses the "start test shot" button on the
"manual cycle control" window of the user interface.)
This forces the installation-specific function
ls_get_shot_number to be told that the shot is in test mode.  Typically,
this would cause this function to return the test shot number
as the shot number to be used for that shot. (So, when the operator 
presses the "start test shot" button on the
"manual cycle control" window of the user interface, the test mode shot number
will always be used.  This prevents confusion for data archival of test shots
with archive files created for the regular tokamak
shot number sequence when the PCS is triggered by
hand for a test, perhaps with test shots being mixed in with 
normal tokamak shots for some reason.)

Installation-specific functions required for the lockout server
---------------------------------------------------------------
A set of installation-specific functions must be provided.  These functions
have a specified calling interface as described below.
These installation-specific functions implement the secondary 
communication method and the "event notification" mechanism discussed above. 

The installation-specific functions must be
located in the installation-specific file lockinstall.h.
This file is included when lockmain.c is compiled.

The required installation-specific functions are the following.

"Event notification" functions
------------------------------
1. int ls_event_init(init_ipcs)

This function is used to do any necessary installation-specific
initialization of the installation-specific event notification
facility.  It is called once when the lockout server process is
started. The input argument init_ipcs is 1 if the HARDWARE_AVAILABLE
flag is true, otherwise init_ipcs is 0.  The return value is not
used.  Any installation-specific context generated by this function
should be stored in global variables.

In the simplest case, this function can be empty.

2. int ls_event_cleanups()

This function is called at the end of every lockout server processing loop
(see below for a discussion of this processing loop).
In an event notification system where the lockout server might need to
respond to events or might need to clean up an event queue, for
instance, this function would do this type of periodic processing.
Typically, this function is called approximately every 1 second.

The return value is not used.  The function has no arguments.  In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.

3. int ls_send_event(char *type)

This function is called to generate installation-specific events at
various stages of the shot sequence.  The points during the shot
sequence when this function is called are documented below in the
discussion of events and the evolution of the lockout server state
machine.

The function argument is a string indicating the type of event that
should the indicated to the installation-specific event system.  The
possible argument values are:
"LOCKOUT": when the shot sequence starts.
"UNLOCK": when the operator unlocks the sequence so further setup
changes can be made.
"RELOCK": when the operator shot setup starts again after the operator
relocks.
"READY": when the final lockout trigger has been received, and all
real time processors are ready for the shot.
"ABORT": whenever a shot is aborted.
"COMPLETE": when the complete shot cycle is finished.

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.

"Secondary communication" functions
-----------------------------------
1. int  ls_init_hardware(rtcpu_count) 

This function is called once when the lockout server process is started
to do any installation-specific initialization of the
secondary communication mechanism that is required.  The argument
rtcpu_count is equal to the number of real time processors in the PCS
configuration.

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.


2. int ls_hardware_flags(int *hw_first_lockout,
                         int *hw_abort,
                         int *hw_final_lock)

This function is called during each processing loop of the lockout
server to determine the present status of 3 flags that can be provided
by the secondary communication mechanism.  The secondary communication
mechanism can provide any, all, or none of these 3 flags.  Typically
the secondary communication mechanism would the a digital input board
of some sort that detects the occurrence of trigger signals from a
central tokamak timing system.

If any of the 3 trigger signals have been received, the corresponding
function argument should be set to 1.  Otherwise, the corresponding
function argument can either be ignored or set to 0.

The function arguments are:
hw_first_lockout: this flag indicates the first lockout event has
occurred (see below for a discussion of the first lockout event).
The first lockout trigger indicates a shot sequence should be started.

hw_abort: this flag indicates that the shot sequence should be
aborted.  The shot sequence can be aborted as a result of setting this
flag only during certain portions of the sequence.  See below for a
detailed outline of the shot sequence and a discussion of when the
sequence can be aborted.

hw_final_lock: this flag indicates that the final lockout event has
occurred (see below for a discussion of the final lockout event).

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.

3. int ls_cpus_ready_for_shot(int *cpu_list, int rtcpu_count)

When a host_realtime process recognizes the final lockout state, it
will start the real time code running on its real time processor.
There could be some initialization code that executes on the real time
processor at this point.  Also, if the PCS system has a watchdog timer
for each real time processor, the real time processor would typically
start touching the watchdog timer at this point to indicate its
readiness for the shot to begin.

After all of the host_realtime processes have acknowledged that they
have recognized the final lockout state, this function is called to
determine if all of the real time processors are ready to begin the
shot. The function argument rtcpu_count indicates the number of real
time processors in the PCS configuration.  The function argument
cpu_list is an integer array with one element for each real time
processor.  If all of the real time processors are ready for the shot,
this function simply returns 0.  If any processors are not yet ready, this
function returns a nonzero value.  Also, in this case,
for each real time processor, this function must set the
corresponding element of the cpu_list array to 0 if the processor is
ready to execute the shot.  The function sets the array element to 1 if
the processor is not yet ready to execute the shot. This information
is used to generate error messages.

In the simplest case this function simply returns 0.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.

This function is typically used to test the status of a watchdog timer
system.  A given real time processor is ready for the discharge to begin
after it starts toggling its watchdog timer bit.

4. int ls_reset_hardware()

This function is called when the lockout server returns to the NULL
state.  This function should be used to set the secondary
communication hardware to the state that it should have when the PCS
is idle and waiting to start a new shot sequence.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.


5. int ls_firstlockout_hardware() 

This function is called when the lockout server makes the transition
from the NULL state to the FIRSTLOCKOUT state.  This function would
typically perform any interaction with the secondary communication
mechanism hardware that is necessary after the event appears that
causes the PCS to start a shot sequence.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.


6. int ls_prepare_for_finallockout() 

This function is called when the lockout server makes the transition
from the FIRSTLOCKOUT state to the FINALLOCKOUT state.  This function would
typically perform any interaction with the secondary communication
mechanism hardware that is necessary after the event appears that
causes the PCS to start the realtime code.

This function is not called if HARDWARE_AVAILABLE is not set to a
nonzero value.

The return value is not used. In the
simplest case this function can be empty.  Any installation-specific
context generated by this function should be stored in global
variables.


Data archival related functions
-------------------------------

1. int ls_get_shot_number(int test_mode,int TEST_SHOT_NUMBER)

This function is called to determine the shot number that should be
used during the current shot sequence.  Typically, this function would
use an installation-specific mechanism to determine the number of the
upcoming shot from the tokamak database.

The function argument test_mode is 0 during a standard shot sequence
It is a nonzero value during a test mode shot.  In test modes, the
operator provides a shot number using the user interface.  The value
of this test mode shot number is provided in the function argument
TEST_SHOT_NUMBER.  Normally, if test_mode is a nonzero value, the
function returns the value of TEST_SHOT_NUMBER.

The return value is the shot number.

This function cannot be empty.  At the minimum, it must return
TEST_SHOT_NUMBER.

2. void delete_shotfile(shot_number) 

This function is in the installation-specific file close_shotfile.c.

This function is called when the lockout server makes transition to
the FINALLOCKOUT state.  This function can be used to do any cleanup
of the files that will be used to contain the archived data obtained
during the upcoming shot.  Typically, what this function might do is
delete any data files that already exist for the shot number given by
the function argument shot_number.  This is useful because sometimes a
given shot number is used more than once (e.g. in test modes of
operation).  Some archiving code simply adds data to an existing
file.  So, if the same file is used repeatedly it will accumulate
unrelated data from various shots.  So, it might be desirable to
delete an existing file as the new shot begins.

In the simplest case this function can be empty.

3. void close_shotfile(int shot_number, int  flag)

This function is in the installation-specific file close_shotfile.c.

This function is called after all of the host_realtime processes have
completed archiving data for the shot.  The function is called with
the argument flag set to 0 and shot_number equal to the shot number
used for the shot that just completed.

This function is called by the lockout server process as the final
step
in the process of archiving data for a shot.  This function can do any
installation-specific tasks that are required at that point.

The lockout server state machine
================================
Here is a detailed outline of the shot sequence state machine that is
implemented in the lockout server process.

The sequence of events that causes the various processes that
make up the plasma control system to cycle through a tokamak discharge is 
monitored and controlled by the lockserver process.  These are the events 
that include pre-shot and post-shot processing. Anything happening in real 
time during the shot is not controlled by the lockserver.

The lockserver implements a state machine; the pcs is always in one of a 
small set of "states" and a transition from one state to another is triggered 
by one of a series of possible "events."

The lockout server continuously executes a polling loop.  This loop is 
implemented in the function ls_io_handler.
It waits for an interval for a message to arrive on its input socket.
If a message arrives, the message is handled by calling the function
ls_handle_message and then the function
handle_state is called. If no
message arrives within the timeout interval, the lockout server simply
calls the function handle_state.  

The function handle_state checks current status of the lockout server
and takes necessary action.  Part of the process of checking the
current status involves polling
the secondary communication mechanism by calling the appropriate
installation-specific function.  If no event is reported by the
installation-specific function, the lockout server loops back to check 
its input socket again. 
So, the events to which the lockout server responds are generated
either by a message into the server's input port or by a report of an
event from an installation-specific function.

In some stages of the shot sequence, the lockout server will check to
see if an expected event is taking too long to arrive.  An example
would be an expected message from a host_realtime process.  If
something is taking too long, the lockout server process will start
generating messages at intervals.

lockout server events
---------------------
The events to which the lockout server responds are the following.
1. a) SET_FIRSTLOCK message into the lockserver input port.
   or
   b) The "get ready for shot" event is reported by the
   installation-specific function ls_hardware_flags (i.e. the
   hw_first_lockout flag is returned set to a nonzero value).

   The lockout server responds to this event if it is in one of these states:
   NULLSTATE
   This event is ignored otherwise.

2. SET_UNLOCK message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   FIRSTLOCKOUT
   This event is ignored otherwise.

3. a) SET_FINALLOCK message into the lockserver.
   or
   b) "final lockout" reported by the installation-specific function
   ls_hardware_flags (i.e. the hw_final_lock flag is returned set to a
   nonzero value).

   The lockout server responds to this event if it is in one of these states:
   FIRSTLOCKOUT, UNLOCKED
   This event is ignored otherwise.

4. SHOTSETUP_COMPLETE message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   FIRSTLOCKOUT
   This event is ignored otherwise.

5. SET_RELOCK message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   FIRSTLOCKOUT (with unlock_pending set), UNLOCKED
   This event is ignored otherwise.

6. a) ABORT_SHOT message into the lockserver input port.
   or
   b) "abort shot" event reported by the installation-specific
   function ls_hardware_flags (i.e. the hw_abort flag is returned set
   to a nonzero value).

   The lockout server responds to this event if it is in one of these states:
   FIRSTLOCKOUT, UNLOCKED.

   The lockout server responds to the ABORT_SHOT message in the
   FINALLOCKOUT and DURINGSHOT states also.

   This event is ignored otherwise.

7. REALTIME_ROUTINE_STARTING message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   FINALLOCKOUT
   This event is ignored otherwise.

8. SHOT_FINISHED_AND_ABORTED message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   ABORTED, FINALLOCKOUT, DURINGSHOT
   This event is ignored otherwise.

9. SHOT_FINISHED message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   DURINGSHOT
   This event is ignored otherwise.

10.ARCHIVE_FILES_WRITTEN message into the lockserver input port.

   The lockout server responds to this event if it is in one of these states:
   DURINGSHOT, WRITINGFILES
   This event is ignored otherwise.

lockout server fine grain state information
-------------------------------------------
The major states of the lockout server are described below (NULL,
FIRSTLOCKOUT, etc).  There is also a set of flags that maintain some
more fine grain information.  When the lockout server is in a
particular state, it will often consult these flags to determine
whether it is possible to change state or to determine which state to
change to next.

One set of flags are the _pending flags: unlock_pending,
finallock_pending, relock_pending, and abort_pending, late_abort_pending.

The other set of flags is the array of structures of type cpu_status
in the global variable cpustat.  These structures maintain separate
flags indicating the status of each CPU.

Aborting a shot
---------------
The ability exists to abort a shot sequence after it has begun.  This
can happen in two ways.
1.  While the shot is in the FIRSTLOCKOUT state, an abort command can
come either through the lockout server's socket or from the secondary
communication mechanism.  In this case, the FINALLOCKOUT and
DURINGSHOT states are skipped.  The PCS goes directly to the
WRITINGFILES state.  However, the host_realtime processes do no
archiving and the shot sequence completes quickly.

2.  After the shot enters the FINALLOCKOUT state, the lockout server
simply waits for the real time processors to execute the shot and report
that the shot is complete.  So, there can be no shot abort during this 
period unless the abort signal is recognized and acknowledged by the real 
time processor. 

After the real time processes start (the PCS will still be in the 
FINALLOCKOUT state or will have entered the DURINGSHOT state as for
a normal shot), the real time process can detect an
installation-specific abort trigger.  This would probably be a hardware
signal that the real time processors can monitor. Also, if an abort message 
comes into
the lockout server's input socket during the FINALLOCKOUT or DURINGSHOT states 
the lockout server sets the late_abort_pending flag.  This flag is returned
as part of the response to the STATE_QUESTION message that the host_realtime
process exchanges with the lockout server.  The host_realtime process can use
the function get_late_abort_pending_flag to obtain the value of the
late_abort_pending flag. So, during the shot the 
real time processor can have the host_realtime process monitor the value 
of the late_abort_pending
flag and communicate it to the real time processor somehow.
If the flag becomes set the real time processor can recognize this and
treat the shot as aborted.   Polling the lockout server for the value of the
late_abort_pending flag will be slow, so probably the real time processor would
only monitor this during the time interval prior to the start of the 
time critical code.

If the real time processor detects an abort command prior to the start
of the time critical code, rather than executing the
real time control code, the real time process returns with a status
indicating that the shot was aborted.  If the real time processor is able to
detect an abort command during the time critical portion of the discharge,
an installation-specific control algorithm could be responsible for
terminating the discharge smoothly.  In this case the shot should probably
be completed with a normal return status so that the data archival will be
done normally.

It is possible that not all of
the real time processors will report an abort.  Some of the processors
might not detect the installation-specific abort trigger before
beginning the real time control code.  These processors will attempt
to complete the shot normally.  However, even if only one processor
reports an abort completion status, the shot will be considered to
have been aborted.  During the WRITINGFILES state at the end of an
aborted shot, no archiving is actually done and the shot sequence
completes quickly.

It is possible that the abort command could be recognized by one or
more real time processors before all processors are recognized by the
lockout server as being ready for the shot.  In this case the shot would
never enter the DURINGSHOT state.  The PCS would go directly from FINALLOCKOUT
to WRITINGFILES.

Determining when to continue execution of a normal shot
-------------------------------------------------------
There is an assumption made here about the behavior of the remainder of
the tokamak operations systems.  During the normal sequence of events
for execution of a shot, the PCS reaches a point where it has determined
that it is completely ready to begin execution of the time critical 
code that will control the production of the plasma.  At this point the PCS
enters the DURINGSHOT state. The assumption is that the remainder of the 
tokamak control systems will not begin sequences (power supply turn-on etc) 
to make plasma before verifying that the PCS is in the DURINGSHOT state.  

The most serious consequence of not waiting for the PCS to reach the 
DURINGSHOT state, of course, is that the plasma probably won't
be produced properly. Also, the real time processors could report completion 
of the shot before the DURINGSHOT state is reached which would cause 
the PCS to fail to complete the shot sequence properly (because the 
SHOT_FINISHED message is ignored unless the state is DURINGSHOT, see below).

Verification of the DURINGSHOT state can be done by monitoring
the PCS state by either polling the message server (preferred method) or
by polling the lockout server.  Also, the lockout server can notify
the other tokamak control systems during the process of entering the DURINGSHOT
state as part of the installation-specific function ls_cpus_ready_for_shot.

lockout server states
---------------------
The states and possible state transitions are summarized here.
For a given state, the  reactions to events are detailed.
If a particular event isn't mentioned under the explanation of a given 
state, that event is ignored when the PCS is in that state.

********************
1. state = NULLSTATE
********************
This is the idle state of the PCS.  This state is entered when the PCS 
processes are started.

(a) event: SET_FIRSTLOCK message into the lockserver input port
           or 
           Poll interval expired with the "get ready for shot"
           event reported by the installation-specific function
	   ls_hardware_flags (i.e. the
	   hw_first_lockout flag is returned set to a nonzero value)..

    actions: go to the new state FIRSTLOCKOUT (call goto_firstlockout)
             call the event function: ls_send_event("LOCKOUT")
	     Send a message to the waveform server indicating that
	     first lockout has occurred.

***********************
2. state = FIRSTLOCKOUT
***********************

   On entering this state: 
    i)   record that none of the rt cpus have finished the post-lockout 
    setup for the shot.
    ii)  record that none of the cpus have indicated that the shot is
    complete, aborted or that archive files are written. 
    iii) If entering from NULLSTATE, clear all of the _pending bits.

(a) event: SET_UNLOCK message into the lockserver input port

    action:
    i) If all rt cpus have done their post-lockout setup for the shot, 
       then:
        go to the new state UNLOCKED
        event function call: ls_send_event("UNLOCK")
	Tell the waveform server that it should unlock.

    ii) If any rt cpus have not completed the post-lockout setup for 
    the shot, then
    set the unlock_pending flag. No state transition is made.

(b) event: SET_FINALLOCK message into the lockserver input port
           or
           Poll interval expired with the "final lockout" 
           event reported by the installation-specific function
	   ls_hardware_flags (i.e. the hw_final_lock flag is returned set to a
	   nonzero value).

    action:
    i) If all rt cpus have done their post-lockout setup for the shot, 
       then:
        go to the new state FINALLOCKOUT
        installation-specific function call: delete_shotfile(shot_number)
        installation-specific function call: ls_prepare_for_finallockout()

    ii) If any rt cpus have not completed the post-lockout setup for the shot,
    then set the finallock_pending bit.
    No state transition is made.

(c) event: SHOTSETUP_COMPLETE message into the lockserver input port

    action:
    Record that the rt cpu sending the message 
    has completed the post-lockout setup for the shot.
    
    If all rt cpus have completed the post-lockout setup for the shot,
    then:
     If the abort_pending flag is set, go to the new state ABORTED
     If the unlock_pending flag is set, go to the new state UNLOCKED
     If the finallock_pending flag is set, go to the new state FINALLOCKOUT
    else, no state transition is made.

(d) event: SET_RELOCK message into the lockserver input port

    action:
     If the unlock_pending bit is set, then set the relock_pending bit.
    Else, ignore the event.

    No state transition is made.

(e) event: ABORT_SHOT message into the lockserver input port
           or
           Poll interval expired with the "abort"
           event reported by the installation-specific function
	   ls_hardware_flags (i.e. the hw_abort flag is returned set
	   to a nonzero value).

    action:
    i) If all rt cpus have done their post-lockout setup for the shot, 
       then:
        go to the new state ABORTED

    ii) If any rt cpus have not completed the post-lockout setup for the shot,
    then set the abort_pending bit.
    No state transition is made.

*******************
3. state = UNLOCKED
*******************

   On entering the state: (i) tell the waveserver to unlock and begin 
                              processing work queue entries again.
                          (ii) Clear the unlock_pending bit.

   If the relock_pending flag is set, then 
    go to the new state FIRSTLOCKOUT.
    call the event function: ls_send_event("RELOCK")

(a) event: SET_RELOCK message into the lockserver input port

    action:
    go to the new state FIRSTLOCKOUT
    call the event function: ls_send_event("RELOCK")

(b) event: SET_FINALLOCK message into the lockserver
           or
           Poll interval expired with the "final lockout" 
           event reported by the installation-specific function
	   ls_hardware_flags (i.e. the hw_final_lock flag is returned set to a
	   nonzero value).

    In the UNLOCKED state, this event is equivalent to a
    SET_RELOCK message followed by a SET_FINALLOCK message.

    Action:
    Set the finallock_pending flag.
    Go to the new state FIRSTLOCKOUT.

(c) event: ABORT_SHOT message into the lockserver input port
           or
           Poll interval expired with the "abort"
           event reported by the installation-specific function
	   ls_hardware_flags (i.e. the hw_abort flag is returned set
	   to a nonzero value).

    In the UNLOCKED state, this event is equivalent to a
    SET_RELOCK message followed by a ABORT_SHOT message.

    action:
    Set the abort_pending flag
    Go to the new state FIRSTLOCKOUT

***********************
4. state = FINALLOCKOUT
***********************

(a) event: REALTIME_ROUTINE_STARTING message into the lockserver input
    port.

    Action:
    Record that the CPU sending the message has recognized that the state
    is  FINALLOCKOUT and is starting its real time code.

(b) event: SHOT_FINISHED_AND_ABORTED message into the lockserver input port

   This message will be received if a real time processor detects an
   abort command and ends the shot sequence an "aborted" status.

   Record the cpu that issued the message and mark that cpu as having
   completed the shot.
   If all cpus have completed the shot, then 
    call the event function: ls_send_event("ABORT")
    go to the new state WRITINGFILES.
    The DURINGSHOT state is skipped because the shot was aborted.

(c) event: poll interval expired

    action:
    If all CPUs have sent the message
    REALTIME_ROUTINE_STARTING (acknowledging the FINALLOCKOUT state),
    then,
     Call the installation-specific function ls_cpus_ready_for_shot.
    else
     do nothing

    ls_cpus_ready_for_shot reports whether all real time processors have
    gotten ready for the
    real time portion of the shot cycle to begin.  (For instance, for
    DIII-D this installation-specific function checks watchdog toggle
    status for all of the real time processors.
    If the watchdog toggle bits for all rt cpus are clear, then the
    "PCS ready" bit is set.)  

   If ls_cpus_ready_for_shot reports that all processors are ready, 
   then,
    call the event function: ls_send_event("READY")
    go to the new state DURINGSHOT
   Else, no state transition is made.

******************
5. state = ABORTED
******************

(a) event: SHOT_FINISHED_AND_ABORTED message into the lockserver input port

   Record the cpu that issued the message and mark that cpu as having
   completed the shot.
   If all cpus have completed the shot, then 
    call the event function: ls_send_event("ABORT")
    go to the new state WRITINGFILES

*********************
6. state = DURINGSHOT
*********************

   On entry: 
    tell waveserver to write the setup pointname

(a) event: SHOT_FINISHED_AND_ABORTED message into the lockserver input port

   Action:
   Record the cpu that issued the message and mark that cpu as having
   completed the shot. Also record that the CPU detected an abort.
   If all cpus have completed the shot, then 
    call the event function: ls_send_event("ABORT")
    go to the new state WRITINGFILES

(b) event: SHOT_FINISHED message into the lockserver input port

   Action:
   Record the cpu that issued the message and mark that cpu as having
   completed the shot.
   If all cpus have completed the shot, then 
    If any CPU indicated that the shot was aborted, then
       call the event function: ls_send_event("ABORT")
    go to the new state WRITINGFILES

(c) event: ARCHIVE_FILES_WRITTEN message into the lockserver input port

   This message should come from the waveserver.  Set the flag indicating
   that the waveserver has written the setup pointname.
   No state transition.

***********************
7. state = WRITINGFILES
***********************

    On entry: if the waveserver has already finished writing the setup
       pointname, then
       - tell the waveform server that the shot is complete.  In response,
         the waveform server will unlock and start processing its
         message queue again.

(a) event: ARCHIVE_FILES_WRITTEN message received

   Record the cpu that issued the message. 
   Move the flag indicating which cpu can write files to the next cpu.
   If the message comes from the waveserver, then 
   tell the waveform server that the shot is complete.

   If all cpus have written the archive files, then 
   close the shot file.
   call the event function: ls_send_event("COMPLETE")
   Go to the new state = NULL

Shot sequence on the real time processor
----------------------------------------
After the FINALLOCKOUT state is reached, the lockout server basically waits
for notification that the shot was executed.  It is up to the host_realtime
process and the real time processors to execute the shot.

When the host_realtime process recognizes the transition to the FINALLOCKOUT 
state, it starts the code running on the real time processor.  The real
time processor then waits for a trigger signal indicating the start of the
time critical portion of the discharge when the plasma is made.  

While waiting,
the real time processor can detect an abort command and finish its function
with an abort return status.  Also while waiting, the real time processor would
typically do something to notify the lockout server that it is really
ready for making plasma.  Typically the real time processor begins toggling
its watchdog status bit.  This indicates to the watchdog circuit that the
real time processor is alive and functioning properly.  This is detected by
the lockout server in the installation-specific function
ls_cpus_ready_for_shot.  Then the lockout server makes the transition to the
DURINGSHOT state.  The start of plasma should not occur before the
DURINGSHOT state is reached.

After the real time processor detects the trigger signal indicating the start 
of the time to make plasma, the time_critical function is called to execute the
preprogrammed control.  The time_critical function exits when one of the
control algorithms determines that the shot is over.  Then the real time
processor ends its function with a normal return status. The host_realtime
process then communicates with the lockout server to complete the shot 
sequence.
------------------------------------------------------------------------------

CONTENTS:
---------
lockserver

ls_io_handler
handle_message
handle_state

talk_to_waveserver
get_cpu_count

goto_nullstate
goto_firstlockout
goto_unlock
goto_finallock
goto_aborted
goto_duringshot
goto_writingfiles

test_writingfiles_complete

time_elapsed24
mysleep
get_localtime_ints

------------------------------------------------------------------------------
Modifications:
2/3-4/93: created
11/19/94: version 10
08/18/95: BGP added fix to abort problems for abort prior to setup completion.
10/24/95: BGP more fixes for the abort problems.
02/01/96: BGP fixed "Cpus not finished with their setup." message problem.
02/02/96: BGP removed all installation specific code from this file.
03/04/96: BGP increase count for cpus not finishing setup.
03/14/96: BGP send reset signal to host realtime process not finishing shot.
03/15/96: BGP stop and restart problem host realtime processes.
03/25/96: BGP added time_elapsed24 routine for shot not complete checking.
03/31/98: BGP print out elapsed set up time in seconds.
07/23/99: BGP moved EMPTY1 compiling out of the infrastructure into install.
11/04/99: BGP changes to support endian differences.
4/19/2000: JRF make changes for the PCS upgrade 32/64 bit compatibility.
5/20/2000: JRF  remove the PCSUPGRADE macros, leave only the upgrade code.
******************************************************************************
*/

/*
-------------------------------------------------------------------------------
Operating system-provided definitions.
-------------------------------------------------------------------------------
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <unistd.h>

/*
-------------------------------------------------------------------------------
The plasma control system definitions.
-------------------------------------------------------------------------------
*/
#include "serverdefs.h"
#include "serverprotos.h"
#include "sharedvars.h"
#include "msg_pcs.h"
#include "MemDebug.h"

/*
Redefine the descriptor generation macros, so that from now on they
will generate the code for a descriptor.
*/
#define DESCRIPTORS_PART_2
#include "descriptors.h"
#undef DESCRIPTORS_PART_2

/*
-------------------------------------------------------------------------------
lockserver specific global variable definitions
-------------------------------------------------------------------------------
*/
int state;    /* The present state of this process. */

int lock_sock;   /* The descriptor of the socket used for communication. */

int unlock_pending = 0;
int finallock_pending = 0;
int relock_pending = 0;
int abort_pending = 0; /* flag which when set means that the PCS should
			  abort as soon as all cpus have finished their
			  setups. */
int late_abort_pending = 0; /* Flag set when an abort message is received
			       during the FINALLOCKOUT or WRITINGFILES
			       state. */
int shot_was_aborted = 0;
 
int shot_number = 0;
unsigned int lockout_count;
struct diagnostic_infra setup_infra;
int ipcs_status;
int rtcpu_count;
int writefile_cpu = 0;

static int cpu_list[MAX_PCPUS];

/*
STRUCTURE: cpu_status
*/
struct cpu_status
{
        int shot_complete;
        int files_written;
        int aborted;
        int shot_setup_done;
        int watchdog_flg;
        int host_pid;
        int realtime_code_started;
} *cpustat;

int HARDWARE_AVAILABLE; /* flag obtained from the command line indicating
			   whether or not an attempt to touch the 
			   hardware from this routine should be made. */
int TEST_MODE;	/* operating mode obtained from the waveserver */
int TEST_SHOT_NUMBER;	/* test shot number obtained from the waveserver */
int FORCED_TEST_MODE; /* this flag is set when the user selects the
			 start test shot button in the pcs user interface */

long get_localtime_long();
int  change_writefile_cpu();
void test_writingfiles_complete();
int get_localtime_ints();

struct  message_time {
   long reference;
   long previous_message;
   long start_time;
   long start_day;
};
void time_elapsed24(struct message_time *record_time);

struct message_time shot_complete_check_time;
struct message_time elapsed_setup_time;
struct message_time finallockout_check_count; 
struct message_time watchdog_check_count; 
struct message_time files_written_check_count;

/*
-------------------------------------------------------------------------------
Variables needed for communication with other processes.
-------------------------------------------------------------------------------
*/
int  waveport;
char wavehost[HOST_LENGTH];

int  lockport;
char lockhost[HOST_LENGTH];

int  msgport;
char msghost[HOST_LENGTH];

processor_type_mask local_processor_type = 0;


/*
******************************************************************************
SUBROUTINE: lockserver main
******************************************************************************
*/
void lockserver(int input_option,
		char *in_wavehost,
		int in_waveport,
		char *in_lockhost,
		int in_lockport,
		char *in_msghost,
		int in_msgport)
{
   /*
     Record the type of processor on which this process is executing.
   */
   get_processor(&local_processor_type);

   HARDWARE_AVAILABLE = input_option;
   TEST_MODE = 0;
   FORCED_TEST_MODE = 0;
   strcpy(wavehost,in_wavehost);
   waveport = in_waveport;
   strcpy(lockhost,in_lockhost);
   lockport = in_lockport;
   strcpy(msghost,in_msghost);
   msgport = in_msgport;
/*
------------------------------------------------------------------------------
initializations for special event handling
------------------------------------------------------------------------------
*/
   if(HARDWARE_AVAILABLE) 
   {
	ls_event_init(1);
   }
   else
   {
        msg_log(PCSMSG,"*** lockserver HARDWARE interaction is OFF ***\n");
	ls_event_init(0);
   }

/*
------------------------------------------------------------------------------
Set up the input socket communication.
------------------------------------------------------------------------------
*/
   msg_log(PCSMSG,"Setting lockserver on fixed port = %d...\n",lockport);
   set_up_socket_fixed_port((int *)&lock_sock, (u_short)lockport,
                 "lockserver (main)");

/* 
------------------------------------------------------------------------------
Start accepting connections. 
------------------------------------------------------------------------------
*/
   listen(lock_sock, 5);

/*
------------------------------------------------------------------------------
Initialize the process state.
------------------------------------------------------------------------------
*/
   state = NULLSTATE;

/*
------------------------------------------------------------------------------
Initialize the lockout count.  This value increments each time a
a lockout message is sent to the waveform server.  This value is
sent with the lockout message and with the reply to the lockout question
message.  The host_realtime routine uses this value to be sure that the
waveform server has processed the last lockout message.  Note that there
is no provision to make sure that this value doesn't overflow.  However,
an overflow is very unlikely since the number of lockout messages in a
day is only one per shot plus a few sent by hand.
------------------------------------------------------------------------------
*/
   lockout_count = 0;

/*
------------------------------------------------------------------------------
Get some needed information from external sources
------------------------------------------------------------------------------
*/
   do
   {
       rtcpu_count = get_cpu_count();

       if(rtcpu_count == -1)
       {
	  msg_log(PCSMSG,"lockserver waiting for waveserver to get ready...\n");
	  sleep(5);
       }
   } while(rtcpu_count == -1);
   msg_log(PCSMSG,"lockserver rtcpu count = %d\n",rtcpu_count);

/*
------------------------------------------------------------------------------
Allocate cpu status storage space.
------------------------------------------------------------------------------
*/
   cpustat = 
    (struct cpu_status *)malloc(rtcpu_count * sizeof(struct cpu_status));
   memset(cpustat, 0, sizeof(struct cpu_status) * rtcpu_count);

/*
------------------------------------------------------------------------------
installation specific routine for hardware initializations
------------------------------------------------------------------------------
*/
   if(HARDWARE_AVAILABLE) ls_init_hardware(rtcpu_count);

/*
------------------------------------------------------------------------------
Go handle i/o and trigger signals.  This routine never returns.
This process exits when the physics operator user interface process exits.
------------------------------------------------------------------------------
*/
   msg_log(PCSPID,"lockserver ready... PID = %d\n",getpid());

   ls_io_handler();

   msg_fini();
}


/*
******************************************************************************
SUBROUTINE: handle_state

Perform actions that depend on the current processing state.
This is called at periodic intervals.
1.  If the current state is NULLSTATE, FIRSTLOCKOUT, or UNLOCKED, call 
the installation-specific function ls_hardware_flags to look for the
events "get ready for shot", "final lockout" and "abort".  If any of
these events are reported, the action taken depends on the current state.
2.  If the current state is FIRSTLOCKOUT, checked to see if any CPUs
are taking too long with the shot setup procedure.  If any delayed
CPUs are found, send a complaint to the message log.
******************************************************************************
*/
int handle_state()
{
   int i,found_flag;

   switch(state){
   case NULLSTATE:
   case FIRSTLOCKOUT:
   case UNLOCKED:
      /* 
	 -----------------------------------------------------------------
	 If the present state is FIRSTLOCKOUT,
	 keep track of how long it is taking for the cpus to do their shot 
	 setups.  and if it exceeds our expected time, begin flagging the user.
	 -----------------------------------------------------------------
       */
      if(state == FIRSTLOCKOUT){
	 found_flag = 0;
	 for(i = 0; i < rtcpu_count; i++)
	 {
	    if(cpustat[i].shot_setup_done != 1)
	    {
	       found_flag = 1; /* found a cpu not finished with it's setup */
	       break;
	    }
	 }

	 if(found_flag == 1)
	 {
	    time_elapsed24(&elapsed_setup_time);

	    if(elapsed_setup_time.reference > 80)
	    { 
	       if((elapsed_setup_time.reference-
		   elapsed_setup_time.previous_message) > 5)
	       {
		  msg_log(PCSSTATE,
			  "Elapsed setup time = %d seconds...\n",
			  elapsed_setup_time.reference);
		  for(i = 0; i < rtcpu_count; i++)
		  {
		     if(cpustat[i].shot_setup_done != 1) {
			cpustat[i].watchdog_flg = 1;
			msg_log(PCSSTATE,"CPU %d; NOT finished.\n",i+1);
		     }
		  }

		  elapsed_setup_time.previous_message = 
		     elapsed_setup_time.reference;
	       }
	    }
	 }
      }

      /*
	look for and respond to any incoming hardware signals
      */
      if(HARDWARE_AVAILABLE && !TEST_MODE)
      {
         int HW_FIRST_LOCKOUT=0,HW_ABORT=0,HW_FINAL_LOCK=0;

	 ls_hardware_flags(&HW_FIRST_LOCKOUT,&HW_ABORT,&HW_FINAL_LOCK);

	 /*
	   What we are interested in at this point depends on the 
	   state of this process.
	 */
	 switch(state)
	 {
	 case NULLSTATE:
	    if(HW_FIRST_LOCKOUT)
	       goto_firstlockout();
	    break;

	 case FIRSTLOCKOUT:
	    /* if there was a hardware abort, the shot will not continue */
	    if(HW_ABORT)
	    {
	       /* if all shot setups are complete at this point and an abort
		  is detected we should immediately abort. */
	       found_flag = 0;
	       for(i = 0; i < rtcpu_count; i++)
	       {
		  if(cpustat[i].shot_setup_done != 1)
		  {
		     found_flag = 1;
		     break;
		  }
	       }
	       if(found_flag == 0)
		  goto_aborted();
	       else
	       /* else, instead of immediately going to aborted state, set a
		  global flag to indicate that the PCS should transition
		  to the aborted state as soon as all cpus have finished
		  their setups.  it is necessary for all cpus to have their
		  setups complete in order for the abort sequence to function
		  properly. */
		  abort_pending = 1;

	       break;
	    }
	    /*
	      We go to final lockout only if all rt cpus
	      have reported that they have completed setup for the shot.
	    */
	    if(HW_FINAL_LOCK)
	    {
	       found_flag = 0;
	       for(i = 0; i < rtcpu_count; i++)
	       {
		  if(cpustat[i].shot_setup_done != 1)
		  {
		     found_flag = 1;
		     break;
		  }
	       }
	       if(found_flag == 0)
		  goto_finallock();
	       else
		  finallock_pending = 1;
	    }
	    break;

	 case UNLOCKED:
	    /* if there was a hardware abort, the shot will not continue */
	    if(HW_ABORT){
	       /*
		 In the UNLOCKED state, the command to abort is treated as
		 a RELOCK message and the abort is set pending.  The abort
		 will actually happen only in the FIRSTLOCKOUT state.
	       */
	       abort_pending = 1;
	       goto_firstlockout();
	       break;
	    }
	    /*
	      we have reached final lockout. immediately go to lockout again.
	    */
	    if(HW_FINAL_LOCK){
	       finallock_pending = 1;
	       goto_firstlockout();
	    }
	    break;
	 } /* switch(state) */
      } /* if(HARDWARE_AVAILABLE && !TEST_MODE) */
      /*
	If something above caused the state to switch to FINALLOCKOUT,
	then continue.  Otherwise, we are done.
      */
      if(state != FINALLOCKOUT)
	 break;
    case FINALLOCKOUT:
    /*
    ------------------------------------------------------------------------
    Make certain that all of the CPUs have acknowledged knowing that the state
    has shifted to FINALLOCKOUT.  Each CPU does this
    by sending the REALTIME_ROUTINE_STARTING message.

    Then, call the installation-specific function
    ls_cpus_ready_for_shot.
    ls_cpus_ready_for_shot reports whether all real time processors have
    gotten ready for the
    real time portion of the shot cycle to begin.

    This is typically used to check whether the real time processors
    have gotten to the point that they are successfully toggling
    their watchdog timer bit.

    The hardware is not accessed in test modes.

    After the DURINGSHOT state is reached, tell the waveserver to write the
    shot setup pointname. This allows the waveserver to take advantage of the 
    time during the shot when the host computer shouldn't be very busy. 

    Note: At this point there is an unresolved problem with the ability to
    abort the shot.  If something happens so that the realtime routine never
    gets ready, then host_realtime is effectively hung and we will be stuck
    waiting to go into DURINGSHOT state.  There is no way to get out of this
    other than by totally restarting the plasma control system code.
    Even if we check the abort bit now while waiting for DURINGSHOT, there
    is no way to signal host_realtime that the shot is aborted because
    host_realtime is sitting waiting for the realtime code to finish.  
    Similarly, allowing the operator to send an abort command won't work.
    However,
    the realtime code is probably not going to finish either because it is hung
    somehow.
    ------------------------------------------------------------------------
    */
    {
       int which_cpu,found_flag;
/*
Check for acknowledgment of final lockout.
*/
       found_flag = 0;
       for(which_cpu = 0; which_cpu < rtcpu_count; which_cpu++)
	  if(cpustat[which_cpu].realtime_code_started == 0)
	     found_flag = 1;

       if(found_flag == 1){
/*
Not all CPUs have acknowledged final lockout.
*/
	  time_elapsed24(&finallockout_check_count);
	  if( (finallockout_check_count.reference > 2) && 
	      (finallockout_check_count.reference - 
	       finallockout_check_count.previous_message > 3) ){
	     finallockout_check_count.previous_message = 
		finallockout_check_count.reference;
	     msg_log(PCSSTATE,"Lock server problems: realtime computers"
		     " are not acknowledging final lockout, after %d secs.\n",
		     finallockout_check_count.reference);
	     for(which_cpu = 0; which_cpu < rtcpu_count; which_cpu++)
	     {
		if(cpustat[which_cpu].realtime_code_started == 0) {
		  cpustat[which_cpu].watchdog_flg = 1;
		  msg_log(PCSMSG,"CPU %d; NOT acknowledged.\n",which_cpu+1);
		} else
		  msg_log(PCSMSG,"CPU %d; has acknowledged.\n",which_cpu+1);
	     }
	  }
       }
       else {
/*
All CPUs have acknowledged final lockout.

If the proper hardware is available and this is not a test mode shot,
check to make certain that all real time CPUs are toggling the watchdog bits.
*/
	  if(HARDWARE_AVAILABLE && !TEST_MODE){
	     if(ls_cpus_ready_for_shot(cpu_list,rtcpu_count)==0){
		ls_send_event("READY");
		goto_duringshot();
	     }
	     else{
		time_elapsed24(&watchdog_check_count);
		if( (watchdog_check_count.reference > 10) && 
		    (watchdog_check_count.reference - 
		     watchdog_check_count.previous_message > 3) ){
		   watchdog_check_count.previous_message = 
		      watchdog_check_count.reference;
		   msg_log(PCSSTATE,"Lock server problems: realtime computers"
			   " are not getting ready, after %d secs.\n",
			   watchdog_check_count.reference);
		   for(which_cpu = 0; which_cpu < rtcpu_count; which_cpu++){
		      if(cpu_list[which_cpu]==0) {
			 msg_log(PCSMSG,"CPU %d; ready\n",which_cpu+1);
                         cpu_list[which_cpu] = 2; /* don't say again */
		      } else if(cpu_list[which_cpu]==1){
			 msg_log(PCSSTATE,"CPU %d; NOT ready\n",which_cpu+1);
                      }
		   }
		}
	     }
	  }
	  else{ /* No hardware available or this is in test mode */
	     ls_send_event("READY");
	     goto_duringshot();
	  }
       }
    }
    break;

    case DURINGSHOT:
    /* 
    ------------------------------------------------------------------------
    Keep track of how much time has elapsed from when the first real time
    CPU reported that the shot is complete.  If the elapsed time exceeds
    a specified limit, then begin reporting warnings to the message server.

    NOTE: this won't generate any warnings if there is a problem with 
    all of the cpus not finishing the shot. Once the DURINGSHOT state is
    reached, the PCS simply waits until one of the real time CPUs reports
    that the shot is complete (or aborted).  The PCS places no limits
    on the duration of the shot.

    Here the time delay allowed after the end of shot notice from the first
    CPU to finish before warning messages are issued can be 
    installation-dependent.  The default interval is 20 seconds.  However,
    if the macro SHOT_COMPLETE_WARNING_DELAY is defined in installdefs.h,
    its value is used to set the interval (in seconds).    

    After the time critical 
    portion of the discharge is completed, the control algorithms can execute
    some finish-up code on the real time CPU.  If this code takes a long
    time because of some installation-specific computations, it might be
    normal for one or more CPUs to take some extra time to complete the shot.
    So, it might be necessary to define an installation-specific value of
    SHOT_COMPLETE_WARNING_DELAY.

    After the warning delay interval, warning messages are issued for
    10 seconds and, simultaneously, a signal is set to the offending
    host_realtime process to attempt to force it to finish the shot.  After
    10 seconds we simply give up on the delayed CPUs and move ahead in the
    shot sequence.
    ------------------------------------------------------------------------
    */
    {
	int num_cpus_done;
	extern int threshold_delay;
	int finish_delay = 10;

	/* how many cpus have completed their shot processing ? */
	num_cpus_done = 0;
	for(i = 0; i < rtcpu_count; i++)
	    if(cpustat[i].shot_complete != 0) num_cpus_done++;

        if(num_cpus_done >= 1 && num_cpus_done != rtcpu_count)
	{
	    time_elapsed24(&shot_complete_check_time);

	    if( (shot_complete_check_time.reference > threshold_delay) &&
		(shot_complete_check_time.reference  - 
		 shot_complete_check_time.previous_message > 3) )
	    {
	     msg_log(PCSSTATE,"Lock server problems: realtime computers"
			    " are not finishing shot\n");
	     msg_log(PCSMSG,"%d sec elapsed\n",
			     shot_complete_check_time.reference);
	     shot_complete_check_time.previous_message = 
		shot_complete_check_time.reference;
	     /*
	     one or more cpus are not completing the shot, attempt
	     to force each of the problem processes to stop by sending
	     a special stop signal to them.
	     */
	     for(i = 0; i < rtcpu_count; i++)
	     {
		if(cpustat[i].shot_complete == 0)
		{
		  cpustat[i].watchdog_flg = 1;
		  msg_log(PCSSTATE,"CPU %d; NOT finished with the shot.\n",
			  i+1);
		  if(cpustat[i].host_pid != 0)
		      kill(cpustat[i].host_pid,SIGUSR1);
		}
	     }

	     /*
	     if this is taking way too long, go to the next state.
	     */
	     if(shot_complete_check_time.reference > 
		threshold_delay + finish_delay)
             {
		 for(i = 0; i < rtcpu_count; i++)
		 {
		/* fill these so that we don't wait for hung cpu to write */
		    if(cpustat[i].shot_complete == 0) {
		      cpustat[i].files_written = 1;
		      cpustat[i].watchdog_flg  = -1;
                    }
		 }
		 goto_writingfiles();
             }
	    }
	} /* if(num_cpus_done >= 1 && num_cpus_done != rtcpu_count) */
        else if(HARDWARE_AVAILABLE && !TEST_MODE)
        {
           int HW_FIRST_LOCKOUT=0,HW_ABORT=0,HW_FINAL_LOCK=0;
           ls_hardware_flags(&HW_FIRST_LOCKOUT,&HW_ABORT,&HW_FINAL_LOCK);
           if (HW_ABORT) late_abort_pending = 1;
        }
    }
    break;

    case WRITINGFILES:
    /* 
    ------------------------------------------------------------------------
    keep track of how long it is taking for the current cpu given permission
    to write to the archive files to complete it's writing, and if it exceeds 
    expected time, begin notifying the user.
    ------------------------------------------------------------------------
    */
       time_elapsed24(&files_written_check_count);
       if( (files_written_check_count.reference > 100) &&
	   (files_written_check_count.reference - 
	    files_written_check_count.previous_message > 5) ) {
	  cpustat[writefile_cpu - 1].watchdog_flg = 1;
	  msg_log(PCSMSG,"CPU %d; delayed writing files,"
		         "elapsed time =%d secs.\n",
		  writefile_cpu,files_written_check_count.reference);
	  files_written_check_count.previous_message = 
	     files_written_check_count.reference;
       }
    break;

    default:
    break;

   } /* end of switch(state) */
   return(0);
}


/*
******************************************************************************
SUBROUTINE: ls_io_handler

This routine handles the polling cycle for the lock server.
The select routine is used with a timeout to check for the
socket being ready for communication. If so, the communication
is handled.  If not, the installation-specific function
ls_hardware_flags is called to
look for events.  Then the function loops back to call
the select routine again with a timeout.
******************************************************************************
*/
void ls_io_handler()
{
   int msgsock,status;
   struct timeval to;
   processor_type_mask client_type;

   time_t last_ping = 0, cur_time;
   int cnt, *hstats;

   char *message_storage;
   int *message_ints;
   int message_cpu,message_pid;
   int message_length,message_type;
   int int_count;

/*
------------------------------------------------------------------------------
Here is the main loop from which we never exit.
------------------------------------------------------------------------------
*/
   while(TRUE)
   {
      /*
      -----------------------------------------------------------------------
      Check to see if the socket is ready to make a connection.
      Here we wait up to 1 second for a process to try to make a connection
      to the input socket.  If after 1 second there is no connection attempt,
      we continue.
      -----------------------------------------------------------------------
      */
     to.tv_sec = 1;
     to.tv_usec = 0;
     status = wait_for_input_connection(lock_sock, &to, 
					"Lockout server (io_handler)",
					&msgsock);
     if(status == -1){
       fprintf(stderr,"Lockout server (io_handler), accept");
       fflush(stderr);
       exit(1);
     }
     else if(status == 1){
       /*
	 ------------------------------------------------------------
	 Get requestor (client) type, the length of the message in bytes,
	 allocate that much memory, and read the remainder of the message.
	 ------------------------------------------------------------
       */
       server_get_message(msgsock, 
			  &client_type,
			  &message_storage, 
			  &message_length,
			  "Lock server (io_handler)");
       /*
	 -------------------------------------------------------------------
	 The message is assumed to consist of an array of type int.
	 However, the number of elements in the array could vary.  So,
	 here we determine how many ints there are and convert them into
	 the local format from the format of the client processor.
	 -------------------------------------------------------------------
       */
       {
	 GEN_int
	 int size,alignment;

	 /* Determine size of an int on the client processor. */
	 D_int->size = 1;
	 convert_size(D_int,0,client_type,TASK_ALL,0,
		      NULL,&size,&alignment);

	 /* Find the count of ints in the input message and
	    convert the data to the local format. */
	 int_count = message_length/size;
	 D_int->size = int_count;
	 message_ints = (int *)convert_data(message_storage,D_int,0,
					    client_type,
					    local_processor_type,
					    NULL,TASK_ALL,
					    NULL,NULL,NULL,NULL);
       }
       /*
	 -------------------------------------------------------------------
	 Extract the type of the message and take care of it.  The first 
	 int in the message always contains the message type code.  Some 
	 messages consist only of a message type code.  

	 Messages from host_realtime have the cpu number and pid of the 
	 host_realtime process in the second and third ints of the message.

	 Messages from the waveserver may, if necessary for a particular
	 message, have -1 in the second and third 
	 ints to indicate that the message is from the waveserver.
	 -------------------------------------------------------------------
       */
       message_type = message_ints[0];

       if(int_count > 1)
	 message_cpu = message_ints[1];
       else
	 message_cpu = 0;

       if(int_count > 2)
	 message_pid = message_ints[2];
       else
	 message_pid = 0;
            
       if(message_cpu > 0)
	 cpustat[message_cpu - 1].host_pid = message_pid;
       /*
	 ------------------------------------
	 perform processing of input message 
	 ------------------------------------
       */
       ls_handle_message(message_type,message_cpu,message_pid,
			 msgsock,client_type);

       /*
	 ------------------------------------------------------------
	 We are finished with the message, so free the storage space.
	 ------------------------------------------------------------
       */
       free(message_storage);
       free(message_ints);

       /*
	 ------------------------------------------------------------
	 Now we are finished with the socket connection.
	 ------------------------------------------------------------
       */
       close(msgsock);
     }

     /*
       ------------------------------------------------------------------------
       What is done now, before looping back to try for another socket message,
       depends on the process state.
       ------------------------------------------------------------------------
     */
     handle_state();

     ls_event_cleanups();

     /*
       ------------------------------------------------------------------------
       Send status of lockserver and realtime processes to msgserver
       ------------------------------------------------------------------------
     */
     hstats = (int *) malloc(rtcpu_count * sizeof(int)); 
     if (hstats == NULL) {
       fprintf(stderr,"Lockout server (io_handler), unable to malloc space\n");
       ping_msg_srvr(0, NULL);
     } else {
       memset(hstats, 0, rtcpu_count * sizeof(int)); 
       cur_time = time(NULL);
       if ((cur_time - last_ping) >= 10) {
         last_ping = cur_time;
         if (cpustat != NULL)
	   for (cnt = 0; cnt < rtcpu_count; cnt++)
	     hstats[cnt] = cpustat[cnt].watchdog_flg;
         ping_msg_srvr(rtcpu_count, hstats);
       }
       free(hstats);
     }

   } /* end of while loop: go back to look for more socket i/o. */
}

/*
******************************************************************************
SUBROUTINE: handle_shot_finished_message
Perform processing required to determine whether all CPUs have 
completed the shot.  If so, switch to the WRITINGFILES state.
******************************************************************************
*/
void handle_shot_finished_message(int message_cpu)
{
  int i,all_cpus_finished;
   /* 
      ----------------------------------------
      Record the cpu that finished.
      If any cpus have not finished yet, 
      then don't do the shot completion stuff.
      ----------------------------------------
    */
   cpustat[message_cpu - 1].shot_complete = 1;
   cpustat[message_cpu - 1].watchdog_flg  = -1;
   all_cpus_finished = TRUE;
   for(i = 0; i < rtcpu_count; i++){
      if(cpustat[i].shot_complete != 1){
	 all_cpus_finished = FALSE;
	 break;
      }
   }
   if(all_cpus_finished == FALSE)
      return;

   /*
     ----------------------------------------------------------
     We get here after all of the cpus have reported in that
     they were finished with the shot.  Switch to the
     WRITINGFILES state.

     If any of the cpus reported that the shot was aborted,then 
     we classify the shot as aborted.
     ----------------------------------------------------------
   */
   for(i = 0; i < rtcpu_count; i++){
      if(cpustat[i].aborted == 1)
	 shot_was_aborted = 1;
   }
   if(shot_was_aborted == 1){
      ls_send_event("ABORT");
      msg_log(PCSSTATE,"SHOT %d WAS ABORTED\n",shot_number);
   }
   else{
      msg_log(PCSSTATE,"SHOT %d REALTIME CPUS FINISHED.\n",
	      shot_number);
   }
   goto_writingfiles();

   return;
}

/*
******************************************************************************
SUBROUTINE: handle_message

Perform processing for input message type.

Valid messages are:
SHUTDOWN_LOCKSERVER
SET_FIRSTLOCK
SET_UNLOCK
SET_RELOCK
SHOTSETUP_COMPLETE
ABORT_SHOT
SET_FINALLOCK
STATE_QUESTION
SHOT_FINISHED_AND_ABORTED
SHOT_FINISHED
ARCHIVE_FILES_WRITTEN
START_TEST_SHOT
REALTIME_ROUTINE_STARTING
******************************************************************************
*/
void ls_handle_message(int message_type,int message_cpu,
		       int message_pid,int msgsock,
		       processor_type_mask client_type)
{
  int i;
  int error,found_flag;

  switch(message_type)
  {
    case SHUTDOWN_LOCKSERVER:
	msg_log(PCSMSG,"SHUTDOWN MESSAGE RECEIVED.  EXITING NOW!\n");
	close(lock_sock);
        fflush(stderr);
	exit(0);
    break;

    case SET_FIRSTLOCK:
       msg_log(PCSMSG,"FIRSTLOCK message received.\n");
    /*
    --------------------------------------------------------------------------
    This message is for a special situation when the user wishes to do
    a transition to the lock state by hand.  For instance, if the get
    ready for shot trigger is broken or for testing.  
    Here, the transition from NULLSTATE to first lockout is made.

    If we are already in any lockout state, then this message is ignored.
    --------------------------------------------------------------------------
    */
	  if(state == NULLSTATE)
	     goto_firstlockout();
    break;

    case SET_UNLOCK:
       msg_log(PCSMSG,"UNLOCK message received.\n");
    /*
    --------------------------------------------------------------------------
    This message indicates a request to make the transition to the
    unlock state.
    --------------------------------------------------------------------------
    */
	  goto_unlock();
    break;

    case SET_RELOCK:
       msg_log(PCSMSG,"RELOCK message received.\n");
    /*
    --------------------------------------------------------------------
    This message would come after the user has sent an unlock
    message and is finished with the unlock state. 
    --------------------------------------------------------------------
    */
	  if(state == FIRSTLOCKOUT)
	  {
	     if(unlock_pending == 1)
		relock_pending = 1;
	  }
	  else if(state == UNLOCKED)
	  {
	     goto_firstlockout();
	  }
    break;

    case SHOTSETUP_COMPLETE:
       msg_log(PCSSTATE,"CPU %d; Shot setup complete\n",message_cpu);

    /*
    -------------------------------------------------------------------------
    An rt cpu is finished doing the shot setup that is performed after
    a transition to the firstlockout state.  It is not possible to make
    a transition out of firstlockout until all cpus have sent this message,
    so this message only occurs during firstlockout state.
    -------------------------------------------------------------------------
    */
	  if(state == FIRSTLOCKOUT)
	  {
	     cpustat[message_cpu - 1].host_pid        = message_pid;
	     cpustat[message_cpu - 1].shot_setup_done = 1;
             cpustat[message_cpu - 1].watchdog_flg    = -1;
	     found_flag = 0;
	     for(i = 0; i < rtcpu_count; i++)
		if(cpustat[i].shot_setup_done != 1)
		{
		   found_flag = 1;
		   break;
		}
	     /* if all cpus are finished with their setups */
	     if(found_flag == 0)
	     {
                msg_log(PCSSTATE, "All Shot Setup Complete\n");
		ls_send_event("SETUP DONE");

		if(abort_pending == 1)
		   goto_aborted();
		else if(unlock_pending == 1)
		   goto_unlock();
		else if(finallock_pending == 1)
		   goto_finallock();
	     }
	  }
    break;

    case ABORT_SHOT:
       msg_log(PCSSTATE,"ABORT message received. state = %d\n",state);
    /*
    -------------------------------------------------------------------------
    If the state is FIRSTLOCKOUT or UNLOCKED, the receipt of this message
    will cause the state to change to ABORTED.  (If the state is UNLOCKED,
    the state will go back to FIRSTLOCKOUT first.) The host_realtime routine 
    does the rest when the change of state is detected.

    If the state is FINALLOCKOUT or DURINGSHOT, the receipt of this message
    causes the late_abort_pending flag to be set.  In this case it is up
    to the real time processor to obtain the value of this flag, recognize
    the abort command, and abort the discharge.  The real time processor
    can obtain the flag value in collaboration with the host_realtime
    process by using the get_late_abort_pending_flag function.

    Otherwise, this message is ignored.
    -------------------------------------------------------------------------
    */
       if(state == FIRSTLOCKOUT){
	  /* if all shot setups are complete at this point
	     we should immediately abort. */
	  found_flag = 0;
	  for(i = 0; i < rtcpu_count; i++)
	  {
	     if(cpustat[i].shot_setup_done != 1)
	     {
		found_flag = 1;
		break;
	     }
	  }
	  if(found_flag == 0)
	     goto_aborted();
	  else
	  /* else, instead of immediately going to aborted state, set a
	     global flag to indicate that the PCS should transition
	     to the aborted state as soon as all cpus have finished
	     their setups.  it is necessary for all cpus to have their
	     setups complete in order for the abort sequence to function
	     properly. */
	     abort_pending = 1;
       }
       else if(state == UNLOCKED){
	  /*
	    In the UNLOCKED state, the message to abort is treated as
	    a RELOCK message and the abort is set pending.  The abort
	    will actually happen only in the FIRSTLOCKOUT state.
	  */
	  abort_pending = 1;
	  goto_firstlockout();
       }
       else if((state == FINALLOCKOUT) || (state == DURINGSHOT)){
msg_log(PCSSTATE,"sending ls_send_late_abort\n");
	  late_abort_pending = 1;
          ls_send_late_abort();
       }
       break;

    case SET_FINALLOCK:
       msg_log(PCSMSG,"FINALLOCK message received.\n");
   /*
    -------------------------------------------------------------------------
    This message would be used for testing and in the case that the watchdog
    board trigger circuits have failed and it is necessary to run the
    shot by hand. This message is ignored if we are not in the correct state.
    If the state is UNLOCKED, then this is treated like a relock message
    and the final lock is set pending.
    In firstlockout state, if all cpus have finished setup then the transition
    to finallock state is made, otherwise the final lock pending flag is set.
    -------------------------------------------------------------------------
    */
	  if(state == FIRSTLOCKOUT)
	  {
	     found_flag = 0;
	     for(i = 0; i < rtcpu_count; i++)
	     {
		if(cpustat[i].shot_setup_done == 0)
		{
		   found_flag = 1;
		   break;
		}
	     }
	     if(found_flag == 1)
		finallock_pending = 1;
	     else
		goto_finallock();
	  }
	  else if(state == UNLOCKED)
	  {
	     goto_firstlockout();
	     finallock_pending = 1;
	  }
    break;

    case STATE_QUESTION:
    {
       DATA_LIST_SETUP
       GEN_int
       int message[5];
       char *data;
    /*
    -------------------------------------------------------------------------
    Send the requester:
    1. the code for the current process state,
    2. the present value of the lockout count, 
    3. the present value of shot_number
    (valid only if the state is FIRSTLOCKOUT),
    4. the number of rt cpu that is presently allowed to write archive
    files (valid only if the state is WRITINGFILES or DURINGSHOT),
    5. the value of the late_abort_pending flag (relevant only if the state
    is FINALLOCKOUT or DURINGSHOT.
    -------------------------------------------------------------------------
    */
       message[0] = state;
       message[1] = lockout_count;
       message[2] = shot_number;
       message[3] = writefile_cpu;
       message[4] = late_abort_pending;

       D_int->size = 5;
       data = (char *)&message[0];
       ADD_TO_DATA_LIST(D_int,data)

       if(send_data_list(msgsock, data_queue) < 0) {
	  perror("Lock server (io_handler), send_data_list");
          fflush(stderr);
	  exit(1);
       }
    }
    break;

    /*
    return current date time information
    */
    case GET_DATE_TIME:
    {
       DATA_LIST_SETUP
       GEN_int
       int message[6];
       char *data;
       int month,day,year,hour,min,sec;

       get_localtime_ints(&month,&day,&year,&hour,&min,&sec);

       message[0] = year;
       message[1] = month;
       message[2] = day;
       message[3] = hour;
       message[4] = min;
       message[5] = sec;

       D_int->size = 6;
       data = (char *)&message[0];
       ADD_TO_DATA_LIST(D_int,data)

       if(send_data_list(msgsock, data_queue) < 0) {
	  perror("Lock server (io_handler), send_data_list");
          fflush(stderr);
	  exit(1);
       }
    }
    break;

    case SHOT_FINISHED_AND_ABORTED:
       msg_log(PCSSTATE,"CPU %d; Shot finished and aborted.\n",message_cpu);
    /*
    -----------------------------------------------------------------------
    This message is received from the host realtime routine when the shot
    finishes because it was aborted.  The abort might have been detected
    by the lockserver while waiting for final lockout or it might have been
    detected by the realtime code while waiting for the trigger to start
    the feedback cycle.  

    When all rt cpus have reported in then the shot is considered complete.
    At that point, the shot status is set and we do other stuff normally 
    done at the end of the shot.

    This message would normally be received during the DURINGSHOT or 
    ABORTED state. However, there is the small chance that a real time
    processor could detect an abort command before the lockout server can
    determine that all real time processors are ready for the shot.  In this
    case, the DURINGSHOT state would never be reached and this message will
    be received during the FINALLOCKOUT state.

    This message is ignored unless we are in the FINALLOCKOUT,
    DURINGSHOT, or ABORTED state.
    -----------------------------------------------------------------------
    */
       if( (state == FINALLOCKOUT) || (state == DURINGSHOT) || 
	   (state == ABORTED) ){
	  cpustat[message_cpu - 1].aborted       = 1;
	  handle_shot_finished_message(message_cpu);
       }
       break;

    case SHOT_FINISHED:
       msg_log(PCSSTATE,"CPU %d; Shot finished\n",message_cpu);
    /*
    -----------------------------------------------------------------------
    The shot finished message comes from the realtime host routine to indicate
    that the shot is over.  
    When all rt cpus have reported in then the shot is considered complete.
    At that point, the shot status is set and we do other stuff normally 
    done at the end of the shot.

    Normal shot completion stuff:
    Go to the writingfiles state.

    This message is ignored unless we are in the DURINGSHOT state.

    Note that because timing of an abort trigger with respect to the
    clock clear trigger or the moment of reaching PRIMARY_START_TIME cannot
    be guaranteed to be the same at all rt cpus, there is a small chance
    that some cpus would think the shot was aborted and some would run the
    shot normally.  If any of the cpus think that the shot was aborted then
    we report the shot as aborted.
    ------------------------------------------------------------------------
    */
       if(state == DURINGSHOT)
	  handle_shot_finished_message(message_cpu);
       break;

    case ARCHIVE_FILES_WRITTEN:
    if(message_cpu != -1) 
       msg_log(PCSSTATE,"CPU %d; Archive Files Written\n",message_cpu);
    /*
    ------------------------------------------------------------------------
    An rt cpu has finished writing its post-shot archive files and pointnames, 
    or the waveserver has finished writing the shot setup pointname.
    
    This message
    should arrive only if the state is DURINGSHOT or WRITINGFILES.

    If the it is the waveserver that just finished (message_cpu = -1) then
    change the value of writefile_cpu to indicate that the waveform server
    has finished archiving the PCS setup.

    If the waveserver sent this message and the state is WRITINGFILES
    respond back to the waveserver telling it that the shot is over.
    We do not want to tell the waveserver that the shot is over until
    after it has written the setup pointname so that none of the info going
    into the setup pointname is altered while the pointname is being written.

    If the shot was aborted, then no cpu will permission to write files.
    The host_realtime detects this and returns an ARCHIVE_FILES_WRITTEN message
    without doing anything. However, if the host_realtime process knows
    already about the abort, it will send back an ARCHIVE_FILES_WRITTEN
    message immediately after detecting the transition to the WRITINGFILES
    state.  So, this message might arrive before any call to
    test_writingfiles_complete (which is the function that would normally
    give permission to a host_realtime process to archive). In this case,
    we simply record which host_realtime process sent the message, but
    the shot is not considered complete until we also hear from the
    waveform server.

    After all cpus have reported in that they have finished writing to the
    archive file, then make the transition to the null state.
    ------------------------------------------------------------------------
    */
	  if(state == DURINGSHOT)
	  {
	     if(message_cpu == -1)
		writefile_cpu = 1;
	  }
	  else if(state == WRITINGFILES)
	  {
	     if(message_cpu == -1){
		writefile_cpu = 1;
		error = talk_to_waveserver(ENDOFSHOT);
		if(error){
		   msg_log(PCSMSG,
			   "ERROR sending ENDOFSHOT to waveserver."
			   " Exiting\n");
                   fflush(stderr);
		   exit(0);
		}
	     }
	     else {
		cpustat[message_cpu - 1].files_written = 1;
		cpustat[message_cpu - 1].watchdog_flg  = -1;
             }
	     /*
	       If we are not still waiting for the waveform server to
	       complete archiving the PCS setup, then test whether all of
	       the host_realtime processes have completed archiving.
	       If not, determine which process to give archiving permission to.
	       If so, switch to the NULL state.
	     */
	     if(writefile_cpu != WAVESERVER_WRITEFILE)
		test_writingfiles_complete();
	  }
	  break;

    case START_TEST_SHOT:
       msg_log(PCSSTATE,"Starting Test Shot\n");
    /*
    --------------------------------------------------------------------------
    The user has selected the start test shot option from the user interface.
    Force test mode to be on for the next shot. The consequence is that
    the shot number used will be the test shot number provided by the 
    user interface.
    ---------------------------------------------------------------------------
    */
       FORCED_TEST_MODE = 1;
       break;

    case REALTIME_ROUTINE_STARTING:
/*
Record for the correct CPU that this message has been received.
*/
     if(state == FINALLOCKOUT) {
        cpustat[message_cpu - 1].realtime_code_started = 1;
        cpustat[message_cpu - 1].watchdog_flg          = -1;
     }

     break;

    default: 
/*
Allow installation specific messages
*/
     if (ls_handle_install_message(message_type,message_cpu,message_pid,
				   msgsock,client_type) != 0)
	 msg_log(PCSMSG, "Lock server (io_handler), bad message type.\n");
     break;

  }
}



/*
******************************************************************************
SUBROUTINE: talk_to_waveserver

Send a message to the waveserver.  The message always consists
of three integers giving a code for what is happening here plus
the lockout count plus the present value of shot_number.

ARGUMENTS:
	message_code - from serverdefs.h, a message number to send
	to the waveserver.

GLOBALS:
	lockout_count
	shot_number
******************************************************************************
*/
int talk_to_waveserver(int message_code)
{
   int length;
   int message[3];

   length = 3 * sizeof(int);
   message[0] = message_code;
   message[1] = lockout_count;
   message[2] = shot_number;
   return pcs_send_message((char *)&message[0],length,wavehost,waveport,
          "Lockserver (talk_to_waveserver)");
}


/*
******************************************************************************
SUBROUTINE: get_cpu_count

Get the number of rt cpus in use from the waveserver.
******************************************************************************
*/
int get_cpu_count()
{
   struct data_request message;
   int reply_length;
   int *reply;
   int count;
/*
Request the cpu count from the waveform server.  
*/
   message.message_type = DATA_REQUEST;
   message.data_type = CPU_COUNT;
   message.identifier1 = 0;
   message.identifier2 = 0;
   message.identifier3 = 0;
   message.identifier4 = 0;
   message.identifier5 = 0;

   reply = (int *)exchange_message((char *)&message,
                                sizeof(struct data_request),
                                &reply_length,
                                wavehost,
                                waveport,
                                "lockserver (get_cpu_count)");

/*
couldn't get rtcpu count from waveserver so return error
*/
   if(reply == NULL) return(-1);

/*
success, return rtcpu count
*/
   count = *reply;
   free(reply);
   return count;
}


/*
******************************************************************************
SUBROUTINE: get_host_names

Get the host names being used from the waveserver.
The host names are returned as a set of null-terminated strings concatenated
together with a null string to mark the end.
******************************************************************************
*/
char *get_host_names()
{
   struct data_request message;
   int reply_length;
   char *reply;
/*
Request the host names from the waveform server.  
*/
   message.message_type = DATA_REQUEST;
   message.data_type = HOST_NAMES_LIST;
   message.identifier1 = 0;
   message.identifier2 = 0;
   message.identifier3 = 0;
   message.identifier4 = 0;
   message.identifier5 = 0;

   reply = (char *)exchange_message((char *)&message,
                                sizeof(struct data_request),
                                &reply_length,
                                wavehost,
                                waveport,
                                "lockserver (get_host_names)");
/*
couldn't get host names from waveserver so return error
*/
   if(reply == NULL) return(NULL);

/*
success, return host names
calling process must free the memory
*/
   return reply;
}


/*
******************************************************************************
SUBROUTINE: process_is_alive
Check if process for given process id is still alive and running.
If it is, return 1.
If not return 0.

An input checkstring can be used to further evaluate the process to make
sure it is the correct one.
******************************************************************************
*/
int
process_is_alive(checkpid,checkstring)
int checkpid;
char *checkstring;
{
    FILE *ptr;
    char syscom[512];
    char buf[512];
    int currentpid;

#ifdef SOLARIS_2
    sprintf(syscom,"/bin/ps -p %d -o pid,args",checkpid);
#else
    sprintf(syscom,"/bin/ps %d",checkpid);
#endif

    if ((ptr = popen(syscom, "r")) != NULL)
    {
	 while (fgets(buf, 512, ptr) != NULL)
	 {
	    sscanf(buf,"%d",&currentpid);
	    if(checkpid == currentpid) 
	    {
		if(checkstring != NULL)
		{
		    if(strstr(buf,checkstring) != NULL)
		    {
			pclose(ptr);
			return(1);
		    }
		}
		else
		{
		    pclose(ptr);
		    return(1);
		}
	    }
	 }
	 pclose(ptr);
    }

    return(0);
}



/*
*****************************************************************************
SECTION: STATE TRANSITIION ROUTINES
*****************************************************************************
*/


/*
******************************************************************************
SUBROUTINE: goto_nullstate

Make the transition from writingfiles to null state.
******************************************************************************
*/
void goto_nullstate()
{
   if(late_abort_pending)
      msg_log(PCSMSG,"The late_abort_pending flag was set.\n");
/*
------------------------------------------------------------------------------
reset some variables
------------------------------------------------------------------------------
*/
   writefile_cpu = 0;
   TEST_MODE = 0;
   FORCED_TEST_MODE = 0;
   abort_pending = 0;
   late_abort_pending = 0;

#if 0
/*
------------------------------------------------------------------------------
note: this does not always work the way we expect it to, for now disable it
until we can work out all the bugs.

make sure all host realtime processes are still running, if any
are not running then we need to restart them.
------------------------------------------------------------------------------
*/
{ 
    char syscom[512];
    char exe_dir[512];
    int host_cpu_num;
    char mode[25];
    int ii;

    /*
    give any host realtime processes which have been forced to stop 
    sufficient time to be removed from the process table.
    */
    sleep(2);

    /*
    loop through host realtime processes
    */
    for(ii = 0; ii < rtcpu_count; ii++)
    {

	/*
	zero out the cpu status structure for current cpu
	*/
        cpustat[ii].watchdog_flg          = 0;
        cpustat[ii].shot_setup_done       = 0;
        cpustat[ii].realtime_code_started = 0;
        cpustat[ii].shot_complete         = 0;
        cpustat[ii].files_written         = 0;
        cpustat[ii].aborted               = 0;

#if !defined(LINUXOS)
	/*
	has the process stopped running?
	*/
	if(!process_is_alive(cpustat[ii].host_pid,"host_realtime"))
	{
	    /*
	    if so, startup a new host real time process to replace it
	    */
	    if (getenv("EXE_DIR") != NULL)
              strcpy(exe_dir, (char *)getenv("EXE_DIR"));
            else
              strcpy(exe_dir, exedir);

	    if (getenv("RUNMODE") != NULL)
              strcpy(mode, (char *)getenv("RUNMODE"));
            else
              strcpy(mode,"OPS");

	    host_cpu_num = ii+1;
	    sprintf(syscom,
              "nohup %s/host_realtime_cpu%d %s %s %d %s %d %s %d > /dev/null &",
              exe_dir, host_cpu_num, mode, wavehost, waveport, lockhost,
              lockport, msghost, msgport);

	    msg_log(PCSMSG,"LOCKSERVER: restarting host_realtime_cpu%d\n",
		    host_cpu_num);

	    system(syscom);
	}
#endif
    }
}
#endif

/*
------------------------------------------------------------------------------
hardware resets
------------------------------------------------------------------------------
*/
   if(HARDWARE_AVAILABLE && !TEST_MODE) ls_reset_hardware();

   state = NULLSTATE;

   return;
}

/*
******************************************************************************
SUBROUTINE: goto_firstlockout

Make the transition from another state to the FIRSTLOCKOUT state.
******************************************************************************
*/
void goto_firstlockout()
{
   int i,error;

/*
------------------------------------------------------------------------------
get the parameter data from the waveserver so we can see if the operating
mode has changed and if the user wants to use his own shot number.

Note: the way this is done is really a bug.  Nominally, we shouldn't
get any setup information from the waveform server until after the
waveform server has stopped processing messages in its queue in response
to seeing a lockout message.  Then we would know that the setup information
being obtained is properly synchronized with the lockout event.  However, here
we are in the process of responding to the lockout event.   We would
normally send a lockout message to the waveform server containing the
shot number.  However, here we actually could be getting the
shot number from the waveform server if this is a test mode shot.
So, we need to know the shot number before sending the lockout message but
to do this properly we can't get the shot number before sending the lockout
message. This really only
affects developers doing test shots, though.  So, we are getting away
with this improper implementation for the time being.
------------------------------------------------------------------------------
*/
{
   struct diagnostic_infra *setup_infra;
   struct data_request message;
   int reply_length;

   message.message_type = DATA_REQUEST;
   message.data_type = PARAMETER_DATA;
   message.identifier1 = -1;
   message.identifier2 = -1;
   message.identifier3 = DOQUEUE;
   message.identifier4 = 0;
   message.identifier5 = 0;

   setup_infra = (struct diagnostic_infra *)exchange_message(
		    (char *)&message,
		    sizeof(struct data_request),
		    &reply_length,
		    wavehost,  /* globally defined */
		    waveport,  /* globally defined */
		    "lockserver: goto_firstlockout"); 

   if(setup_infra == NULL)
   {
        msg_log(PCSMSG,
		"ERROR reading setup_infra from waveserver.  Exiting\n");
        fflush(stderr);
	exit(0);
   }

   TEST_MODE = setup_infra->software_test | setup_infra->hardware_test;
   TEST_SHOT_NUMBER = setup_infra->test_shot;

   free(setup_infra);
}

/*
------------------------------------------------------------------------------
Clear the status bits for each of the rt cpus.
If we are making a transition from the NULLSTATE , clear everything.
Otherwise, we have already been in the firstlockout state for this shot
and we want to keep open the possibility of aborting the shot so we
only need to clear the setup done flags to ensure that each rt cpu does
the setup again after the new transition to firstlockout.
------------------------------------------------------------------------------
*/
   if(state == NULLSTATE)
   {
      for(i = 0; i < rtcpu_count; i++)
      {
         cpustat[i].watchdog_flg          = -1;
         cpustat[i].shot_setup_done       = 0;
         cpustat[i].realtime_code_started = 0;
         cpustat[i].shot_complete         = 0;
         cpustat[i].files_written         = 0;
         cpustat[i].aborted               = 0;
      }
   }
   else
   {
      for(i = 0; i < rtcpu_count; i++)
         cpustat[i].shot_setup_done = 0;
   }

/*
------------------------------------------------------------------------------
Clear the various _pending flags.
------------------------------------------------------------------------------
*/
   if(state == NULLSTATE)
   {
      unlock_pending = 0;
      finallock_pending = 0;
      relock_pending = 0;
      shot_was_aborted = 0;
   }

/*
------------------------------------------------------------------------------
hardware resets
------------------------------------------------------------------------------
*/
   if( (HARDWARE_AVAILABLE && !TEST_MODE) && (state == NULLSTATE) )
   {
	ls_firstlockout_hardware();
   }

/*
------------------------------------------------------------------------------
Get the shot number for the coming shot.
This shot number is passed to the waveserver and host_realtime routines
so that all processes agree on the shot number.

If the shot number cannot be obtained, try several times before giving up.
------------------------------------------------------------------------------
*/
   if(state == NULLSTATE)
   {
      shot_number = ls_get_shot_number(
          TEST_MODE|FORCED_TEST_MODE,TEST_SHOT_NUMBER);

      msg_log(PCSPRGRS, "Commencing setup for SHOT=%d \n", shot_number);
   }

/*
------------------------------------------------------------------------------
Change the state and tell other processes about it.
------------------------------------------------------------------------------
*/
   if(state == NULLSTATE)
      ls_send_event("LOCKOUT");
   else
      ls_send_event("RELOCK");

   state = FIRSTLOCKOUT;
   lockout_count = lockout_count + 1;
   error = talk_to_waveserver(LOCKOUT_IN);
   if(error){
      msg_log(PCSMSG,
	      "ERROR sending LOCKOUT_IN to waveserver."
	      " Exiting\n");
      fflush(stderr);
      exit(0);
   }
/*
------------------------------------------------------------------------------
All done.
------------------------------------------------------------------------------
*/
   elapsed_setup_time.reference = 0;
   elapsed_setup_time.previous_message = 0;

   msg_log(PCSMSG,"*****************************************************\n");
   msg_log(PCSSTATE,"NEW STATE: FIRST LOCKOUT\n");
   msg_log(PCSMSG,"*****************************************************\n");

   return;
}


/*
******************************************************************************
SUBROUTINE: goto_unlock

Make a transition to the unlock state.  This occurs immediately
in response to a SET_UNLOCK message or there could be a delayed
response.

A SET_UNLOCK message is ignored unless we are in the first lockout state.
In null state an unlock makes no sense, in final lockout or during the shot
it is illegal to unlock. If we are already unlocked then we don't want
to send any more unlock messages.  
We can only unlock if all rt cpus have finished the preshot setup started
after lockout.  If all cpus are not finished, simply note that an unlock
is pending.
******************************************************************************
*/
void goto_unlock()
{
   int error,i;

   if(state == FIRSTLOCKOUT)
   {
      unlock_pending = 1;
      for(i = 0; i < rtcpu_count; i++)
         if(cpustat[i].shot_setup_done != 1) return;

      unlock_pending = 0;
      state = UNLOCKED;
      error = talk_to_waveserver(UNLOCK_IN);
      if(error){
	 msg_log(PCSMSG,
		 "ERROR sending UNLOCK_IN to waveserver."
		 " Exiting\n");
         fflush(stderr);
	 exit(0);
      }

      ls_send_event("UNLOCK");

      msg_log(PCSMSG,
              "*****************************************************\n");
      msg_log(PCSSTATE,"NEW STATE: UNLOCK\n");
      msg_log(PCSMSG,
              "*****************************************************\n");

/*
This completes a transition to the unlocked state.  Now, if the relock_pending
flag is set, we want to make an immediate transition to the firstlockout
state.
*/
      if(relock_pending != 0)
      {
         relock_pending = 0;
         goto_firstlockout();
      }
   }

   return;
}


/*
******************************************************************************
SUBROUTINE: goto_finallock

Make a transition from the firstlockout to the finallock state.
******************************************************************************
*/
void goto_finallock()
{
int which_cpu;

   state = FINALLOCKOUT;
   finallockout_check_count.reference = 0;
   watchdog_check_count.reference = 0;
   finallockout_check_count.previous_message = 0;
   watchdog_check_count.previous_message = 0;
   delete_shotfile(shot_number);
   if(HARDWARE_AVAILABLE && !TEST_MODE) ls_prepare_for_finallockout();

   msg_log(PCSMSG, "*****************************************************\n");
   msg_log(PCSSTATE,"NEW STATE: FINAL LOCK\n");
   msg_log(PCSMSG, "*****************************************************\n");
/*
Initialize the cpu_list array to 1 (bad)
*/
   for(which_cpu = 0; which_cpu < rtcpu_count; which_cpu++)
       cpu_list[which_cpu] = 1;
   return;
}


/*
******************************************************************************
SUBROUTINE: goto_aborted

Make a transition to the aborted state.
******************************************************************************
*/
void goto_aborted()
{
   state = ABORTED;

   msg_log(PCSMSG, "*****************************************************\n");
   msg_log(PCSSTATE,"NEW STATE: ABORTED\n");
   msg_log(PCSMSG, "*****************************************************\n");

   return;
}


/*
******************************************************************************
SUBROUTINE: goto_duringshot

Make a transition from finallockout state to duringshot state.
******************************************************************************
*/
void goto_duringshot()
{
   int error;

   state = DURINGSHOT;
   msg_log(PCSSTATE,"READY FOR SHOT %d\n",shot_number);

   writefile_cpu = WAVESERVER_WRITEFILE;
   error = talk_to_waveserver(WRITEVAXDATA);
   if(error){
      msg_log(PCSMSG,
	      "ERROR sending WRITEVAXDATA to waveserver."
	      " Exiting\n");
      fflush(stderr);
      exit(0);
   }
   shot_complete_check_time.reference = 0;
   shot_complete_check_time.previous_message = 0;

   msg_log(PCSMSG, "*****************************************************\n");
   msg_log(PCSSTATE,"NEW STATE: DURINGSHOT\n");
   msg_log(PCSMSG, "*****************************************************\n");

   return;
}


/*
******************************************************************************
SUBROUTINE: goto_writingfiles

Make the transition to the writingfiles state.
******************************************************************************
*/
void goto_writingfiles()
{
   int error;
   state = WRITINGFILES;

   msg_log(PCSMSG, "*****************************************************\n");
   msg_log(PCSSTATE,"NEW STATE: WRITING FILES\n");
   msg_log(PCSMSG, "*****************************************************\n");

   /*
     --------------------------------------------------------------------
     If we are entering the WRITINGFILES state from the ABORTED 
     or FINALLOCKOUT state, the waveform
     server will never have been told to archive the PCS setup so
     writefile_cpu will not equal WAVESERVER_WRITEFILE. But, the
     waveform server will have been given a lockout command so it is still
     necessary to tell it that the shot is complete so that it will unlock.

     If we are entering the WRITINGFILES state from the DURINGSHOT state,
     the waveform server will have been told to archive the PCS setup.
     Before allowing the host_realtime processes to do archiving, we
     must make certain that the waveform server has completed archiving.

     If the waveserver has completed writing the setup pointname, then
     notify the waveserver that the shot is over.  We want the waveserver
     to finish writing the setup pointname before unlocking and starting
     to process messages again.

     Then, enable a real time CPU to archive data.
     ----------------------------------------------------------------------
   */
   if(writefile_cpu != WAVESERVER_WRITEFILE)
   {
      error = talk_to_waveserver(ENDOFSHOT);
      if(error){
	 msg_log(PCSMSG,
		 "ERROR sending ENDOFSHOT to waveserver."
		 " Exiting\n");
         fflush(stderr);
	 exit(0);
      }
      change_writefile_cpu();
   }

   return;
}

/*
******************************************************************************
SUBROUTINE: change_writefile_cpu

Alter the CPU enabled for writing files.  
If all CPUs have finished, return 1, otherwise return 0.

If the shot was aborted, the writefile_cpu flag will always be 0 to indicate
to the host_realtime processes that they should not actually archive
data.
******************************************************************************
*/
int  change_writefile_cpu()
{
   int i,all_cpus_finished;

   all_cpus_finished = 1;
   for(i = 0; i < rtcpu_count; i++){
      if(cpustat[i].files_written == 0){
	 if(shot_was_aborted == 1)
	    writefile_cpu = 0;
	 else
	    writefile_cpu = i + 1;
	 files_written_check_count.reference = 0;
	 files_written_check_count.previous_message = 0;
	 all_cpus_finished = 0;
	 break;
      }
   }

   return all_cpus_finished;
}


/*
******************************************************************************
SUBROUTINE: test_writingfiles_complete

Check to see if all CPUs have finished writing data files.  
If not, change the CPU enabled for writing files to one that has not finished.
If so, switch back to the null state.
******************************************************************************
*/
void test_writingfiles_complete()
{
   int all_cpus_finished;

   all_cpus_finished = change_writefile_cpu();
	        /*
	        ----------------------------------------------------
	        If all the cpus have finished writing to the 
	        archive files, close shotfiles and go to null state.
	        ----------------------------------------------------
	        */
   if(all_cpus_finished == 1) 
   {

      msg_log(PCSSTATE,"ARCHIVE FILES FOR SHOT %d WRITTEN\n",
	      shot_number);

      close_shotfile(shot_number,0);
      
      sleep(2); /* make sure this is LAST message printed */

      if(shot_was_aborted == 1)
      {
	 msg_log(PCSMSG,
		 "*****************************************************\n");
	 msg_log(PCSSTATE,
                 "SHOT %d ABORTED; PCS READY, WAITING FOR SHOT....\n",
                 shot_number);
	 msg_log(PCSMSG,
		 "*****************************************************\n");
	 msg_log(PCSMSG, " \n");
      }
      else
      {
	 msg_log(PCSMSG,
		 "*****************************************************\n");
	 msg_log(PCSSTATE,
                 "SHOT %d COMPLETED; PCS READY, WAITING FOR SHOT....\n",
                 shot_number);
	 msg_log(PCSMSG,
		 "*****************************************************\n");
	 msg_log(PCSMSG, " \n");
      }

      ls_send_event("COMPLETE");
      goto_nullstate();
   }

   return;
}

/*
*****************************************************************************
SECTION: clock routines
*****************************************************************************
*/


/*
*****************************************************************************
SUBROUTINE: mysleep

A version of sleep which does not rely on system interrupts.
*****************************************************************************
*/
int mysleep(int seconds)
{
struct message_time elapsed_time;

    elapsed_time.reference = 0;
    time_elapsed24(&elapsed_time);

    while( elapsed_time.reference < seconds)
    {
	time_elapsed24(&elapsed_time);
    }
    return 0;
}

/*
******************************************************************************
SUBROUTINE: get_localtime_ints
return the local time in integers for:
        month, day, year, hour, minute, second
ex:     1,     16,  1996, 11,   20,     22
******************************************************************************
*/
int get_localtime_ints(month,day,year,hour,min,sec)
int *month,*day,*year,*hour,*min,*sec;
{
#define MAXCHAR 1000
   char newtime[100];
   char      *ctime();
   time_t    time();

   int       i;
   time_t    timeofday;
   char      *loctime;

   /* get the local time */
   timeofday = time((time_t*)0);
   loctime   = ctime(&timeofday);

   /* loctime has a '\n' in it, so delete that character */
   i=0;
   while ((loctime[i] != '\n') && (i<MAXCHAR)) {
      newtime[i] = loctime[i];
      i++;
   }
   newtime[i] = '\0';

   {
        char str_month[4];
        sscanf(newtime,"%*s %s %d %d%*c%d%*c%d %d",
                str_month,day,hour,min,sec,year);
        if(strcmp(str_month,"Jan")==0) *month = 1;
        else if(strcmp(str_month,"Feb")==0) *month = 2;
        else if(strcmp(str_month,"Mar")==0) *month = 3;
        else if(strcmp(str_month,"Apr")==0) *month = 4;
        else if(strcmp(str_month,"May")==0) *month = 5;
        else if(strcmp(str_month,"Jun")==0) *month = 6;
        else if(strcmp(str_month,"Jul")==0) *month = 7;
        else if(strcmp(str_month,"Aug")==0) *month = 8;
        else if(strcmp(str_month,"Sep")==0) *month = 9;
        else if(strcmp(str_month,"Oct")==0) *month = 10;
        else if(strcmp(str_month,"Nov")==0) *month = 11;
        else if(strcmp(str_month,"Dec")==0) *month = 12;
   }
#undef MAXCHAR
   return(0);
}

/*
******************************************************************************
SUBROUTINE: time_elapsed24
24 hour clock counter which uses system clock time to keep track of number
of seconds from a given start period.

A call to this routine with *record_time.reference
set to 0 will initialize the clock
counter.  Successive calls will return the number of seconds elapsed
since the initialization into *record_time.reference.
******************************************************************************
*/
void time_elapsed24(struct message_time *record_time)
{
   int current_time,month,day,year,hour,min,sec;

    if(record_time->reference == 0)
    {
	get_localtime_ints(&month,&day,&year,&hour,&min,&sec);

	record_time->start_day = day;
	record_time->start_time = sec + (min*60) + (hour*60*60);
	record_time->reference = 1;
    }

    else
    {
	get_localtime_ints(&month,&day,&year,&hour,&min,&sec);

	if(day == record_time->start_day)
	    current_time = sec + (min*60) + (hour*60*60);
	else
	    current_time = sec + (min*60) + (hour*60*60) + (1*60*60*24);

	record_time->reference = current_time - record_time->start_time;
    }
}

