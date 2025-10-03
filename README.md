# PortaRF
Custom Hardware and Firmware Bin Files for the PortaRF Gadget.

# Introduction
The PortaRF is an open-source hardware platform that combines the [HackRF One R10](https://github.com/greatscottgadgets/hackrf/tree/master/hardware/hackrf-one) and [PortaPack H4M](https://github.com/OpenSourceSDRLab/mayhem-firmware/tree/next/hardware/portapack_h4m) into a single Board. It supports the transmission and reception of radio signals ranging from 1 MHz to 6 GHz, rendering it an exceptionally versatile tool for RF exploration and development.  It is a portable, standalone device for signal analysis and transmission without requiring a computer.

# Hardware Improvements
1. Doubled flash storage capacity
We have replaced the original W25Q80DVSSIQ with W25Q16DV SPI Flash chip, expanding the original firmware storage capacity from 1M byte to 2M byte. This provides a broader creative space for Mayhem firmware developers, enabling the implementation of more complex signal processing algorithms and functions.

2. 4.0-inch IPS resistive touch screen
The screen has been upgraded to a 4.0-inch IPS full-view high-definition display. Compared with the original 3.2-inch non-IPS matte screen, it has made a significant improvement in color reproduction, viewing angle and touch accuracy. Whether it's spectrum analysis or parameter settings, the text is clearer and the operation is more precise.

3. 3000mAh lithium battery system
The battery capacity has been increased to 3000mAh, significantly extending the battery life. At the same time, an intelligent battery protection board has been used in this battery, providing protection against overcharging, overdischarging, short circuiting, and overcurrent. This lithium battery is produced by a legitimate factory with guaranteed quality.

4. User-friendly hand-held directional keys
PortaRF has added a gamepad-style directional button. It is particularly suitable for rapid directional operations in functions such as spectrum scanning and game applications. It will significantly enhance the direction control efficiency and comfort.

5. Modular Expansion Board Design
The expansion board adopts a Board to Board connector, a socket interface similar to the memory module, which is smaller in size and more stable in installation. This can optimize the internal space utilization.

6. Light Guide Column LED Status Indication System
PortaRF also incorporates the light guide column design, making the LED status lights more visible. Regardless of day or night, the equipment status is clearly visible.

7. Nice shell design
We have designed a brand new shell for the PortaRF and added many design elements, such as the buckle of the shell, which makes the shell fit tightly. The slot of the memory card is designed with grooves, the SD card can be inserted and removed without the need for tools. The dual speaker sound hole design enables better sound output from the speakers.

8. AI voice-assisted (Beta version)
PortaRF incorporates voice-controlled interaction technology. Users can perform operations such as device activation, application switching, and system reboot through natural voice commands. This feature is particularly suitable for scenarios where hands are not available, providing additional convenience operations.

# Applications

<div class="editor_table_wrap">&nbsp;
<table border="1" style="width: 81.0373%; height: 526px;">
<tbody>
<tr style="height: 26.2px;">
<td style="width: 55.25%; height: 26.2px;">
<h3 class="text-xl font-semibold text-indigo-800 mb-3">Receive Applications</h3>
</td>
<td style="width: 44.7339%; height: 26.2px;">
<h3 class="text-xl font-semibold text-indigo-800 mb-3">Transmit Applications</h3>
</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Sniff &amp; Capture</strong>: Sniff &amp; capture signals for instant replay or save to SD card for analysis</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>ADS-B</strong>: Transmit ADS-B (Aviation) signals for testing</td>
</tr>
<tr style="height: 44.8px;">
<td style="width: 55.25%; height: 44.8px;"><strong>ADS-B</strong>: Receive and analyze aviation signals with real-time location display</td>
<td style="width: 44.7339%; height: 44.8px;"><strong>APRS</strong>: Transmit APRS packets for GPS, weather stations and telemetry</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>AIS</strong>: Receive and analyze marine vessel signals</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>GPS</strong>: Transmit GPS signals for testing</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>AFSK</strong>: Receive, demodulate and analyze AFSK signals</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>Key Fob</strong>: Key-fob capture, replay and emulation for multiple models</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Bluetooth</strong>: Sniff MAC addresses of BLE devices</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>Morse Code</strong>: Morse code transmission tool</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>NRF</strong>: Decode NRF24L01 signals, commonly used for drones</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>Burger Pager</strong>: Transmit restaurant pager signals</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Audio</strong>: Decode WFM/NFM/AM radio transmissions</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>POCSAG</strong>: Transmit pager signals</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Spectrum Analyzer</strong>: Waterfall spectrum analyzer for detecting signals</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>SSTV</strong>: Transmit Slow Scan Television signals</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Analog TV</strong>: Decode and watch analog TV signals, used on FPV drones</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>RDS</strong>: Send Radio Data System messages</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>ERT Meters</strong>: Decode ERT Meter signals</td>
<td style="width: 44.7339%; height: 22.4px;"><strong>OOK</strong>: Send On-Off Keying packages</td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>POCSAG</strong>: Sniff and decode pager signals</td>
<td style="width: 44.7339%; height: 22.4px;"></td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>Radiosonde</strong>: Sniff and decode weather station signals</td>
<td style="width: 44.7339%; height: 22.4px;"></td>
</tr>
<tr style="height: 22.4px;">
<td style="width: 55.25%; height: 22.4px;"><strong>TPMS</strong>: Decode Tire Pressure Monitoring System sensors</td>
<td style="width: 44.7339%; height: 22.4px;">&nbsp; &nbsp; &nbsp; &nbsp;</td>
</tr>
</tbody>
</table>
