# Music Visualizer
<p align="center">
  <img height="250" src="https://github.com/justcallmekoko/MusicVisualizer/blob/master/images/music_visualizer_board.JPG?raw=true">
  <img width="350" height="250" src="https://github.com/justcallmekoko/MusicVisualizer/blob/master/images/mirrored_visualizer.gif?raw=true">
  <br>
  <a href="https://www.instagram.com/just.call.me.koko">Instagram</a>
  - <a href="https://www.youtube.com/channel/UCQdi3MDHHMm7u3cMkEY329Q">YouTube</a>
  <br>
  <br>
  <b>A music visualizer based on the ATMEGA32U4-MU.</b>
</p>

# Table of Cadence
- [Intro](#intro)
- [Usage](#usage)

# Intro
*Oh god, how did it come to this?*
A while back I had published a [project](https://github.com/justcallmekoko/Arduino-FastLED-Music-Visualizer) detailing the process for creating your own music visualizer using an Arduino Uno and a [Sparkfun Spectrum Shield](https://www.sparkfun.com/products/13116). I love Sparkfun and I love Arduino, but I also love making things as simple and intuitive as possible. For these reasons, I have designed the Arduino compatible Music Visualizer based on the ATmega328p-au. It is the single board version of the original Arduino Music Visualizer from my previous project with a few other changes. Rather than using the [FastLED](https://github.com/FastLED/FastLED) library, I have employed the [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) library which made for some more simple code in the end. I have opted for the surface mount version of the ATmega328p for a more compact package. There is no need for any soldering or jumper wires as the connections between the spectrum analyzer, arduino, and RGB strip are already included on the board. Pretty much this project is as plug and play as possible.

# Usage
