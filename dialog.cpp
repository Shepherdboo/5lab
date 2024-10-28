#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QTextEdit *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    textEdit(editor) // Сохраняем указатель на редактор
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::getFindText() const {
    return ui->findLineEdit->text();
}

QString Dialog::getReplaceText() const {
    return ui->replaceLineEdit->text();
}

void Dialog::on_replaceButton_clicked()
{
    QString findText = getFindText();
    QString replaceText = getReplaceText();

    // Создаём курсор для работы с текстом
    QTextCursor cursor = textEdit->textCursor();

    // Перемещаем курсор в начало текста
    cursor.movePosition(QTextCursor::Start);

    // Поиск всех вхождений текста
    while (!cursor.isNull() && !cursor.atEnd()) {
        // Найти следующее вхождение
        cursor = textEdit->document()->find(findText, cursor);
        if (!cursor.isNull()) {
            // Сохраняем формат текста, который мы заменяем
            QTextCharFormat format = cursor.charFormat();

            // Удаляем найденный текст и вставляем новый с тем же форматом
            cursor.removeSelectedText();
            cursor.insertText(replaceText, format);
        }
    }

    // Закрываем диалоговое окно после завершения
    this->close();
}

void Dialog::on_closeButton_clicked()
{
    this->close();
}

void Dialog::on_findButton_clicked()
{
    QString findText = getFindText();

    if (findText.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter text to find."));
        return;
    }

    QTextCursor cursor = textEdit->textCursor();
    cursor = textEdit->document()->find(findText, cursor);

    if (!cursor.isNull()) {
        // Если текст найден, выделяем его
        textEdit->setTextCursor(cursor);
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(cursor);
        textEdit->setFocus();
    } else {
        QMessageBox::information(this, tr("Information"), tr("Text not found."));
    }
}
