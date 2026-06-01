import sys

def read_graph(input_txt):
    graph = {}  #Set node as key and neighbours as values
    expected_node = 1   #Node starts at 1
    try:
        with open(input_txt, 'r') as file:  #Read file
            for line in file:   #Split input file by line
                parts = line.strip().split(',')   #Split line by parts

                if not all(part.isdigit() for part in parts):   #Input file must be consisted as number
                    raise ValueError("Invalid input format: inputs should be number")

                node = int(parts[0])    #First one indicate Node
                neighbours = set(map(int, parts[1:]))    #Others indicate Node`s neighbours

                if node != expected_node:   #Check order of node sequentially increasing
                    raise ValueError(f"Invalid input format: Node {expected_node} is missing")

                if node in graph:   #If same number node is exist then occur error
                    raise ValueError(f"Invalid input format: Duplicate node {node} detected")

                if not neighbours:   #All node has at least one neighbour
                    raise ValueError(f"Invalid input format: Node {node} must have at least one neighbour")

                graph[node] = neighbours   #Value: neighbours
                expected_node += 1  #Move to next node

        for node, neighbours in graph.items():   #Get key and values from dictionary
            for neighbour in neighbours:
                if neighbour not in graph or node not in graph[neighbour]:   #Check node or neighbour is in graph
                    raise ValueError(f"Invalid input format: Inconsistent interference relationship between {node} and {neighbour}")

        if len(graph) > 50:   #Input size is max as 50
            raise ValueError("Invalid input format: number of inputs can be set by 50")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
    return graph

def top_down_colouring(graph):
    if not graph:   #Check graph is empty
        print("Error: Input graph is empty")
        sys.exit(1)
        
    nodes = list(graph.keys())   #Make list by all nodes
    nodes.sort(key=lambda x: (-len(graph[x]), x))   #Sort with most values to least values
    
    all_colours = [chr(i) for i in range(65, 91)]    #Colour is capital alphabet
    colour_map = {}

    for node in nodes:
        used_colours = {colour_map[neighbour] for neighbour in graph[node] if neighbour in colour_map}   #Check neighbours colour
        available_colours = next((colour for colour in all_colours if colour not in used_colours), None)   #All colour - neighbours colour
        if available_colours is None:   #Check neghbours use all colour
            print("Error: No available colour (more than 26 needed)")
            sys.exit(1)
        colour_map[node] = available_colours   #Set capital alphabet of least ascii code to node
    return colour_map

def write_output(output_txt, colour_map):
    try:
        with open(output_txt, 'w') as file:
            sorted_nodes = sorted(colour_map.keys())   #Sort by node order
            for i, node in enumerate(sorted_nodes):   #Write output file
                if i < len(sorted_nodes) - 1:  
                    file.write(f"{node}{colour_map[node]}\n")
                else:
                    file.write(f"{node}{colour_map[node]}")
    except Exception as e:
        print(f"Error writing to file: {e}")
        sys.exit(1)

def main():
    if len(sys.argv) != 3:  #Expecting 3 arguments
        print("Format: python3 ColourGraph.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    graph = read_graph(input_file)
    colour_map = top_down_colouring(graph)
    write_output(output_file, colour_map)

if __name__ == "__main__":
    main()