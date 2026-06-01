-- Define an Algebraic Data Type for MyLists
data MyList = Black                             -- Represents a fully black cell 
            | White                             -- Represents a fully white cell
            | Node MyList MyList MyList MyList  -- A node containing four subtrees (quadrants)
            deriving (Eq, Show)                 -- Enable equality checking and string representation

-- Function to create an all black cell
allBlack :: Int -> MyList
allBlack _ = Black  -- Ignores input size as allBlack is always represented as a single Black cell

-- Function to create an all white cell
allWhite :: Int -> MyList
allWhite _ = White  -- Ignores input size as allWhite is always represented as a single White cell

-- Function to create a MyList with four subtrees arranged clockwise
clockwise :: MyList -> MyList -> MyList -> MyList -> MyList
clockwise q1 q2 q3 q4 = Node q1 q2 q3 q4  -- q1 (top-left), q2 (top-right), q3 (bottom-right), q4 (bottom-left)

-- Function to create a MyList with four subtrees arranged anticlockwise
anticlockwise :: MyList -> MyList -> MyList -> MyList -> MyList
anticlockwise q1 q2 q3 q4 = Node q1 q4 q3 q2  -- q1 (top-left), q4 (bottom-left), q3 (bottom-right), q2 (top-right)

-- Function to calculates the depth of a given quadtree
calDepth :: MyList -> Int
-- Recursively compute the depth of all four quadrants
calDepth (Node q1 q2 q3 q4) = 1 + maximum[calDepth q1, calDepth q2, calDepth q3, calDepth q4]
-- Base case: If the node is a leaf (Black or White), return depth 0
calDepth _ = 0

-- Data structure representing a single cell in the quadtree
data Cell = Cell {
    content :: MyList,         -- Color of the block (Black, White)
    x_min   :: Int,            -- Starting x-coordinate of the cell
    x_max   :: Int,            -- Ending x-coordinate of the cell
    y_min   :: Int,            -- Starting y-coordinate of the cell
    y_max   :: Int,            -- Ending y-coordinate of the cell
    neighbors :: [MyList]      -- List of neighboring cells' colors
} deriving (Eq, Show)

-- Recursively assigns coordinates to each cell in the quadtree
giveCoordinates :: MyList -> Int -> Int -> Int -> Int -> [Cell]
giveCoordinates Black x_min x_max y_min y_max =
    [Cell Black x_min x_max y_min y_max []]            -- Initialize a black cell with no neighbors
giveCoordinates White x_min x_max y_min y_max =
    [Cell White x_min x_max y_min y_max []]            -- Initialize a white cell with no neighbors
giveCoordinates (Node q1 q2 q3 q4) x_min x_max y_min y_max =
    let mid_x = (x_min + x_max) `div` 2                -- Calculate the midpoint for x-coordinates
        mid_y = (y_min + y_max) `div` 2                -- Calculate the midpoint for y-coordinates
    in giveCoordinates q1 x_min mid_x mid_y y_max ++   -- Assign coordinates to the top-left quadrant (Q1)   
        giveCoordinates q2 mid_x x_max mid_y y_max ++  -- Assign coordinates to the top-right quadrant (Q2)
        giveCoordinates q4 x_min mid_x y_min mid_y ++  -- Assign coordinates to the bottom-left quadrant (Q4)
        giveCoordinates q3 mid_x x_max y_min mid_y     -- Assign coordinates to the bottom-right quadrant (Q3)

-- Function to check if two cells are neighbors (adjacent in the up, down, left, or right direction)
checkNeighbor :: Cell -> Cell -> Bool
checkNeighbor a b =
    (y_max a == y_min b && x_min a < x_max b && x_max a > x_min b) ||  -- Check the upper neighbors
    (y_min a == y_max b && x_min a < x_max b && x_max a > x_min b) ||  -- Check the lower neighbors
    (x_max a == x_min b && y_min a < y_max b && y_max a > y_min b) ||  -- Check the right neighbors
    (x_min a == x_max b && y_min a < y_max b && y_max a > y_min b)     -- Check the left neighbors

-- Find neighbors in all cells
findNeighbors :: [Cell] -> [Cell]
findNeighbors cells =
    -- Updates the neighbor list for each cell in the given list
    [ cell { neighbors = nowNeighbors cells cell } | cell <- cells ]

-- Find the neighbor in the current cell
nowNeighbors :: [Cell] -> Cell -> [MyList]
nowNeighbors cells cell =
    [content c | c <- cells,
        -- Check if the cell shares a vertical edge (top or bottom)
        ((y_max c == y_min cell || y_min c == y_max cell) && (x_min c < x_max cell && x_max c > x_min cell)) ||  
        -- Check if the cell shares a horizontal edge (left or right)
        ((x_max c == x_min cell || x_min c == x_max cell) && (y_min c < y_max cell && y_max c > y_min cell))]

