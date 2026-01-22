📘 ESP8266 + QMC5883L Digital Compass with Web Interface
Overview

This project implements a web-based digital compass using an ESP8266 (NodeMCU) and a QMC5883L 3-axis magnetic sensor. The system provides accurate 0–360° heading output, automatic calibration, true north correction, and cardinal direction labels (N, NE, E, etc.), all displayed on a large, high-contrast web page accessible from any browser on the local network.

Key Features

Full 0–360° Heading Calculation
Uses correct axis mapping and atan2(x, y) to ensure full-circle heading without quadrant collapse.

Auto-Calibration Mode
On startup, the system enters a 30-second calibration phase where minimum and maximum X/Y magnetic values are recorded while the sensor is rotated. Hard-iron magnetic offsets are calculated automatically, eliminating the need for manual tuning.

True North Correction
Applies a configurable magnetic declination offset (default set for India) to convert magnetic north into true north.

Direction Labels
Converts heading values into standard compass directions:
N, NE, E, SE, S, SW, W, NW.

Fast and Stable Sampling
The compass is read continuously at a high rate, ensuring smooth and responsive heading updates.

Web-Based Display
A built-in HTTP server displays:

Large, bold heading value in degrees

Direction label

Auto-refreshing interface for near-real-time updates
No additional apps or software are required.

Low Serial Overhead
Uses Serial.begin(9600) for compatibility and debugging without impacting performance.

How It Works

Initialization

ESP8266 initializes I²C communication with the QMC5883L sensor.

Wi-Fi connection is established.

Auto-calibration timer starts.

Auto-Calibration Phase

The sensor collects minimum and maximum magnetic field values on the X and Y axes.

Offsets are computed to compensate for hard-iron distortion.

Heading Calculation

Offset-corrected X and Y values are used.

Heading is calculated using:

heading = atan2(X, Y)


Result is converted to degrees and normalized to 0–360°.

Declination correction is applied.

Direction Mapping

Heading angle is mapped to a compass direction (N/NE/E/etc.).

Web Display

ESP8266 serves a responsive HTML page showing the heading and direction.

Page refreshes periodically for live updates.

Applications

Amateur radio antenna alignment

Robotics navigation reference

Direction indicators for outdoor projects

Educational demonstrations of geomagnetic sensing

IoT dashboards and sensor networks

Notes

Calibration should be performed away from metal objects and strong magnetic fields.

For best accuracy, mount the sensor flat and rigid.

Declination value can be adjusted based on geographic location.

This implementation transforms the QMC5883L into a reliable, accurate, and network-accessible digital compass, suitable for real-world applications and further expansion.
