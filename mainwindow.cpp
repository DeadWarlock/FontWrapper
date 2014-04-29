#include "precompile.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

const std::string FONTS_DIR("C:\\Windows\\Fonts\\");
const std::string NORMAL_FACE("DejaVuSansMono.ttf");
const std::string ITALIC_FACE("DejaVuSansMono-Oblique.ttf");
const std::string BOLD_FACE("DejaVuSansMono-Bold.ttf");
const std::string BOLD_ITALIC_FACE("DejaVuSansMono-BoldOblique.ttf");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FTInitData data { FONTS_DIR + NORMAL_FACE,
                FONTS_DIR + ITALIC_FACE,
                FONTS_DIR + BOLD_FACE,
                FONTS_DIR + BOLD_ITALIC_FACE,
                10 };
    qDebug() << "=========";
    FTEngine eng1(data);
//    qDebug() << "=========";
//    FTEngine eng2(data);
//    qDebug() << "=========";
//    FTEngine eng3(std::move(eng2));
//    qDebug() << "++++++++";
    ft_engine_ = std::move(eng1);
//    qDebug() << "=========";
//    ft_engine_ = std::move(eng2);
    qDebug() << "done";

}

MainWindow::~MainWindow()
{
    delete ui;
}
