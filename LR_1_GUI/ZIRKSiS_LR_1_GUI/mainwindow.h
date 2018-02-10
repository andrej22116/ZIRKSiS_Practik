#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "aesalgorithm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onSelectInputEncryptFile();
    void onSelectOutputEncryptFile();
    void onSelectInputDecryptFile();
    void onSelectOutputDecryptFile();

    void onChangedEncryptKey(QString newKey);
    void onChangedDecryptKey(QString newKey);

    void onStartEncrypt();
    void onStartDecrypt();

private:
    Ui::MainWindow *ui;
    AESAlgorithm aes;

    QString selectInputFile();
    QString selectOutputFile();
};

#endif // MAINWINDOW_H
