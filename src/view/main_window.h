#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QCompleter>
#include <QMessageBox>

#include "src/view/disc_editor.h"
#include "src/view/song_editor.h"
#include "src/view/band_editor.h"
#include "src/view/singer_editor.h"
#include "src/view/help_note.h"
#include "src/database.h"
#include "src/model/variantmaptablemodel.h"
#include "src/model/simplecolumn.h"
#include "src/model/lookupcolumn.h"

#undef TAG
#define TAG "[MainWindow]"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_editDiscs_clicked();
    void on_btn_editSingers_clicked();
    void on_btn_editSongs_clicked();
    void on_btn_editBands_clicked();
    void on_actionDiscs_triggered();
    void on_actionSongs_triggered();
    void on_actionBands_triggered();
    void on_actionSingers_triggered();
    void on_btn_applyFilter_clicked();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionUserManual_triggered();

public slots:
    void updateLibrary();
    void updateDiscs();
    void updateSongs();
    void updateAuthors();
    void syncDiscFilter();
    void syncSongFilter();
    void syncAuthorFilter();

private:
    Ui::MainWindow *ui;

    DiscEditor *m_discEditor;
    SongEditor *m_songEditor;
    BandEditor *m_bandEditor;
    SingerEditor *m_singerEditor;
    HelpNote *m_helpNote;

    QCompleter *m_discCompleter;
    QCompleter *m_songCompleter;
    QCompleter *m_authorCompleter;

    Database *m_database;
    VariantMapTableModel *m_library;
    VariantMapTableModel *m_discs;
    VariantMapTableModel *m_songs;
    VariantMapTableModel *m_authors;

    QLabel *m_dbStatus;
    Filter m_filter;
    QString m_currentFile;
};

#endif // MAIN_WINDOW_H
