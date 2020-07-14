# Button Pad MIDI Controller

This is a 4x4 Button MIDI controller with 6 side buttons that each load their own sets of MIDI notes that can be triggered at the press of the button.

Each arcade button lights up when it's pressed, and triggers a MIDI note which is sent to the computer. Which side button is active is indicated by which has its LED lit.

The buttons are connected by a button matrix that allows multiple buttons to be pressed at the same time, therefor allowing for multiple notes to be active and played.

 It runs off of a USB-C version of an Arduino Pro Micro, which the included code is written for. I went with the Pro Micro specifically because its processor, the ATmega 32U4 has built in USB, which allows any computer to recieve MIDI notes from the device without any extra serial to MIDI interfacing. I also added two SX1509 I/O expanders to help with the side buttons and lighting LEDs individually.

 I was inspired to make this when I learned about the MIDI Fighter 3D by DJ Tech Tools. It seems like such a fun instrument and I thought it'd be really fun to try and make one myself, and it was. Throughout this project I learned a lot about electronics in order to get the parts working together just how I wanted them, C++ in order to get the Arduino working well, and 3D modelling to make the housing.

 In future versions of this I want to revise some parts of the Arduino's loop because I think it does certain things every run that aren't necessary, and I want figure out a better solution to the wiring of the internals. I may look into making a PCB for the project as that could be a better way to consolidate the internals.