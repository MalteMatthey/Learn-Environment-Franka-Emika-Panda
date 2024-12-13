#ifndef NOTEBOOK_CONVERTER_HPP
#define NOTEBOOK_CONVERTER_HPP

#include <QObject>
#include <QString>
#include <QDir>
#include <QByteArray>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @class NotebookConverter
 * @brief A class for converting Jupyter notebooks to Python scripts and processing task pools.
 * 
 * The NotebookConverter class provides functionality to convert Jupyter notebooks into Python scripts,
 * ignoring code cells with a "solution" tag. It also processes and modifies notebooks from a task pool,
 * removing solution code and adding metadata tags to indicate modified cells.
 * 
 * The class includes methods for reading and writing files, parsing JSON content, and processing notebook cells.
 * It ensures that the structure of the notebook is preserved while removing solution code and adding necessary metadata.
 */
class NotebookConverter : public QObject {
    Q_OBJECT
public:
    explicit NotebookConverter(QObject *parent = nullptr);

    /**
     * @brief Converts a Jupyter notebook to a Python script. Ignoring code cells with "solution" tag.
     * @param notebookPath Path to the notebook file.
     * @return True if conversion is successful, else False.
     */
    bool convertNotebook(const QString &notebookPath);

    /**
     * @brief Modifies and copies all notebooks from task_pool to users workspace.
     */
    void processTaskPool();

    /**
     * @brief Toggles the solution code in a Jupyter notebook.
     * @param filePath Path to the notebook file.
     * @param solutionFilePath Path to the solution notebook file.
     */
    void toggleSolution(const QString &filePath, const QString &solutionFilePath);

    /**
     * @brief Removes solution code from a Jupyter notebook.
     * @param notebookPath Path to the notebook file.
     */
    void resetNotebook(const QString &notebookSolutionPath);

    /**
     * @brief Checks if the notebook contains any solution cells.
     * @param notebookPath Path to the notebook file.
     * @return True if solution cells are present, else False.
     */
    static bool hasSolutionCells(const QString &notebookPath);

private:
    /**
     * @brief Removes solution code from a Jupyter notebook.
     * 
     * This method processes a Jupyter notebook to remove any solution code 
     * that is marked with specific markers ("#### YOUR CODE HERE ####"). 
     * It reads the notebook file, parses its JSON content, processes each 
     * cell to remove the solution code, and then writes the modified content 
     * back to the notebook file. The method ensures that the notebook's 
     * structure is preserved and adds metadata tags to indicate which cells 
     * have had their solutions removed.
     * 
     * @param notebookPath Path to the notebook file.
     */
    void removeSolutionFromNotebook(const QString &notebookPath);

    QByteArray readFile(const QString &notebookPath);
    json parseJson(const QByteArray &data, const QString &notebookPath);
    void processCells(json &notebook, const QString &notebookPath);
    void processCell(json &cell, const QString &notebookPath, int i);
    void writeFile(const json &notebook, const QString &notebookPath);
    void copyAndModifyNotebooks(const QDir &sourceDir, const QDir &destDir);

    void removeSolutionCells(const QString &notebookPath);
    void addSolutionCells(const QString &notebookPath, const QString &solutionPath);
    void manipulateSolutionCellContent(json &solutionCell);
};

#endif // NOTEBOOK_CONVERTER_HPP