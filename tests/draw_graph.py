import json

def clean_data(f):
    # Read the JSON file
    with open(f, 'r') as file:
        data = file.read()

    # Remove all new lines and spaces
    data = ''.join(data.split())

    data = data.replace(',}', '}')

    return data
def draw_node(node):
    if node is None:
        return ""
    else:
        left_edge_label = f"[edge label={{node[midway, left] {{{node.get('left_name', '')}}}}}]"
        right_edge_label = f"[edge label={{node[midway, right] {{{node.get('right_name', '')}}}}}]"
        return f"[{{${node['string']}$}}{left_edge_label}{draw_node(node.get('left'))}{right_edge_label}{draw_node(node.get('right'))}]"

def draw_tree(root):
    latex_code = r"""
\documentclass{article}
\usepackage{forest}

\begin{document}

\begin{forest}
  for tree={
    circle,
    draw,
    minimum width=1.5em,
    inner sep=2pt,
    s sep=20pt,
    l sep=20pt,
  },
  where n children=0{
    tier=terminus,
  }{},
""" + draw_node(root) + r"""
\end{forest}

\end{document}
"""
    return latex_code

# Example JSON data with edge names
# json_data = '''
# {
#   "string": "root",
#   "left_name": "left_edge",
#   "right_name": "right_edge",
#   "left": {
#     "string": "left"
#   },
#   "right": {
#     "string": "right",
#     "left_name": "right_left_edge",
#     "right_name": "right_right_edge",
#     "left": {
#       "string": "right_left"
#     },
#     "right": {
#       "string": "right_right"
#     }
#   }
# }
# '''
from graphviz import Digraph, Graph
import sys
def graphviz_plot(root_node):


    def create_graph(node,node_num,dot):
        
        global tc_num
       
        dot.node(str(node_num), node["string"])
        curr_node_name=str(node_num)
        node_num+=1

        # Add left child if it exists
        # if "left" in node:
        #     left_child,g,node_num = create_graph(node["left"],node_num,dot)
        #     dot=g
        #     #print(left_child)
        #     dot.edge(curr_node_name,left_child,str(node["left_name"]))

        # # Add right child if it exists
        # if "right" in node:
        #     right_child,g,node_num = create_graph(node["right"],node_num,dot)
        #     dot=g
        #     dot.edge(curr_node_name,right_child,str(node["right_name"]))
        
        for name in node.keys():
            if(name[0:4]=="edge" and name[4]!="_"):
                left_child,g,node_num = create_graph(node[name],node_num,dot)
                dot=g
                #print(left_child)
                dot.edge(curr_node_name,left_child,str(node[name[0:4]+"_name"+name[4:]]))

        return curr_node_name,dot,node_num

    # Start with the root node
    dot=Graph()

    tree_root,tree_graph,nodes = create_graph(root_node,0,dot)

    # Render the graph
    tree_graph.render(f"binary_tree_{tc_num}")



tc_num=sys.argv[1]
json_data=json.loads(clean_data(f"tc_{tc_num}.json"))
#print(json_data)
# Generate LaTeX code for drawing the tree with edge labels

graphviz_plot(json_data)
latex_code = draw_tree(json_data)

# Write LaTeX code to a file
with open("tree_with_edge_labels.tex", "w") as f:
    f.write(latex_code)  