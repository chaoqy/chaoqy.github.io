---
layout: post
title: Computer Vision Model
description: Real-time computer vision model for enemy detection and tracking as part of the Queen's Knights Robotics Team.
skills: 
- Python
main-image: /plate.png 
---

---
# Computer Vision Model
I aggregated and curated a large-scale image dataset from multiple external sources, then wrote a custom Python script to standardize inconsistent annotation formats into a unified structure. To improve model robustness, I also built a data augmentation pipeline in Python that applied geometric transformations and lighting variations to increase dataset diversity and generalization.

Using this dataset, I trained and fine-tuned a YOLO-based object detection model, which improved performance by +2.7 mAP@0.50:0.95 (0.577 -> 0.604).

## Demo
{% include image-gallery.html images="cv_test.gif" %}  

[See this for better quality](https://github.com/chaoqy/chaoqy.github.io/blob/main/_projects/computer-vision/cv_test.mov)
