#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dialog=new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    QStringList filters;
    filters<<"All images (*.jpg *.jpeg *.png *.gif *.bmp)"
           <<"JPEG images (*.jpg *.jpeg)"
           <<"PNG images (*.png)"
           <<"GIF images (*.gif)"
           <<"Bitmaps (*.bmp)";
    dialog->setNameFilters(filters);
    dialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    connect(dialog,SIGNAL(fileSelected(QString)),this,SLOT(dialogFileSelected(QString)));
    connect(ui->browseBtn,SIGNAL(clicked(bool)),this,SLOT(browseBtnClicked()));
    connect(ui->loadBtn,SIGNAL(clicked(bool)),this,SLOT(loadBtnClicked()));
    connect(ui->saveAsBtn,SIGNAL(clicked(bool)),this,SLOT(saveAsBtnClicked()));
    connect(ui->fitToWindowBtn,SIGNAL(clicked(bool)),this,SLOT(fitToWindow()));
    connect(ui->resetZoomBtn,SIGNAL(clicked(bool)),this,SLOT(resetZoom()));
    connect(ui->resetBtn,SIGNAL(clicked(bool)),this,SLOT(resetBtnClicked()));
    connect(ui->autoClipBtn,SIGNAL(clicked(bool)),this,SLOT(autoClipBtnClicked()));
    originalImageData=0;
    image=0;
    scene=new QGraphicsScene();
    pixmapItem=new QGraphicsPixmapItem();
    scene->addItem(pixmapItem);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::browseBtnClicked()
{
    dialog->exec();
}

void MainWindow::loadBtnClicked()
{
    QString path=ui->pathBox->text();
    if(path.length()==0)
    {
        QMessageBox::critical(this,"Error","No file selected.");
        return;
    }
    QFile f(path);
    if(!f.exists())
    {
        QMessageBox::critical(this,"Error","The selected file does not exist.");
        return;
    }
    image=new QImage(path);
    if(image->isNull())
    {
        QMessageBox::critical(this,"Error","The selected file has an unsupported format.");
        return;
    }
    originalImageWidth=image->width();
    originalImageHeight=image->height();
    originalImageData=qImageToBitmapData(image);
    scene->setSceneRect(0,0,originalImageWidth,originalImageHeight);
    pixmapItem->setPixmap(QPixmap::fromImage(*image));
    ui->graphicsView->viewport()->update();
    fitToWindow();
}

void MainWindow::saveAsBtnClicked()
{
    if(originalImageData==0)
        return;
    QString path=QFileDialog::getSaveFileName(this,"Save as...",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"JPG image (*.jpg);;PNG image (*.png);;GIF image (*.gif);;Bitmap (*.bmp)");
    if(path=="")
        return;
    image->save(path,0,100);
}

void MainWindow::fitToWindow()
{
    if(image==0||image->isNull())
        return;
    int width=image->width();
    int height=image->height();
    QRect rect=ui->graphicsView->contentsRect();
    int availableWidth=rect.width()-ui->graphicsView->verticalScrollBar()->width();
    int availableHeight=rect.height()-ui->graphicsView->horizontalScrollBar()->height();
    if((width-availableWidth)>(height-availableHeight))
        ui->graphicsView->setZoomFactor((float)((float)availableWidth)/((float)width));
    else if(height>availableHeight)
        ui->graphicsView->setZoomFactor((float)((float)availableHeight)/((float)height));
    else
        ui->graphicsView->setZoomFactor(1.0);
}

void MainWindow::resetZoom()
{
    ui->graphicsView->setZoomFactor(1.0);
}

void MainWindow::dialogFileSelected(QString path)
{
    ui->pathBox->setText(path);
    ui->loadBtn->click();
}

void MainWindow::resetBtnClicked()
{
    if(image==0||image->isNull())
        return;

    delete image;
    image=new QImage((uchar*)originalImageData,originalImageWidth,originalImageHeight,QImage::Format_ARGB32);
    scene->setSceneRect(0,0,originalImageWidth,originalImageHeight);
    pixmapItem->setPixmap(QPixmap::fromImage(*image));
    ui->graphicsView->viewport()->update();
    fitToWindow();
}

