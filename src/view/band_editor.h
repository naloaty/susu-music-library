#ifndef BANDEDITOR_H
#define BANDEDITOR_H

#include <QDialog>
#include <QItemSelection>
#include <QMessageBox>

#include "src/database.h"
#include "src/model/simplecolumn.h"

#undef TAG
#define TAG "[BandEditor]"

namespace Ui {
class BandEditor;
}

class BandEditor : public QDialog
{
    Q_OBJECT

public:
    explicit BandEditor(Database *db, QWidget *parent = nullptr);
    ~BandEditor();

public slots:
    void updateBands();
    void handleBandSelection(const QItemSelection &selection);

private slots:
    void on_btn_deleteBand_clicked();
    void on_btn_newBand_clicked();
    void on_btn_applyBandChanges_clicked();

private:
    Ui::BandEditor *ui;

    Database *m_database;
    VariantMapTableModel *m_bands;
};

#endif // BANDEDITOR_H
