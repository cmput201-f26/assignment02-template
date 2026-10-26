# Assignment 2 - A* Search & Dynamic Allocation

## Introduction

> [!CAUTION]
> This assignment will require the use of dynamic memory. If your program contains memory leaks, you will lose marks. See the [marking scheme section](#marking-20-pts) for more details, as well as the [valgrind section](#memory-leaks-and-valgrind) on how to test for leaks.

In this assignment, you are asked to develop an implementation of the A* searching algorithm. Your code should be written in a file called `assg.c` located in the source directory `src`.

To perform the search, your program must:
1. Parse the input that specifies the grid layout and barrier positions (refer to the [Setup Phase](#input-and-output-specifications) section).
2. Execute the A* search from the start position to the end position.
3. Track and communicate the progress of the search by outputting the new additions to the closed list during each turn (refer to the [Simulation Phase](#time-step-input-and-output) section).
4. Output the final path calculated by the algorithm as the final step of the simulation (refer to the [Final Path](#final-path) section).




## The A* Algorithm

A* (A-star) is a popular and efficient pathfinding algorithm used to find the shortest path between a starting node and an end node on a grid or graph. It uses a heuristic to guide its search, evaluating nodes using the function:

$$f(n) = g(n) + h(n)$$

Where:
*   **$g(n)$** is the cost of the path from the start node to node $n$.
*   **$h(n)$** is the heuristic estimate of the cost to get from node $n$ to the end node. In this assignment, you **must** use the **Manhattan distance** as the heuristic function, calculated as:
    $$|x_1 - x_2| + |y_1 - y_2|$$
*   **$f(n)$** is the total estimated cost of a path passing through node $n$.

### Open and Closed Lists

The algorithm maintains two lists to track its progress:
1.  **Open List:** The frontier. This contains all nodes that have been discovered but not yet evaluated. Nodes in the open list are prioritized by their $f(n)$ value (lowest first).
2.  **Closed List:** The visited set. This contains all nodes that have already been evaluated. Once a node is in the closed list, it will not be re-evaluated, preventing loops and redundant search.

During the search, the algorithm repeatedly selects the node with the lowest $f(n)$ from the open list, moves it to the closed list, and examines its neighbors, adding new neighbors to the open list or updating their costs if a shorter path to them is found. You must explore the neighbors in this order: left, right, top, bottom.

In this assignment, your program must communicate the progress of the search by providing the new additions to the closed list (i.e., the nodes evaluated during that step) as the **Action(s)** in the turn output. Note that it is not the entire closed list that is printed on each step, but rather only the newly added nodes for that specific time-step.

### Grid Details

A wall in this game is a rectangle identified by the coordinates of its south west corner and its width and height. If you think of a wall as a grid, then a 'barrier' is a square. **Walls cannot be right next to each other and cannot overlap**. Though they may be diagonally adjacent.

> *Note:* In this assignment, we consider north to be the Y direction increasing and east to be the X direction increasing.


## Heap and Data Structure Specifications

To implement A* efficiently, you must use the structures and function signatures declared in the header file assg.h.

### 1. Min-Heap (Open List)
The **Open List** is managed as a Min-Heap (priority queue) of `Node` pointers. The heap is ordered primarily by the node's $f$-cost. 

*   **Min-Heap Ordering Property:** For any node at index $i$, its parent at index $(i-1)/2$ must satisfy:
    $$\text{parent.f} \le \text{child.f}$$
*   **Tie-Breaking Rule:** If two nodes have equal $f$-cost, priority must be given to the node with the **lower $h$-cost** (closer to the goal). If $h$-costs are also equal, prioritize the node with the **smaller $x$-coordinate**; if $x$-coordinates are also equal, prioritize the node with the **smaller $y$-coordinate**.
*   **Decrease Key:** When A* finds a shorter path to an existing node in the Open List, its $g$-cost is updated. You must use `heap_decrease_key` to update the costs and restore the heap property (bubbling the node up).

### 2. Closed List
The **Closed List** tracks already visited nodes. In this assignment, it is implemented as a dynamically resizing array of `Node` pointers.
*   When a node is popped from the Open List (extracted min), it is added to the Closed List and printed during that time-step.
*   You must check if a neighbor is already in the Closed List before processing it.

### Required Functions (defined in `src/assg.h`)

*   `create_node(x, y, g, h, parent)`: Dynamically allocates and initializes a new `Node`.
*   `heap_init(heap)`: Initializes the heap array, size, and capacity.
*   `heap_insert(heap, node)`: Inserts a new node into the heap, maintaining the min-heap properties.
*   `heap_extract_min(heap)`: Removes and returns the node with the lowest $f$-cost from the heap.
*   `heapify_up(heap, index)` / `heapify_down(heap, index)`: Restores min-heap invariants by bubbling nodes up or down.
*   `heap_find(heap, x, y)`: Searches the heap for a node at coordinates $(x, y)$ and returns its index in the heap array (or `-1` if not found).
*   `heap_is_empty(heap)`: Returns true if heap is empty, false if heap still contains nodes.
*   `heap_decrease_key(heap, index, new_g, new_h)`: Lowers a node's costs and restores the heap property.
*   `closed_list_init(list)` / `closed_list_add(list, node)` / `closed_list_contains(list, x, y)`: Manages the visited states list.
*   `heap_free(heap)` / `closed_list_free(list)`: Frees all dynamically allocated memory.
---
# Input / Output Specification for the Milestone

This milestone is used to test and verify the **Open List (Min-Heap)** and **Closed List** operations before running the full $A^*$ search Algorithm.


## 1. List Targeting: `[O]` vs `[C]`

The insertion header and each command use a tag to identify the target list:

* **`[O]`** $\rightarrow$ Targets the **Open List** (Min-Heap)
* **`[C]`** $\rightarrow$ Targets the **Closed List** (The Visited Set)

## 2. Milestone Input Sequence

Milestone input consists of two phases:

1. **Insertion phase:** The input begins with `[O] Insert:`, followed by one node per line in the format `x, y, g, h`. The keyword `done` ends this phase.
2. **Command phase:** After `done`, the input marker `Commands:` indicates that subsequent lines contain commands to process. No further insertions occur during this phase.

Both `done` and `Commands:` are input markers and must not produce output. Ignore blank lines between the two phases.

## 3. Command Specifications

### `[O] Insert:`

**Behavior:** Reads coordinates and node values line-by-line, creates nodes, and adds them to the Open List while maintaining min-heap properties. Reading `done` ends the insertion phase. The program then reads the `Commands:` line and begins processing commands. `[O] Insert:` appears only once, at the beginning of the milestone input.

**Format:**
```text
[O] Insert:
x, y, g, h
...
done
```


### `[O] Extract`

**Behavior:**
1. Removes the node with the lowest total cost ($f = g + h$) from the Open List.
2. Restores min-heap ordering.
3. Adds the extracted node to the Closed List.
4. Prints the extracted node's details.


**Output Format:**
```text
Extracted Node (x, y), f = f-value, g = g-value, h = h-value
```


### `[O] Find (x, y)` / `[C] Find (x, y)`

* **Behavior:** Searches either the Open List or Closed List for node $(x, y)$.

**Output Format:**
* If found: `Node (x, y) has been found`
* If not found: `Node (x, y) has not been found`


### `[O] IsEmpty` / `[C] IsEmpty`

* **Behavior:** Checks whether the targeted list currently contains zero nodes.
* **Output Format:** `True` / `False`



### `[O] DecreaseKey (x, y), new_g = new_g_value`

**Behavior:** Finds the specified node in the Open List, updates its `g`-cost to `new_g`, leaves its `h`-cost unchanged, and recalculates `f = g + h`. Call `heap_decrease_key()` with the new `g`-cost and the node's existing `h`-cost to restore min-heap ordering.

**Output Format:**
```text
Decreased key for Node (x, y): f = new_f_value, g = new_g_val, h = h_value
```
### 4. Input & Output Example For Milestone

#### Input

```text
[O] Insert:
0, 0, 0, 10
5, 5, 5, 5
9, 9, 8, 8
done

Commands:
[O] IsEmpty
[O] Find (5, 5)
[O] DecreaseKey (5, 5), new_g = 1
[O] Extract
[C] Find (5, 5)
[C] Find (0, 0)
[C] IsEmpty
[O] Extract
[O] Extract
[O] IsEmpty
```
#### Output

```text
False
Node (5, 5) has been found
Decreased key for Node (5, 5): f = 6, g = 1, h = 5
Extracted Node (5, 5), f = 6, g = 1, h = 5
Node (5, 5) has been found
Node (0, 0) has not been found
False
Extracted Node (0, 0), f = 10, g = 0, h = 10
Extracted Node (9, 9), f = 16, g = 8, h = 8
True
```
> [!NOTE]
> **Milestone only:** Since dynamic memory allocation has not yet been taught, you may use fixed-size arrays with a capacity of 100 entries each for the Open and Closed Lists. You may also use a fixed-size array of 100 `Node` structs to store the nodes themselves. Milestone test cases will contain at most 100 inserted nodes in total.
>
> The Open and Closed Lists must still store `Node` pointers. For the milestone, `create_node()` may initialize an unused element of the fixed-size node array and return a pointer to it instead of dynamically allocating a node. This storage must remain valid throughout command processing.
>
> Do not call `free()` on fixed-size arrays or their elements. If your milestone implementation uses only fixed-size storage, `heap_free()` and `closed_list_free()` do not need to free that storage.
>
> For the final submission, nodes must be dynamically allocated, and both lists must use dynamically allocated, resizable arrays. Final test cases may require more than 100 nodes.
>
> All other requirements, including use of the provided structs and function signatures, min-heap ordering, and tie-breaking rules, still apply.

## Input and Output Specifications for Final Submission

### Setup Input And Output

The program prompts the user for the simulation setup in the following sequence:

1. **Start Position**: The program prints `Start:` and the user inputs the starting coordinates in the format `(<int>, <int>)`.
2. **End Position**: The program prints `End:` and the user inputs the ending coordinates in the format `(<int>, <int>)`.
3. **Walls**: The program prints `Walls:` and the user inputs wall definitions.

Following the `Walls:` prompt, the user inputs an arbitrary number of lines—each representing a wall—and concludes by entering `done`.

The full setup interaction format is as follows:

```
Start:
(<int>, <int>)
End:
(<int>, <int>)
Walls:
(<int>, <int>), <int>, <int>
...
(<int>, <int>), <int>, <int>
done
```
> [!Note] Only the labels `Start:`, `End:`, and `Walls:` are output by the program; all coordinates and the `done` command are user input.

Each wall entry is formatted with four integers:
* The first two integers represent the `X` and `Y` coordinates of the wall's southwest corner.
* The third and fourth integers represent its width and height, respectively.

#### Input Guarantees

* Each `<int>` is guaranteed to be in the range $[-2^{31}, 2^{31}-1]$.
* Representing the entire grid in a 2D array will be a reasonable size (will not crash the lab machine, but can be very large).
* All walls are guaranteed to not overlap with any previously placed wall.
* All walls are also guaranteed to not directly border any other wall.
* The width and height of each wall is guaranteed to be at least one.
* The user is guaranteed to place at least one wall.

The pathfinding search takes place on a grid representing the search space. **This grid is defined as the smallest rectangle that encompasses the start position, the end position, and all of the walls.**
The minimum coordinates include the start, end, and each wall's southwest cell. The maximum coordinates include the start, end, and each wall's northeast occupied cell, `(x + w - 1, y + h - 1)`.

The grid dimensions are:
* `width = max_x - min_x + 1`
* `height = max_y - min_y + 1`

Once all walls have been placed, the program should print out the coordinates of the southwest corner of the grid and its width and height in this format:

```
SW Corner:
(<int>, <int>), <int>, <int>
```

Each `<int>` being the values for X, Y, width, height respectively.

Then your program should print the header `Start Search:` to communicate the search has been started.

### Time-Step Input and Output

The pathfinding search progresses in discrete time-steps. Each time-step consists of the algorithm evaluating node(s) from the Open List and adding them to the Closed List.

For each time-step, the program must print:
1. **Newly Evaluated Nodes**: A space-separated list of coordinate pairs `(x, y)` representing the nodes that were newly added to the Closed List during this step.
2. **Status Code**: A single character on a new line indicating the result of the step:
   * `P`: **Progress**. The search step succeeded, the goal/destination has not yet been reached, and the search continues.
   * `F`: **Fail**. The search failed to find a path (e.g., the Open List became empty before reaching the end position).
   * `G`: **Goal Reached**. The end/goal position has been successfully reached, concluding the simulation.

The interaction format for a single time-step looks like this:

```
Start Search:
(x1, y1) (x2, y2) ...
P
```

#### Final Path

Once the simulation successfully concludes (marked by a `G` status code), the program must print the final calculated path for visualization. 

The final path must be printed in the following format:

```
Path:
(x1, y1) (x2, y2) (x3, y3) ...
```

Where the coordinates are space-separated and trace the complete path starting from the start position to the end position.

## Example Program Execution for Final Submission

Below is a trace of a sample run of your program.

```text
Start:
(0, 0)
End:
(9, 9)
Walls:
(4, 4), 3, 3
done
SW Corner:
(0, 0), 10, 10
Start Search:
(0, 0)
P
(0, 1)
P
(0, 2)
P
(0, 3)
P
(0, 4)
P
(0, 5)
P
(0, 6)
P
(0, 7)
P
(0, 8)
P
(0, 9)
P
(1, 9)
P
(2, 9)
P
(3, 9)
P
(4, 9)
P
(5, 9)
P
(6, 9)
P
(7, 9)
P
(8, 9)
P
(9, 9)
G
Path:
(0, 0) (0, 1) (0, 2) (0, 3) (0, 4) (0, 5) (0, 6) (0, 7) (0, 8) (0, 9) (1, 9) (2, 9) (3, 9) (4, 9) (5, 9) (6, 9) (7, 9) (8, 9) (9, 9)
```


## Memory Leaks and Valgrind
For this assignment, we will be checking for memory leaks. Do not forget to free any memory that was allocated throughout the program. To check for memory leaks we will use valgrind:
* Compile your program with the `-g` flag.
  * For example, `gcc -g -Wall -Werror -std=c99 assg.c -o assg`.
* Run valgrind to check for memory leaks using the command `valgrind —leak-check=full ./assg`.
* All allocated memory must be freed by the end of the program. If your program passes a mark case but leaks memory, 50% of the points for that case will be deducted.


## Optional Extension: Weighted A* Search (Bonus Marks)

This optional extension introduces a position-dependent variant of Weighted A*. Each traversable cell receives a heuristic weight based on its Manhattan distance to the goal. Cells closer to the goal receive larger weights.

The search uses `f(n) = g(n) + w(n) * h(n)` to determine priority. This changes the search order compared with ordinary A*, but does not guarantee faster search or a shortest path.

### 1. Terrain Mapping
The static grid is now replaced with five different terrains determined by the proximity to the goal:

| Weight ($w$) | Terrain Type | Proximity to Goal | Description |
| :--- | :--- | :--- | :--- |
| **Impassable** | **Water** | Anywhere | The Walls |
| **1** | **Grass** | Furthest | 
| **2** | **Sand** | Far-Medium | 
| **3** | **Stone** | Medium-Close | 
| **4** | **Mountain** | Very Close | 
| **5** | **Mountain Peak**| Immediate / At Goal |

### 2. Calculating $H_{\max}$
Because the search space dynamically resizes based on user input, the algorithm must first establish the grid's maximum possible search distance ($H_{\max}$). 

Once all wall barriers are placed, the program determines the **Southwest (SW) Corner** $(X_{\min}, Y_{\min})$, the **width**, and the **height** of the bounding box. 

$H_{\max}$ is calculated **once** during the setup phase by finding the maximum Manhattan distance from any of these four outer corners to the goal node:

$$H_{\max} = \max\Big( h(\text{SW}), h(\text{SE}), h(\text{NW}), h(\text{NE}) \Big)$$

Where the distance from a corner to the goal is computed as:
$$h(\text{corner}) = |x_{\text{corner}} - x_{\text{goal}}| + |y_{\text{corner}} - y_{\text{goal}}|$$

### 3. Weight Calculations via Linear Scale Inversion
To add terrain to the grid, a **linear scale inversion** is applied using the calculated $H_{\max}$. This maps the shrinking distance ($h(n) \to 0$) into an increasing weight integer ($1 \le w \le 5$).

For each traversable node, calculate its weight before inserting it into the Open List, its weight is calculated as:

$$w = \left\lfloor 5 - 4 \times \left(\frac{h(n)}{H_{\max}}\right) \right\rfloor$$

* **At the Furthest Distance ($h(n) = H_{\max}$):** The fraction evaluates to $1$, yielding a base weight of `1` (**Grass**).
* **At the Goal ($h(n) = 0$):** The fraction evaluates to $0$, yielding a maximum weight of `5` (**Mountain Peak**).
  
### 4. Weighted A* Algorithm
With dynamic weights, we now use **Weighted A***. The rest of the algorithm is identical to A* but the calculation of f(n) changes to include the weight bias:

$$f(n) = g(n) + w \cdot h(n)$$

Where:
* $f(n)$ is the total estimated cost of a path through node $n$.
* $g(n)$ is the cost of the path from the start node to node $n$. 
* $h(n)$ is the heuristic estimate of the cost to get from node $n$ to the end node. In this assignment, you **must** use the **Manhattan distance** as the heuristic function, calculated as:
    $$|x_1 - x_2| + |y_1 - y_2|$$
* $w$ represents the terrain weight.

### 5. Example Run for Weighted A* Extension

```text
Start: (0, 0)
End: (2, 2)
Walls: (1, 1), 1, 1
done
SW Corner:
(0, 0), 3, 3
Start Search:
(0, 0)
P
(0, 1)
P
(1, 0)
P
(0, 2)
P
(1, 2)
P
(2, 2)
G
Path:
(0, 0) (0, 1) (0, 2) (1, 2) (2, 2)
```
> [!NOTE]
> Create a new branch for the extension, call it 'extension-submission'
> 
## Marking Scheme

The marking scheme for this assignment is as follows:

This assignment is split into two parts, MS and FS

- MS contributes 50% to your Assignment 1 mark (/20).
    - MS is marked using your submitted `assg.c` file, *within* the `milestone-submission` branch on your submitted repo.
- FS contributes 50% to your Assignment 1 mark (/20).
    - FS is marked using your submitted `assg.c` file, on your `main` branch at the time of the final due date.
- Passing the provided test cases guarantees at least 50%. The other 50% will be based on our additional marking cases, which you do not have access to before submission.
    - The expected output for MS is the `1-X-output.txt` files located in the `Milestone Test Cases` directory.
    - The expected output for FS is the `1-X-output.txt` files located in the `Final Submission Test Cases` directory.
- You are encouraged to create your own checkscript to test your program automatically.
    - A checkscript should run your program with prepared input files and compare its actual output against expected output files, reporting which tests pass or fail.
    - You may use the check script from assignment 1 and refine it so it can be used for Assignment 2
- If you do not pass the testcases, you may still pass some of those marking cases and get some marks, so you should submit whatever you have!

**Bonus Marks:** Completing the extension (Weighted A*) on this assignment will earn you a 3% bonus directly to your final course grade.
* The bonus marks will be determined by the marking cases for the optional extension, which you will not have access to.
* Please make sure the extension is on a separate branch and not on the main branch with your final submission.


The final assignment mark will be the higher grade between:
- Milestone 1 + Final Submission (50% Milestone 1 + 50% Final Submission)
  OR
- Final Submission (100% Final Submission)

###  Submission Instructions

Once you have pushed your `milestone-submission` branch (or your `main` branch for the final submission), complete your submission by filling out the Google Form below:

> [!IMPORTANT]
> Submit your repository details using the official submission form:
> [Google Form Submission Link](https://forms.gle/zNGrp8ajoquZ7THt5)

Make sure your repository link is correct and that your `milestone-submission` branch has been pushed prior to the deadline.
