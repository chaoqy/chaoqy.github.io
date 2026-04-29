---
layout: post
title: Discord Bots
description: Built and maintained Discord bots serving a userbase of 10,000+ users, using a modular architecture (cogs, handlers, services) and PostgreSQL for persistent data storage and feedback management.
skills: 
- Python
- PostgreSQL
- Git
main-image: /discord.webp 
---

---
# Character Information Bot
A Discord bot I created during my time at FunCat Games as an intern to process game data, with efficient lookup structures for 100+ characters and items. The bot has multiple commands with flexible searching by name, attribute, and nickname, and automated stat calculation system.

## Demo
{% include image-gallery.html images="char_bot_demo.gif" %}  

# Game Feedback Bot
Another Discord bot that I made as an intern; receives feedback from players via direct messages and stores to a PostgreSQL database. Has a user-based timecool down so users are unable to spam the bot. I added admin commands to export all the feedback as a text file and to clear all stored entries. 

## Demo

#### Sending Feedback
{% include image-gallery.html images="feedback_bot_demo1.gif" %}  

#### Exporting and Clearing Feedback

{% include image-gallery.html images="feedback_bot_demo2.gif" %}  

