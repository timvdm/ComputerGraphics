#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "pdbwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->aoCheckBox, SIGNAL(stateChanged(int)), this, SLOT(aoCheckBoxChanged(int)));
    connect(ui->aoOnlyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(aoOnlyCheckBoxChanged(int)));
    connect(ui->mapSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(mapSizeSpinBoxChanged(int)));
    connect(ui->numRaysSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numRaysSpinBoxChanged(int)));
    connect(ui->calculatePushButton, SIGNAL(clicked()), this, SLOT(computeAmbientOcclusion()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

PDBWidget* MainWindow::pdbWidget()
{
  return ui->pdb;
}

void MainWindow::aoCheckBoxChanged(int state)
{
  if (state == Qt::Checked) {
    QMessageBox::warning(this, "Computing Ambient Occlusion Maps",
        "Computing ambient occlusion maps may take a few seconds. The application will not respond to user input during this period.");
    ui->pdb->computeAmbientOcclusion();
  }
  ui->pdb->enableAO(state == Qt::Checked);
  ui->pdb->update();
  //showAmbientOcclusionMaps();
}

void MainWindow::aoOnlyCheckBoxChanged(int state)
{
  ui->pdb->setAOOnly(state == Qt::Checked);
  ui->pdb->update();
}

void MainWindow::mapSizeSpinBoxChanged(int value)
{
  ui->pdb->setAOMapSize(value);
  ui->pdb->update();
}

void MainWindow::numRaysSpinBoxChanged(int value)
{
  ui->pdb->setNumRays(value);
  ui->pdb->update();
}

void MainWindow::computeAmbientOcclusion()
{
  QMessageBox::warning(this, "Computing Ambient Occlusion Maps",
      "Computing ambient occlusion maps may take a few seconds. The application will not respond to user input during this period.");
  ui->pdb->computeAmbientOcclusion();
  ui->pdb->update();
}
    
void MainWindow::showAmbientOcclusionMaps()
{
  const GFX::Buffer<GFX::ColorF> &aoMap = ui->pdb->aoMap();
  int size = aoMap.height();
  int numAtoms = aoMap.width() / size;

  int w = (1000 / size);
  int h = numAtoms / w + 1;



  QImage image(w * size, h * size, QImage::Format_RGB32);


  for (int a = 0; a < numAtoms; ++a) {
    int b = a / w;
    for (int j = 0; j < size; ++j)
      for (int i = (a % w) * size; i < ((a + 1) % w) * size; ++i) {
        const GFX::Color &color = aoMap(i + a * size, j);
        image.setPixel(i, j + b * size, color.toARGB());
      }
  }

  QLabel *lbl = new QLabel;
  lbl->setAttribute(Qt::WA_DeleteOnClose);
  lbl->setPixmap(QPixmap::fromImage(image));
  lbl->show();
}

