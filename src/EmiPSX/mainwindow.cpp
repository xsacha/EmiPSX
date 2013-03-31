#include "mainwindow.h"
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QIODevice>
#include <QInputDialog>
#include <QGraphicsObject>
#include <QDeclarativeView>
#include <QDebug>
#include <QApplication>
#ifdef Q_OS_SYMBIAN
#include <SoundDevice.h>
#endif

extern QByteArray *emuAudioBuffer;
extern QImage *emuFrameBuffer;

QString sRomFile;
void getChanges();
void update_audio(int len);
extern QDeclarativeView *menuView;
extern QGraphicsObject *rootObj;
int emu_frameskip = 0;
int emu_framerate = 0;
int emu_sound_enable = 1;
int emu_sound_rate = 0;
int emu_volume = 50;
int emu_state = 0;
extern int emu_psx_bias;
extern int emu_audio_xa; /* 0=XA enabled, 1=XA disabled */
extern int emu_video_mdec; /* 0=Black&White Mdecs Only Disabled, 1=Black&White Mdecs Only Enabled */
extern int emu_audio_cd; /* 0=Enable Cd audio, 1=Disable Cd audio */
extern int emu_enable_bios; /* 0=BIOS, 1=HLE */
extern int emu_cpu_type;
extern int emu_skip_gpu; /* skip GPU primitives */
extern int emu_frame_limit; /* frames to wait */
extern int emu_video_light; /* lighting */
extern int emu_video_blend; /* blending */
int emu_clock;
extern float emu_clock_multiplier;
extern int emu_screen_width, emu_screen_height;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
    timeCounter = new QTime();
    loopTimer = new QTimer();
    loopTimer->start(0);
    timeCounter->start();
    icurrRate = 0;
    iRomLoaded = 0;
    iGameState = 0;
    iSkipBlit = 0;
    iZoom = 0;
    audioOut = NULL;
    bEmulating = false;
    iVCTransparency = 5;
    iBuffer_Size = 512;
    audioBuffer = new QByteArray(4096*16,0);
    connect(loopTimer, SIGNAL(timeout()), this,  SLOT(runEmuLoop()));
    thread = new EmuThread(this);
    audioDevice = NULL;
    emuInited = false;
    bSmoothImage = false;
    sRomDir = "E:/PSX";
    sSaveDir = sRomDir;
    iRefresh = 0;
    setAttribute(Qt::WA_AcceptTouchEvents);
}

MainWindow::~MainWindow()
{
    exitEmu();
}

int MainWindow::init()
{
    menuView->hide();
    loadVirtualKeys();
    return 0;
}

void MainWindow::exitEmu()
{
    saveConfig();
    if (emuInited)
        emuClose();
    QApplication::quit();
}
void MainWindow::chooseDir(bool save)
{
    QString temp = QFileDialog::getExistingDirectory(this, tr("Select %1 Directory").arg(save ? "Save" : "ROM"),"e:/",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!temp.isEmpty()) {
        if (save)
        {
            sSaveDir = temp;
            rootObj->setProperty("savedir", sSaveDir);
        } else
        {
            sRomDir = temp;
            rootObj->setProperty("romdir", sRomDir);
        }
    }
}

