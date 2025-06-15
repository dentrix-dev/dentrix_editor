import numpy as np
import trimesh
import argparse
import json
import os

def load_json(path):
    with open(path) as f:
        return json.load(f)

def write_files(tooth_faces, fname):

    finame = fname.split("/")[-1].split(".")[0]
    fname = os.path.basename(os.path.dirname(fname))

    folder = os.getcwd() + fr'\{fname}'
    # if fname not in os.listdir():
    #     os.mkdir(folder)

    with open(folder + rf"/{finame}.txt", "w") as f:
        wisdom_teeth_ids = [18,28,38,48]
        wisdom_teeth = []
        flag = False
        for tooth_id in sorted(tooth_faces.keys()):
            face_indices = tooth_faces[tooth_id]

            print(f"Tooth ID: {tooth_id}")

            face_indices_str = ' '.join(map(str, face_indices))

            if tooth_id in wisdom_teeth_ids:
                flag = True
                wisdom_teeth.append(face_indices_str)
            f.write(f"{face_indices_str}\n\n")
        if flag:
            for i in wisdom_teeth:
                f.write(f"{i}\n\n")

def extract_faces_by_tooth(mesh, vertex_labels, valid_tooth_labels=range(11, 50)):
    face_labels = {}
    faces = mesh.faces

    bg = 0
    for i, tri in enumerate(faces):
        tri_labels = vertex_labels[tri]
        unique, counts = np.unique(tri_labels, return_counts=True)
        majority_label = unique[np.argmax(counts)]

        if majority_label in valid_tooth_labels:
            if majority_label not in face_labels:
                face_labels[majority_label] = []
            face_labels[majority_label].append(i)
        else:
            bg += 1
    print(f"Background faces: {bg}")
    return face_labels  


def process_mesh(mesh_path, label_json_path):
    # fname = os.path.basename(os.path.dirname(mesh_path))

    print(f"\nProcessing: {mesh_path.split('/')[-1]}")
    print("Loading mesh...")
    mesh = trimesh.load(mesh_path, process=False)
    vertex_labels = np.array(load_json(label_json_path)["labels"])

    tooth_faces = extract_faces_by_tooth(mesh, vertex_labels)

    # Print statistics
    total_tooth_faces = 0
    for tooth_id, faces in tooth_faces.items():
        total_tooth_faces += len(faces)
        print(f"Tooth {tooth_id}: {len(faces)} faces")

    total_faces = len(mesh.faces)
    print(f"Total tooth faces: {total_tooth_faces}")
    print(f"Total mesh faces: {total_faces}")
    print(f"Background faces: {total_faces - total_tooth_faces}")

    write_files(tooth_faces=tooth_faces, fname=mesh_path)
    # print("visulaizing")
    # visualize_teeth_by_face(mesh, tooth_faces)

def main():
    parser = argparse.ArgumentParser(description="Process upper and lower jaw meshes with labels.")
    parser.add_argument("--upper_mesh", required=True, help="Path to upper jaw .obj file")
    parser.add_argument("--lower_mesh", required=True, help="Path to lower jaw .obj file")
    parser.add_argument("--upper_label", required=True, help="Path to upper jaw labels .json")
    parser.add_argument("--lower_label", required=True, help="Path to lower jaw labels .json")

    args = parser.parse_args()

    process_mesh(args.upper_mesh, args.upper_label)
    process_mesh(args.lower_mesh, args.lower_label)

if __name__ == "__main__":
    main()

