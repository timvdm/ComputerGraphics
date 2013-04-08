#include <QtGui>

#include "pdbwidget.h"

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <pdb_file>" << std::endl;
    return -1;
  }

  QApplication app(argc, argv);

  PDBWidget widget(argv[1], 500, 500);
  widget.show();

  return app.exec();
}
