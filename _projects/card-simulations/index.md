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
In the trading card game Riftbound, a 39-card deck is built around a 9-rune finisher, Dazzling Aurora. I wrote a Monte Carlo simulator in C to answer the question: *by which turn can the player cast it, and how much does a 2-rune vs. 3-rune opening change that*?

## What It Models
- **Opening hand**: a 4-card draw, with a mulligan of up to 2 cards.
- **Rune economy**: runes that accumulate permanently, per-turn income, and “recycle” costs that remove runes from the pool for good.
- **Ramp cards**: Mobilize and Catalyst of Aeons add runes that arrive exhausted and become usable the next turn.
- **Card selection**: Dredge Up (draw 1; two uses per copy), Clairvoyance (look at the top 5 cards, reorder them, draw 2) and Promising Future (look at 5, play one for free).

## Technical Highlights
- **Greedy turn policy**: each turn the simulated player repeatedly casts the best affordable card in a fixed priority order, ramp first and then card selection. It re-checks the win condition after every action that can draw a card.
- **Probability-gated decision**: the one card with a permanent rune cost, Promising Future, is only cast when the exact hypergeometric probability of finding Aurora in the top 5 cards of the actual remaining deck meets a threshold.
- **Efficient deck representation**: the deck is a fixed-size circular buffer, so draws and put-top/put-bottom operations are O(1) with no array shifting.

[Repo link](https://github.com/c00perate/riftbound_aurora)

# Yu-Gi-Oh! Consistency Calculator
In the modern format of the trading card game Yu-Gi-Oh!, nearly everything hinges on whether the player going first is able execute their full combo. However, most combos require more than simply drawing 1 card, especially to beat any interruptions coming from the opponent. I wrote a Monte Carlo simulator in C to answer the question: *how consistent is a player's deck*?

## What It Models
- **Deck construction**: the deck comes from `deck.txt` (card name and copy count). Remaining slots are padded with dead “blank” cards up to the deck size the player enter at runtime.
- **Combos**: each combo is a “possibility” made of conditions like “at least N copies of card X”. A hand is valid if it satisfies all conditions of at least one possibility, so combos are an OR of ANDs.
- **Draw engine**: five draw spells are hardcoded by name: Pot of Desires, Pot of Extravagance, Upstart Goblin, Pot of Prosperity, and Pot of Duality. If the opening hand doesn’t satisfy a combo directly, the program tries playing these cards to dig deeper.
- **Best-play result**: the search tries every legal order of activations and every choice among revealed cards. The success rate is therefore the chance that a winning line exists, not the chance an average player finds it.

## Technical Highlights
- **Bitmask pre-filter**: each hand keeps a `uint64_t` presence mask. Most combos are ruled out with one `(mask & required) == required` check before the per-card counts are examined.
- **Pre-drawn lookahead buffer**: the cards below the hand are stored in an Extras array, so draw effects pull from a pre-randomized “top of deck” and stay deterministic and cheap.

[Repo link](https://github.com/c00perate/ygo_calculator)
