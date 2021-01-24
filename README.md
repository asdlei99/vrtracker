Profile, record and playback openvr events and state

The idea is that to be able to work in VR it's still nice for programmers to do most of their work in 2D.  The ability
to see the events arriving and state changing over time is part of the profiling features.  

Further to reproduce scenarios, provide mechanisms to capture and reference a mechanism to capture reference 
3D states and motion and then insert that back into a stream.  This is the record and playback part: 
Provide a set of alternative visualizers and controllers to allow the VR Session to be baselined and tweaked.

Implicit is the idea that a VR session consists of a serializable stream of events and state.  The bulk of the code here captures and serializes
these events and state.
