# Harmoino
The repository contains a minimal example illustrating how to receive commands from a Logitech Harmony remote using an Arduino (or equivalent) and an NRF24L01+ radio chip connected via the SPI interface.

## What and why
I created this repository after researching how to repurpose my Harmony remote in a home automation project. I always liked the simple (and now discontinued) Harmony remote sold with the Harmony (Smart) Hub. It is, in my view, an excellent remote with good ergonomics and battery life. I was never similarly fond of the activity-based control schemes used in the Harmony system, so I wanted to repurpose the remote for home automation without involving the hub or Harmony software. However, the present repository is designed only to provide a minimal starting point for others to get started with their projects without requiring additional equipment other than the Arduino, the NRF24L01+ chip, and a Harmony remote (and its hub for setup). I initially used a Rohde and Schwarz signal and spectrum analyzer to gather data from packets transmitted between the remote and hub, but this is expensive equipment not readily available. Instead, the repository now has most radio parameters built into the code and a simple method to query the Hub for the unique Hub remote nRF24 network address used for the communications.

## Usage

### Connecting the NRF24L01+ to the Arduino
The NRF24L01+ chip needs to be connected to the Arduino via the SPI interface. I recommend following the excellent [Simple nRF24L01+ 2.4GHz transceiver demo](https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123) by Robin2, which also provides some discussions on troubleshooting. The sketches provided with this repository should work with no hardware modification if the CE_PIN and CNS_PIN definitions in the code are set as in the simple nRF24L01+ tutorial.

### Retrieve the unique RF24 network address
One needs to know the unique network address used by the hub and the remote to use the central sketch of the repository. This address is unique to any particular pair of remote and hub and can be changed when pairing the remote to a hub. While it should be possible to assign the remote to any chosen address, I have not yet researched the protocol sufficiently to know how to do this. Instead, one will have to receive the address from the hub.

Make sure the remote and hub are paired. The remote and hub should already be paired if you have a new remote or have not messed with your old one. If not, follow the instructions on [support.myharmony.com](https://support.myharmony.com/en-gb/how-to-re-pair-harmony-with-your-remote-or-keyboard). Once the remote and hub are paired, run the **NetworkAdress.ino** sketch with a 9600 baud serial monitor. Make sure the Harmony Hub is powered on and press the pair/reset button on the back of the hub. Within a second or two, the *unique network adress* to use in the following steps should be printed to the serial monitor. Note that you do not need to configure the hub to take these steps. If you have an already configured hub, the LED on the front should turn green when fully booted. If the hub is not configured and connected to WiFi the LED may flash read once booted, but you can still enter the pairing mode at this stage.

##¤ Using the sketch to receive button presses from the remote
Replace the dummy 0xFFFFFFFF address in the **SimpleHub.ino** sketch with the unique network address obtained in the previous step, and compile and run the sketch with a 9600 baud serial monitor. If all goes to plan, each RF24 packet transmitted from the remote should now be printed to the serial monitor. You will have to take it from here...

## The Harmony protocol in brief
The Harmony Hub and remote are built around the [nRF24L01+ Single Chip 2.4GHz transciever](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf) and communicate using the Enhanced ShockBurst protocol. The remote is (primarily) in Tx mode, and the hub is in Rx mode. The communication rate is 2Mbps, with dynamic payloads, a 40-bit network address, and a 16-bit CRC.

The hub can listen to any one of 12 different radio channels (5, 8, 14, 17, 32, 35, 41, 44, 62, 65, 71, and 74). After being idle for a while, the remote will, upon a button press, try to reach the hub on all 12 channels before locking on to a single channel. In particular, if the unique 40-bit network address is 0x0AABBCCDDEE then the remote will first try to reach the hub at network address 0x0AABBCCDD00 until the hub acknowledges a packet received on whatever channel it was listening to. The first byte of the two first packets sent to this address will be 0xEE (the least significant byte of the network address), after which all remaining packets are sent to 0x0AABBCCDDEE with the first byte or each packet set to 0x00. The last byte of each packet in all communications is always chosen so that the bytes with a packet sum to 0 modulo 256. This seems a bit redundant to me, given that a 16-bit CRC already protects each packet, but who am I to judge? Each button press generates two 10-byte packets and then another two 10-byte packets once the remote is released. Only bytes 1 to 4 of the first packet seem unique to the button pressed. When a button is held down, the remote sends a 5-byte packet every 100ms or so, and when no button is held, another 5-byte packet every 1s or so for about 30s until the remote goes quiet and the process is restarted.

When pairing a remote and hub the remote sends a 22-byte packet to the (shared) address 0xBB0ADCA575 followed shortly by several 5-byte packets to which the hub responds using the ACK payload feature of the nRF24L01 transceiver to send back a 22-byte packet to the remote. The packet sent back to the remote contains the network address for regular communication. Thus, the hub provides the remote with the address to use and not the other way around. The NetworkAddress.ino sketch uses this feature to receive the network address from the hub but stops short of completing the pairing process. When pairing a remote with the regular hub, the network address is increased by one each time they are paired, but this does not happen when running the NetworkAddress.ino sketch, so there is more to it. A complete implementation of the pairing protocol should presumably enable giving the remote any network address and thus allow for pairing up to 5 remotes with a single Arduino by using the nRF24L01+ 6 data pipe MultiCeiver feature.

Finally, it should be noted that the packets contain a lot more information than what would be needed in just implementing the remote functionality. However, I do not fully understand all aspects of this information. However, there seems to be some commonality between Harmony and Logitech wireless products, so this information will likely have more meaning for other products.

## Testing

I have tested the scripts of the repository on an original Harmony branded Hub with the remote without a screen, and a more recent Logitech branded Harmony Companion remote (the one with buttons for smart home lights and switches) to make sure they work as intended. I have, however, not had access to the Logitech Harmony Ultimate All in One Remote with Customizable Touch Screen or older Harmony (RF) remotes. I would thus appreciate feedback if you could get it to work with one of these.

## Acknowledgements

First, I wish to acknowledge the [Hacking the Harmony RF Remote](https://haukcode.wordpress.com/2015/04/16/hacking-the-harmony-rf-remote/) blog post on Hakan's Coding and Stuff. This post gave me crucial pointers early on, especially with the radio hardware. I can also recommend the discussion thread for helpful information on pairing the Harmony Remote with the Logitech Unifying Receiver on a PC if you are looking for a more software-based solution. Second, the [Simple nRF24L01+ 2.4GHz transceiver demo](https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123) by Robin2 was tremendously helpful in getting started with the RF24 library for the Arduino, and it inspired the minimalistic SimpleHub implementation.