void MainWindow::loadVirtualKeys()
{
    QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
    iKeys[Up]= new VirtualKey(config.value("buttons/Up", QRect(5, 170, 180, 60)).toRect(), Qt::Key_Up);
    iKeys[Up]->img = QImage(":/app/data/setaUP.png").scaled(iKeys[Up]->rect.width()/3, iKeys[Up]->rect.height());

    iKeys[Left]= new VirtualKey(config.value("buttons/Left", QRect(5, 170, 60, 180)).toRect(), Qt::Key_Left);
    iKeys[Left]->img = QImage(":/app/data/setaLEFT.png").scaled(iKeys[Left]->rect.width(), iKeys[Left]->rect.height()/3);

    iKeys[Right]= new VirtualKey(config.value("buttons/Right", QRect(125, 170, 60, 180)).toRect(), Qt::Key_Right);
    iKeys[Right]->img = QImage(":/app/data/setaRIGHT.png").scaled(iKeys[Right]->rect.width(), iKeys[Right]->rect.height()/3);

    iKeys[Down]= new VirtualKey(config.value("buttons/Down", QRect(5, 290, 180, 60)).toRect(), Qt::Key_Down);
    iKeys[Down]->img = QImage(":/app/data/setaDOWN.png").scaled(iKeys[Down]->rect.width()/3, iKeys[Down]->rect.height());

    iKeys[A]= new VirtualKey(config.value("buttons/C", QRect(570, 240, 60, 60)).toRect(), Qt::Key_A);
    iKeys[A]->img = QImage(":/app/data/buttonC.png").scaled(iKeys[Down]->rect.width()/3, iKeys[Down]->rect.height());

    iKeys[B]= new VirtualKey(config.value("buttons/X", QRect(510, 290, 60, 60)).toRect(), Qt::Key_B);
    iKeys[B]->img = QImage(":/app/data/buttonX.png").scaled(iKeys[Down]->rect.width()/3, iKeys[Down]->rect.height());

    iKeys[X]= new VirtualKey(config.value("buttons/T", QRect(510, 190, 60, 60)).toRect(), Qt::Key_X);
    iKeys[X]->img = QImage(":/app/data/buttonT.png").scaled(iKeys[Down]->rect.width()/3, iKeys[Down]->rect.height());

    iKeys[Y]= new VirtualKey(config.value("buttons/S", QRect(450, 240, 60, 60)).toRect(), Qt::Key_Y);
    iKeys[Y]->img = QImage(":/app/data/buttonS.png").scaled(iKeys[Down]->rect.width()/3, iKeys[Down]->rect.height());

    iKeys[Start]= new VirtualKey(config.value("buttons/Start", QRect(320, 300, 70, 30)).toRect(), Qt::Key_Return);
    iKeys[Start]->img = QImage(":/app/data/stbutton.png").scaled(iKeys[Start]->rect.width(), iKeys[Start]->rect.height());

    iKeys[Select]= new VirtualKey(config.value("buttons/Select", QRect(230, 300, 70, 30)).toRect(), Qt::Key_Shift);
    iKeys[Select]->img = QImage(":/app/data/slbutton.png").scaled(iKeys[Select]->rect.width(), iKeys[Select]->rect.height());

    iKeys[TL]= new VirtualKey(config.value("buttons/TL", QRect(10, 10, 70, 30)).toRect(), Qt::Key_L);
    iKeys[TL]->img = QImage(":/app/data/lbutton.png").scaled(iKeys[TL]->rect.width(), iKeys[TL]->rect.height());

    iKeys[TR]= new VirtualKey(config.value("buttons/TR", QRect(550, 10, 70, 30)).toRect(), Qt::Key_R);
    iKeys[TR]->img = QImage(":/app/data/rbutton.png").scaled(iKeys[TR]->rect.width(), iKeys[TR]->rect.height());

    iKeys[Menu]= new VirtualKey(config.value("buttons/Menu", QRect(280, 10, 80, 40)).toRect(), Qt::Key_Escape);
    iKeys[Menu]->img = QImage(":/app/data/mbutton.png").scaled(iKeys[Menu]->rect.width(), iKeys[Menu]->rect.height());

    /****************LOAD KEYMAP**************************/
    iKeys[Up]->key = (Qt::Key)config.value("keymap/UP",Qt::Key_Up).toInt();
    iKeys[Down]->key = (Qt::Key)config.value("keymap/DOWN",Qt::Key_Down).toInt();
    iKeys[Left]->key = (Qt::Key)config.value("keymap/LEFT",Qt::Key_Left).toInt();
    iKeys[Right]->key = (Qt::Key)config.value("keymap/RIGHT",Qt::Key_Right).toInt();
    iKeys[A]->key = (Qt::Key)config.value("keymap/A",Qt::Key_A).toInt();
    iKeys[B]->key = (Qt::Key)config.value("keymap/B",Qt::Key_B).toInt();
    iKeys[X]->key = (Qt::Key)config.value("keymap/X",Qt::Key_X).toInt();
    iKeys[Y]->key = (Qt::Key)config.value("keymap/Y",Qt::Key_Y).toInt();
    iKeys[TL]->key = (Qt::Key)config.value("keymap/L",Qt::Key_L).toInt();
    iKeys[TR]->key = (Qt::Key)config.value("keymap/R",Qt::Key_R).toInt();
    iKeys[Start]->key = (Qt::Key)config.value("keymap/START",Qt::Key_Return).toInt();
    iKeys[Select]->key = (Qt::Key)config.value("keymap/SELECT",Qt::Key_Shift).toInt();
    setvControlTransparency();
}

