---
layout: post
title: Card Simulations
description: Created various Monte-Carlo simulations in C to estimate probability of endpoints for different card games. 
skills:
- C
main-image: /ygo_back.jpg
---

---
# Riftbound Consistency Calculator
In the trading card game Riftbound, a 39-card deck is built around a 9-rune finisher, Dazzling Aurora. I wrote a Monte Carlo simulator in C to answer one question: *by which turn can the player cast it, and how much does a 2-rune vs. 3-rune opening change that*?

## What it models
- **Opening hand**: a 4-card draw, with a mulligan of up to 2 cards.
- **Rune economy**: runes that accumulate permanently, per-turn income, and “recycle” costs that remove runes from the pool for good.
- **Ramp cards**: Mobilize and Catalyst of Aeons add runes that arrive exhausted and become usable the next turn.
- **Card selection**: Dredge Up (draw 1; two uses per copy), Clairvoyance (look at the top 5 cards, reorder them, draw 2) and Promising Future (look at 5, play one for free).
- **Platewyrm Egg**: a permanent that produces temporary “energy”. Energy counts toward costs but can’t pay a recycle requirement.

## Technical highlights
- **Greedy turn policy**: each turn the simulated player repeatedly casts the best affordable card in a fixed priority order, ramp first and then card selection. It re-checks the win condition after every action that can draw a card.
- **Probability-gated decision**: the one card with a permanent rune cost, Promising Future, is only cast when the exact hypergeometric probability of finding Aurora in the top 5 cards of the actual remaining deck meets a threshold.
- **Efficient deck representation**: the deck is a fixed-size circular buffer, so draws and put-top/put-bottom operations are O(1) with no array shifting.

[Repo link](https://github.com/c00perate/riftbound_aurora)
