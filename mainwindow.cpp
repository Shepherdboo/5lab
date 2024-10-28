#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTextChanged(false) // Изначально не изменено
{
        ui->setupUi(this);
        connect(ui->textEdit, &QTextEdit::textChanged, this, [this]() {
        isTextChanged = true; // Устанавливаем флаг при изменении текста
        });
        loadSettings();
        new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(on_saveFile_clicked()));      // Ctrl+S для сохранения
        new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(on_openFile_clicked()));      // Ctrl+O для открытия
        new QShortcut(QKeySequence("Ctrl+F"), this, SLOT(on_search_clicked()));
}

MainWindow::~MainWindow() {
    delete ui;
}


//Сохранение настроек
void MainWindow::saveSettings() {
    QSettings settings("MyApp", "TextEditor");

        // Сохранение размера и цвета текста
        settings.setValue("fontSize", ui->sizeChange->currentText());
        settings.setValue("fontColor", ui->colorChange->currentText());
}

void MainWindow::loadSettings() {
    QSettings settings("MyApp", "TextEditor");

        // Загрузка размера и цвета текста
        QString fontSize = settings.value("fontSize", "12").toString(); // Значение по умолчанию
        QString fontColor = settings.value("fontColor", "Black").toString();

        // Установка размера шрифта
        int index = ui->sizeChange->findText(fontSize);
        if (index != -1)
            ui->sizeChange->setCurrentIndex(index);
        on_sizeChange_currentIndexChanged(fontSize); // Применить размер

        // Установка цвета текста
        index = ui->colorChange->findText(fontColor);
        if (index != -1)
            ui->colorChange->setCurrentIndex(index);
        on_colorChange_currentIndexChanged(fontColor); // Применить цвет
}

void MainWindow::saveAsFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "", tr("HTML Files (*.html);;All Files (*)"));
       if (!fileName.isEmpty()) {
           currentFile = fileName;
           on_saveFile_clicked();
       }
   }

void MainWindow::on_saveFile_clicked()
{
    if (currentFile.isEmpty()) {
        saveAsFile();
    } else {
        QFile file(currentFile);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            QString htmlContent = ui->textEdit->toHtml(); // Сохраняем как HTML
            out << htmlContent;
            saveSettings();
        }
    }
    isTextChanged = false;
}

void MainWindow::on_newFile_clicked()
{
    // Проверка на несохраненные изменения
        if (isTextChanged) {
            QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Confirmation"),
                tr("Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No,
                QMessageBox::Save);

            // Обработка ответа пользователя
            if (resBtn == QMessageBox::Save) {
                on_saveFile_clicked(); // Сохранить файл
            } else if (resBtn == QMessageBox::Cancel) {
                return; // Отменить операцию создания нового файла
            }
        }
    ui->textEdit->clear();
    currentFile.clear();
    loadSettings();
}

void MainWindow::on_openFile_clicked()
{
    if (isTextChanged) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Confirmation"),
            tr("Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No,
            QMessageBox::Save);

        if (resBtn == QMessageBox::Save) {
            on_saveFile_clicked(); // Сохранить файл
        } else if (resBtn == QMessageBox::Cancel) {
            return; // Отменить операцию открытия
        }
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("HTML Files (*.html);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            // Загружаем текст с HTML-тегами
            QString htmlContent = in.readAll();
            ui->textEdit->setHtml(htmlContent);
            currentFile = fileName;
            loadSettings();
        } else {
            QMessageBox::warning(this, tr("HTML Editor"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        }
    }
}