#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_dbStatus = new QLabel(this);
    m_dbStatus->setText(tr("No file loaded."));
    ui->statusbar->addWidget(m_dbStatus);

    m_database = new Database(this);

    m_discEditor = new DiscEditor(m_database, this);
    m_songEditor = new SongEditor(m_database, this);
    m_bandEditor = new BandEditor(m_database, this);
    m_singerEditor = new SingerEditor(m_database, this);
    m_helpNote = new HelpNote(this);

    m_discs = new VariantMapTableModel(this);
    m_discs->registerColumn(new SimpleColumn("discId"));
    m_discs->registerColumn(new SimpleColumn("name"));
    m_discs->setIdColumn("discId");

    m_songs = new VariantMapTableModel(this);
    m_songs->registerColumn(new SimpleColumn("songId"));
    m_songs->registerColumn(new SimpleColumn("name"));
    m_songs->registerColumn(new SimpleColumn("authorId"));
    m_songs->setIdColumn("songId");

    m_authors = new VariantMapTableModel(this);
    m_authors->registerColumn(new SimpleColumn("authorId"));
    m_authors->registerColumn(new SimpleColumn("displayName"));
    m_authors->setIdColumn("authorId");

    // Discs
    m_discCompleter = new QCompleter(this);
    m_discCompleter->setModel(m_discs);
    m_discCompleter->setCompletionColumn(1);
    m_discCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_discCompleter->setFilterMode(Qt::MatchContains);
    ui->cb_discFilter->setModel(m_discs);
    ui->cb_discFilter->setModelColumn(1);
    ui->cb_discFilter->setCompleter(m_discCompleter);

    // Songs
    m_songCompleter = new QCompleter(this);
    m_songCompleter->setModel(m_songs);
    m_songCompleter->setCompletionColumn(1);
    m_songCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_songCompleter->setFilterMode(Qt::MatchContains);
    ui->cb_songFilter->setModel(m_songs);
    ui->cb_songFilter->setModelColumn(1);
    ui->cb_songFilter->setCompleter(m_songCompleter);

    // Authors
    m_authorCompleter = new QCompleter(this);
    m_authorCompleter->setModel(m_authors);
    m_authorCompleter->setCompletionColumn(1);
    m_authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_authorCompleter->setFilterMode(Qt::MatchContains);
    ui->cb_authorFilter->setModel(m_authors);
    ui->cb_authorFilter->setModelColumn(1);
    ui->cb_authorFilter->setCompleter(m_authorCompleter);

    // Library
    m_library = new VariantMapTableModel(this);
    m_library->registerColumn(new LookupColumn("disc", m_discs, "discId", "name", "Disc"));
    m_library->registerColumn(new LookupColumn("song", m_songs, "songId", "name", "Song"));
    m_library->registerColumn(new LookupColumn("author", m_authors, "authorId", "displayName", "Author"));

    ui->tbl_database->setModel(m_library);

    QHeaderView *dbRow = ui->tbl_database->verticalHeader();
    dbRow->setSectionResizeMode(QHeaderView::Fixed);
    dbRow->setMinimumSectionSize(22);
    dbRow->setDefaultSectionSize(22);

    QObject::connect(m_database, &Database::endUpdate, this, &MainWindow::updateLibrary);
    QObject::connect(m_database, &Database::discsChanged, this, &MainWindow::updateDiscs);
    QObject::connect(m_database, &Database::songsChanged, this, &MainWindow::updateSongs);
    QObject::connect(m_database, &Database::authorsChanged, this, &MainWindow::updateAuthors);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLibrary()
{
    m_library->replaceRows(m_database->select(m_filter));
    qDebug() << TAG << "Updated library.";
}

void MainWindow::updateDiscs()
{
    m_discs->replaceRows(m_database->selectDiscs());
    syncDiscFilter();
    qDebug() << TAG << "Updated discs.";
}

void MainWindow::updateSongs()
{
    m_songs->replaceRows(m_database->selectSongs());
    syncSongFilter();
    qDebug() << TAG << "Updated songs.";
}

void MainWindow::updateAuthors()
{
    m_authors->replaceRows(m_database->selectAuthors());
    syncAuthorFilter();
    qDebug() << TAG << "Updated authors.";
}

void MainWindow::syncDiscFilter()
{
    int discRow = m_discs->getRowById(m_filter.discId);

    if (discRow < 0)
        m_filter.discId = -1;

    ui->cb_discFilter->setCurrentIndex(discRow);
}

void MainWindow::syncSongFilter()
{
    int songRow = m_songs->getRowById(m_filter.songId);

    if (songRow < 0)
        m_filter.songId = -1;

    ui->cb_songFilter->setCurrentIndex(songRow);
}

