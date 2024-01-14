//
// Created by AlexanderJustin183 on 2024/1/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include <QPixmap>
#include <QIcon>
#include <QMessageBox>
#include "images.h"
#include "mainwindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imgList = Images::normalImages;
    imgIdx = 0;

    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stop_triggered()));
    connect(ui->actionFeed, SIGNAL(triggered()), this, SLOT(feed_triggered()));
    connect(ui->actionWalk, SIGNAL(triggered()), this, SLOT(walk_triggered()));
    connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(play_triggered()));
    connect(ui->actionDoctor, SIGNAL(triggered()), this, SLOT(doctor_triggered()));

    tm1 = new QTimer(this);
    tm1->start(500);
    tm2 = new QTimer(this);
    tm2->start(5000);

    connect(tm1, SIGNAL(timeout()), this, SLOT(animation_timer()));
    connect(tm2, SIGNAL(timeout()), this, SLOT(tick_timer()));

    FILE *err = freopen("./savedata.dat", "r", stdin);

    tm timestamp_then_tm{};
    string timestamp_then_str;

    if(err != nullptr)
    {
        cin >> pet.happiness >> pet.health >> pet.hunger >> timestamp_then_str >> pet.tc;
    }
    else
    {
        cout << "new game" << endl;
        pet.happiness = 8, pet.health = 8, pet.hunger = 0, pet.tc = 0;
        time_t timestamp_now;
        time(&timestamp_now);
        tm *timestamp_now_tm = localtime(&timestamp_now);
        timestamp_then_tm = *timestamp_now_tm;
    }

    sscanf(timestamp_then_str.c_str(), "%d-%d-%d/%d:%d:%d",
           &timestamp_then_tm.tm_year,
           &timestamp_then_tm.tm_mon,
           &timestamp_then_tm.tm_mday,
           &timestamp_then_tm.tm_hour,
           &timestamp_then_tm.tm_min,
           &timestamp_then_tm.tm_sec);


    time_t timestamp_then = mktime(&timestamp_then_tm);
    time_t timestamp_now;
    time(&timestamp_now);

    time_t diff = timestamp_now - timestamp_then;

    fclose(stdin);
    cout << diff << endl;
    int ticks = (int) diff / 50;
    while(ticks--)
    {
        ++pet.tc;
        if(pet.tc == 60) pet.tc = 0;
        if(pet.tc <= 48)
        {
            pet.sleeping = false;
            if(pet.hunger < 8) ++pet.hunger;
        }
        else
        {
            pet.sleeping = true;
            if(pet.hunger < 8 && pet.tc % 3 == 0)
                ++pet.hunger;
        }
        if(pet.hunger == 7 && pet.tc % 2 == 0 && pet.health > 0)
            --pet.health;
        if(pet.hunger == 8 && pet.health > 0)
            --pet.health;
    }
    if(pet.sleeping) imgList = Images::sleepImages;
    else imgList = Images::normalImages;
}

MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::sleepTest()
{
    if(pet.sleeping)
    {
        QMessageBox::StandardButton res = QMessageBox::warning(this, "WARNING",
                                                               "Are you sure to wake your pet up? "
                                                               "He'll be unhappy about it!",
                                                               QMessageBox::Yes | QMessageBox::No,
                                                               QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            pet.sleeping = false;
            pet.happiness -= 4;
            pet.forceAwake = true;
            return true;
        }
        else return false;
    }
    else return true;
}
void MainWindow::stop_triggered()
{
    if(!pet.sleeping)
    {
        imgList = Images::normalImages;
        pet.walking = pet.eating = pet.playing = pet.doctor = false;
    }
}
void MainWindow::feed_triggered()
{
    if(sleepTest())
    {
        imgList = Images::eatImages;
        pet.eating = true, pet.playing = pet.walking = pet.doctor = false;
    }
}
void MainWindow::walk_triggered()
{
    if(sleepTest())
    {
        imgList = Images::walkImages;
        pet.walking = true, pet.playing = pet.eating = pet.doctor = false;
    }
}
void MainWindow::play_triggered()
{
    if(sleepTest())
    {
        imgList = Images::playImages;
        pet.playing = true, pet.eating = pet.walking = pet.doctor = false;
    }
}
void MainWindow::doctor_triggered()
{
    if(sleepTest())
    {
        imgList = Images::doctorImages;
        pet.doctor = true, pet.walking = pet.eating = pet.playing = false;
    }
}
void MainWindow::updateGauge()
{
    ui->progressBar_1->setProperty("value", (8 - pet.hunger) * (100 / 8.0));
    ui->progressBar_2->setProperty("value", pet.happiness * (100 / 8.0));
    ui->progressBar_3->setProperty("value", pet.health * (100 / 8.0));
}

void MainWindow::animation_timer()
{
    if(pet.sleeping && !pet.forceAwake)
        imgList = Images::sleepImages;
    ++imgIdx;
    if(imgIdx >= imgList.size())
        imgIdx = 0;
    QIcon icon;
    string curImage = imgList[imgIdx];
    QPixmap pxm;
    pxm.load(QString::fromStdString(curImage));
    icon.addPixmap(pxm, QIcon::Disabled, QIcon::Off);
    ui->petPic->setIcon(icon);
    updateGauge();

}
void MainWindow::tick_timer()
{
    ++pet.tc;
    if(pet.tc == 60) pet.tc = 0;
    if(pet.tc <= 48 || pet.forceAwake)
        pet.sleeping = false;
    else pet.sleeping = true;
    if(pet.doctor)
        ++pet.health, ++pet.hunger;
    else if(pet.walking && pet.tc % 2 == 0)
        ++pet.happiness, ++pet.health, ++pet.hunger;
    else if(pet.playing)
        ++pet.happiness, ++pet.hunger;
    else if(pet.eating)
        pet.hunger -= 2;
    else if(pet.sleeping)
    {
        if(pet.tc % 3 == 0)
            ++pet.hunger;
    }
    else
    {
        ++pet.hunger;
        if(pet.tc % 2 == 0)
            --pet.happiness;
    }
    if(pet.hunger > 8) pet.hunger = 8;
    if(pet.hunger < 0) pet.hunger = 0;
    if(pet.hunger == 7 && pet.tc % 2 == 0)
        --pet.health;
    if(pet.hunger == 8)
        --pet.health;
    if(pet.health > 8) pet.health = 8;
    if(pet.health < 0) pet.health = 0;

    if(pet.happiness > 8) pet.happiness = 8;
    if(pet.happiness < 0) pet.happiness = 0;

    updateGauge();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    freopen("./savedata.dat", "w", stdout);
    char timestamp_then_str[51];

    time_t timestamp_now;
    time(&timestamp_now);
    tm *timestamp_now_tm = localtime(&timestamp_now);

    sprintf(timestamp_then_str, "%d-%d-%d/%d:%d:%d",
            timestamp_now_tm->tm_year,
            timestamp_now_tm->tm_mon,
            timestamp_now_tm->tm_mday,
            timestamp_now_tm->tm_hour,
            timestamp_now_tm->tm_min,
            timestamp_now_tm->tm_sec);

    cout << pet.happiness << ' ' << pet.health << ' ' << pet.hunger << ' ' << timestamp_then_str << ' ' << pet.tc;
    fclose(stdout);

    event->accept();
}

