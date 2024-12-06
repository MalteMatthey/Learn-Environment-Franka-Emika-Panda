import os
import sys
from nbformat import read, NO_CONVERT
from nbconvert import ScriptExporter

def convert(notebook_path):
    """
    Converts a Jupyter notebook to a Python file and saves it to the specified path.

    :param notebook_path: Path to the Jupyter notebook file (input).
    """
    # Construct the output path for the converted Python script
    output_path = os.path.join(os.path.dirname(__file__), "converted.py")

    # Create a ScriptExporter instance
    exporter = ScriptExporter()
    
    # Open and read the notebook file as a valid nbformat object
    with open(notebook_path, 'r', encoding='utf-8') as nb_file:
        notebook_content = read(nb_file, as_version=NO_CONVERT)  # Load the notebook correctly

    # Convert the notebook to a Python script
    python_script, _ = exporter.from_notebook_node(notebook_content)

    # Save the Python script, create the file if it doesn't exist, and overwrite if it does
    with open(output_path, 'w', encoding='utf-8') as py_file:
        py_file.write(python_script)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 convert.py <path-to-notebook>")
        sys.exit(1)

    notebook_path = sys.argv[1]

    convert(notebook_path)
