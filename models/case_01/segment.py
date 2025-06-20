import numpy as np
import trimesh
import argparse
import json
import os
from pathlib import Path

def load_json(path):
    with open(path) as f:
        return json.load(f)

def write_files(tooth_faces, fname):
    finame = os.path.splitext(os.path.basename(fname))[0]
    folder = os.path.join(os.getcwd(), os.path.basename(os.path.dirname(fname)))

    if not os.path.exists(folder):
        os.makedirs(folder)

    output_path = os.path.join(folder, f"{finame}.txt")
    with open(output_path, "w") as f:
        for tooth_id in sorted(tooth_faces.keys()):
            face_indices = tooth_faces[tooth_id]
            face_indices_str = ' '.join(map(str, face_indices))
            f.write(f"tooth_id: {tooth_id}\n{face_indices_str}\n\n")

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
    fname = os.path.basename(os.path.dirname(mesh_path))

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

