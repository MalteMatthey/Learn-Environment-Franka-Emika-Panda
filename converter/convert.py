import os
import sys
from nbformat import read, NO_CONVERT
from nbconvert import ScriptExporter


def convert(notebook_path):
    """
    Konvertiert ein Jupyter-Notebook in eine Python-Datei und speichert es im angegebenen Pfad.

    :param notebook_path: Pfad zur Jupyter-Notebook-Datei (Input).
    :param output_path: Zielpfad für die konvertierte Python-Datei (Output).
    """

    output_path = os.path.join(os.path.dirname(__file__), "converted.py")

    exporter = ScriptExporter()
    
    # Öffne und lese die Notebook-Datei als gültiges nbformat-Objekt
    with open(notebook_path, 'r', encoding='utf-8') as nb_file:
        notebook_content = read(nb_file, as_version=NO_CONVERT)  # Lade das Notebook korrekt

    # Konvertieren des Notebooks
    python_script, _ = exporter.from_notebook_node(notebook_content)

    # Speichern des Python-Skripts
    with open(output_path, 'w', encoding='utf-8') as py_file:
        py_file.write(python_script)

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Verwendung: python3 convert_and_run.py <Pfad-zum-Notebook> ")
        sys.exit(1)

    notebook_path = sys.argv[1]

    convert(notebook_path)
    