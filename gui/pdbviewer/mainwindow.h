#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class PDBWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    PDBWidget* pdbWidget();
   
public slots:
    void aoCheckBoxChanged(int state);
    void aoOnlyCheckBoxChanged(int state);
    void mapSizeSpinBoxChanged(int value);
    void numRaysSpinBoxChanged(int value);

    void computeAmbientOcclusion();
    void showAmbientOcclusionMaps();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
