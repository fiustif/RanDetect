import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from mpl_toolkits.mplot3d import Axes3D

def read_data(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            values = [value.rstrip(', ') for value in line.strip().split(',')]
            # Remove empty elements
            values = [value for value in values if value]
            if values:
                row = [float(value) for value in values]
                data.append(row)
    return np.array(data)

def plot_3d_combined(model_data, multipliers_data):
    fig = plt.figure()

    # Model data
    ax1 = fig.add_subplot(131, projection='3d')
    x, y = np.meshgrid(range(model_data.shape[1]), np.flipud(range(model_data.shape[0])))
    ax1.plot_surface(x, y, model_data, rstride=1, cstride=1, alpha=0.7)
    ax1.set_xlabel('Rep Length')
    ax1.set_ylabel('Reps Distance')
    ax1.set_zlabel('Value')
    ax1.set_title("Model Data")

    # Multipliers
    ax2 = fig.add_subplot(132, projection='3d')
    x, y = np.meshgrid(range(multipliers_data.shape[1]), np.flipud(range(multipliers_data.shape[0])))
    ax2.plot_surface(x, y, multipliers_data, rstride=1, cstride=1, alpha=0.7)
    ax2.set_xlabel('Rep Length')
    ax2.set_ylabel('Reps Distance')
    ax2.set_zlabel('Value')
    ax2.set_title("Multipliers")

    # Colored graph
    ax3 = fig.add_subplot(133, projection='3d')
    x, y = np.meshgrid(range(model_data.shape[1]), np.flipud(range(model_data.shape[0])))
    
    # Colormap
    colors = plt.cm.RdBu_r(multipliers_data / multipliers_data.max())
    
    ax3.plot_surface(x, y, model_data, facecolors=colors, rstride=1, cstride=1, alpha=0.7)
    ax3.set_xlabel('Rep Length')
    ax3.set_ylabel('Reps Distance')
    ax3.set_zlabel('Value')
    ax3.set_title("Model Data Colored by Multipliers")

    plt.show()

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    print('''
    ____              ____       __            __ 
   / __ \____ _____  / __ \___  / /____  _____/ /_
  / /_/ / __ `/ __ \/ / / / _ \/ __/ _ \/ ___/ __/
 / _, _/ /_/ / / / / /_/ /  __/ /_/  __/ /__/ /_  
/_/ |_|\__,_/_/ /_/_____/\___/\__/\___/\___/\__/  Visualizer 1.0
                                                  
    ''''')
    folder_name = input("[*] Enter model name: ")
    folder_path = os.path.join(script_dir, folder_name)
    
    model_path = os.path.join(folder_path, "Model.r4nd")
    multipliers_path = os.path.join(folder_path, "Multipliers.r4nd")

    if os.path.exists(model_path) and os.path.exists(multipliers_path):
        model_data = read_data(model_path)
        multipliers_data = read_data(multipliers_path)

        plot_3d_combined(model_data, multipliers_data)

    else:
        print("[*] [ERROR] Unable to find specified model")

if __name__ == "__main__":
    main()
