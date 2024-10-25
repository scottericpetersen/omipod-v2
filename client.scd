/* OMIPOD V2 WIP Proof-of-concept

Topology: many-to-many pods (Arduino) to clients (SuperCollider via a broker (Python

*/

~pod_name = "/pod1";
~pod_name2 = "/pod2";


//n = NetAddr("10.0.0.137", 9001); // python program @ home
n = NetAddr("10.66.213.19", 9001); // python program @ yale

n.sendMsg('/register.', "/pod1", "10.66.213.19", 57120); // mesg to register for sensor data
n.sendMsg('/register', "/pod2", "10.0.0.137", 57120); // mesg to register for sensor data


(
OSCdef(\pod1, {|msg, time, addr, recvPort| msg.postln }, ~pod_name);
)

(
OSCdef(\pod2, {|msg, time, addr, recvPort| msg.postln }, ~pod_name2);
)

// -- 2.2 -- Simple Sonification Demo

SynthDef(\plinky, {|freq=400| Out.ar(0, SinOsc.ar(freq).clip * Env.perc(0.01, 0.1, 0.1).kr(2)) }).add;

(
OSCdef(\pod1, {|msg, time, addr, recvPort|
	var light = msg[1].asInteger.linlin(150, 4095, 60, 72).round(1);
	var light2 = msg[2].asInteger.linlin(150, 4095, 60, 72).round(1);
	msg.postln;
	time.postln;
	Synth(\plinky, [\freq, light.midicps]);
	Synth(\plinky, [\freq, light2.midicps]);
}, ~pod_name);
)


// -- 2.1 -- Simple Sonification Demo

SynthDef(\plinky, {|freq=400| Out.ar(0, SinOsc.ar(freq).clip * Env.perc(0.01, 0.1, 0.1).kr(2)) }).add;

(
OSCdef(\pod1, {|msg, time, addr, recvPort|
	var light = msg[1].asInteger.linlin(150, 4095, 60, 72).round(1);
	msg.postln;
	Synth(\plinky, [\freq, light.midicps]);
}, ~pod_name);
)

(
OSCdef(\pod2, {|msg, time, addr, recvPort|
	var light = msg[1].asInteger.linlin(150, 4095, 48, 60).round(1);
	msg.postln;
	Synth(\plinky, [\freq, light.midicps]);
}, ~pod_name2);
)