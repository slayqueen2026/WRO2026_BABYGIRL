# HA_LEGACY - WRO Future Engineers 2026

## 1. Introduction & Team Information
**Team Profile**  
Team name: HA_LEGACY  
Country: Panama  
Memebers: Bianca Polato and Samira Santos  
Coach: Julian Vigl 

**Project Abstract**  
Our vehicle is built around a single design principle: predictability under randomness. Since the Future Engineers track layout changes before every run, our code prioritizes a tight, consistent turning radius over raw top speed, using a four-wheel motion base with a two-wheel Ackermann steering front system tuned for repeatable cornering rather than aggressive acceleration. Our core performance objectives are completing all laps in the Open Challenge without a single wall touch, reacting to obstacle pillars within 200ms of detection, and executing a reliable parallel-parking maneuver from any starting position within the parking zone.  

To handle the dynamic track, the car relies on a laser distance sensor and a Nicla Vision module for visual detection. Rather than hard-coding a single path, our control software recalculates its steering target every frame based on live sensor input, which lets the same code handle any track configuration presented on competition day. To keep the car centered and stable through corners and straights, we implemented a PID controller that continuously compares the live wall-distance reading against our target distance and applies smooth, proportional steering corrections. This replaced our earlier reactive/threshold-based adjustments, which caused abrupt, oscillating steering; the PID loop gives us a much smoother, more consistent wall-following behavior with fewer overcorrections.  

## 2. Mobility & Mechanical Design    
For our car, we chose a four-wheel motion base with two-wheel Ackermann steering on the front axle over differential drive because it more closely mirrors real car dynamics and gave us a smaller, more predictable turning radius on the tight obstacle-course corners. The chassis is mainly built from Lego, but connective pieces for certain electronic elements were made from PETG. Early prototypes used a smaller wheel in the back for balance, but this reduced traction in the rear of the car; we disassembled a toy monster truck and used its wheels, which significantly improved rear traction, meaning the wheels would no longer skid along the track.  

## 3. Power System  
Our main SBC (single-board computer) is an Arduino Uno R3. It is powered by a 9V battery, and its 3V3 pin output is used to power the Nicla Vision module, keeping our vision/sensing electronics on a clean, low-power logic line.  

For traction, we use a separate LiPo battery (4S, 14.8V) feeding the drive motor(s) through an L298N motor driver. We chose a 4S LiPo over a simpler battery setup because it gives us enough voltage and current headroom through the L298N to deliver strong torque and fast acceleration out of corners, while staying compact and rechargeable compared to bulkier disposable battery packs. Because the L298N also has some inherent voltage drop across its transistors, running at 14.8V ensures the motor still receives sufficient effective voltage after that loss.  

Keeping the logic (Arduino + Nicla Vision) and traction (LiPo + L298N) circuits electrically separate was a deliberate choice: it means voltage drops or electrical noise from the motor don't reset or interfere with the sensors or microcontroller, which is critical since our steering corrections depend on stable, uninterrupted sensor readings.  

 ## 4. Systems Thinking & Engineering Decisions
Midway through prototyping, we experimented with replacing our DC motor with a stepper motor, expecting its precise step control to make speed and distance more predictable for lane-following. During testing, the stepper motor ran inconsistently: it would sometimes fail to respond at all, other times stutter or stall unexpectedly, even when our code hadn't changed between runs. We checked our wiring and driver connections multiple times but couldn't consistently reproduce the failure or pin down whether the issue was a loose connection, an underpowered driver, or a wiring mistake in our setup.  

Rather than lose more build time chasing an intermittent hardware fault we couldn't isolate, we made the call to revert to our original DC motor design, which we knew worked reliably from earlier testing. This let us get back to consistent driving behavior and refocus our remaining time on tuning rather than debugging an unclear connection issue. To get the torque we needed out of the DC motor for sharp cornering, we settled on our LiPo (4S, 14.8V) and L298N driver combination, which gave us the acceleration and control we were originally hoping the stepper motor would provide.  

As a team, we also decided to reposition our sensors, moving them from their original location closer to the front wheels. With the sensors originally mounted further back, there was a noticeable delay between a turn appearing in the sensor's field of view and the car actually reacting to it, since the car had already moved closer to the turn by the time the data was processed. Moving the sensors nearer the front wheels shortened that distance between detection and reaction, giving the car, and our PID wall-following loop,  more time to compute and apply a smooth correction, resulting in noticeably smoother, more accurate cornering. This was a unanimous decision after comparing test runs before and after the change, since the improvement in turning behavior was immediately obvious on the track.  

The trade-off in both cases was giving up a more "ideal" solution, a precise stepper motor and a centrally located sensor layout, in favor of configurations that were less theoretically optimal but more reliable and effective in practice, a reasonable call given our timeline and testing constraints.