void MainWindow::syncAuthorFilter()
{
    int authorRow = m_authors->getRowById(m_filter.authorId);

    if (authorRow < 0)
        m_filter.authorId = -1;

    ui->cb_authorFilter->setCurrentIndex(authorRow);
}

void MainWindow::on_btn_editDiscs_clicked()
{
    m_discEditor->show();
}


void MainWindow::on_btn_editSingers_clicked()
{
    m_singerEditor->show();
}


void MainWindow::on_btn_editSongs_clicked()
{
    m_songEditor->show();
}


void MainWindow::on_btn_editBands_clicked()
{
    m_bandEditor->show();
}


void MainWindow::on_actionDiscs_triggered()
{
    on_btn_editDiscs_clicked();
}


void MainWindow::on_actionSongs_triggered()
{
    on_btn_editSongs_clicked();
}


void MainWindow::on_actionBands_triggered()
{
    on_btn_editBands_clicked();
}


void MainWindow::on_actionSingers_triggered()
{
    on_btn_editSingers_clicked();
}


void MainWindow::on_btn_applyFilter_clicked()
{
    QString disc = ui->cb_discFilter->currentText();
    QString song = ui->cb_songFilter->currentText();
    QString author = ui->cb_authorFilter->currentText();

    if (!disc.isEmpty() && ui->cb_discFilter->findText(disc) < 0) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Please select disc from the list or leave it empty."));
        msg.exec();
        ui->cb_discFilter->setFocus();
        return;
    }

    if (disc.isEmpty())
        ui->cb_discFilter->setCurrentIndex(-1);

    if (!song.isEmpty() && ui->cb_songFilter->findText(song) < 0) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Please select song from the list or leave it empty."));
        msg.exec();
        ui->cb_songFilter->setFocus();
        return;
    }

    if (song.isEmpty())
        ui->cb_songFilter->setCurrentIndex(-1);

    if (!author.isEmpty() && ui->cb_authorFilter->findText(author) < 0) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Please select author from the list or leave it empty."));
        msg.exec();
        ui->cb_authorFilter->setFocus();
        return;
    }

    if (author.isEmpty())
        ui->cb_authorFilter->setCurrentIndex(-1);

    m_filter = {
        m_discs->getIdByRow(ui->cb_discFilter->currentIndex()),
        m_songs->getIdByRow(ui->cb_songFilter->currentIndex()),
        m_authors->getIdByRow(ui->cb_authorFilter->currentIndex()),
        ui->chk_discRequired->isChecked()
    };

    qDebug() << TAG << "Filter updated:"
             << "discId=" << m_filter.discId
             << "songId=" << m_filter.songId
             << "authorId=" << m_filter.authorId
             << "discRequired=" << m_filter.discRequired;

    updateLibrary();
    ui->tab_widget->setCurrentIndex(0);
}


void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Database (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
        QList<QString> files = dialog.selectedFiles();
        QString filename = files[0];

        qDebug() << TAG << "QFileDialog: file selected:" << filename;

        try {
            m_database->loadFromJsonFile(filename);
            m_dbStatus->setText(filename);
            m_currentFile = filename;
        } catch (DbException &e) {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);

            switch (e) {
                case DbException::FILE_OPEN:
                    msg.setText(tr("Cannot open \"%1\" for reading. Try again.").arg(filename));
                    break;

                case DbException::BAD_CONTENT:
                    msg.setText(tr("Cannot load database: the selected file is not a valid database file or it's corrupted."));
                    break;
            }

            msg.exec();
        }
    } else {
        qDebug() << TAG << "QFileDialog: no file selected";
    }
}


void MainWindow::on_actionSave_triggered()
{
    if (!m_currentFile.isEmpty()) {
        try {
            m_database->saveToJsonFile(m_currentFile);
            ui->statusbar->showMessage("Saved to current file!", 5000);
        } catch (DbException &e) {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setText(tr("Cannot save database to current file. Try again."));
            msg.exec();
        }
    } else {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("No file is currently open. Thus nowhere to save."));
        msg.exec();
    }
}


void MainWindow::on_actionSave_As_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Database (*.json)"));
    dialog.setDefaultSuffix(".json");
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec()) {
        QList<QString> files = dialog.selectedFiles();
        QString filename = files[0];

        qDebug() << TAG << "QFileDialog: file specified:" << filename;

        try {
            m_database->saveToJsonFile(filename);

            if (m_currentFile.isEmpty()) {
                m_dbStatus->setText(filename);
                m_currentFile = filename;
            }

            ui->statusbar->showMessage("Saved to specified file!", 5000);
        } catch (DbException &e) {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setText(tr("Cannot save database to specified file. Try again."));
            msg.exec();
        }

    } else {
        qDebug() << TAG << "QFileDialog: no file specified";
    }
}

void MainWindow::on_actionUserManual_triggered()
{
    m_helpNote->show();
}

