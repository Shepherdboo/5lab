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

//Кнопки сохранить, очистить, вернуть, создать новый файл,

void MainWindow::on_clearButton_clicked()
{
    tempFilePath = QDir::temp().filePath("tempTextFile.html");
    QFile tempFile(tempFilePath);
    if (tempFile.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&tempFile);
        // Сохраняем текст с HTML-тегами
        QString htmlContent = ui->textEdit->toHtml();
        out << htmlContent;
    }
    ui->textEdit->clear();
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


void MainWindow::on_copy_clicked()
{
    ui->textEdit->copy();
}

void MainWindow::on_pasteText_clicked()
{
    ui->textEdit->paste();
}

void MainWindow::on_undo_clicked()
{
    if (QFile::exists(tempFilePath)) {
        QFile tempFile(tempFilePath);
        if (tempFile.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&tempFile);
            ui->textEdit->setText(in.readAll());
        }
    }
}

//Диалоговое окно с поиском и заменой текста

void MainWindow::performFindReplace(const QString &findText, const QString &replaceText)
{
    QString content = ui->textEdit->toHtml();
    content.replace(findText, replaceText);
    ui->textEdit->setPlainText(content);
}

void MainWindow::on_search_clicked()
{
    Dialog dialog(ui->textEdit, this); // Передаем указатель на textEdit
    dialog.exec();
}

//Комбобоксы с размером шрифта и цветом текста

void MainWindow::on_sizeChange_currentIndexChanged(const QString& size)
{
    // Обработка изменения размера текста
    qreal newSize = size.toDouble();  // Преобразуем текст в число

    // Меняем размер шрифта выделенного текста
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextCharFormat format;
    format.setFontPointSize(newSize);
    cursor.mergeCharFormat(format);  // Применяем формат к выделенному тексту

    // Устанавливаем курсор обратно
    ui->textEdit->setTextCursor(cursor);
}


void MainWindow::on_colorChange_currentIndexChanged(const QString& color)
{
    // Создаем объект QColor на основе выбранного цвета
        QColor selectedColor;
        if (color == "Black") selectedColor = Qt::black;
        else if (color == "Red") selectedColor = Qt::red;
        else if (color == "Green") selectedColor = Qt::green;
        else if (color == "Blue") selectedColor = Qt::blue;
        else if (color == "Pink") selectedColor = Qt::magenta;  // Используем magenta для pink
        else if (color == "Orange") selectedColor = QColor(255, 165, 0); // Параметры RGB для оранжевого
        else if (color == "Yellow") selectedColor = Qt::yellow;

        // Получаем текущий текстовый курсор
        QTextCursor cursor = ui->textEdit->textCursor();

        // Меняем цвет шрифта выделенного текста
        QTextCharFormat format;
        format.setForeground(selectedColor);
        cursor.mergeCharFormat(format);  // Применяем формат к выделенному тексту

        // Устанавливаем курсор обратно
        ui->textEdit->setTextCursor(cursor);
}

//сохранение настроек перед закрытием

void MainWindow::closeEvent(QCloseEvent *event) {
    // Проверяем, есть ли изменения в тексте
        if (!ui->textEdit->toHtml().isEmpty()) { // Или используйте флаг для отслеживания изменений
            QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Confirmation"),
                tr("Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No,
                QMessageBox::Save);

            // Обработка ответа пользователя
            if (resBtn == QMessageBox::Save) {
                on_saveFile_clicked(); // Сохранить файл перед закрытием
            } else if (resBtn == QMessageBox::Cancel) {
                event->ignore(); // Отменяем закрытие
                return;
            }
        }
    saveSettings(); // Сохраняем настройки перед закрытием
    event->accept(); // Принимаем событие закрытия
}

void MainWindow::insertTable(int rows, int columns)
{
    QString htmlTable = "<table border='1' cellpadding='5' cellspacing='0'>";

    for (int r = 0; r < rows; r++) {
        htmlTable += "<tr>";
        for (int c = 0; c < columns; c++) {
            htmlTable += "<td>&nbsp;</td>"; // Пустая ячейка
        }
        htmlTable += "</tr>";
    }
    htmlTable += "</table>";

    // Вставляем HTML-таблицу в textEdit
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.insertHtml(htmlTable);
    ui->textEdit->setTextCursor(cursor); // Возвращаем курсор обратно
}

void MainWindow::on_insertTable_clicked()
{
    bool ok;
    int rows = QInputDialog::getInt(this, tr("Rows"), tr("Number of rows:"), 2, 1, 100, 1, &ok);
    if (!ok) return; // Пользователь отменил операцию

    int columns = QInputDialog::getInt(this, tr("Columns"), tr("Number of columns:"), 3, 1, 100, 1, &ok);
    if (!ok) return; // Пользователь отменил операцию

    insertTable(rows, columns);
}
