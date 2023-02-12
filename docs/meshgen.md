comman problrems:

crashing for node in way not existing in nodes
there is a problem with the osm file - probaly the one in cash - this is probably from a timeout on osm request check osm file to be sure
timeout errors are displayed at the bottom of the file

right now it seems chunks of division 10 are just too big to queerry wihtout timing out


problem chunks:

(40.715332,-74.267578)_origin--(0.021973,0.021973)_size - geom not multi polly after intersection operation on building with chunk bounds


new problem chunks
(40.253906,-74.443359)_origin--(0.087891,0.087891)_size -- it has a runway which has 2 nodes but gets turned into a line of one point idk why


(40.781250,-74.179688)_origin--(0.043945,0.043945)_size -- when running basePath = mesh::binterseciton({{basePath}},{{chunk.chunk.polygon()}})[0][0]; (currnetly line 125 of bulding creator) it has an out of bounds exeption -- i dont know what edge case but will tempoareatly 

(40.693359,-74.047852)_origin--(0.043945,0.043945)_size -- casues access violation in converting from binary op library during a bibtersection op line 314 of gerneration system.cpp: pol.polygon = mesh::binterseciton(framePoints, {pol.polygon})[0];