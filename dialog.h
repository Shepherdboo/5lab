#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QMessageBox>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QTextEdit *editor, QWidget *parent = nullptr);
    ~Dialog();

    QString getFindText() const;
    QString getReplaceText() const;

private slots:
    void on_closeButton_clicked();

    void on_replaceButton_clicked();

    void on_findButton_clicked();

private:
    Ui::Dialog *ui;
    QTextEdit *textEdit;
};

#endif // DIALOG_H
