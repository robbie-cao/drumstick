/*
    Virtual Piano test using the MIDI Sequencer C++ library
    Copyright (C) 2006-2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QSettings>
#include <QNetworkInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include "sonivoxsettingsdialog.h"
#include "ui_sonivoxsettingsdialog.h"

const QString QSTR_PREFERENCES("SonivoxEAS");
const QString QSTR_BUFFERTIME("BufferTime");
const QString QSTR_REVERBTYPE("ReverbType");
const QString QSTR_REVERBAMT("ReverbAmt");
const QString QSTR_CHORUSTYPE("ChorusType");
const QString QSTR_CHORUSAMT("ChorusAmt");

SonivoxSettingsDialog::SonivoxSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SonivoxSettingsDialog)
{
    ui->setupUi(this);
    ui->combo_Reverb->addItem(QStringLiteral("Large Hall"), 0);
    ui->combo_Reverb->addItem(QStringLiteral("Hall"), 1);
    ui->combo_Reverb->addItem(QStringLiteral("Chamber"), 2);
    ui->combo_Reverb->addItem(QStringLiteral("Room"), 3);
    ui->combo_Reverb->addItem(QStringLiteral("None"), -1);
    ui->combo_Reverb->setCurrentIndex(4);

    ui->combo_Chorus->addItem(QStringLiteral("Preset 1"), 0);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 2"), 1);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 3"), 2);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 4"), 3);
    ui->combo_Chorus->addItem(QStringLiteral("None"), -1);
    ui->combo_Chorus->setCurrentIndex(4);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed,
            this, &SonivoxSettingsDialog::restoreDefaults);
}

SonivoxSettingsDialog::~SonivoxSettingsDialog()
{
    delete ui;
}

void SonivoxSettingsDialog::accept()
{
    writeSettings();
    QDialog::accept();
}

void SonivoxSettingsDialog::showEvent(QShowEvent *event)
{
    readSettings();
    event->accept();
}

void SonivoxSettingsDialog::readSettings()
{

    QSettings settings;
    settings.beginGroup(QSTR_PREFERENCES);
    int bufferTime = settings.value(QSTR_BUFFERTIME, 60).toInt();
    int reverbType = settings.value(QSTR_REVERBTYPE, 1).toInt();
    int reverbAmt = settings.value(QSTR_REVERBAMT, 25800).toInt();
    int chorusType = settings.value(QSTR_CHORUSTYPE, -1).toInt();
    int chorusAmt = settings.value(QSTR_CHORUSAMT, 0).toInt();
    settings.endGroup();

    ui->spnTime->setValue(bufferTime);
    ui->dial_Reverb->setValue(reverbAmt);
    ui->dial_Chorus->setValue(chorusAmt);
    int reverbIndex = ui->combo_Reverb->findData(reverbType);
    int chorusIndex = ui->combo_Chorus->findData(chorusType);
    ui->combo_Reverb->setCurrentIndex(reverbIndex);
    ui->combo_Chorus->setCurrentIndex(chorusIndex);
}

void SonivoxSettingsDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup(QSTR_PREFERENCES);
    settings.setValue(QSTR_BUFFERTIME, ui->spnTime->value());
    settings.setValue(QSTR_REVERBTYPE, ui->combo_Reverb->currentData());
    settings.setValue(QSTR_CHORUSTYPE, ui->combo_Chorus->currentData());
    settings.setValue(QSTR_REVERBAMT, ui->dial_Reverb->value());
    settings.setValue(QSTR_CHORUSAMT, ui->dial_Chorus->value());
    settings.endGroup();
    settings.sync();
}

void SonivoxSettingsDialog::restoreDefaults()
{
    ui->spnTime->setValue(60);
    ui->combo_Reverb->setCurrentIndex(1);
    ui->dial_Reverb->setValue(25800);
    ui->combo_Chorus->setCurrentIndex(4);
    ui->dial_Chorus->setValue(0);
}
