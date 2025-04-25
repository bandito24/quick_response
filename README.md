# Quick Response: ESP32-Based Game

This game is designed to run on an ESP32 microcontroller with an LED strip.  
I used the Freenove WS2812, but you should be able to swap in other strips by adjusting the LED count variable found in `main.c`.

The goal is simple: hit the button as the LED moves in a circle and eventually lands on top of another light.

- **Blinking green** means you cleared the level.
- **Blinking red** means you missed — time to restart!

Each level you clear speeds things up.  
See if you can make it to **Super Saiyan mode** — when the LEDs start flashing purple, you're down to a 50ms reaction time. It's harder than it sounds!

---

## Hardware Setup

Connect your LED strip as usual:

- `S` (Signal) → **GPIO Pin 4**
- `V` (Voltage) → **3.3V Power**
- `G` (Ground) → **Ground**

You'll also need a button:

- Connect one side of the button to **3.3V** through a resistor.
- Connect the other side to **GPIO Pin 13**.
- Pressing the button should pull the signal **low (to ground)**.

---

## How to Play

1. Wire up the ESP32 and LED strip.
2. Flash the project onto the ESP32.
3. Power it up and react as fast as you can!
4. Watch for green (success) or red (restart).
5. Beat enough levels to unlock **Super Saiyan Mode** (purple flash)!

---

## Feedback

I'd love to hear if you try this quick-reaction game — let me know how far you get!
