#include "singer_editor.h"
#include "ui_singer_editor.h"

SingerEditor::SingerEditor(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingerEditor)
{
    ui->setupUi(this);
    m_database = db;

    m_singers = new VariantMapTableModel(parent);
    m_singers->registerColumn(new SimpleColumn("authorId"));
    m_singers->registerColumn(new SimpleColumn("firstName", "First name"));
    m_singers->registerColumn(new SimpleColumn("lastName", "Last name"));
    m_singers->setIdColumn("authorId");

    ui->tbl_singers->setModel(m_singers);
    ui->tbl_singers->hideColumn(0);

    QHeaderView *singerRow = ui->tbl_singers->verticalHeader();
    singerRow->setSectionResizeMode(QHeaderView::Fixed);
    singerRow->setMinimumSectionSize(22);
    singerRow->setDefaultSectionSize(22);

    QObject::connect(m_database, &Database::authorsChanged, this, &SingerEditor::updateSingers);
    QObject::connect(ui->tbl_singers->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &SingerEditor::handleSingerSelection);
}

SingerEditor::~SingerEditor()
{
    delete ui;
}

void SingerEditor::updateSingers()
{
    m_singers->replaceRows(m_database->selectSingers());
    qDebug() << TAG << "Updated singers.";
}

void SingerEditor::handleSingerSelection(const QItemSelection &selection)
{
    if (selection.indexes().isEmpty()) {
        ui->edt_firstName->clear();
        ui->edt_lastName->clear();
        ui->edt_firstName->setEnabled(false);
        ui->edt_lastName->setEnabled(false);
        ui->btn_deleteSinger->setEnabled(false);
        ui->btn_applySingerChanges->setEnabled(false);
    } else {
        int row = selection.indexes().first().row();
        int firstNameCol = m_singers->getColByKey("firstName");
        int lastNameCol = m_singers->getColByKey("lastName");

        QModelIndex firstNameIndex = m_singers->index(row, firstNameCol);
        QModelIndex lastNameIndex = m_singers->index(row, lastNameCol);

        QString firstName = m_singers->data(firstNameIndex, Qt::DisplayRole).toString();
        QString lastName = m_singers->data(lastNameIndex, Qt::DisplayRole).toString();

        ui->edt_firstName->setText(firstName);
        ui->edt_lastName->setText(lastName);

        ui->edt_firstName->setEnabled(true);
        ui->edt_lastName->setEnabled(true);
        ui->btn_deleteSinger->setEnabled(true);
        ui->btn_applySingerChanges->setEnabled(true);
    }
}

void SingerEditor::on_btn_deleteSinger_clicked()
{
    QItemSelection selection = ui->tbl_singers->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    int row = selection.indexes().first().row();
    int authorId = m_singers->getIdByRow(row);
    m_database->removeAuthor(authorId);

    if (row < m_singers->rowCount(QModelIndex()))
        ui->tbl_singers->selectRow(row);
    else if (row > 0)
        ui->tbl_singers->selectRow(row-1);
    else
        handleSingerSelection(QItemSelection());

    ui->tbl_singers->setFocus();
}


void SingerEditor::on_btn_newSinger_clicked()
{
    int authorId = m_database->insertSinger(tr("First"), tr("Last"));
    int row = m_singers->getRowById(authorId);
    ui->tbl_singers->selectRow(row);
    ui->tbl_singers->setFocus();
    ui->edt_firstName->setText("");
    ui->edt_lastName->setText("");
}


void SingerEditor::on_btn_applySingerChanges_clicked()
{
    QItemSelection selection = ui->tbl_singers->selectionModel()->selection();

    if (selection.isEmpty())
        return;

    QString firstName = ui->edt_firstName->text();
    QString lastName = ui->edt_lastName->text();

    if (firstName.trimmed().isEmpty()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Singers's first name cannot contain whitespaces or be empty."));
        msg.exec();
        ui->edt_firstName->setFocus();
        return;
    }

    if (lastName.trimmed().isEmpty()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText(tr("Singers's last name cannot contain whitespaces or be empty."));
        msg.exec();
        ui->edt_lastName->setFocus();
        return;
    }

    int singerRow = selection.indexes().first().row();
    int authorId = m_singers->getIdByRow(singerRow);

    Singer singer(authorId, firstName, lastName);
    m_database->updateSinger(singer);
    ui->tbl_singers->selectRow(singerRow);
    ui->tbl_singers->setFocus();

}

