Hardware
========

1. Assembly
-----------
Completed Assembly
^^^^^^^^^^^^^^^^^^
This is what a completed assembly looks like:
.. figure:: img/CompletedAssembly.png
   :align: center

Assembly Steps
^^^^^^^^^^^^^^

2. CAD Files
------------
- Antenna mounting arms that hold the antenna end for extending and retracting

  | Antenna_Arms_-_Antenna_support_3-1.STL
  | Antenna_Arms_-_Antenna_support_4-1.STL

- Case for dimensioning and represening Raspbery Pi

  | Raspberry_Pi_case.STL

- Plate used by belt drive on a rail system to extend and retract the antenna

  | Creality_X_Carriage_Plate.STL

- Channel used to guide small groups of wire into position

  | Wiring_Channel.STL

- Combination files of parts that make the V-wheel assembly that attaches to the Creality X Carriage Plate

  | assembled_v-wheel_-_bearing-1.STL
  | assembled_v-wheel_-_bearing-2.STL
  | assembled_v-wheel_-_spacerexternal-1.STL
  | assembled_v-wheel_-_spacerinner-1.STL
  | assembled_v-wheel_-_wheel-1.STL

- The long legs that hold the higher section of track. (x2)
 
  | V-Rail_long_leg.STL

- The legs that hold the lower section of track. (x2)

  | V-Rail_short_leg.STL

- The 27 cm sections of V-Rail that the Carriage Plate uses to guide the antennas. (x2)

  | V-Rail_track.STL

- These combine to make the fully assembled base of our antenna 

  | Assembled_antenna_2_-antenna_base-1.STL
  | Assembled_antenna_2_-antenna_base2-1.STL
  | Assembled_antenna_2_-antenna_base3-1.STL
  | Assembled_antenna_2_-antenna_base4-1.STL
  | Assembled_antenna_2_-antenna_base5-1.STL
  | Assembled_antenna_2_-antenna_base6-1.STL
  | Assembled_antenna_2_-antenna_base7-1.STL
  | Assembled_antenna_2_-antenna_nut-1.STL

- These are current files for the reflector holders (newer models coming soon). Holder 2 is shaped to flow around the encoder that currently blocks its path

  | Reflector_Holder_1.STL
  | Reflector_Holder_2.STL

- Stand-in model for the rotary encoders that we are using

  | rotary_encoder.STL
  
- Motor mount and reinforcing brace to attach to internals of long and short v-rail legs in the center of entire project

  | motor_bracket_brace.STL
  | motor_bracket.STL

- Similar to Antenna arms, these parts are for holding the 2 opposite pointing electret microphones in order to scan for sounds and sound origins

  | mic_support_1.STL
  | mic_support_2.STL

- Mount that hold the two encoders 

  | encoder_mount2.STL

- Central antenna mount that attaches to the top of the central encoder, aligning the antennas with the holders

  | antenna_mount_and_encoder_cap.STL 

3. Electrical Components
------------------------
3.1 Circuit Components
^^^^^^^^^^^^^^^^^^^^^^
- `Pololu A4988 Motor Driver <https://www.pololu.com/product/1182>`_
- `Nema 17 Bi-polar 0.9 Degree Stepper Motor <https://www.omc-stepperonline.com/nema-17-bipolar-09deg-11ncm-156ozin-12a-36v-42x42x21mm-4-wires-17hm08-1204s.html>`_ [17HM08-1204S]
- `Incremental Photoelectric Rotary Encoder 400 P/R <https://www.dfrobot.com/wiki/index.php/Incremental_Photoelectric_Rotary_Encoder_-_400P/R_SKU:_SEN0230>`_ [SEN0230]
- SPDT Omron Microswitch

3.2 Software Defined Radio
^^^^^^^^^^^^^^^^^^^^^^^^^^
- `HackRF One Great Scott Gadgets <https://greatscottgadgets.com/hackrf/one/>`_

3.3 Power Supply
^^^^^^^^^^^^^^^^
- `AC/DC Adadpter - 24VDC 2.5A <https://www.circuittest.com/rpr-2402a5-p5.html>`_ [RPR-2402A5-P5]


4. Circuit Schematics
---------------------

.. figure:: img/AIConfigurableAntennaSchematic.png
   :align: center

5. Wiring Diagram
-----------------