void MainWindow::saveVirtualKeys()
{
    QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
#define grab_(item, value) rootObj->findChild<QObject*>(item)->property(value).toInt()
#define grab_rect(item) grab_(item, "x"), grab_(item, "y"), grab_(item, "width"), grab_(item, "height")
#define grab_rect_dpad(item) grab_(item, "x") + grab_("dpadbtn","x"), grab_(item, "y") + grab_("dpadbtn","y"), grab_(item, "width"), grab_(item, "height")
    config.setValue("buttons/Up", QRect(grab_rect_dpad("upbtn")));
    config.setValue("buttons/Down", QRect(grab_rect_dpad("downbtn")));
    config.setValue("buttons/Left", QRect(grab_rect_dpad("leftbtn")));
    config.setValue("buttons/Right", QRect(grab_rect_dpad("rightbtn")));
    config.setValue("buttons/C", QRect(grab_rect("abtn")));
    config.setValue("buttons/X", QRect(grab_rect("bbtn")));
    config.setValue("buttons/T", QRect(grab_rect("xbtn")));
    config.setValue("buttons/S", QRect(grab_rect("ybtn")));
    config.setValue("buttons/TL", QRect(grab_rect("lbtn")));
    config.setValue("buttons/TR", QRect(grab_rect("rbtn")));
    config.setValue("buttons/Start", QRect(grab_rect("stbtn")));
    config.setValue("buttons/Select", QRect(grab_rect("slbtn")));
    config.setValue("buttons/Menu", QRect(grab_rect("menubtn")));
    config.sync();
    loadVirtualKeys();
    if( iRomLoaded){
        hideMenu();
    }
}

void MainWindow::loadConfig()
{
    QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
    emu_frameskip = config.value("video/frameSkip", 0).toInt();
    emu_psx_bias = config.value("misc/bias", 2).toInt();;
    emu_audio_xa = config.value("audio/xa", 0).toInt();
    emu_video_mdec = config.value("video/mdec", 0).toInt();
    emu_audio_cd = config.value("audio/cdda", 0).toInt();
    emu_enable_bios = config.value("misc/bios", 1).toInt();
    emu_cpu_type = config.value("misc/cpu", 1).toInt();;
    emu_skip_gpu = config.value("video/gpuskip", 0).toInt();
    emu_frame_limit = config.value("video/limitframes", 1).toInt();
    emu_video_light = config.value("video/light", 1).toInt();
    emu_video_blend = config.value("video/blend", 1).toInt();
    emu_clock = config.value("misc/clock", 2).toInt();
    emu_clock_multiplier = 1.0f / (float)(emu_clock + 1);
    emu_framerate = config.value("video/framerate", 0).toInt();
    emu_sound_enable = config.value("audio/enable", 1).toInt();
    emu_sound_rate = config.value("audio/rate", 0).toInt();
    iVCTransparency = config.value("misc/vctransparency", 5).toInt();
    emu_volume = config.value("audio/volume", 50).toInt();
    iSkipBlit = config.value("video/blitSkip", 0).toInt();
    iZoom = config.value("video/zoom", 0).toInt();
    bSmoothImage = config.value("video/smooth", false).toBool();
    sRomFile = config.value("misc/lastRom").toString();
    sRomDir = config.value("misc/romdir", "E:/PSX").toString();
    sSaveDir = config.value("misc/savedir", "E:/PSX").toString();
    iBuffer_Size = 512 << emu_sound_rate;
#define set_checked(item, x)  rootObj->findChild<QObject*>(item)->setProperty("checked", x);
#define set_selected(item, x) rootObj->findChild<QObject*>(item)->setProperty("selectedIndex", x);
#define set_value(item, x)    rootObj->findChild<QObject*>(item)->setProperty("value", x);
    set_checked("senaBox", emu_sound_enable);
    set_checked("lightBox", emu_video_light);
    set_checked("blendBox", emu_video_blend);
    set_checked("sgpuBox", emu_skip_gpu);
    set_checked("biosBox", emu_enable_bios);
    set_checked("mdecBox", emu_video_mdec);
    set_checked("frameBox", emu_framerate);
    set_checked("xaBox", emu_audio_xa);
    set_checked("cdaBox", emu_audio_cd);
    set_checked("flimBox", emu_frame_limit);
    set_checked("smoothBox", bSmoothImage);
    set_checked("sblitBox", iSkipBlit);
    set_selected("biasDial", emu_psx_bias);
    set_selected("fskipdial", emu_frameskip);
    set_selected("sratedial", emu_sound_rate);
    set_selected("isizedial", iZoom);
    set_selected("clockDial", emu_clock);
    set_value("volumeSlider", emu_volume);
    set_value("vctranslider", iVCTransparency);
    rootObj->setProperty("romdir", sRomDir);
    rootObj->setProperty("savedir", sSaveDir);
    emuChangeConfig();
    setvControlTransparency();
    changeVolume(emu_volume);
    if( !config.contains("video/frameSkip")) saveConfig();
    raiseMenu();
}

