# Loop Interchange Implementation

In LLVM IR, each loop has three main pieces:
- PHI node: holds the loop counter (i or j)
- Comparison: checks if we should exit (like i < N)
- Increment: adds 1 to the counter each iteration

## How It Works

### Step 1: Find Loop Structure

First, I locate the PHI nodes and comparison instructions for both loops. The PHI nodes tell me where the loop counters are, and the comparisons show me the loop bounds.

### Step 2: Clone the Increment Instructions

This step took me a while to figure out. At first, I tried just swapping the PHI node values directly, but that broke SSA form. The problem is that each increment instruction (like i++ or j++) lives in its own loop's latch block. If I swap the PHI values without moving the increments, the outer PHI would try to use an instruction from the inner latch block, which violates dominance rules.

The solution is to clone each increment instruction and put it in the right latch block:
- Clone j++ and put it in the outer latch
- Clone i++ and put it in the inner latch

Then I update the cloned instructions to reference the correct PHI nodes.

### Step 3: Swap PHI Values

Now I can safely swap the incoming values of the PHI nodes. The outer PHI gets the inner loop's start value and the new j++ instruction, and vice versa for the inner PHI.

### Step 4: Fix Variable References

After swapping the PHIs, I need to update all the places in the code that use these variables. Anywhere the inner loop body used the outer loop counter, it should now use the inner counter instead, and the opposite for the outer loop.

### Step 5: Swap Loop Bounds

The final step is swapping the bounds. If the outer loop originally ran N times and the inner ran M times, now the outer should run M times and the inner should run N times. I do this by swapping the bound values in the comparison instructions.

At the end, I clean up by removing the old increment instructions that are no longer used.
