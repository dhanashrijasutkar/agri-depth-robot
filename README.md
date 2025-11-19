# Smart Agricultural Robot — Depth-Based Soil Probe
Author: Dhanashri Jasutkar

## Summary
Embedded firmware and demo scripts for a semi-autonomous soil-probing robot prototype. Contains an ESP8266 Wi-Fi remote firmware skeleton, a simulator to generate sample readings for screenshots, and placeholders for documentation and photos.

## Repo content
- `arduino/main.ino` — ESP8266 Wi-Fi remote / servo + motor firmware (skeleton).
- `arduino/config.h` — pin definitions and constants.
- `tests/sim_probe.py` — demo simulator printing sample probe readings.
- `docs/` — block diagrams and prototype photos.
- `.gitignore`, `LICENSE`

## How to preview
- Open `arduino/main.ino` to review firmware logic.
- Run simulator:
```bash
python3 tests/sim_probe.py

