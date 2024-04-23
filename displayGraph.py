#napisane przy pomocy chatgpt 

import argparse
import matplotlib.pyplot as plt
import numpy as np

def read_cube_coordinates(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        cube_coordinates = []
        for line in lines:
            if line.strip():  # Check if the line is not empty
                coords = tuple(map(int, line.strip().split(',')))
                cube_coordinates.append(coords)
    file.close()
    return cube_coordinates

def main():
    parser = argparse.ArgumentParser(description='Plot 3D cubes from two files with specified graph size.')
    parser.add_argument('--size', metavar='N', type=int, nargs=3, default=(20, 20, 20),
                        help='size of the graph in X Y Z dimensions (default: 20 20 20)')
    parser.add_argument('obstacle_file', metavar='obstacle_file', type=str,
                        help='path to the file containing obstacle coordinates')
    parser.add_argument('path_file', metavar='path_file', type=str,
                        help='path to the file containing path coordinates')

    args = parser.parse_args()

    # Read cube coordinates from two separate files
    cube_coordinates1 = read_cube_coordinates(args.obstacle_file)
    cube_coordinates2 = read_cube_coordinates(args.path_file)

    # prepare some coordinates
    x, y, z = np.indices(tuple(args.size))

    # Initialize voxelarray
    voxelarray1 = np.zeros_like(x, dtype=bool)
    voxelarray2 = np.zeros_like(x, dtype=bool)

    # Populate voxelarray with cubes from first file
    for cube_coord in cube_coordinates1:
        cube_mask = (x == cube_coord[0]) & (y == cube_coord[1]) & (z == cube_coord[2])
        voxelarray1 |= cube_mask

    # Populate voxelarray with cubes from second file
    for cube_coord in cube_coordinates2:
        cube_mask = (x == cube_coord[0]) & (y == cube_coord[1]) & (z == cube_coord[2])
        voxelarray2 |= cube_mask

    # set the colors of each object
    colors = np.empty(voxelarray1.shape, dtype=object)
    colors[voxelarray1] = 'blue'
    colors[voxelarray2] = 'red'

    # and plot everything with transparency
    ax = plt.figure().add_subplot(projection='3d')
    ax.voxels(voxelarray1 | voxelarray2, facecolors=colors, edgecolor='k', alpha=0.5)

    plt.show()

if __name__ == "__main__":
    main()
