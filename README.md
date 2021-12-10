# Laser Tag 2: Adult Edition
 It's just laser tag. Really.


## Summary

Have you ever wanted to play laser tag, but not wanted to get sick afterward? Have you ever wanted to risk permanent eye damage playing a game designed for children? Do you like playing with lasers? If you answered yes to any of these questions, this is the project for you - the long-awaited sequel to laser tag, this time for adults only. Traditional laser tag is expensive, bulky, and normally takes place in disease-infested arcades and similar facilities where children that don't wash their hands run amok. This first-of-its-kind prototype wearable fixes all of those issues by breaking the shackles tethering you to the local laser taggery.

I've designed and built a gauntlet with a suite of sensors and a simple display that will allow you to untether your favorite game and take it on the go - with a 3-day battery life, you can keep the game going far beyond the bounds of a normal laser tag game. The laser is real, so you can snipe your enemies from hundreds of feet away before they even know you're there. And the helpful position indicator and bright green housing helps distinguish between players and civilians.

I jest, of course - the objective of the game isn't to cause permanent eye damage or to assert your eternal glory over your enemies (mostly). But it's been a tough couple of years for everyone. We've been stuck indoors, paranoid of human contact and compulsively washing our groceries and mail like weirdos. I designed this project to help people blow off some steam and get some much-needed stress relief in a time where that is all too rare. The choice of game wasn't random either - I chose laser tag in particular because it was the single most juvenile and childish game I could think of involving technology. The real purpose of this game is to allow the players to run around like children while shooting lasers at each other, in the hopes that I can provide some much-needed escapism and fun in this dark time. If you're reading this after COVID has been dealt with, then this probably makes a lot less sense. Enjoy the lasers, I guess.


## Block Diagram
![IMG_2179](https://user-images.githubusercontent.com/13169435/145651159-483db003-28f4-4e8d-9af0-226ab599d411.png)

## Components
Here's a list of the components I used. Everything can be ordered off of Adafruit as of December 2021.

* Adafruit nRF52840 Feather Express
* Adafruit TCS34725 RGB Color Sensor
* Adafruit AT42QT1010 Capacitive Touch Breakout
* 5 mW laser
* 3.3V Relay
* 200 mAh LiPo battery
* Adafruit 14-segment LED HT16K33 Backpack
* Electrical tape (lots)
* Velcro strips (for arm straps)


## Photos (Pending)
![IMG_2175](https://user-images.githubusercontent.com/13169435/145650438-7cb39916-6bf8-42c2-8634-b7fcb80dbade.png)
![IMG_2176](https://user-images.githubusercontent.com/13169435/145650457-c7d7a7d5-ed05-4544-957b-71fefd19259f.png)
![IMG_2177](https://user-images.githubusercontent.com/13169435/145650463-1bd263f7-2e01-48a1-9ec2-00cb9e63e372.png)

## Post-Mortem
If you're interested in continuing the project from here, here's three things that you should know (as well as the top 3 things I wish I could have implemented if I had more time):

1. Don't use phototransistors. They're too good to be true. The amount of signal processing code required to get the phototransistors working was ridiculous compared to the 20 minutes it took me to get the color sensor working. You're deliberately overloading the color sensor and looking for the signal when it does so, since the RGB sensor wasn't designed to handle a laser shining right at it.
2. If I could do it all over again, I wouldn't use the Feather. I'd use an Arduino with a BLE shield or something similar. The lack of documentation for the Feather means that getting anything done on the hardware level is like pulling teeth. You can access pins using their number, but that's about all the documentation is helpful with. No mention of interrupts (seriously, I ctrl+F'd it and there wasn't a single mention of interrupts in the documentation), no mention of special pin functions or how to interact with the pins, it's a mess. Just use something else.
3. I'd use a debounced button instead of a capacitive touch board. The cap touch board I used was really easy to use and it didn't *really* cause any issues, but  it wiggles around in the breadboard and triggers itself sometimes. A button is easier and more stable, though you do need a pull-up for it. 

Here's what I wish I could have implemented with more time:

1. Second laser tag unit. Seriously. I only had time to make one gauntlet, and it really didn't sit right with me, since this is a two-player game. If you're reading this and want to continue where I left off, make two of them. Seriously.
2. Try to get the laser firing function working off of an interrupt instead of polling like it's doing now. It's just wasting cycles and I don't know why interrupts break everything. It seems like it's the digitalWrite() function in particular that is refusing to cooperate, so maybe if you re-code it in baremetal C you can get the laser to fire on an interrupt.
3. Redesign the enclosure/make a PCB. I wanted to make the enclosure more sleek and Mandalorian-like, but I ran out of time and filament. This is more of an aesthetic thing, but it would go a long way. As for the PCB, it's honestly just too difficult and time-consuming to do in a class like this, but if you feel the urge to do it, please do. I'll send you cookies.
