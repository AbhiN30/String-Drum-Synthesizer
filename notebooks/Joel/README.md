# String-Drum-Synthesizer

Dates:

Week 1: 10/07/2024-10/11/2024
Week 2: 10/14/2024-10/18/2024
Week 3: 10/21/2024-10/25/2024
Week 4: 10/28/2024-11/01/2024
Week 5: 11/04/2024-11/08/2024
Week 6: 11/11/2024-11/15/2024
Week 7: 11/18/2024-11/22/2024
Week 8: 11/25/2024-11/29/2024
Week 9: 12/02/2024-12/06/2024
Week 10: 12/09/2024-12/13/2024

# Week 1: 10/07/2024-10/11/2024

## This week's objectives and responsibilities:

1. Order Parts (Everyone)
2. Prepare for the Design Review Meeting (Everyone)
3. Develop PCB Schematic (Everyone)

## TA Meeting Notes:

We received feedback on our initial project proposal and Design Document critiquing our visual aid, tolerance analysis, and more ethical/safety concerns.

## Record of what was accomplished:

This week we finalized our Design Document in preparation for the Design Review session. I prepared a demo/mock up of the modified Karplus-Strong algorithm using [Reaper](https://www.reaper.fm/) (commercial software for music making). Our presentation featured explanations of the power block, amplifier subsystem, synthesizer algorithm, and tolerance analysis about transmission lines and audio. We attended another group's presentation on their pinball machine diagnosing tool. I was in charge of the schematic design and was still working out details pertaing to the amplifier and processor preventing us from ordering parts or working on the PCB.

## Partner Summary:

N/A (Worked on everything together this week)

# Week 2: 10/14/2024-10/18/2024

## This week's objectives and responsibilities:

1. Implement modified Karplus-Strong Algorithm in C/C++ (Joel)
2. Research how to display data from STM32 via HDMI (Abhi)

## TA Meeting Notes:

Manvi advised us to add another ethical concern for the Design Document and to clean up the block diagram. In addition, we discussed how to actually acheive a target output impedance. Together with Manvi, we concluded that we needed to conduct more research on the topic.

Below is the final schematic:
![Alt Text](Schematic.png)

## Record of what was accomplished:

This week, we finalized our components list, and Abhi placed the orders. I tried to finish the PCB design but was unable to do so before the PCB review.

## Partner Summary:

- Changed Visual Aid
- Added to schematic for HDMI processing

# Week 3: 10/21/2024-10/25/2024

## This week's objectives and responsibilities:

1. Solder components to PCB (Joel)
2. Configure PCB Enclosure with necessary openings (Abhi)

## TA Meeting Notes:

We discussed our circuit design for each subsystem with Manvi. She said we needed to get our PCB design done.

Below is the final PCB layout:
![Alt text](Images/PCB_Layout.png)

## Record of what was accomplished:

I worked on the PCB design and made half of the connections before realizing the HDMI pinout was completely wrong. The datasheet was not used initially for the HDMI connections and upon looking at the schematic it was clear without even looking at the datasheet that the connections were wrong. This caused us to rectify the schematic and PCB over several days and ultimately lead to us missing the 1st round of PCB orders.

## Partner Summary:

- Fixed the circuit schematic for the Screen subsystem with me

# Week 4: 10/28/2024-11/01/2024

## This week's objectives and responsibilities:

1. Individually test Hardware Subsystems to check if R&V is met (Joel)
2. Individually test Software Subsystems to check if R&V is met (Abhi)

## TA Meeting Notes:

We debugged issues with our PCB design with Manvi.

## Record of what was accomplished:

We finalized our PCB but were too late for the 2nd round of orders, so our PCB was in the 3rd round.

## Partner Summary:

- We finished the PCB design

# Week 5: 11/04/2024-11/08/2024

## This week's objectives and responsibilities:

1. Integrate hardware and software subsystems (Everyone)

## TA Meeting Notes:

We received feedback on our Design Document about making the block diagram more compact and readable.

## Record of what was accomplished:

I soldered the amplifier circuit on perf board. There was no signal passed through from the signal generator, so I reassembled on a breadboard. The op-amp I chose created significant crossover distortion, and the solution was to choose a better op-amp.

## Partner Summary:

- Abhi changed the block diagram this week

# Week 6: 11/11/2024-11/15/2024

## This week's objectives and responsibilities:

1. Debugging (Everyone)

## TA Meeting Notes:

I discussed the problems with the amplifier circuit with Manvi.

## Record of what was accomplished:

I soldered components on the PCB only to discover our Power Block schematic had the wrong resistor values. I used voltage regulators, resistors, and capacitors to breadboard and eventually solder a working Power Block onto perf board.

## Partner Summary:

- Abhi wrote "Hello World" on the microprocessor

# Week 7: 11/18/2024-11/22/2024

## This week's objectives and responsibilities:

1. Preparing for and attending the Mock Demo (Everyone)

## TA Meeting Notes:

Our meeting was the Mock Demo this week. We could not show much as some individual parts worked but lacked integration into the rest of the project. We could show that our I2S signal was correct and therefore the double buffer system worked. However, we were not able to show that any knobs were working or able to convert the I2S signal to audio. Manvi advised us to create more structure for our actual demo as well as to focus on our high-level requirement.s

## Record of what was accomplished:

I wrote code for a double buffer system to output audio. In addition I setup to I2S system, which required 3 pins to output (Clock, Write Enable, Data). I verified that Write Enable was oscillating a 48 kHz, our sample rate.

Most of the double buffer code is in 3 functions. 2 are the full and half callbacks. These are called when the STM32 is ready for the next N samples to output. Each of the callbacks changes a pointer to a different half of the buffer to allow for the process to read samples as we write new ones. The last function is to write the samples to this buffer.

Below is the relavent double buffer code:
![Alt text](Images/STM32_Double_Buffer_Code.png)

## Partner Summary:

- Abhi wrote code to read a value from an emotive knob.
