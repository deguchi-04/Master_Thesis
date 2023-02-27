# Needs tensorflow 1.5
#python -m tensorflow.python.tools.optimize_for_inference 
#--input /home/thaidy/Documents/Master_Thesis/TF_CNN//frozen_graph.pb 
#--output /home/thaidy/Documents/Master_Thesis/TF_CNN/opt.pb 
#--frozen_graph=True --input_names=x --output_names=Identity

import tensorflow as tf

optimized_graph_path = "opt.pb"
output_pbtxt = "opt.pbtxt" # Read the graph.
with tf.compat.v1.gfile.FastGFile(optimized_graph_path, "rb") as f:
    graph_def = tf.compat.v1.GraphDef()
    graph_def.ParseFromString(f.read())# Remove Const nodes.
for i in reversed(range(len(graph_def.node))):
    if graph_def.node[i].op == 'Const':
        del graph_def.node[i]
    for attr in ['T', 'data_format', 'Tshape', 'N', 'Tidx', 'Tdim',
                 'use_cudnn_on_gpu', 'Index', 'Tperm', 'is_training',
                 'Tpaddings']:
        if attr in graph_def.node[i].attr:
            del graph_def.node[i].attr[attr]# Save as text.
tf.compat.v1.train.write_graph(graph_def, "", output_pbtxt, as_text=True)