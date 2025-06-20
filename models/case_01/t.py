import fastmesh as fm
vertices_all, faces_all, normals_all = fm.load("./API3O9JV_lower.bmesh")
fm.write(filepath="API3O9JV_lower.obj", filetype="obj", vertices=vertices_all, faces=faces_all,to_pointcloud=False)
