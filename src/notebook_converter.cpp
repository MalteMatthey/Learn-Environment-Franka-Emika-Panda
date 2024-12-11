#include "learn_environment/notebook_converter.hpp"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <ros/package.h>
#include <ros/ros.h>

using json = nlohmann::json;

const QString NotebookConverter::CONVERTED_SCRIPT_PATH = "/converter/converted.py";
const QString PACKAGE_NAME = "learn_environment";
const QString SOLUTION_SCRIPTS_SOURCE_PATH = "/task_pool/solution_scripts";
const QString USER_WORKSPACE = "/tasks";
const QString TASK_CELL_TAG = "task_cell";
const QString SOLUTION_REMOVED_CELL_TAG = "solution_removed_cell";
const QString SOLUTION_CELL_TAG = "solution_cell";
const QString WRITE_CODE_MARKER = "#### YOUR CODE HERE ####";
const QString SOLUTION_CELL_HEADER = "##############################################################\n"
                                     "####     THIS IS A SOLUTION CELL. IT WILL NOT EXECUTE.    ####\n"
                                     "#### YOU CAN RUN THE SOLUTION DIRECTLY WITHIN THE PLUGIN. ####\n"
                                     "####    USE THIS CELL AS INSPIRATION FOR YOUR OWN CODE.   ####\n"
                                     "##############################################################\n";
const QString SOLUTION_CODE_PLACEHOLDER_START = "# ↓↓↓↓ SOLUTION CODE HERE ↓↓↓↓ #";
const QString SOLUTION_CODE_PLACEHOLDER_END = "# ↑↑↑↑ SOLUTION CODE HERE ↑↑↑↑ #";

QString NotebookConverter::getConvertedScriptPath() {
    return CONVERTED_SCRIPT_PATH;
}

QString NotebookConverter::getPackagePath() {
    QString packagePath;
    try {
        packagePath = QString::fromStdString(ros::package::getPath(PACKAGE_NAME.toStdString()));
    } catch (...) {
        qCritical() << "An error occurred while getting the package path of" << PACKAGE_NAME;
        return QString();
    }
    return packagePath;
}

NotebookConverter::NotebookConverter(QObject *parent) : QObject(parent) {}

bool NotebookConverter::convertNotebook(const QString &notebookPath) {
    QFile inputFile(notebookPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open notebook:" << notebookPath;
        return false;
    }

    QByteArray data = inputFile.readAll();
    inputFile.close();

    json notebookJson;
    try {
        notebookJson = json::parse(data.toStdString());
    } catch (json::parse_error &e) {
        qDebug() << "JSON parse error in notebook:" << notebookPath;
        return false;
    }

    if (!notebookJson.contains("cells")) {
        qDebug() << "No cells found in notebook:" << notebookPath;
        return false;
    }
    
    QString packagePath = getPackagePath();
    if (packagePath.isEmpty()) {
        return false;
    }

    QString outputPath = packagePath + CONVERTED_SCRIPT_PATH;
    QFile outputFile(outputPath);
    QDir().mkpath(QFileInfo(outputPath).dir().path());

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to create output file:" << outputPath;
        return false;
    }

    QTextStream out(&outputFile);
    for (const auto &cell : notebookJson["cells"]) {
        if (cell["cell_type"] == "code") {
            // ignore cells with solution tag
            bool hasSolutionTag = false;
            if (cell["metadata"].contains("tags")) {
                for (const auto &tag : cell["metadata"]["tags"]) {
                    if (tag.get<std::string>() == "solution") {
                        hasSolutionTag = true;
                        break;
                    }
                }
            }
            if (hasSolutionTag) {
                continue;
            }

            for (const auto &line : cell["source"]) {
                out << QString::fromStdString(line.get<std::string>());
            }
            out << "\n\n";
        }
    }

    outputFile.close();
    qDebug() << "Conversion successful for:" << notebookPath;
    return true;
}

void NotebookConverter::processTaskPool() {
    qDebug() << "Processing task pool...";
    QDir sourceDir(getPackagePath() + SOLUTION_SCRIPTS_SOURCE_PATH);
    QDir destDir(getPackagePath() + USER_WORKSPACE);

    // Recursively copy and modify notebooks
    copyAndModifyNotebooks(sourceDir, destDir);
}

void NotebookConverter::copyAndModifyNotebooks(const QDir &sourceDir, const QDir &destDir) {
    // Ensure the destination directory exists
    if (!destDir.exists()) {
        QDir().mkpath(destDir.absolutePath());
    }

    // Copy subdirectories
    for (const QString &dirName : sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QDir sourceSubDir(sourceDir.absoluteFilePath(dirName));
        QDir destSubDir(destDir.absoluteFilePath(dirName));
        copyAndModifyNotebooks(sourceSubDir, destSubDir);
    }

    // Copy and modify files
    for (const QString &fileName : sourceDir.entryList(QStringList("*.ipynb"), QDir::Files)) {
        QString sourceFilePath = sourceDir.absoluteFilePath(fileName);
        QString destFilePath = destDir.absoluteFilePath(fileName);

        // Check if the destination file exists
        if (!QFile::exists(destFilePath)) {
            // Copy file to destination
            if (QFile::copy(sourceFilePath, destFilePath)) {
                // Apply removeSolutionFromNotebook to the copied file
                removeSolutionFromNotebook(destFilePath);
            } else {
                qWarning() << "Failed to copy file:" << sourceFilePath;
            }
        } else {
            qDebug() << "File already exists, skipping:" << destFilePath;
        }
    }
}