void MainWindow::autoClipBtnClicked()
{
    if(image==0||image->isNull())
        return;

    QString colorBoxText=ui->colorBox->text();
    int colorBoxTextLength=colorBoxText.length();
    const bool useAlphaChannel=image->hasAlphaChannel()&&colorBoxTextLength==0;
    double neighborPixelRadius=ui->neighborPixelRadiusBox->value();
    int rNeighborPixelRadius=round(neighborPixelRadius);
    int minNumberOfNeighborPixels=ui->minimalNeighborPixelsBox->value();

    uint32_t bgColor;
    if(!useAlphaChannel)
    {
        if(colorBoxTextLength==0)
        {
            QMessageBox::critical(this,"Error","This image has no alpha channel. Please enter a background color.");
            return;
        }
        char *dup=strdup(colorBoxText.toStdString().c_str());
        bool success=getColorFromHexString(dup,bgColor);
        free(dup);
        if(!success)
        {
            QMessageBox::critical(this,"Error","This image has no alpha channel. The specified background color is invalid.");
            return;
        }
    }
    else
        bgColor=0x00000000; // Must be 0x000000000 by default

    int originalImageWidthM1=originalImageWidth-1;
    int originalImageHeightM1=originalImageHeight-1;

    uint32_t *newImageData=(uint32_t*)malloc(originalImageWidth*originalImageHeight*sizeof(uint32_t));
    for(int y=0;y<originalImageHeight;y++)
    {
        int offset=y*originalImageWidth;
        for(int x=0;x<originalImageWidth;x++)
        {
            int pos=offset+x;
            uint32_t color=originalImageData[pos];
            uint32_t alpha=getAlpha(color);
            if(alpha==0)
            {
                newImageData[pos]=color;
                continue;
            }
            // Check if pixel matches criteria
            int lowerLimitX=x-rNeighborPixelRadius;
            int lowerLimitY=y-rNeighborPixelRadius;
            if(lowerLimitX<0)
                lowerLimitX=0;
            if(lowerLimitY<0)
                lowerLimitY=0;

            int upperLimitX=x+rNeighborPixelRadius;
            int upperLimitY=y+rNeighborPixelRadius;
            if(upperLimitX>originalImageWidthM1)
                upperLimitX=originalImageWidthM1;
            if(upperLimitY>originalImageHeightM1)
                upperLimitY=originalImageHeightM1;

            int neighborPixelCount=0;

            for(int _y=lowerLimitY;_y<=upperLimitY;_y++)
            {
                int _offset=_y*originalImageWidth;
                for(int _x=lowerLimitX;_x<=upperLimitX;_x++)
                {
                    double distance=sqrt(pow((double)(x-_x),2.0)+pow((double)(y-_y),2.0));
                    if(distance>neighborPixelRadius) // This check must be performed!
                        continue;
                    if(_x==x&&_y==y)
                        continue;
                    int _pos=_offset+_x;
                    uint32_t _color=originalImageData[_pos];
                    if(useAlphaChannel)
                    {
                        uint32_t _alpha=getAlpha(_color);
                        if(_alpha>0)
                        {
                            neighborPixelCount++;
                        }
                        else
                            continue;
                    }
                    else
                    {
                        if(_color!=bgColor)
                        {
                            neighborPixelCount++;
                        }
                        else
                            continue;
                    }
                }
            }

            if(neighborPixelCount<minNumberOfNeighborPixels)
                newImageData[pos]=bgColor; // 0x000000000 by default
            else
                newImageData[pos]=color;
        }
    }

    delete image;
    image=new QImage((uchar*)newImageData,originalImageWidth,originalImageHeight,QImage::Format_ARGB32);
    scene->setSceneRect(0,0,originalImageWidth,originalImageHeight);
    pixmapItem->setPixmap(QPixmap::fromImage(*image));
    ui->graphicsView->viewport()->update();
    fitToWindow();
}

