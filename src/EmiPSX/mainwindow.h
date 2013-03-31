/***********************************************************
 Copyright (C) 2013 AndreBotelho(andrebotelhomail@gmail.com)
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the
 Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.
 ***********************************************************/
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QMap>
#include <QGraphicsScene>
#include <QTimer>
#include <QTime>
#include <QPixmap>
#include <QImage>
#include <QTextItem>
#include <QRect>
#include <QEvent>
#include <QFileDialog>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QDeclarativeImageProvider>
#include <QSettings>
#include <QThread>
#include "virtualkey.h"
#include "emuinterface.h"

class EmuThread;

class MainWindow : public QMainWindow, QDeclarativeImageProvider
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static uint* GetKeyState(int);
    QKeyEvent* pollEvents();
    int init();
    void setDevice();
    void unsetDevice();
    void loadVirtualKeys();
    void refresh();
    int iRomLoaded;
    bool bEmulating;
    QTime* timeCounter;
    QAudioOutput * audioOut;
    QByteArray *audioBuffer;
    QString sSaveDir;
    int iSampleCounter;
    void setMessage(char* text, QPoint p);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    EmuThread* thread;

public slots:
    void loadConfig();
    Q_INVOKABLE void exitEmu();
    Q_INVOKABLE void saveConfig();
    void updateScreen(int, int);
    void runEmuLoop();
    void resetEmu();
    void raiseMenu();
    void hideMenu();
    Q_INVOKABLE void getRomFile();
    void pauseEmu();
    void resumeEmu();
    Q_INVOKABLE void setGameState(int state);
    void setvControlTransparency();
    Q_INVOKABLE void setZoom(int zoom);
    Q_INVOKABLE void saveState();
    Q_INVOKABLE void loadState();
    Q_INVOKABLE void emuContinue();
    void loadRomFile(QString rom);
    void processAudio(QAudio::State state);
    Q_INVOKABLE void saveVirtualKeys();
    Q_INVOKABLE void chooseDir(bool save);
    Q_INVOKABLE void changeVolume(int volume);
    Q_INVOKABLE void setKeys(int, int);

signals:
    void UpdateImgs(void);
    Q_INVOKABLE void UpdateRomDir();
    Q_INVOKABLE void UpdateSaveDir();

protected:
    virtual void focusInEvent(QFocusEvent *) {}
    virtual void closeEvent(QCloseEvent *) { exitEmu(); }
    virtual void focusOutEvent(QFocusEvent *) { pauseEmu(); }
    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *) {}
    bool event(QEvent *event);
    void softEventCreator( QPoint bPoint, bool type, int press, bool end);
    enum Buttons { Up = 0, Down, Left, Right, Start, Select,
                    TR, TL, A, B, X, Y, Menu};
    QMap<Buttons, VirtualKey* > iKeys;
    QMap<Buttons, int > iKeymap;
    QTimer* loopTimer;
    int icurrRate;
    int iGameState;
    int iSkipBlit;
    int isWidth;
    int isHeight;
    QIODevice * audioDevice;
    int iZoom;
    QImage *controlImg;
    int iVCTransparency;
    int iBuffer_Size;
    int iMaxSaveStates;
    bool emuInited;
    QString sRomDir;
    bool bSmoothImage;
    int iOrientation;
    int iRefresh;
    QString sInfo;
    QPoint info;
};

class EmuThread : public QThread
{
    Q_OBJECT

public:
    explicit EmuThread(QObject* parent) : QThread(parent) { }
    ~EmuThread() { }
    void exec() {
        return;
        for (;;)
        {
            emuLoop();
        }
    }
protected:
    void run() { exec(); }
};

#endif // MAINWINDOW_H