void MainWindow::saveConfig()
{
    QObject *tmp = rootObj->findChild<QObject*>("senaBox");
        if (tmp) emu_sound_enable = tmp->property("checked").toBool();
        tmp = rootObj->findChild<QObject*>("lightBox");
        if (tmp) emu_video_light = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("blendBox");
        if (tmp) emu_video_blend = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("sgpuBox");
        if (tmp) emu_skip_gpu = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("biosBox");
        if (tmp) emu_enable_bios = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("mdecBox");
        if (tmp) emu_video_mdec = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("frameBox");
        if (tmp) emu_framerate = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("xaBox");
        if (tmp) emu_audio_xa = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("cdaBox");
        if (tmp) emu_audio_cd = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("flimBox");
        if (tmp) emu_frame_limit = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("frameBox");
        if (tmp) emu_framerate = tmp->property("checked").toInt();
        tmp = rootObj->findChild<QObject*>("sblitBox");
        if (tmp) iSkipBlit = tmp->property("checked").toBool();
        tmp = rootObj->findChild<QObject*>("smoothBox");
        if (tmp) bSmoothImage = tmp->property("checked").toBool();
        tmp = rootObj->findChild<QObject*>("vctranslider");
        if (tmp) iVCTransparency = tmp->property("value").toInt();
        tmp = rootObj->findChild<QObject*>("cpuDial");
        if (tmp) emu_cpu_type = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("biasDial");
        if (tmp) emu_psx_bias = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("fskipdial");
        if (tmp) emu_frameskip = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("clockDial");
        if (tmp) emu_clock = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("sratedial");
        if (tmp) emu_sound_rate = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("isizedial");
        if (tmp) iZoom = tmp->property("selectedIndex").toInt();
        tmp = rootObj->findChild<QObject*>("volumeSlider");
        if (tmp) emu_volume = tmp->property("value").toInt();
    iBuffer_Size = 512 << emu_sound_rate;
    QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
    config.setValue("video/frameSkip", emu_frameskip);
    config.setValue("video/framerate", emu_framerate);
    config.setValue("audio/enable", emu_sound_enable);
    config.setValue("audio/rate", emu_sound_rate);
    config.setValue("audio/volume", emu_volume);
    config.setValue("misc/bias",emu_psx_bias);
    config.setValue("audio/xa",emu_audio_xa);
    config.setValue("video/mdec",emu_video_mdec);
    config.setValue("audio/cdda",emu_audio_cd);
    config.setValue("misc/bios",emu_enable_bios);
    config.setValue("misc/cpu",emu_cpu_type);
    config.setValue("video/gpuskip",emu_skip_gpu);
    config.setValue("video/limitframes",emu_frame_limit);
    config.setValue("video/light",emu_video_light);
    config.setValue("video/blend",emu_video_blend);
    config.setValue("misc/clock",emu_clock);
    config.setValue("video/blitSkip", iSkipBlit);
    config.setValue("video/smooth", bSmoothImage);
    config.setValue("video/zoom", iZoom);
    config.setValue("misc/romdir", sRomDir);
    config.setValue("misc/savedir", sSaveDir);
    if( iRomLoaded )
        config.setValue("misc/lastRom", sRomFile);
    config.sync();
    loadConfig();
    if( iRomLoaded){
        hideMenu();
    }
}

