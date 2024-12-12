# Min-Path: Efficient Path Computing on Highways
## Overview
This project provides a solution to the "Min-Path" problem, which aims to compute the most efficient travel path on a highway represented as a sequence of service stations. 
The program minimizes the number of stops between a starting and destination station, considering constraints such as vehicle autonomy and station availability. 
This implementation is written in C and includes efficient data structures like binary search trees and heaps for optimal performance.
## Description
The highway is modeled as a series of service stations located at distinct distances from the start. Each station offers electric rental cars with varying battery ranges. 
Travelers can rent a car at each station and travel to the next reachable station without backtracking. 
The goal is to determine:
  - The path with the minimum number of stops.
  - The lexicographically smallest path in case of ties.

Example: For a highway with stations at distances `[20, 30, 45, 50]` and vehicles with ranges at respective stations:
  - Station 20: [5, 10, 15, 25]
  - Station 30: [40]
  - Station 45: [30]
  - Station 50: [20, 25]

The shortest path from 20 to 50 is `20 → 30 → 50`, not `20 → 45 → 50`.

## Features
The program supports the following operations:

### 1. Add a Station:
Adds a station with a specific number of vehicles and their respective ranges.

`aggiungi-stazione <distance> <number-of-cars> <car-range-1> ... <car-range-n>`

### 2. Remove a Station:
Deletes a station and its associated vehicles.

`demolisci-stazione <distance>`

### 3. Add a Car:
Adds a vehicle to an existing station.

`aggiungi-auto <station-distance> <car-range>`

### 4. Remove a Car:
Removes a vehicle with a specific range from a station.

`rottama-auto <station-distance> <car-range>`

### 5. Compute a Path:
Computes the shortest path between two stations.

`pianifica-percorso <start-distance> <end-distance>`

## Technical Implementation
### Data Structures
  - Binary Search Tree (BST): Stores stations in order of their distances.
  - Max Heap: Manages vehicles at each station based on their autonomy.
  - Queue: Implements a Breadth-First Search (BFS) algorithm for pathfinding.
### Algorithms
  - Pathfinding:
    - Forward and backward traversal using BFS.
    - Prioritizes lexicographically smallest paths in case of ties.
  - Heap Operations:
    - Insert, delete, and fetch maximum car range.
  - Station Management:
    - Dynamic insertion and deletion from the BST.
### Performance
  - Supports up to `40,000` stations and `512` cars per station.
  - Efficient operations with logarithmic complexity for BST and heap operations.


  






 
