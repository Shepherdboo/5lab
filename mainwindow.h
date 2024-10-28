#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTabWidget>
#include <QTableWidget>
#include <QFile>
#include <QDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QToolButton>
#include <QToolBar>
#include <QTextBlock>
#include <QMessageBox>
#include <QShortcut>
#include <QSettings>
#include <QComboBox>
#include <QCloseEvent>
#include <QInputDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
      void saveAsFile();

      void on_newFile_clicked();

      void on_openFile_clicked();

      void on_saveFile_clicked();

      void on_sizeChange_currentIndexChanged(const QString& size);

      void on_colorChange_currentIndexChanged(const QString& color);

private:
    Ui::MainWindow *ui;
    QTextEdit *textEdit;
    QString currentFile;
    QComboBox *sizeChange;
    QComboBox *colorChange;
    void saveSettings();
    void loadSettings();
    void closeEvent(QCloseEvent *event);
    bool isTextChanged;
    };


#endif // MAINWINDOW_H