QPixmap MainWindow::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 180;
    int height = 180;
    if (size)
        *size = QSize(width, height);

    QString path;
    if(!sRomFile.isEmpty())
    {
        QFileInfo fi(sRomFile);
        path = QString("%1/%2.%3.jpg").arg(sSaveDir).arg(fi.fileName()).arg(emu_state+1);
    }
    QPixmap pix(*size);
    if(path.isEmpty() || !pix.load(path)){
        pix.fill();
        QPainter painter(&pix);
        QFont f = painter.font();
        f.setPixelSize(20);
        painter.setFont(f);
        painter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, "No Save Image");
        painter.end();
    }
    return pix;
}

void MainWindow::resumeEmu(){
    if(audioOut) audioOut->resume();
    if(iRomLoaded) bEmulating = true;
#if 1
    if (iRomLoaded) thread->start();
#endif
    if(loopTimer) loopTimer->start(0);
}

void MainWindow::setvControlTransparency(){
    QColor color;
    for (QMap<Buttons, VirtualKey*>::iterator i = iKeys.begin(); i != iKeys.end(); ++i){
        for ( int x = 0; x < i.value()->img.width(); x++ )	{
            for ( int y = 0; y < i.value()->img.height(); y++ ) {
                color.setRgba(i.value()->img.pixel( x, y ));
                if(color.alpha()!=0) color.setAlpha((iVCTransparency + 1) * 25);
                i.value()->img.setPixel( x, y, color.rgba() );
            }
        }
        i.value()->pix = QPixmap::fromImage(i.value()->img);
    }
}

void MainWindow::emuContinue(){
    if (!iRomLoaded){
        loadRomFile(sRomFile);
        loadConfig();
    }
    if( iRomLoaded){
        hideMenu();
    }
}

void MainWindow::pauseEmu(){
    bEmulating = false;
#if 1
    thread->quit();
#endif
    if( loopTimer ) loopTimer->stop();
    if( audioOut ) audioOut->suspend();
}

void MainWindow::setGameState(int state){
    emu_state = iGameState = state;
    rootObj->findChild<QObject*>("savePathView")->setProperty("currentIndex", iGameState);
}

void MainWindow::setZoom(int zoom){
    iZoom = zoom;
}

void MainWindow::raiseMenu(){
    pauseEmu();
    menuView->showFullScreen();
}

void MainWindow::hideMenu(){
    menuView->hide();
    resumeEmu();
}

void MainWindow::loadState(){
    if(iRomLoaded){
        emu_state = iGameState;
        emuLoadState(sSaveDir);
        hideMenu();
    }
}

void MainWindow::saveState(){
    if(iRomLoaded){
        emu_state = iGameState;
        emuSaveState(sSaveDir);
        if(iRomLoaded){
            QFileInfo fi(sRomFile);
            QString path = QString("%1/%2.%3.jpg").arg(sSaveDir).arg(fi.fileName()).arg(iGameState);
            QImage out = emuFrameBuffer->copy(0, 0, isWidth, isHeight);
            out.save(path);
        }
        hideMenu();
    }
}

void MainWindow::setDevice()
{
    connect(audioOut, SIGNAL(stateChanged (QAudio::State)), this,  SLOT(processAudio(QAudio::State)));
    if(audioOut) audioDevice = audioOut->start();
    unsigned int *pointer_to_abstract_audio = (unsigned int*)( (unsigned char*)audioOut + 8 );
    unsigned int *dev_sound_wrapper = (unsigned int*)(*pointer_to_abstract_audio) + 13;
    unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);
    CMMFDevSound *dev_sound = (CMMFDevSound*)(*temp);
    int volume = (dev_sound->MaxVolume()/100)*emu_volume;
    dev_sound->SetVolume(volume);
}

void MainWindow::changeVolume(int i)
{
    if( audioOut ){
        unsigned int *pointer_to_abstract_audio = (unsigned int*)( (unsigned char*)audioOut + 8 );
        unsigned int *dev_sound_wrapper = (unsigned int*)(*pointer_to_abstract_audio) + 13;
        unsigned int *temp = ((unsigned int*)(*dev_sound_wrapper) + 6);
        CMMFDevSound *dev_sound = (CMMFDevSound*)(*temp);
        int volume = (dev_sound->MaxVolume()/100)*i;
        dev_sound->SetVolume(volume);
    }
    return;
}

