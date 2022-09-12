Newt Display Arduino Library
========================================

![Newt](newt-angle-01_jpg_tile-xl.jpg)

Newt is a battery-powered, always-on, wall-mountable display that can go online to retrieve weather, calendars, sports scores, to-do lists, quotes…really anything on the Internet! It is powered by an ESP32-S2 microcontroller that you can program with Arduino, CircuitPython, MicroPython, or ESP-IDF.

This library includes the source code used in the Newt display. You can modify this code to customize Newt.

You can read more about Newt at [*https://phambili.tech/newt*](https://phambili.tech/newt)

Planned Enhancement/Fixes/Fixes for next release
---------------------------------------------

**Version 1.1.0- RC1**

All - you can now download version 1.1.0-RC1 for manual testing. We've added a ton of new features. I'm in the process of writing up the official documentation, but at a high level:

- New API for alerts (beta)
- New API for riddles/brainteasers
- Major change in weather API (new provider and additional weather details)
  - UV
  - Sunrise/sunset
  - Humidity
  - Feels like weather
- Updated settings options:
  - 12/24 Hour clock
  - Nighttime screen mode
  - more Settings details
  - Quote or Weather (or rotating) main screen
  - Imperial or Metric units
- Hello World Arduino example that explains how do:
  - Write to the screen
  - Enable the touchpads
  - Emit a beep from the device
  
You can find instructions for manual download here-> https://github.com/Phambili-Tech/Newt_Display/discussions/37 

**Version 1.1.0+ - roadmap**
- change timezone
- change location
- icon documentation
- service-side code docuumentation

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 


Distributed as-is; no warranty is given.
