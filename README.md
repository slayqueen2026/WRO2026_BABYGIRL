# HA_LEGACY - WRO Future Engineers 2026

## 1. Introduction & Team Information
**Team Profile**  
Team name: HA_LEGACY  
Country: Panama  
Memebers: Bianca Polato and Samira Santos  
Coach: Julian Vigl 

**Project Abstract**  
Our vehicle is built around a single design principle: predictability under randomness. Since the Future Engineers track layout changes before every run, our code prioritizes a tight, consistent turning radius over raw top speed, using a 4-wheel Ackermann steering system tuned for repeatable cornering rather than aggressive acceleration. Our core performance objectives are completing all laps in the Open Challenge without a single wall touch,  reacting to obstacle pillars within 200ms of detection, and executing a reliable parallel-parking maneuver from any starting position within the parking zone.  
  
To handle the dynamic track, the car relies on a laser distance sensor. Rather than hard-coding a single path, our control software recalculates its steering target every frame based on live sensor input, which lets the same code handle any track configuration presented on competition day.

## 2. Mobility & Mechanical Design    
For our car, we chose Ackermann steering over differential drive because it more closely mirrors real car dynamics and gave us a smaller, more predictable turning radius on the tight obstacle-course corners. The chassis is mainly built from lego, but connective pieces were made for vertain electronic elements made form PETG. Early prototypes used smaller wheel in the back for balance, but this made the car have less traction in the rear of the car; we disassembled a toy moster truck and used its wheels, which imrpoved significatly the rear traction meaning the wheel would not skid along the track.    
  
Our drive system runs on three 9V batteries wired in series, supplying roughly 27V to the DC motor for stronger torque and faster acceleration out of corners. We chose series over parallel because our early low-voltage tests showed the motor struggling to accelerate quickly enough after obstacle maneuvers, and the added voltage solved this without needing a different motor. Because series wiring raises voltage rather than current capacity, we're aware our runtime per battery set is limited, so we carry spares and swap packs between test rounds rather than relying on one set all day. The motor circuit is kept fully separate from our logic circuit, which runs off its own regulated 5V line off a separate small battery, so voltage drops or noise from the motor don't reset or interfere with the sensors.

 ## 3. Systems Thinking & Engineering Decisions
Midway through prototyping, we experimented with replacing our DC motor with a stepper motor, expecting its precise step control to make speed and distance more predictable for lane-following. During testing, the stepper motor ran inconsistently, it would sometimes fail to respond at all, other times stutter or stall unexpectedly, even when our code hadn't changed between runs. We checked our wiring and driver connections multiple times but couldn't consistently reproduce the failure or pin down whether the issue was a loose connection, an underpowered driver, or a wiring mistake in our setup.  
  
Rather than lose more build time chasing an intermittent hardware fault we couldn't isolate, we made the call to revert to our original DC motor design, which we knew worked reliably from earlier testing. This let us get back to consistent driving behavior and refocus our remaining time on tuning rather than debugging an unclear connection issue. To get the torque we needed out of the DC motor for sharp cornering, we wired three 9V batteries in series for roughly 27V, which gave us the acceleration we were originally hoping the stepper motor would provide.  
  
As a team, we also decided to reposition our sensors, moving them from their original location closer to the front wheels. With the sensors originally mounted further back, there was a noticeable delay between a turn appearing in the sensor's field of view and the car actually reacting to it, since the car had already moved closer to the turn by the time the data was processed. Moving the sensors nearer the front wheels shortened that distance between detection and reaction, giving the car more time to adjust and resulting in noticeably smoother, more accurate cornering. This was a unanimous decision after comparing test runs before and after the change, since the improvement in turning behavior was immediately obvious on the track.
  
The trade-off in both cases was giving up a more "ideal" solution, a precise stepper motor and a centrally located sensor layout, in favor of configurations that were less theoretically optimal but more reliable and effective in practice, a reasonable call given our timeline and testing constraints.
