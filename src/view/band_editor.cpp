#include "band_editor.h"
#include "ui_band_editor.h"

BandEditor::BandEditor(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BandEditor)
{
    ui->setupUi(this);
    m_database = db;

    m_bands = new VariantMapTableModel(parent);
    m_bands->registerColumn(new SimpleColumn("authorId"));
    m_bands->registerColumn(new SimpleColumn("name", "Band"));
    m_bands->setIdColumn("authorId");

    ui->tbl_bands->setModel(m_bands);
    ui->tbl_bands->hideColumn(0);

    QHeaderView *bandRow = ui->tbl_bands->verticalHeader();
    bandRow->setSectionResizeMode(QHeaderView::Fixed);
    bandRow->setMinimumSectionSize(22);
    bandRow->setDefaultSectionSize(22);

    QObject::connect(m_database, &Database::authorsChanged, this, &BandEditor::updateBands);
    QObject::connect(ui->tbl_bands->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &BandEditor::handleBandSelection);
}

BandEditor::~BandEditor()
{
    delete ui;
}

void BandEditor::updateBands()
{
    m_bands->replaceRows(m_database->selectBands());
    qDebug() << TAG << "Updated bands.";
}

void BandEditor::handleBandSelection(const QItemSelection &selection)
{
    if (selection.indexes().isEmpty()) {
        ui->edt_bandName->clear();
        ui->edt_bandName->setEnabled(false);
        ui->btn_deleteBand->setEnabled(false);
        ui->btn_applyBandChanges->setEnabled(false);
    } else {
        int row = selection.indexes().first().row();
        int nameCol = m_bands->getColByKey("name");

        QModelIndex nameIndex = m_bands->index(row, nameCol);
        QString bandName = m_bands->data(nameIndex, Qt::DisplayRole).toString();

        ui->edt_bandName->setText(bandName);

        ui->edt_bandName->setEnabled(true);
        ui->btn_deleteBand->setEnabled(true);
        ui->btn_applyBandChanges->setEnabled(true);
    }
}

void BandEditor::on_btn_deleteBand_clicked()
{
    QItemSelection selection = ui->tbl_bands->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    int row = selection.indexes().first().row();
    int authorId = m_bands->getIdByRow(row);
    m_database->removeAuthor(authorId);

    if (row < m_bands->rowCount(QModelIndex()))
        ui->tbl_bands->selectRow(row);
    else if (row > 0)
        ui->tbl_bands->selectRow(row-1);
    else
        handleBandSelection(QItemSelection());

    ui->tbl_bands->setFocus();
}


void BandEditor::on_btn_newBand_clicked()
{
    int bandId = m_database->insertBand(tr("New Band"));
    int row = m_bands->getRowById(bandId);
    ui->tbl_bands->selectRow(row);
    ui->tbl_bands->setFocus();
    ui->edt_bandName->setText("");
}


void BandEditor::on_btn_applyBandChanges_clicked()
{
    QItemSelection selection = ui->tbl_bands->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    QString bandName = ui->edt_bandName->text();

    if (bandName.trimmed().isEmpty()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Band name cannot contain whitespaces or be empty."));
        msg.exec();
        ui->edt_bandName->setFocus();
        return;
    }

    int bandRow = selection.indexes().first().row();
    int bandId = m_bands->getIdByRow(bandRow);

    Band band(bandId, bandName);
    m_database->updateBand(band);
    ui->tbl_bands->selectRow(bandRow);
    ui->tbl_bands->setFocus();
}

