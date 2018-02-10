#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QErrorMessage>
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ButtonSelectInputFile, &QPushButton::clicked, this, &MainWindow::onSelectInputEncryptFile);
    connect(ui->ButtonSelectOutputFile, &QPushButton::clicked, this, &MainWindow::onSelectOutputEncryptFile);
    connect(ui->ButtonSelectInputFile_2, &QPushButton::clicked, this, &MainWindow::onSelectInputDecryptFile);
    connect(ui->ButtonSelectOutputFile_2, &QPushButton::clicked, this, &MainWindow::onSelectOutputDecryptFile);

    connect(ui->EditKey, &QLineEdit::textChanged, this, &MainWindow::onChangedEncryptKey);
    connect(ui->EditKey_2, &QLineEdit::textChanged, this, &MainWindow::onChangedDecryptKey);
    connect(ui->ButtonStartEncrypt, &QPushButton::clicked, this, &MainWindow::onStartEncrypt);
    connect(ui->ButtonStartDecrypt, &QPushButton::clicked, this, &MainWindow::onStartDecrypt);

    ui->ButtonStartEncrypt->setDisabled(true);
    ui->ButtonStartDecrypt->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onSelectInputEncryptFile()
{
    QString path = selectInputFile();
    ui->EditInputFilename->setText(path);
}
void MainWindow::onSelectOutputEncryptFile()
{
    QString path = selectOutputFile();
    ui->EditOutputFilename->setText(path);
}
void MainWindow::onSelectInputDecryptFile()
{
    QString path = selectInputFile();
    ui->EditInputFilename_2->setText(path);
}
void MainWindow::onSelectOutputDecryptFile()
{
    QString path = selectOutputFile();
    ui->EditOutputFilename_2->setText(path);
}

void MainWindow::onChangedEncryptKey(QString newKey)
{
    if (newKey.length() < 48)
        ui->ButtonStartEncrypt->setDisabled(true);
    else
        ui->ButtonStartEncrypt->setDisabled(false);
}
void MainWindow::onChangedDecryptKey(QString newKey)
{
    if (newKey.length() < 48)
        ui->ButtonStartDecrypt->setDisabled(true);
    else
        ui->ButtonStartDecrypt->setDisabled(false);
}

void MainWindow::onStartEncrypt()
{

    AESAlgorithm aes;
    if (!aes.setHexKey(ui->EditKey->text().toStdString(), 192))
    {
        QErrorMessage msg(this);
        msg.showMessage(QString::fromUtf8("Не удалось установить ключ!"));
        return;
    }
    if (!aes.encryptSmallFile(ui->EditInputFilename->text().toStdString(),
                         ui->EditOutputFilename->text().toStdString()))
    {
        QErrorMessage msg(this);
        msg.showMessage(QString::fromUtf8("Не удалось зашифровать файл!"));
        return;
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Успех!");
    msg.setText("Файл успешно зашифрован!");
    msg.exec();
}
void MainWindow::onStartDecrypt()
{
    AESAlgorithm aes;
    if (!aes.setHexKey(ui->EditKey->text().toStdString(), 192))
    {
        QErrorMessage msg(this);
        msg.showMessage(QString::fromUtf8("Не удалось установить ключ!"));
        return;
    }
    if (!aes.decryptSmallFile(ui->EditInputFilename_2->text().toStdString(),
                         ui->EditOutputFilename_2->text().toStdString()))
    {
        QErrorMessage msg(this);
        msg.showMessage(QString::fromUtf8("Не удалось расшифровать файл!"));
        return;
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Успех!");
    msg.setText("Файл успешно расшифрован!");
    msg.exec();
}


QString MainWindow::selectInputFile()
{
    return QFileDialog::getOpenFileName(this);
}
QString MainWindow::selectOutputFile()
{
    return QFileDialog::getSaveFileName(this);
}
