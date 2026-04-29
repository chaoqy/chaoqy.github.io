---
layout: post
title: Line Following Robot
description: Built a line-following robot using Arduino with a PD controller and a custom lift-and-curl mechanism for object manipulation.
skills: 
- Arduino
main-image: /linefollower.jpg
---

---
# Line Following Robot
I built a line-following robot using an Arduino and IR sensors to detect both lines and obstacles. When the robot detects an obstacle, it uses a small lift-and-curl mechanism I designed with LEGO to pick up and drop off pieces.

For motion, I used a PD controller (closed-loop) to keep the robot stable while driving straight, continuously correcting its path based on sensor input. For turns at intersections, I initially tried a closed-loop approach, but ran into oscillation issues where the robot would wiggle back and forth before settling. After experimenting with different approaches, I found that an open-loop turning strategy produced much smoother and more reliable results in practice, so I chose that for the final implementation.

## Demo
{% include image-gallery.html images="line_follower.gif" %}    