void NotebookConverter::removeSolutionFromNotebook(const QString &notebookPath) {
    Q_UNUSED(notebookPath);
    qDebug() << "Modifying notebook:" << notebookPath;

    QByteArray data = readFile(notebookPath);
    if (data.isEmpty()) return;

    json notebook = parseJson(data, notebookPath);
    if (notebook.is_null()) return;

    processCells(notebook, notebookPath);

    writeFile(notebook, notebookPath);
}

QByteArray NotebookConverter::readFile(const QString &notebookPath) {
    QFile file(notebookPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open notebook:" << notebookPath;
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();
    return data;
}

json NotebookConverter::parseJson(const QByteArray &data, const QString &notebookPath) {
    json notebook;
    try {
        notebook = json::parse(data.toStdString());
    } catch (json::parse_error &e) {
        qWarning() << "JSON parse error:" << notebookPath;
        return nullptr;
    }
    return notebook;
}

void NotebookConverter::processCells(json &notebook, const QString &notebookPath) {
    int i = 1;
    for (auto &cell : notebook["cells"]) {
        if (cell["cell_type"] == "code") {
            processCell(cell, notebookPath, i);
            i++;
        }
    }
}

void NotebookConverter::processCell(json &cell, const QString &notebookPath, int i) {
    int markerCount = 0;
    for (const auto &line : cell["source"]) {
        std::string lineStr = line.get<std::string>();
        if (lineStr.find(WRITE_CODE_MARKER.toStdString()) != std::string::npos) {
            markerCount++;
        }
    }

    if (markerCount % 2 != 0) {
        qWarning() << "Unmatched marker in notebook:" << notebookPath;
        return;
    }

    std::vector<std::string> newSource;
    bool insideMarkerBlock = false;

    for (const auto &line : cell["source"]) {
        std::string lineStr = line.get<std::string>();

        if (lineStr.find(WRITE_CODE_MARKER.toStdString()) != std::string::npos) {
            if (!insideMarkerBlock) {
                // Start of marker block
                insideMarkerBlock = true;
                newSource.push_back(lineStr); // add the marker in the modified notebook
                newSource.push_back("raise NotImplementedError()");
            } else {
                // End of marker block
                insideMarkerBlock = false;
            }
            continue;
        }

        if (!insideMarkerBlock) {
            newSource.push_back(lineStr);
        }
    }

    cell["source"] = newSource;

    if (!cell["metadata"].contains("tags")) {
        cell["metadata"]["tags"] = json::array();
    }

    std::string numberedTag = TASK_CELL_TAG.toStdString() + "_" + std::to_string(i);
    cell["metadata"]["tags"].push_back(numberedTag);

    if (markerCount > 0) {
        cell["metadata"]["tags"].push_back(SOLUTION_REMOVED_CELL_TAG.toStdString());
    }
}

void NotebookConverter::writeFile(const json &notebook, const QString &notebookPath) {
    QFile file(notebookPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Failed to write notebook:" << notebookPath;
        return;
    }

    QTextStream out(&file);
    out << QString::fromStdString(notebook.dump(4));
    file.close();
}


void NotebookConverter::toggleSolution(const QString &filePath, const QString &solutionFilePath) {
    QString fullFilePath = getPackagePath() + filePath;
    QString fullSolutionFilePath = getPackagePath() + solutionFilePath;
    if (!QFile::exists(fullFilePath) || !QFile::exists(fullSolutionFilePath)) {
        qCritical() << "File not found:" << fullFilePath << "or" << fullSolutionFilePath;
        return;
    }

    QByteArray data = readFile(fullFilePath);
    if (data.isEmpty()) return;

    json notebook = parseJson(data, fullFilePath);
    if (notebook.is_null()) return;

    bool hasSolution = false;
    for (const auto &cell : notebook["cells"]) {
        if (cell["metadata"].contains("tags")) {
            const auto &tags = cell["metadata"]["tags"];
            if (std::find(tags.begin(), tags.end(), SOLUTION_CELL_TAG.toStdString()) != tags.end()) {
                hasSolution = true;
                break;
            }
        }
    }

    if (hasSolution) {
        qDebug() << "Removing solution cells from:" << fullFilePath;
        removeSolutionCells(fullFilePath);
    } else {
        qDebug() << "Adding solution cells to:" << fullFilePath;
        addSolutionCells(fullFilePath, fullSolutionFilePath);
    }
}

void NotebookConverter::addSolutionCells(const QString &notebookPath, const QString &solutionPath) {
    // Read and parse the target notebook
    QByteArray notebookData = readFile(notebookPath);
    if (notebookData.isEmpty()) return;
    json notebook = parseJson(notebookData, notebookPath);
    if (notebook.is_null()) return;

    // Read and parse the solution notebook
    QByteArray solutionData = readFile(solutionPath);
    if (solutionData.isEmpty()) return;
    json solutionNotebook = parseJson(solutionData, solutionPath);
    if (solutionNotebook.is_null()) return;

    if (!notebook.contains("cells")) return;

    // Iterate with index to allow insertion
    for (size_t i = 0; i < notebook["cells"].size(); ++i) {
        auto &cell = notebook["cells"][i];
        if (cell.contains("metadata") && cell["metadata"].contains("tags")) {
            const auto &tags = cell["metadata"]["tags"];
            // Find numberedTag
            std::string numberedTag;
            for (const auto &tag : tags) {
                if (tag.get<std::string>().find(TASK_CELL_TAG.toStdString()) != std::string::npos) {
                    numberedTag = tag.get<std::string>();
                    break;
                }
            }
            if (numberedTag.empty()) continue;

            // Check for SOLUTION_REMOVED_CELL_TAG
            bool hasSolutionRemoved = false;
            for (const auto &tag : tags) {
                if (tag.get<std::string>() == SOLUTION_REMOVED_CELL_TAG.toStdString()) {
                    hasSolutionRemoved = true;
                    break;
                }
            }
            if (!hasSolutionRemoved) continue;

            // Extract index from numberedTag using rfind
            size_t underscorePos = numberedTag.rfind('_');
            if (underscorePos == std::string::npos) continue;
            int index = 0;
            try {
                std::string indexStr = numberedTag.substr(underscorePos + 1);
                index = std::stoi(indexStr);
            } catch (...) {
                continue;
            }

            // Find the i-th code cell in the solution notebook
            int codeCellCount = 0;
            json solutionCell;
            for (const auto &solCell : solutionNotebook["cells"]) {
                if (solCell["cell_type"] == "code") {
                    if (codeCellCount == index - 1) {
                        solutionCell = solCell;
                        break;
                    }
                    codeCellCount++;
                }
            }

            if (!solutionCell.is_null()) {
                // Add SOLUTION_CELL_TAG
                if (!solutionCell["metadata"].contains("tags")) {
                    solutionCell["metadata"]["tags"] = json::array();
                }
                solutionCell["metadata"]["tags"].push_back(SOLUTION_CELL_TAG.toStdString());

                manipulateSolutionCellContent(solutionCell);

                // Insert solution cell right after the current task cell
                notebook["cells"].insert(notebook["cells"].begin() + i + 1, solutionCell);
                i++; // Skip the inserted solution cell
            }
        }
    }

    // Write the updated notebook back to file
    writeFile(notebook, notebookPath);
}

void NotebookConverter::manipulateSolutionCellContent(json &solutionCell) {
    std::vector<std::string> source = solutionCell["source"].get<std::vector<std::string>>();

    // Insert the header at the beginning of the source
    source.insert(source.begin(), SOLUTION_CELL_HEADER.toStdString() + "\n");

    // Replace WRITE_CODE_MARKER with SOLUTION_CODE_PLACEHOLDER
    bool isStart = true;
    for (auto &line : source) {
        size_t pos = line.find(WRITE_CODE_MARKER.toStdString());
        while (pos != std::string::npos) {
            if (isStart) {
                line.replace(pos, WRITE_CODE_MARKER.length(), SOLUTION_CODE_PLACEHOLDER_START.toStdString());
                isStart = false;
            } else {
                line.replace(pos, WRITE_CODE_MARKER.length(), SOLUTION_CODE_PLACEHOLDER_END.toStdString());
                isStart = true;
            }
            pos = line.find(WRITE_CODE_MARKER.toStdString(), pos + SOLUTION_CODE_PLACEHOLDER_START.length());
        }
    }

    solutionCell["source"] = source;
}

void NotebookConverter::removeSolutionCells(const QString &notebookPath) {
    QByteArray data = readFile(notebookPath);
    if (data.isEmpty()) return;

    json notebook = parseJson(data, notebookPath);
    if (notebook.is_null()) return;

    // Iterate in reverse to safely remove cells
    for (int i = notebook["cells"].size() - 1; i >= 0; --i) {
        auto &cell = notebook["cells"][i];
        if (cell["metadata"].contains("tags")) {
            const auto &tags = cell["metadata"]["tags"];
            if (std::find(tags.begin(), tags.end(), SOLUTION_CELL_TAG.toStdString()) != tags.end()) {
                notebook["cells"].erase(notebook["cells"].begin() + i);
            }
        }
    }

    writeFile(notebook, notebookPath);
}