void MainWindow::unsetDevice()
{
    if(audioOut)audioOut->stop();
}
void MainWindow::runEmuLoop()
{
    if( bEmulating && iRomLoaded){
        if( iSampleCounter >= iBuffer_Size ) processAudio(QAudio::ActiveState);
#if 1
        while( !iRefresh ){
            emuLoop();
        }
#else
        if (!thread->isRunning())
            thread->start();
#endif
        iRefresh = 0;
    }
}

void MainWindow::refresh()
{
    iRefresh = 1;
}
void MainWindow::resetEmu()
{
    if( !iRomLoaded ) return;
    emuReset();
    hideMenu();
}

void MainWindow::setKeys(int w, int codeint)
{
    Qt::Key code = (Qt::Key)codeint;
    switch(w){
    case 0: iKeys[Up]->key = code; break;
    case 1: iKeys[Down]->key = code; break;
    case 2: iKeys[Left]->key = code; break;
    case 3: iKeys[Right]->key = code; break;
    case 4: iKeys[A]->key = code; break;
    case 5: iKeys[B]->key = code; break;
    case 6: iKeys[X]->key = code; break;
    case 7: iKeys[Y]->key = code; break;
    case 8: iKeys[TL]->key = code; break;
    case 9: iKeys[TR]->key = code; break;
    case 10: iKeys[Start]->key = code; break;
    case 11: {
        iKeys[Select]->key = code;
        QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
        config.setValue("keymap/UP", iKeys[Up]->key);
        config.setValue("keymap/DOWN", iKeys[Down]->key);
        config.setValue("keymap/LEFT", iKeys[Left]->key);
        config.setValue("keymap/RIGHT", iKeys[Right]->key);
        config.setValue("keymap/A", iKeys[A]->key);
        config.setValue("keymap/B", iKeys[B]->key);
        config.setValue("keymap/X", iKeys[X]->key);
        config.setValue("keymap/Y", iKeys[Y]->key);
        config.setValue("keymap/L", iKeys[TL]->key);
        config.setValue("keymap/R", iKeys[TR]->key);
        config.setValue("keymap/SELECT", iKeys[Select]->key);
        config.setValue("keymap/START", iKeys[Start]->key);
        config.sync();
        hideMenu();
        break;
    }
    default: break;
    }

}

void MainWindow::processAudio( QAudio::State state )
{
    if (bEmulating && iRomLoaded && emu_sound_enable){
        if(audioDevice && emuAudioBuffer){
            audioDevice->write(*emuAudioBuffer , iSampleCounter);
        }
    }
    iSampleCounter = 0;
}

void MainWindow::getRomFile(){
    pauseEmu();
    loadConfig();
    loadRomFile(QFileDialog::getOpenFileName(this, tr("Open Rom"), sRomDir, tr("Rom Files (*.smc *.sfc *.zip *.img *.cue *.bin)")));
    if( iRomLoaded ){
        QSettings config("E:/PSX/config.ini", QSettings::IniFormat);
        config.setValue("misc/lastRom", sRomFile);
        config.sync();
        hideMenu();
    } else
        raiseMenu();
}

void MainWindow::loadRomFile( QString romfile){
    if(romfile.isEmpty()) return;
    if(iRomLoaded && romfile == sRomFile) return;
    int current_state = emu_state;
    emu_state = 6;
    if(emuInited)
        emuClose();
    emuInit();
    emuInited = true;
    if(iRomLoaded){
        emuSaveState(sSaveDir);
    }
    sRomFile = romfile;
    if(emuLoadRom(&sRomFile))
        iRomLoaded = 1;
    emu_state = current_state;
}

