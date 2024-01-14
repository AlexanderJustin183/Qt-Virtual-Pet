//
// Created by AlexanderJustin183 on 2024/1/4.
//

#ifndef VIRTUALPET_MAINWINDOW_H
#define VIRTUALPET_MAINWINDOW_H

#include "includes.h"
#include <QMainWindow>
#include <QTimer>
#include "Pet.h"
#include <QCloseEvent>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    Pet pet;
    vector<string> imgList;
    int imgIdx;
    QTimer *tm1, *tm2;

protected Q_SLOTS:
    void stop_triggered();
    void feed_triggered();
    void walk_triggered();
    void play_triggered();
    void doctor_triggered();
    void animation_timer();
    void tick_timer();

protected:
    bool sleepTest();
    void updateGauge();
    void closeEvent(QCloseEvent *event) override;
};


#endif //VIRTUALPET_MAINWINDOW_H