uint32_t *MainWindow::qImageToBitmapData(QImage *image)
{
    int32_t width=image->width();
    int32_t height=image->height();
    uint32_t *out=(uint32_t*)malloc(width*height*sizeof(uint32_t));
    for(int32_t y=0;y<height;y++)
    {
        int32_t offset=y*width;
        QRgb *scanLine=(QRgb*)image->scanLine(y); // Do not free!
        for(int32_t x=0;x<width;x++)
        {
            QRgb color=scanLine[x];
            uint32_t alpha=qAlpha(color);
            uint32_t red=qRed(color);
            uint32_t green=qGreen(color);
            uint32_t blue=qBlue(color);
            out[offset+x]=(alpha<<24)|(red<<16)|(green<<8)|blue;
        }
        // Do not free "scanLine"!
    }
    return out;
}

bool MainWindow::getColorFromHexString(const char *str, uint32_t &color)
{
    int length=strlen(str);
    if(length<3)
        return false;

    if(str[0]=='#')
    {
        str++;
        length--;
    }
    else if(str[0]=='0'&&(str[1]=='x'||str[1]=='X'))
    {
        str+=2;
        length-=2;
    }

    if(length==8) // 0xAARRGGBB; A=255
    {
        char nStr[9];
        nStr[0]=str[0];
        nStr[1]=str[1];
        nStr[2]=str[2];
        nStr[3]=str[3];
        nStr[4]=str[4];
        nStr[5]=str[5];
        nStr[6]=str[6];
        nStr[7]=str[7];
        nStr[8]=0;
        text_t lengthPlaceholder;
        uint8_t *bytes=(uint8_t*)text::bytesFromHexString(nStr,lengthPlaceholder);
        uint32_t a=(uint32_t)bytes[0];
        uint32_t r=(uint32_t)bytes[1];
        uint32_t g=(uint32_t)bytes[2];
        uint32_t b=(uint32_t)bytes[3];
        color=getColor(a,r,g,b);
        free(bytes);
        return true;
    }
    else if(length==6) // 0xRRGGBB; A=255
    {
        char nStr[9];
        nStr[0]='F';
        nStr[1]='F';
        nStr[2]=str[0];
        nStr[3]=str[1];
        nStr[4]=str[2];
        nStr[5]=str[3];
        nStr[6]=str[4];
        nStr[7]=str[5];
        nStr[8]=0;
        text_t lengthPlaceholder;
        uint8_t *bytes=(uint8_t*)text::bytesFromHexString(nStr,lengthPlaceholder);
        uint32_t a=bytes[0];
        uint32_t r=bytes[1];
        uint32_t g=bytes[2];
        uint32_t b=bytes[3];
        color=getColor(a,r,g,b);
        free(bytes);
        return true;
    }
    else if(length==4) // 0xARGB
    {
        char nStr[9];
        nStr[0]=str[0];
        nStr[1]=str[0];
        nStr[2]=str[1];
        nStr[3]=str[1];
        nStr[4]=str[2];
        nStr[5]=str[2];
        nStr[6]=str[3];
        nStr[7]=str[3];
        nStr[8]=0;
        text_t lengthPlaceholder;
        uint8_t *bytes=(uint8_t*)text::bytesFromHexString(nStr,lengthPlaceholder);
        uint32_t a=bytes[0];
        uint32_t r=bytes[1];
        uint32_t g=bytes[2];
        uint32_t b=bytes[3];
        color=getColor(a,r,g,b);
        free(bytes);
        return true;
    }
    else if(length==3) // 0xRGB; A=255
    {
        char nStr[9];
        nStr[0]='F';
        nStr[1]='F';
        nStr[2]=str[0];
        nStr[3]=str[0];
        nStr[4]=str[1];
        nStr[5]=str[1];
        nStr[6]=str[2];
        nStr[7]=str[2];
        nStr[8]=0;
        text_t lengthPlaceholder;
        uint8_t *bytes=(uint8_t*)text::bytesFromHexString(nStr,lengthPlaceholder);
        uint32_t a=bytes[0];
        uint32_t r=bytes[1];
        uint32_t g=bytes[2];
        uint32_t b=bytes[3];
        color=getColor(a,r,g,b);
        free(bytes);
        return true;
    }
    else // Invalid format
        return false;
}