void MainWindow::softEventCreator( QPoint bPoint, bool type, int press, bool end){
    QMap<Buttons, VirtualKey*>::iterator i;
    for (i = iKeys.begin(); i != iKeys.end(); ++i){
        if(i.value()->isOver(bPoint) ){
            if(i.value()->iPressed == 0 || !type){
                QApplication::sendEvent(this, new QKeyEvent(type ? QEvent::KeyPress : QEvent::KeyRelease, i.value()->key, Qt::NoModifier));
                i.value()->iPressed = type;
            } else
                i.value()->iPressed = 1;
        } else if(i.value()->iPressed == 1 && press == 0){
            if (end)
            {
                QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, i.value()->key, Qt::NoModifier));
                i.value()->iPressed = 0;
            } else
                i.value()->iPressed = -1;
        } else if(i.value()->iPressed == -1 && end){
            QApplication::sendEvent(this, new QKeyEvent(QEvent::KeyRelease, i.value()->key, Qt::NoModifier));
            i.value()->iPressed = 0;
        }
    }
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    {
        QList<QTouchEvent::TouchPoint> tp = (static_cast<QTouchEvent*>(event))->touchPoints();
        for ( int i = 0; i < tp.length(); i++ )
        {
            switch(tp[i].state())
            {
            case Qt::TouchPointPressed:
            case Qt::TouchPointMoved:
            case Qt::TouchPointStationary:
            case Qt::TouchPointReleased:
                softEventCreator(tp[i].screenPos().toPoint(), (tp[i].state() != Qt::TouchPointReleased), i, i == (tp.length() - 1));
                break;
            }
        }
        event->accept();
        return true;
    }
    case QEvent::TouchEnd:
    {
        QList<QTouchEvent::TouchPoint> tp = (static_cast<QTouchEvent*>(event))->touchPoints();
        for ( int i = 0; i < tp.length(); i++ )
            softEventCreator(tp[i].screenPos().toPoint(), false, i, i == (tp.length() - 1));
        break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        softEventCreator(static_cast<QMouseEvent*>(event)->pos(), static_cast<QMouseEvent*>(event)->type() != QEvent::MouseButtonRelease, 0, true);
        break;
    default:
        break;
    }
    return QWidget::event(event);
}


void MainWindow::keyPressEvent( QKeyEvent *event){
    if(event->key() == Qt::Key_Escape) raiseMenu();
    emuProcessKeys( event->key(), true);
}

void MainWindow::keyReleaseEvent( QKeyEvent *event){
    QMap<Buttons, int >::iterator i;
    emuProcessKeys( event->key(), false);
}

void MainWindow::setMessage(char* message, QPoint p){
    sInfo = message;
    info = p;
}

int framecount = 0;
int framerate = 0;
void MainWindow::updateScreen(int width, int height){
    static int oldtime = timeCounter->elapsed();
    if( timeCounter->elapsed() >= oldtime + 1000 ){
        oldtime = timeCounter->elapsed();
        framerate = framecount;
        framecount = 0;
    }
    isWidth = width;
    isHeight = height;
    static int fcounter = 0;
    if( fcounter && iSkipBlit){
        fcounter = 0;
        framecount++;
        return;
    }
    fcounter++;
    repaint();
}

void MainWindow::paintEvent( QPaintEvent * event){
    QPainter painter;
    QRectF target, source;
    painter.begin(this);
    if( bSmoothImage )painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if( iRomLoaded && bEmulating ){
        source = QRectF(0.0, 0.0, isWidth, isHeight);
        if(iZoom){
            target = QRectF(0.0, 0.0, width(), height());
        }else{
            target = QRectF(80.0, 0.0, 480, 360);
        }
        painter.drawImage(target, *emuFrameBuffer, source);
        framecount++;
        if( !sInfo.isNull()) painter.drawText(info, sInfo);
        if( emu_framerate){
            painter.drawText(10, 50, QString("FPS: %1").arg(framerate));
        }
    }
    if( iVCTransparency ){
        for (QMap<Buttons, VirtualKey*>::iterator i = iKeys.begin(); i != iKeys.end(); ++i)
        {
            if (i.value()->iPressed)
            {
                painter.save();
                painter.scale(1.3, 1.3);
                painter.drawPixmap((i.value()->rect.x())/1.3f-(i.value()->pix.width()/2)+(i.value()->rect.width()/2), (i.value()->rect.y())/1.3f-(i.value()->pix.height()/2)+(i.value()->rect.height()/2), i.value()->pix);
                painter.restore();
            }
            else
                painter.drawPixmap(i.value()->rect.x()-(i.value()->pix.width()/2)+(i.value()->rect.width()/2), i.value()->rect.y()-(i.value()->pix.height()/2)+(i.value()->rect.height()/2), i.value()->pix);

        }
    }
    painter.end();
}