-- Function to invert the color of a given MyList element.
changeColor :: MyList -> MyList
changeColor Black = White -- If the input is Black, it returns White
changeColor White = Black -- If the input is White, it returns Black
changeColor node  = node  -- If the input is a Node, it remains unchanged

-- Function to count the number of neighboring cells 
countOpposite :: Cell -> Int
countOpposite cell =
    length $ filter (\col -> col == changeColor (content cell)) (neighbors cell)
    -- Filter the neighbors that have the opposite color
    -- Return the count of opposite-colored neighbors

-- Function to determine whether the cell's color should be inverted
checkColor :: Cell -> Cell
checkColor cell =
    let totalNeighbors = length (neighbors cell)            -- Get the total number of neighbors
        oppositeCount  = countOpposite cell                 -- Count the number of opposite-colored neighbors
    in if oppositeCount > totalNeighbors `div` 2            -- Majority condition
        then cell { content = changeColor (content cell) }  -- Invert color
        else cell                                           -- Keep the same color

-- Merges a list of cells back into a quadtree structure
mergeCells :: Int -> Int -> Int -> Int -> [Cell] -> MyList
mergeCells xmin xmax ymin ymax cells =
    -- Extract the cells that exactly match the given region boundaries
    let regionCells = [c | c <- cells, x_min c == xmin, x_max c == xmax, y_min c == ymin, y_max c == ymax] 
    in case regionCells of
        -- If there is only one cell in the region, return its content (Black or White)
        [singleCell] -> content singleCell
        -- Otherwise, divide the region into four quadrants and recursively merge them
        _ ->
            let mid_x = (xmin + xmax) `div` 2                -- Compute the middle x-coordinate
                mid_y = (ymin + ymax) `div` 2                -- Compute the middle y-coordinate
                q1 = mergeCells xmin mid_x mid_y ymax cells  -- Merge top-left quadrant
                q2 = mergeCells mid_x xmax mid_y ymax cells  -- Merge top-right quadrant
                q3 = mergeCells xmin mid_x ymin mid_y cells  -- Merge bottom-left quadrant
                q4 = mergeCells mid_x xmax ymin mid_y cells  -- Merge bottom-right quadrant
            in Node q1 q2 q4 q3                              -- Construct a new Node from the four quadrants

-- The blur function applies color transformation based on neighbor analysis
-- ===============================
-- The blur function:
-- 1. We calculate depth to know how subdivided the quadtree is
-- 2. We assign coordinates for accurate neighbor detection
-- 3. Neighbors are checked based on shared edges
-- 4. We only flip color if a majority of neighbors are opposite
--    -> This ensures a structured blur without losing all contrast
-- 5. The quadtree is then reconstructed, preserving its hierarchy
-- ===============================
blur :: MyList -> MyList
blur root =
    let depth         = calDepth root                       -- Compute the depth of the quadtree
        size          = 2 ^ depth                           -- Determine the quadtree resolution (2^depth)
        cells         = giveCoordinates root 0 size 0 size  -- Assign coordinates to each cell
        allNeighbors = findNeighbors cells                  -- Find all neighbors for each cell
        updatedCells  = map checkColor allNeighbors         -- Apply color update based on neighbors
    in mergeCells 0 size 0 size updatedCells                -- Reconstruct the quadtree from updated cells

-- My main idea does not rely on the absolute size of each rectangular block, 
-- but rather on the depth of the quadtree, which determines how much area a block occupies within the overall rectangle. 
-- Consequently, if the smallest block occupies a relatively large portion of the image, performance can improve.

-- In this assignment, I focused on two primary goals:
-- 1. Accurate identification of neighbors
-- 2. Ensuring data processing completes in a reasonable time

-- Initially, I planned to determine a node’s color by checking sibling and parent neighbors using quadtree recursion. 
-- While sibling neighbors were straightforward, handling parent neighbors with different depths proved challenging—especially 
-- if neighbor of the parent node’s size was larger or smaller than the current node. 
-- Because I could not devise a robust way to handle those cases, I pivoted to using coordinate-based neighbor detection.

-- The coordinate-based method accurately finds neighbors, but its 𝑂(𝑛^2) time complexity makes it less efficient. 
-- I explored optimization by introducing a bin technique, which reduces the number of blocks compared by partitioning the image 
-- into sub-areas. Although this reduced runtime from 35 seconds to 13 seconds (for a 2^10 input in the GHCi interpreter), 
-- it failed some test scenarios by misidentifying neighbors at bin boundaries.

-- Therefore, I prioritized accuracy and retained the original coordinate-based approach. 
-- When compiled with GHC (-O2), this method successfully processes a 2^10 input in under 3 seconds. However, 
-- I believe there is still a more efficient way to optimize it, even though I have yet to discover it.
