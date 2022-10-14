#ifndef SINGER_EDITOR_H
#define SINGER_EDITOR_H

#include <QDialog>
#include <QItemSelection>
#include <QMessageBox>

#include "src/database.h"
#include "src/model/simplecolumn.h"

#undef TAG
#define TAG "[SingerEditor]"

namespace Ui {
class SingerEditor;
}

class SingerEditor : public QDialog
{
    Q_OBJECT

public:
    explicit SingerEditor(Database *db, QWidget *parent = nullptr);
    ~SingerEditor();

public slots:
    void updateSingers();
    void handleSingerSelection(const QItemSelection &selection);

private slots:
    void on_btn_deleteSinger_clicked();
    void on_btn_newSinger_clicked();
    void on_btn_applySingerChanges_clicked();

private:
    Ui::SingerEditor *ui;

    Database *m_database;
    VariantMapTableModel *m_singers;
};

#endif // SINGER_EDITOR_H
